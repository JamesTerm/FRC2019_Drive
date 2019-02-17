
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
private:
	//helper functions
	__inline double RPS_To_LinearVelocity(double RPS) const
	{
		return RPS * m_TankRobotProps.MotorToWheelGearRatio * M_PI * m_TankRobotProps.WheelDiameter;
	}
	__inline double LinearVelocity_To_RPS(double Velocity) const
	{
		return (Velocity / (M_PI * m_TankRobotProps.WheelDiameter)) * (1.0 / m_TankRobotProps.MotorToWheelGearRatio);
	}

protected: //from RobotControlCommon
	virtual size_t RobotControlCommon_Get_PWMSpeedController_EnumValue(const char *name) const
	{
		return Tank_Robot2::GetSpeedControllerDevices_Enum(name);
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
				PWMSpeedController_GetInstance(Tank_Robot2::eLeftDrive1), PWMSpeedController_GetInstance(Tank_Robot2::eLeftDrive2),
				PWMSpeedController_GetInstance(Tank_Robot2::eRightDrive1), PWMSpeedController_GetInstance(Tank_Robot2::eRightDrive2),
				PWMSpeedController_GetInstance(Tank_Robot2::eLeftDrive3), PWMSpeedController_GetInstance(Tank_Robot2::eRightDrive3));
			SetSafety(m_UseSafety);
			const double EncoderPulseRate = (1.0 / 360.0);
			Encoder_SetDistancePerPulse(Tank_Robot2::eLeftDrive1, EncoderPulseRate), Encoder_SetDistancePerPulse(Tank_Robot2::eRightDrive1, EncoderPulseRate);
			Encoder_Start(Tank_Robot2::eLeftDrive1), Encoder_Start(Tank_Robot2::eRightDrive1);
		}
		assert(robot_props);
		//This will copy all the props
		m_TankRobotProps = robot_props->GetTankRobotProps();
		//Note: These reversed encoder properties require reboot of cRIO
		//printf("Tank_Robot_Control::Initialize ReverseLeft=%d ReverseRight=%d\n",m_TankRobotProps.LeftEncoderReversed,m_TankRobotProps.RightEncoderReversed);
		Encoder_SetReverseDirection(Tank_Robot2::eLeftDrive1, m_TankRobotProps.LeftEncoderReversed);
		Encoder_SetReverseDirection(Tank_Robot2::eRightDrive1, m_TankRobotProps.RightEncoderReversed);
	}
	virtual ~Tank_Robot2_Control()
	{
		Encoder_Stop(Tank_Robot2::eLeftDrive1), Encoder_Stop(Tank_Robot2::eRightDrive1); 
		m_RobotDrive->SetSafetyEnabled(false);
	}
	
	virtual void Tank_Drive_Control_TimeChange(double dTime_s)
	{
		m_dTime_s = dTime_s;
		//For now just copy over whats in the drive, but later look into using the encoder simulator
		#ifdef _Win32
		const double MaxSpeed = 5.0;  //hard coded for now since this code is temporary
		float LeftVoltage, RightVoltage;
		m_RobotDrive->GetLeftRightMotorOutputs(LeftVoltage, RightVoltage);
		double velocity = LeftVoltage * MaxSpeed;
		double rps = LinearVelocity_To_RPS(velocity) * dTime_s;
		Encoder_TimeChange(Tank_Robot2::eLeftDrive1, dTime_s, rps);
		velocity = RightVoltage * MaxSpeed;
		rps = LinearVelocity_To_RPS(velocity) * dTime_s;
		Encoder_TimeChange(Tank_Robot2::eRightDrive1, dTime_s, rps);
		#endif
	}

	virtual void Reset_Encoders();
	//Note: this returns linear velocity in meters per second
	virtual void GetLeftRightVelocity(double &LeftVelocity, double &RightVelocity)
	{
		LeftVelocity = 0.0, RightVelocity = 0.0;
		double LeftRate = Encoder_GetRate(Tank_Robot2::eLeftDrive1);
		LeftRate = m_KalFilter_EncodeLeft(LeftRate);
		LeftRate = m_Averager_EncoderLeft.GetAverage(LeftRate);
		LeftRate = IsZero(LeftRate) ? 0.0 : LeftRate;

		double RightRate = Encoder_GetRate(Tank_Robot2::eRightDrive1);
		RightRate = m_KalFilter_EncodeRight(RightRate);
		RightRate = m_Averager_EncodeRight.GetAverage(RightRate);
		RightRate = IsZero(RightRate) ? 0.0 : RightRate;

		LeftVelocity = RPS_To_LinearVelocity(LeftRate);
		RightVelocity = RPS_To_LinearVelocity(RightRate);
		//Dout(m_TankRobotProps.Feedback_DiplayRow, "l=%.1f r=%.1f", LeftVelocity, RightVelocity);
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
};


  /***********************************************************************************************************************************/
 /*																Tank_Robot2															*/
/***********************************************************************************************************************************/


void Tank_Robot2::TimeChange(double dTime_s)
{
	SmartDashboard::PutNumber("LeftVoltage", m_Velocity[0]);
	SmartDashboard::PutNumber("RightVoltage", m_Velocity[1]);
	//TODO pass to speed controllers here
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
			m_Controller_Velocity[0] = m_Velocity[0] = value;
		});
		em->EventValue_Map[csz_Joystick_SetRightVelocity].Subscribe([&](double value)
		{
			m_Controller_Velocity[1] = m_Velocity[1] = value;
		});
	}
}

void Tank_Robot2::BindAdditionalUIControls(bool Bind, void *joy, void *key)
{
	m_TankRobotProps->Get_RobotControls().BindAdditionalUIControls(Bind, joy, key);
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
}

void Tank_Robot2_Properties::LoadFromScript(Scripting::Script& script)
{
	const char* err = NULL;
	m_ControlAssignmentProps.LoadFromScript(script);
	err = script.GetFieldTable("controls");
	if (!err)
	{
		m_RobotControls.LoadFromScript(script);
		script.Pop();
	}
}
