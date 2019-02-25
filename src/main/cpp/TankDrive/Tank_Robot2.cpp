
#include "../Base/Base_Includes.h"
#include <math.h>
#include <assert.h>
#include "../Base/Vec2d.h"
#include "../Base/Misc.h"
#include "../Base/Event.h"
#include "../Base/EventMap.h"
#include "../Base/Script.h"
#include "../Common/Entity_Properties.h"
#include "../Common/Physics_1D.h"
#include "../Common/Physics_2D.h"
#include "../Common/Entity2D.h"
#include "../Common/Goal.h"
#include "../Common/Ship.h"
#include "../Common/AI_Base_Controller.h"
#include "../Common/Vehicle_Drive.h"
#include "../Common/PIDController.h"
#include "../Common/Poly.h"

#include "../Base/Joystick.h"
#include "../Base/JoystickBinder.h"
#include "../Common/UI_Controller.h"
#ifndef _Win32
#include <frc/WPILib.h>
#include <ctre/Phoenix.h>
#include "../Common/InOut_Interface.h"
#else
#include "../Common/Ship_1D.h"
#include "../Common/Ship.h"
#include "../Common/AI_Base_Controller.h"
#include "../Common/Vehicle_Drive.h"
#include "../Common/PIDController.h"
#include "../Common/Poly.h"
#include "../Common/Robot_Control_Interface.h"
#include "../Common/Rotary_System.h"
#include "../Common/Calibration_Testing.h"
#endif
#include "../Common/Debug.h"
#include "../Common/Robot_Control_Common.h"
#include "Tank_Robot2.h"

#include "../Common/SmartDashboard.h"

using namespace Framework::Base;
using namespace std;


class Tank_Robot2_Control : public frc::RobotControlCommon
{
private:
	std::unique_ptr<frc::RobotDrive_Interface> m_RobotDrive;

	double m_dTime_s=0.0;  //Stamp the current time delta slice for other functions to use
	Tank_Robot2_Props m_TankRobotProps; //cached in the Initialize from specific robot
	//Encoder averagers:
	KalmanFilter m_KalFilter_EncodeLeft, m_KalFilter_EncodeRight;
	Averager<double, 4> m_Averager_EncoderLeft, m_Averager_EncodeRight;
	//-----------------
	const bool m_UseSafety;
	bool m_IsLowGear = false;
private:
	//helper functions
	__inline double GetMotorToWheelGearRatio() const
	{
		return m_IsLowGear ? m_TankRobotProps.low_gear.MotorToWheelGearRatio : m_TankRobotProps.high_gear.MotorToWheelGearRatio;
	}
	__inline double RPS_To_LinearVelocity(double RPS) const
	{
		return RPS * GetMotorToWheelGearRatio() * M_PI * m_TankRobotProps.WheelDiameter;
	}
	__inline double LinearVelocity_To_RPS(double Velocity) const
	{
		return (Velocity / (M_PI * m_TankRobotProps.WheelDiameter)) * (1.0 / GetMotorToWheelGearRatio());
	}

protected: //from RobotControlCommon
	enum SpeedControllerDevices
	{
		eLeftDrive1,
		eLeftDrive2,
		eLeftDrive3,
		eRightDrive1,
		eRightDrive2,
		eRightDrive3
	};

	static SpeedControllerDevices GetSpeedControllerDevices_Enum(const char *value)
	{
		return Enum_GetValue<SpeedControllerDevices>(value, csz_Tank_Robot2_SpeedControllerDevices_Enum, _countof(csz_Tank_Robot2_SpeedControllerDevices_Enum));
	}

	virtual size_t RobotControlCommon_Get_PWMSpeedController_EnumValue(const char *name) const
	{
		return GetSpeedControllerDevices_Enum(name);
	}
	virtual size_t RobotControlCommon_Get_DigitalInput_EnumValue(const char *name) const { return (size_t)-1; }
	virtual size_t RobotControlCommon_Get_AnalogInput_EnumValue(const char *name) const { return (size_t)-1; }
	virtual size_t RobotControlCommon_Get_DoubleSolenoid_EnumValue(const char *name) const { return (size_t)-1; }

public:
	//double Get_dTime_s() const { return m_dTime_s; }
	Tank_Robot2_Control(bool UseSafety = true) : m_UseSafety(UseSafety)
	{
		//ResetPos();  //reserved
	}
	virtual void Initialize(const Tank_Robot2_Properties *props)
	{
		using namespace frc;
		const Tank_Robot2_Properties *robot_props = props;
		//Note: Initialize may be called multiple times so we'll only set this stuff up on first run
		if (!m_RobotDrive)
		{
			const frc::Control_Assignment_Properties &cap = robot_props->Get_ControlAssignmentProps();
			RobotControlCommon_Initialize(cap);
			//Now to determine if the lua specifies to use PWMSpeedController or VictorSPX, because SPX is not a kind of PWM it can not be solved inherited
			//like with Victor and VictorSP; however, once it is resolved here all calls from m_RobotDrive will go to the appropriate implementation
			//We'll look for one of the drive names it SPX... it can be either left or right... and we'll pick the first one from those
			const char *csz_LeftDrive = csz_Tank_Robot2_SpeedControllerDevices_Enum[eLeftDrive1];
			//unfortunately I have to copy the props because find_if doesn't like using a const iterator
			Control_Assignment_Properties::Controls_1C Victors = cap.GetVictorSPXs();
			auto pos = std::find_if(Victors.begin(), Victors.end(),[csz_LeftDrive](Control_Assignment_Properties::Control_Element_1C &m) -> bool
				{
					return strcmp(csz_LeftDrive,m.name.c_str())==0;
				});
			const bool is_victor_spx = (pos != Victors.end());
			
			if (is_victor_spx)
				m_RobotDrive = std::make_unique<RobotDrive_SPX>(
					VictorSPX_GetInstance(eLeftDrive1), VictorSPX_GetInstance(eLeftDrive2),
					VictorSPX_GetInstance(eRightDrive1), VictorSPX_GetInstance(eRightDrive2),
					VictorSPX_GetInstance(eLeftDrive3), VictorSPX_GetInstance(eRightDrive3));
			else
				m_RobotDrive = std::make_unique<RobotDrive2>(
					PWMSpeedController_GetInstance(eLeftDrive1), PWMSpeedController_GetInstance(eLeftDrive2),
					PWMSpeedController_GetInstance(eRightDrive1), PWMSpeedController_GetInstance(eRightDrive2),
					PWMSpeedController_GetInstance(eLeftDrive3), PWMSpeedController_GetInstance(eRightDrive3));

			SetSafety(m_UseSafety);
			const double EncoderPulseRate = (1.0 / 360.0);
			Encoder_SetDistancePerPulse(eLeftDrive1, EncoderPulseRate), Encoder_SetDistancePerPulse(eRightDrive1, EncoderPulseRate);
			Encoder_Start(eLeftDrive1), Encoder_Start(eRightDrive1);
		}
		assert(robot_props);
		//This will copy all the props
		m_TankRobotProps = robot_props->GetTankRobotProps();
		//Note: These reversed encoder properties require reboot of cRIO
		//printf("Tank_Robot_Control::Initialize ReverseLeft=%d ReverseRight=%d\n",m_TankRobotProps.LeftEncoderReversed,m_TankRobotProps.RightEncoderReversed);
		Encoder_SetReverseDirection(eLeftDrive1, m_TankRobotProps.LeftEncoderReversed);
		Encoder_SetReverseDirection(eRightDrive1, m_TankRobotProps.RightEncoderReversed);
	}
	virtual ~Tank_Robot2_Control()
	{
		Encoder_Stop(eLeftDrive1), Encoder_Stop(eRightDrive1); 
		m_RobotDrive->SetSafetyEnabled(false);
	}
	
	virtual void Tank_Drive_Control_TimeChange(double dTime_s)
	{
		m_dTime_s = dTime_s;
		//For now just copy over whats in the drive, but later look into using the encoder simulator
		#ifdef _Win32
		const double MaxSpeed = GetMaxSpeed();
		float LeftVoltage, RightVoltage;
		m_RobotDrive->GetLeftRightMotorOutputs(LeftVoltage, RightVoltage);
		double velocity = LeftVoltage * MaxSpeed * m_TankRobotProps.VoltageScalar_Left;
		double rps = LinearVelocity_To_RPS(velocity) * dTime_s;
		Encoder_TimeChange(eLeftDrive1, dTime_s, rps);
		velocity = RightVoltage * MaxSpeed * m_TankRobotProps.VoltageScalar_Right;
		rps = LinearVelocity_To_RPS(velocity) * dTime_s;
		Encoder_TimeChange(eRightDrive1, dTime_s, rps);
		#endif
	}

	virtual void Reset_Encoders()
	{
		m_KalFilter_EncodeLeft.Reset(), m_KalFilter_EncodeRight.Reset();
		Encoder_SetReverseDirection(eLeftDrive1, m_TankRobotProps.LeftEncoderReversed);
		Encoder_SetReverseDirection(eRightDrive1, m_TankRobotProps.RightEncoderReversed);
	}
	//Note: this returns linear velocity in meters per second
	virtual void GetLeftRightVelocity(double &LeftVelocity, double &RightVelocity)
	{
		LeftVelocity = 0.0, RightVelocity = 0.0;
		double LeftRate = Encoder_GetRate(eLeftDrive1);
		LeftRate = m_KalFilter_EncodeLeft(LeftRate);
		LeftRate = m_Averager_EncoderLeft.GetAverage(LeftRate);
		LeftRate = IsZero(LeftRate) ? 0.0 : LeftRate;

		double RightRate = Encoder_GetRate(eRightDrive1);
		RightRate = m_KalFilter_EncodeRight(RightRate);
		RightRate = m_Averager_EncodeRight.GetAverage(RightRate);
		RightRate = IsZero(RightRate) ? 0.0 : RightRate;

		LeftVelocity = RPS_To_LinearVelocity(LeftRate);
		RightVelocity = RPS_To_LinearVelocity(RightRate);
		//Dout(m_TankRobotProps.Feedback_DiplayRow, "l=%.1f r=%.1f", LeftVelocity, RightVelocity);
		SmartDashboard::PutNumber("LeftEncoder", LeftVelocity);
		SmartDashboard::PutNumber("RightEncoder", RightVelocity);
	}
	virtual void UpdateLeftRightVoltage(double LeftVoltage, double RightVoltage)
	{
		//For now leave this disabled... should not need to script this
		//Dout(2, "l=%.1f r=%.1f", LeftVoltage, RightVoltage);
		if (!m_TankRobotProps.ReverseSteering)
		{
			m_RobotDrive->SetLeftRightMotorOutputs(
				(float)(LeftVoltage * m_TankRobotProps.VoltageScalar_Left),
				(float)(RightVoltage * m_TankRobotProps.VoltageScalar_Right));
		}
		else
		{
			m_RobotDrive->SetLeftRightMotorOutputs(
				(float)(RightVoltage * m_TankRobotProps.VoltageScalar_Right),
				(float)(LeftVoltage * m_TankRobotProps.VoltageScalar_Left));
		}
	}

	void SetSafety(bool UseSafety)
	{
		if (UseSafety)
		{
			//I'm giving a whole second before the timeout kicks in... I do not want false positives!
			m_RobotDrive->SetExpiration(1.0);
			m_RobotDrive->SetSafetyEnabled(true);
		}
		else
			m_RobotDrive->SetSafetyEnabled(false);

	}

	__inline double GetMaxSpeed() const
	{
		return m_IsLowGear ? m_TankRobotProps.low_gear.MaxSpeed : m_TankRobotProps.high_gear.MaxSpeed;
	}

	//Depreciated:  only for XML configuration
	const char *HandlePWMHook_GetActiveName(const char *Name)
	{
		//printf("Drive: Get External PWMSpeedController %s[%d,%d]\n", Name, module, Channel);
		SpeedControllerDevices motor = GetSpeedControllerDevices_Enum(Name);
		//Translate our naming convention to the config naming convention
		const char *ConfigNaming = nullptr;
		switch (motor)
		{
		case eLeftDrive1:		ConfigNaming = "left_0";		break;
		case eLeftDrive2:		ConfigNaming = "left_1";		break;
		case eLeftDrive3:		ConfigNaming = "left_2";		break;
		case eRightDrive1:		ConfigNaming = "right_0";		break;
		case eRightDrive2:		ConfigNaming = "right_1";		break;
		case eRightDrive3:		ConfigNaming = "right_2";		break;
		}
		return ConfigNaming;
	}
};


class DriveNotify
{
private:
	Tank_Robot2 *m_pParent;
	bool m_IsDriverMoving = false;  //keep track of last state
public:
	DriveNotify(Tank_Robot2 *Parent) : m_pParent(Parent)
	{}
	void TimeChange(double dTime_s)
	{
		const bool current_state = m_pParent->IsDriverMoving();
		//now to do a valve operation check... only fires an event when there is change
		if (current_state != m_IsDriverMoving)
		{
			Entity2D_Kind::EventMap *em = m_pParent->GetEventMap();
			em->EventOnOff_Map["IsDriverMoving"].Fire(current_state);
			//SmartDashboard::PutBoolean("IsDriverMoving", current_state);
			//printf("IsMoving=%d\n",current_state);
			m_IsDriverMoving = current_state;
		}
	}
};

class Tank_Robot2_PID
{
private:
	PIDController2 m_PIDController_Left, m_PIDController_Right;
	double m_ErrorOffset_Left=0.0, m_ErrorOffset_Right=0.0; //used for calibration in brake (a.k.a. aggressive stop) mode
	bool m_UsingEncoders=false;
	//This will be the velocity applied to the controller on a time change
	Vec2d m_Velocity;
	double m_PreviousLeftVelocity=0.0, m_PreviousRightVelocity=0.0; //used to compute acceleration
	Tank_Robot2_Props m_TankRobotProps;  //copy the properties for ease of access
	//we'll listen for gear changes to avoid needing access to robot control
	bool m_IsLowGear = false;
public:
	Tank_Robot2_PID() :
		m_PIDController_Left(0.0, 0.0, 0.0), m_PIDController_Right(0.0, 0.0, 0.0)  //these will be overridden in properties
	{
	}
	//depreciated
	void SetVelocity(const Vec2d &velocity) { m_Velocity = velocity; }
	Vec2d GetVelocity() const { return m_Velocity; }

	__inline double GetMaxSpeed() const
	{
		return m_IsLowGear ? m_TankRobotProps.low_gear.MaxSpeed : m_TankRobotProps.high_gear.MaxSpeed;
	}

	void Reset()
	{
		const double MaxSpeed = GetMaxSpeed();
		m_PIDController_Left.Reset(), m_PIDController_Right.Reset();
		if (m_IsLowGear)
		{
			m_PIDController_Left.SetPID(m_TankRobotProps.low_gear.LeftPID[0], m_TankRobotProps.low_gear.LeftPID[1], m_TankRobotProps.low_gear.LeftPID[2]);
			m_PIDController_Right.SetPID(m_TankRobotProps.low_gear.RightPID[0], m_TankRobotProps.low_gear.RightPID[1], m_TankRobotProps.low_gear.RightPID[2]);
		}
		else
		{
			m_PIDController_Left.SetPID(m_TankRobotProps.high_gear.LeftPID[0], m_TankRobotProps.high_gear.LeftPID[1], m_TankRobotProps.high_gear.LeftPID[2]);
			m_PIDController_Right.SetPID(m_TankRobotProps.high_gear.RightPID[0], m_TankRobotProps.high_gear.RightPID[1], m_TankRobotProps.high_gear.RightPID[2]);
		}
		//ensure teleop has these set properly
		m_ErrorOffset_Left = m_ErrorOffset_Right = 0.0;
		m_PreviousLeftVelocity = m_PreviousRightVelocity = 0.0;

		const double OutputRange = MaxSpeed * 0.875;  //create a small range
		const double InputRange = 20.0;  //create a large enough number that can divide out the voltage and small enough to recover quickly
		m_PIDController_Left.SetInputRange(-MaxSpeed, MaxSpeed);
		m_PIDController_Left.SetOutputRange(-InputRange, OutputRange);
		m_PIDController_Left.Enable();
		m_PIDController_Right.SetInputRange(-MaxSpeed, MaxSpeed);
		m_PIDController_Right.SetOutputRange(-InputRange, OutputRange);
		m_PIDController_Right.Enable();
		m_ErrorOffset_Left = m_ErrorOffset_Right = 0.0;
	}

	void SetUseEncoders(bool UseEncoders)
	{
		if (!UseEncoders)
		{
			if (m_UsingEncoders)
			{
				//first disable it
				m_UsingEncoders = false;
				printf("Disabling encoders for drive\n");
				//Now to reset stuff
				Reset();
				//m_EncoderGlobalVelocity = Vec2d(0.0, 0.0);
			}
		}
		else
		{
			if (!m_UsingEncoders)
			{
				m_UsingEncoders = true;
				printf("Enabling encoders for drive\n");
				//setup the initial value with the encoders value
				Reset();
			}
		}
	}

	void Initialize(const Tank_Robot2_Properties *props)
	{
		const Tank_Robot2_Properties *RobotProps = props;
		//This will copy all the props
		m_TankRobotProps = RobotProps->GetTankRobotProps();
		m_IsLowGear = false;  //start out high until we know otherwise
		Reset();
		//This can be dynamically called so we always call it
		SetUseEncoders(!m_TankRobotProps.IsOpen);
	}

	//adjust the controller voltage given the current voltage and the encoder's velocity, returns the adjusted voltage in Vec2d
	Vec2d TimeChange(double dTime_s,double LeftVoltage,double RightVoltage, double Encoder_LeftVelocity,double Encoder_RightVelocity)
	{
		const double MaxSpeed = GetMaxSpeed();
		const double LeftVelocity = LeftVoltage * MaxSpeed;
		const double RightVelocity = RightVoltage * MaxSpeed;
		const bool UseAggressiveStop = m_TankRobotProps.UseAggressiveStop;
		if (m_UsingEncoders)
		{
			if (!UseAggressiveStop)
			{
				double control_left = 0.0, control_right = 0.0;
				//only adjust calibration when both velocities are in the same direction, or in the case where the encoder is stopped which will
				//allow the scaler to normalize if it need to start up again.
				if (((LeftVelocity * Encoder_LeftVelocity) > 0.0) || IsZero(Encoder_LeftVelocity))
					m_ErrorOffset_Left = m_PIDController_Left(LeftVelocity, Encoder_LeftVelocity, dTime_s);
				else
					m_ErrorOffset_Left = 0.0;
				if (((RightVelocity * Encoder_RightVelocity) > 0.0) || IsZero(Encoder_RightVelocity))
					m_ErrorOffset_Right = m_PIDController_Right(RightVelocity, Encoder_RightVelocity, dTime_s);
				else
					m_ErrorOffset_Right = 0.0;
			}
			else
			{
				m_ErrorOffset_Left = m_PIDController_Left(LeftVelocity, Encoder_LeftVelocity, dTime_s);
				m_ErrorOffset_Right = m_PIDController_Right(RightVelocity, Encoder_RightVelocity, dTime_s);
			}
		}
		if (m_TankRobotProps.PID_Console_Dump)
		{
			//fow now just use the smart dashboard
			SmartDashboard::PutNumber("PID_left_EO", m_ErrorOffset_Left);
			SmartDashboard::PutNumber("PID_Right_EO", m_ErrorOffset_Right);
		}
		const double AdjustedLeftVoltage = (LeftVelocity + m_ErrorOffset_Left) / MaxSpeed;
		const double AdjustedRightVoltage = (RightVelocity + m_ErrorOffset_Right) / MaxSpeed;
		return Vec2d(AdjustedLeftVoltage, AdjustedRightVoltage);
	}
};

const char *csz_Joystick_SetCurrentSpeed = "Joystick_SetCurrentSpeed";
const char *csz_Joystick_Analog_Turn = "Joystick_Analog_Turn";

class Tank_Robot2_AcradeDrive
{
private:
	Tank_Robot2 *m_pParent;
	double m_Forward_Voltage=0.0;  //range -1 to 1 where positive is forward
	double m_Right_Voltage=0.0;  //range -1 to 1 where positive is clockwise
public:
	Tank_Robot2_AcradeDrive(Tank_Robot2 *Parent) : m_pParent(Parent)
	{}
	void Reset()
	{
		m_Forward_Voltage = m_Right_Voltage = 0.0;
	}

	void BindAdditionalEventControls(bool Bind)
	{
		Entity2D_Kind::EventMap *em = m_pParent->GetEventMap();
		if (Bind)
		{
			em->EventValue_Map[csz_Joystick_SetCurrentSpeed].Subscribe([&](double value)
			{
				m_Forward_Voltage = value;
			});
			em->EventValue_Map[csz_Joystick_Analog_Turn].Subscribe([&](double value)
			{
				m_Right_Voltage = value;
			});
		}
	}

	//adjust the controller voltage given the current voltage adjusted voltage in Vec2d
	//We work with the existing voltage, as this makes it possible to work as a pass thru if this control is not used
	//A pass thru is more efficient than branching because it allows the CPU to precache, and adds take no time at all
	//It is possible for tank steering and arcade to work at the same time, since they are added together clip operation occur down stream
	//so we needn't do them here (In reality this is not an issue since in practice the controls are mutually exclusive)
	//Note this should happen before applying the PID
	Vec2d TimeChange(double dTime_s, double LeftVoltage, double RightVoltage)
	{
		double AdjustedLeftVoltage = LeftVoltage;
		double AdjustedRightVoltage = RightVoltage;
		//Base the rotation blend strength on how much forward is used; The abs of m_Forward_Voltage is ensured to be <= 1.0
		const double RCW_Blend = fabs(m_Right_Voltage * (1.0- (fabs(m_Forward_Voltage)<1.0?m_Forward_Voltage:1.0) * 0.5) );
		const double FWD_Blend = 1.0 - RCW_Blend;  //setup forward scalar
		//We blend the rotation with the linear, this way neither function will saturate and they both of equal influence no matter what variation
		AdjustedRightVoltage += m_Forward_Voltage * FWD_Blend + (m_Right_Voltage * RCW_Blend);
		AdjustedLeftVoltage += m_Forward_Voltage * FWD_Blend - (m_Right_Voltage * RCW_Blend);
		return Vec2d(AdjustedLeftVoltage, AdjustedRightVoltage);
	}
};

struct Tank_Robot2_Ancillary
{
	Tank_Robot2_Ancillary(Tank_Robot2 *Parent) : m_pParent(Parent),driveNotify(Parent),driveArcade(Parent)
	{}
	Tank_Robot2 *m_pParent;
	DriveNotify driveNotify;
	Tank_Robot2_PID drivePID;
	Tank_Robot2_AcradeDrive driveArcade;
	void Reset()
	{
		drivePID.Reset();
		driveArcade.Reset();
	}
};

  /***********************************************************************************************************************************/
 /*																Tank_Robot2															*/
/***********************************************************************************************************************************/

void Tank_Robot2::ResetPos()
{
	//It is important that these are reset incase a switch between auton and tele leaves external set
	m_Controller_Voltage = m_External_Voltage = Vec2d(0.0, 0.0);
	m_DriveControl->Reset_Encoders();
	m_Ancillary->Reset();
}

Tank_Robot2::Tank_Robot2(RobotCommon *robot) : m_pParent(robot) 
{
	m_DriveControl = make_shared<Tank_Robot2_Control>();
	m_Ancillary = make_shared<Tank_Robot2_Ancillary>(this);
	ResetPos();  //may need this
}

void Tank_Robot2::Initialize(const Tank_Robot2_Properties *props) 
{ 
	m_TankRobotProps = props; 
	m_DriveControl->Initialize(props);
	m_Ancillary->drivePID.Initialize(props);
}

void Tank_Robot2::TimeChange(double dTime_s)
{
	const double left_voltage = m_Controller_Voltage[0]+m_External_Voltage[0];
	const double right_voltage = m_Controller_Voltage[1]+m_External_Voltage[1];
	Vec2D AdjustedVoltage(left_voltage, right_voltage);  //written this way to monitor differences
	double EncoderLeft, EncoderRight;
	//Note: This call will implicitly update smart dashboard of its readings
	m_DriveControl->GetLeftRightVelocity(EncoderLeft, EncoderRight);  //we still want to read encoders if present
	AdjustedVoltage = m_Ancillary->driveArcade.TimeChange(dTime_s, left_voltage, right_voltage);
	//all PID here
	AdjustedVoltage = m_Ancillary->drivePID.TimeChange(dTime_s, AdjustedVoltage[0], AdjustedVoltage[1], EncoderLeft, EncoderRight);
	SmartDashboard::PutNumber("LeftVoltage", AdjustedVoltage[0]);
	SmartDashboard::PutNumber("RightVoltage", AdjustedVoltage[1]);
	m_DriveControl->UpdateLeftRightVoltage(AdjustedVoltage[0],AdjustedVoltage[1]);
	m_DriveControl->Tank_Drive_Control_TimeChange(dTime_s);
	m_Ancillary->driveNotify.TimeChange(dTime_s);
}

const char *csz_Joystick_SetLeftVelocity = "Joystick_SetLeftVelocity";
const char *csz_Joystick_SetRightVelocity = "Joystick_SetRightVelocity";
const char *csz_External_SetLeftVelocity = "External_SetLeftVelocity";
const char *csz_External_SetRightVelocity = "External_SetRightVelocity";

void Tank_Robot2::BindAdditionalEventControls(bool Bind)
{
	Entity2D_Kind::EventMap *em = m_pParent->GetEventMap();
	if (Bind)
	{
		em->EventValue_Map[csz_Joystick_SetLeftVelocity].Subscribe([&](double value)
		{
			m_Controller_Voltage[0] =  value;
		});
		em->EventValue_Map[csz_Joystick_SetRightVelocity].Subscribe([&](double value)
		{
			m_Controller_Voltage[1] =  value;
		});
		em->EventValue_Map[csz_External_SetLeftVelocity].Subscribe([&](double value)
		{
			m_External_Voltage[0] = value;
		});
		em->EventValue_Map[csz_External_SetRightVelocity].Subscribe([&](double value)
		{
			m_External_Voltage[1] = value;
		});
	}
	m_Ancillary->driveArcade.BindAdditionalEventControls(Bind);
}

void Tank_Robot2::BindAdditionalUIControls(bool Bind, void *joy, void *key)
{
	m_TankRobotProps->Get_RobotControls().BindAdditionalUIControls(Bind, joy, key);
}

void Tank_Robot2::SetDriveExternalPWMSpeedControllerHook(std::function<void *(size_t, size_t, const char *, const char*, bool &)> callback)
{
	m_DriveControl->SetExternalPWMSpeedControllerHook(callback);
}

const char *Tank_Robot2::HandlePWMHook_GetActiveName(const char *Name)
{
	return m_DriveControl->HandlePWMHook_GetActiveName(Name);
}

  /***********************************************************************************************************************************/
 /*													Tank_Robot2_Properties															*/
/***********************************************************************************************************************************/

//declared as global to avoid allocation on stack each iteration
const char * const g_Tank_Robot2_Controls_Events[] =
{
	csz_Joystick_SetLeftVelocity,csz_Joystick_SetRightVelocity,csz_Joystick_SetCurrentSpeed,csz_Joystick_Analog_Turn
};

const char *Tank_Robot2_Properties::ControlEvents::LUA_Controls_GetEvents(size_t index) const
{
	return (index < _countof(g_Tank_Robot2_Controls_Events)) ? g_Tank_Robot2_Controls_Events[index] : NULL;
}
Tank_Robot2_Properties::ControlEvents Tank_Robot2_Properties::s_ControlsEvents;

Tank_Robot2_Properties::Tank_Robot2_Properties() : m_RobotControls(&s_ControlsEvents)
{
	Tank_Robot2_Props props;
	memset(&props, 0, sizeof(Tank_Robot2_Props));

	//Late assign this to override the initial default
	const double c_WheelDiameter = 0.1524;  //6 inches
	props.WheelDiameter = c_WheelDiameter;
	props.high_gear.LeftPID[0] = props.high_gear.RightPID[0] = 1.0; //set PIDs to a safe default of 1,0,0
	props.high_gear.MotorToWheelGearRatio = 1.0;  //most-likely this will be overridden
	props.VoltageScalar_Left = props.VoltageScalar_Right = 1.0;  //May need to be reversed
	props.IsOpen = true;  //Always true by default until control is fully functional
	props.HasEncoders = true;  //no harm in having passive reading of them
	props.PID_Console_Dump = false;
	props.UseAggressiveStop = false;  //This is usually in coast for most cases from many teams
	props.ReverseSteering = false;
	props.LeftEncoderReversed = false;
	props.RightEncoderReversed = false;
	props.low_gear = props.high_gear;
	m_TankRobotProps = props;
}

void Tank_Robot2_Properties::LoadFromScript(Scripting::Script& script)
{
	const char* err = NULL;
	err = script.GetFieldTable("tank_drive");
	if (!err)
	{
		double fValue;

		err = script.GetField("wheel_diameter_in", NULL, NULL, &fValue);
		if (!err)
			m_TankRobotProps.WheelDiameter = Inches2Meters(fValue);

		//TODO move these into a GearProps function to get props for low and high gear

		err = script.GetField("max_speed", NULL, NULL, &fValue);
		if (!err)
			m_TankRobotProps.high_gear.MaxSpeed = fValue;
		err = script.GetField("max_speed_ft", NULL, NULL, &fValue);
		if (!err)
			m_TankRobotProps.high_gear.MaxSpeed = Feet2Meters(fValue);
		script.GetField("encoder_to_wheel_ratio", NULL, NULL, &m_TankRobotProps.high_gear.MotorToWheelGearRatio);
		double VoltageScalar;
		err = script.GetField("voltage_multiply", NULL, NULL, &VoltageScalar);
		if (!err)
			m_TankRobotProps.VoltageScalar_Left = m_TankRobotProps.VoltageScalar_Right = VoltageScalar;
		err = script.GetField("voltage_multiply_left", NULL, NULL, &m_TankRobotProps.VoltageScalar_Left);
		err = script.GetField("voltage_multiply_right", NULL, NULL, &m_TankRobotProps.VoltageScalar_Right);

		err = script.GetFieldTable("left_pid");
		if (!err)
		{
			err = script.GetField("p", NULL, NULL, &m_TankRobotProps.high_gear.LeftPID[0]);
			ASSERT_MSG(!err, err);
			err = script.GetField("i", NULL, NULL, &m_TankRobotProps.high_gear.LeftPID[1]);
			ASSERT_MSG(!err, err);
			err = script.GetField("d", NULL, NULL, &m_TankRobotProps.high_gear.LeftPID[2]);
			ASSERT_MSG(!err, err);
			script.Pop();
		}
		err = script.GetFieldTable("right_pid");
		if (!err)
		{
			err = script.GetField("p", NULL, NULL, &m_TankRobotProps.high_gear.RightPID[0]);
			ASSERT_MSG(!err, err);
			err = script.GetField("i", NULL, NULL, &m_TankRobotProps.high_gear.RightPID[1]);
			ASSERT_MSG(!err, err);
			err = script.GetField("d", NULL, NULL, &m_TankRobotProps.high_gear.RightPID[2]);
			ASSERT_MSG(!err, err);
			script.Pop();
		}

		string sTest;
		err = script.GetField("is_closed", &sTest, NULL, NULL);
		if (!err)
		{
			m_TankRobotProps.HasEncoders = true;
			if ((sTest.c_str()[0] == 'n') || (sTest.c_str()[0] == 'N') || (sTest.c_str()[0] == '0'))
				m_TankRobotProps.IsOpen = true;
			else
				m_TankRobotProps.IsOpen = false;
		}
		else
		{
			m_TankRobotProps.IsOpen = true;
			m_TankRobotProps.HasEncoders = false;
		}
		err = script.GetField("show_pid_dump", &sTest, NULL, NULL);
		if (!err)
		{
			if ((sTest.c_str()[0] == 'y') || (sTest.c_str()[0] == 'Y') || (sTest.c_str()[0] == '1'))
				m_TankRobotProps.PID_Console_Dump = true;
		}
		err = script.GetField("use_aggressive_stop", &sTest, NULL, NULL);
		if (!err)
		{
			if ((sTest.c_str()[0] == 'y') || (sTest.c_str()[0] == 'Y') || (sTest.c_str()[0] == '1'))
				m_TankRobotProps.UseAggressiveStop = true;
		}
		err = script.GetField("reverse_steering", &sTest, NULL, NULL);
		if (!err)
		{
			if ((sTest.c_str()[0] == 'y') || (sTest.c_str()[0] == 'Y') || (sTest.c_str()[0] == '1'))
				m_TankRobotProps.ReverseSteering = true;
		}
		err = script.GetField("left_encoder_reversed", &sTest, NULL, NULL);
		if (!err)
		{
			if ((sTest.c_str()[0] == 'y') || (sTest.c_str()[0] == 'Y') || (sTest.c_str()[0] == '1'))
				m_TankRobotProps.LeftEncoderReversed = true;
		}
		err = script.GetField("right_encoder_reversed", &sTest, NULL, NULL);
		if (!err)
		{
			if ((sTest.c_str()[0] == 'y') || (sTest.c_str()[0] == 'Y') || (sTest.c_str()[0] == '1'))
				m_TankRobotProps.RightEncoderReversed = true;
		}
		script.Pop();
	}

	m_ControlAssignmentProps.LoadFromScript(script);
	err = script.GetFieldTable("controls");
	if (!err)
	{
		m_RobotControls.LoadFromScript(script);
		script.Pop();
	}
}
