
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
	std::unique_ptr<frc::RobotDrive2> m_RobotDrive;

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
			RobotControlCommon_Initialize(robot_props->Get_ControlAssignmentProps());
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

	const char *HandlePWMHook_GetActiveName(const char *Name)
	{
		//printf("Drive: Get External PWMSpeedController %s[%d,%d]\n", Name, module, Channel);
		SpeedControllerDevices motor = GetSpeedControllerDevices_Enum(Name);
		//Translate our naming convention to the config naming convention
		const char *ConfigNaming = nullptr;
		switch (motor)
		{
		case eLeftDrive1:		ConfigNaming = "Left_0";		break;
		case eLeftDrive2:		ConfigNaming = "Left_1";		break;
		case eLeftDrive3:		ConfigNaming = "Left_2";		break;
		case eRightDrive1:		ConfigNaming = "Right_0";		break;
		case eRightDrive2:		ConfigNaming = "Right_1";		break;
		case eRightDrive3:		ConfigNaming = "Right_2";		break;
		}
		return ConfigNaming;
	}
};


  /***********************************************************************************************************************************/
 /*																Tank_Robot2															*/
/***********************************************************************************************************************************/

Tank_Robot2::Tank_Robot2(RobotCommon *robot) : m_pParent(robot) 
{
	m_DriveControl = make_shared<Tank_Robot2_Control>();
}

void Tank_Robot2::Initialize(const Tank_Robot2_Properties *props) 
{ 
	m_TankRobotProps = props; 
	m_DriveControl->Initialize(props);
}

void Tank_Robot2::TimeChange(double dTime_s)
{
	SmartDashboard::PutNumber("LeftVoltage", m_Controller_Voltage[0]);
	SmartDashboard::PutNumber("RightVoltage", m_Controller_Voltage[1]);
	#if 1
	const double left_voltage = m_Controller_Voltage[0];
	const double right_voltage = m_Controller_Voltage[1];
	//TODO PID here
	m_Velocity[0] = m_Controller_Voltage[0] * m_DriveControl->GetMaxSpeed();
	m_Velocity[1] = m_Controller_Voltage[1] * m_DriveControl->GetMaxSpeed();
	double EncoderLeft, EncoderRight;
	//Note: This call will implicitly update smart dashboard of its readings
	m_DriveControl->GetLeftRightVelocity(EncoderLeft, EncoderRight);  
	m_DriveControl->UpdateLeftRightVoltage(left_voltage,right_voltage);
	m_DriveControl->Tank_Drive_Control_TimeChange(dTime_s);
	#endif
}

const char *csz_Joystick_SetLeftVelocity = "Joystick_SetLeftVelocity";
const char *csz_Joystick_SetRightVelocity = "Joystick_SetRightVelocity";

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
	}
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
	csz_Joystick_SetLeftVelocity,csz_Joystick_SetRightVelocity
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
