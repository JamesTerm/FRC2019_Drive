
#include "Base/Base_Includes.h"
#include <math.h>
#include <assert.h>
#include "Base/Vec2d.h"
#include "Base/Misc.h"
#include "Base/Event.h"
#include "Base/EventMap.h"
#include "Base/Script.h"
#include "Common/Entity_Properties.h"
#include "Common/Physics_1D.h"
#include "Common/Physics_2D.h"
#include "Common/Entity2D.h"
#include "Common/Goal.h"
#include "Common/Ship_1D.h"
#include "Common/Ship.h"
#include "Common/AI_Base_Controller.h"
#include "Common/Vehicle_Drive.h"
#include "Common/PIDController.h"
#include "Common/Poly.h"
#include "Common/Robot_Control_Interface.h"
#include "Common/Rotary_System.h"
#include "Common/Servo_System.h"
#include "Base/Joystick.h"
#include "Base/JoystickBinder.h"
#include "Common/UI_Controller.h"
#include "Common/PIDController.h"
#include "Base/Joystick.h"
#include "Base/JoystickBinder.h"
#ifndef _Win32
#include <frc/WPILib.h>
#include "Common/InOut_Interface.h"
#else
#include "Common/Calibration_Testing.h"
#endif
#include "Common/Debug.h"
#include "Common/Robot_Control_Common.h"
#include "TankDrive/Tank_Robot.h"
//This was depreciated and integrated ... stubbed out
// #include "TankDrive/src/Tank_Robot_Control.h"
//This isn't needed
// #include "TankDrive/src/Servo_Robot_Control.h"

#include "FRC2019_Robot.h"
#include "Common/SmartDashboard.h"
using namespace Framework::Base;
using namespace std;

//#define __EnableRobotArmDisable__


  /***********************************************************************************************************************************/
 /*													FRC2019_Robot::Robot_Claw														*/
/***********************************************************************************************************************************/

FRC2019_Robot::Robot_Claw::Robot_Claw(FRC2019_Robot *parent,Rotary_Control_Interface *robot_control) :
	Rotary_Velocity_Control("Claw",robot_control,eRoller),m_pParent(parent),m_Grip(false),m_Squirt(false)
{
}

void FRC2019_Robot::Robot_Claw::TimeChange(double dTime_s)
{
	const double Accel=m_Ship_1D_Props.ACCEL;
	const double Brake=m_Ship_1D_Props.BRAKE;

	//Get in my button values now use xor to only set if one or the other is true (not setting automatically zero's out)
	if (m_Grip ^ m_Squirt)
		SetCurrentLinearAcceleration(m_Grip?Accel:-Brake);

	__super::TimeChange(dTime_s);
}

void FRC2019_Robot::Robot_Claw::CloseClaw(bool Close)
{
	m_pParent->m_RobotControl->CloseSolenoid(eClaw,Close);
}

void FRC2019_Robot::Robot_Claw::Grip(bool on)
{
	m_Grip=on;
}

void FRC2019_Robot::Robot_Claw::Squirt(bool on)
{
	m_Squirt=on;
}

void FRC2019_Robot::Robot_Claw::BindAdditionalEventControls(bool Bind)
{
	Base::EventMap *em=GetEventMap(); //grrr had to explicitly specify which EventMap
	if (Bind)
	{
		em->EventValue_Map["Claw_SetCurrentVelocity"].Subscribe(ehl,*this, &FRC2019_Robot::Robot_Claw::SetRequestedVelocity_FromNormalized);
		em->EventOnOff_Map["Claw_Close"].Subscribe(ehl, *this, &FRC2019_Robot::Robot_Claw::CloseClaw);
		em->EventOnOff_Map["Claw_Grip"].Subscribe(ehl, *this, &FRC2019_Robot::Robot_Claw::Grip);
		em->EventOnOff_Map["Claw_Squirt"].Subscribe(ehl, *this, &FRC2019_Robot::Robot_Claw::Squirt);
	}
	else
	{
		em->EventValue_Map["Claw_SetCurrentVelocity"].Remove(*this, &FRC2019_Robot::Robot_Claw::SetRequestedVelocity_FromNormalized);
		em->EventOnOff_Map["Claw_Close"]  .Remove(*this, &FRC2019_Robot::Robot_Claw::CloseClaw);
		em->EventOnOff_Map["Claw_Grip"]  .Remove(*this, &FRC2019_Robot::Robot_Claw::Grip);
		em->EventOnOff_Map["Claw_Squirt"]  .Remove(*this, &FRC2019_Robot::Robot_Claw::Squirt);
	}
}

  /***********************************************************************************************************************************/
 /*													FRC2019_Robot::Robot_Arm														*/
/***********************************************************************************************************************************/

FRC2019_Robot::Robot_Arm::Robot_Arm(FRC2019_Robot *parent,Rotary_Control_Interface *robot_control) : 
	Rotary_Position_Control("Arm",robot_control,eArm),m_pParent(parent),m_Advance(false),m_Retract(false)
{
}

void FRC2019_Robot::Robot_Arm::TimeChange(double dTime_s)
{
	const double Accel=m_Ship_1D_Props.ACCEL;
	const double Brake=m_Ship_1D_Props.BRAKE;

	//Get in my button values now use xor to only set if one or the other is true (not setting automatically zero's out)
	if (m_Advance ^ m_Retract)
		SetCurrentLinearAcceleration(m_Advance?Accel:-Brake);

	__super::TimeChange(dTime_s);
	#if 0
	#ifdef __DebugLUA__
	Dout(m_pParent->m_RobotProps.GetIntakeDeploymentProps().GetRotaryProps().Feedback_DiplayRow,7,"p%.1f",RAD_2_DEG(GetPos_m()));
	#endif
	#endif
	#ifdef Robot_TesterCode
	const FRC2019_Robot_Props &props=m_pParent->GetRobotProps().GetFRC2019RobotProps();
	const double c_GearToArmRatio=1.0/props.ArmToGearRatio;
	double Pos_m=GetPos_m();
	double height=AngleToHeight_m(Pos_m);
	DOUT4("Arm=%f Angle=%f %fft %fin",m_Physics.GetVelocity(),RAD_2_DEG(Pos_m*c_GearToArmRatio),height*3.2808399,height*39.3700787);
	#endif
	}

double FRC2019_Robot::Robot_Arm::GetPosRest()
{
	return 0.0;
}
void FRC2019_Robot::Robot_Arm::CloseRist(bool Close)
{
	m_pParent->m_RobotControl->CloseSolenoid(eRist,Close);
}

void FRC2019_Robot::Robot_Arm::SetRequestedVelocity_FromNormalized(double Velocity)
{ 
	//An example of tracing variables when something isn't working
	//SmartDashboard::PutNumber("TestArmPot", Velocity);
	__super::SetRequestedVelocity_FromNormalized(Velocity); 
}

//Declare event names for arm
const char *csz_Arm_SetPosRest = "Arm_SetPosRest";
const char *csz_Arm_SetPosHatch = "Arm_SetPoshatch";


void FRC2019_Robot::Robot_Arm::BindAdditionalEventControls(bool Bind)
{
	Base::EventMap *em=GetEventMap(); //grrr had to explicitly specify which EventMap

	if (Bind)
	{
		em->EventValue_Map["Arm_SetCurrentVelocity"].Subscribe(ehl,*this, &FRC2019_Robot::Robot_Arm::SetRequestedVelocity_FromNormalized);
		em->EventOnOff_Map["Arm_SetPotentiometerSafety"].Subscribe(ehl,*this, &FRC2019_Robot::Robot_Arm::SetPotentiometerSafety);

		em->Event_Map[csz_Arm_SetPosRest].Subscribe([&]()
			{	SetIntendedPosition(1.0);  //inches from ground
			});
		em->Event_Map[csz_Arm_SetPosHatch].Subscribe([&]()
			{	SetIntendedPosition(20.0);  //TODO pull from lua, as we need to calibrate on the field
			});
		
		em->EventOnOff_Map["Arm_Advance"].Subscribe([&](bool on)
			{	m_Advance = on;
			});
		em->EventOnOff_Map["Arm_Retract"].Subscribe([&](bool on)
			{	m_Retract = on;
			});

		em->EventOnOff_Map["Arm_Rist"].Subscribe(ehl, *this, &FRC2019_Robot::Robot_Arm::CloseRist);
	}
	else
	{
		//Note: I'm not going to worry about removing V2 events because these will auto cleanup
		em->EventValue_Map["Arm_SetCurrentVelocity"].Remove(*this, &FRC2019_Robot::Robot_Arm::SetRequestedVelocity_FromNormalized);
		em->EventOnOff_Map["Arm_SetPotentiometerSafety"].Remove(*this, &FRC2019_Robot::Robot_Arm::SetPotentiometerSafety);

		em->EventOnOff_Map["Arm_Rist"]  .Remove(*this, &FRC2019_Robot::Robot_Arm::CloseRist);
	}
}

  /***********************************************************************************************************************************/
 /*															FRC2019_Robot															*/
/***********************************************************************************************************************************/
FRC2019_Robot::FRC2019_Robot(const char EntityName[],FRC2019_Control_Interface *robot_control,bool UseEncoders) : 
	Tank_Robot(EntityName,robot_control,UseEncoders), m_RobotControl(robot_control), m_Arm(this,robot_control), m_Claw(this,robot_control)
{
}

void FRC2019_Robot::Initialize(Entity2D_Kind::EventMap& em, const Entity_Properties *props)
{
	__super::Initialize(em,props);
	//TODO construct Arm-Ship1D properties from FRC 2011 Robot properties and pass this into the robot control and arm
	m_RobotControl->Initialize(props);

	const FRC2019_Robot_Properties *RobotProps=dynamic_cast<const FRC2019_Robot_Properties *>(props);
	m_RobotProps=*RobotProps;  //Copy all the properties (we'll need them for high and low gearing)

	m_Arm.Initialize(em,RobotProps?&RobotProps->GetArmProps():NULL);
	m_Claw.Initialize(em,RobotProps?&RobotProps->GetClawProps():NULL);
}
void FRC2019_Robot::ResetPos()
{
	__super::ResetPos();
	m_Arm.ResetPos();
	m_Claw.ResetPos();
}

const FRC2019_Robot_Properties &FRC2019_Robot::GetRobotProps() const
{
	return m_RobotProps;
}

FRC2019_Robot_Props::Autonomous_Properties &FRC2019_Robot::GetAutonProps()
{
	return m_RobotProps.GetFRC2019RobotProps_rw().Autonomous_Props;
}

void FRC2019_Robot::TimeChange(double dTime_s)
{
	//monitor the AutonTest CheckBox
	if (m_SmartDashboard_AutonTest_Valve)
	{
		//check if it's turned off now
		if (!SmartDashboard::GetBoolean("Test_Auton"))
		{
			//Fire as event so that other people can listen to this event
			//I do not care about the freeze arm 
			//We have other check boxes now, and each goal can control how it wants to work without needing to modify robot code
			GetEventMap()->EventOnOff_Map["StopAuton"].Fire(false);
		}
	}
	else
	{
		//check if it is now on
		if (SmartDashboard::GetBoolean("Test_Auton"))
			TestAutonomous();
	}

	//For the simulated code this must be first so the simulators can have the correct times
	m_RobotControl->Robot_Control_TimeChange(dTime_s);
	__super::TimeChange(dTime_s);
	Entity1D &arm_entity=m_Arm;  //This gets around keeping time change protected in derived classes
	arm_entity.TimeChange(dTime_s);
	Entity1D &claw_entity=m_Claw;  //This gets around keeping time change protected in derived classes
	claw_entity.TimeChange(dTime_s);
}

void FRC2019_Robot::CloseDeploymentDoor(bool Close)
{
	m_RobotControl->CloseSolenoid(eDeployment,Close);
}

void FRC2019_Robot::BindAdditionalEventControls(bool Bind)
{
	Entity2D_Kind::EventMap *em=GetEventMap(); //grrr had to explicitly specify which EventMap
	if (Bind)
	{
		em->EventOnOff_Map["Robot_CloseDoor"].Subscribe(ehl, *this, &FRC2019_Robot::CloseDeploymentDoor);
		//no longer restricted to simulation
		em->Event_Map["TestAuton"].Subscribe(ehl, *this, &FRC2019_Robot::TestAutonomous);
		em->Event_Map["Complete"].Subscribe(ehl,*this,&FRC2019_Robot::GoalComplete);
		em->EventOnOff_Map["StopAuton"].Subscribe(ehl, *this, &FRC2019_Robot::StopAuton);
	}
	else
	{
		em->EventOnOff_Map["Robot_CloseDoor"]  .Remove(*this, &FRC2019_Robot::CloseDeploymentDoor);
		//no longer restricted to simulation
		em->Event_Map["TestAuton"]  .Remove(*this, &FRC2019_Robot::TestAutonomous);
		em->Event_Map["Complete"]  .Remove(*this, &FRC2019_Robot::GoalComplete);
		em->EventOnOff_Map["StopAuton"].Remove(*this, &FRC2019_Robot::StopAuton);
	}

	Ship_1D &ArmShip_Access=m_Arm;
	ArmShip_Access.BindAdditionalEventControls(Bind);
	Ship_1D &ClawShip_Access=m_Claw;
	ClawShip_Access.BindAdditionalEventControls(Bind);
	__super::BindAdditionalEventControls(Bind);
}

void FRC2019_Robot::BindAdditionalUIControls(bool Bind,void *joy,void *key)
{
	m_RobotProps.Get_RobotControls().BindAdditionalUIControls(Bind,joy,key);
	__super::BindAdditionalUIControls(Bind,joy,key);  //call super for more general control assignments
}

void FRC2019_Robot::StopAuton(bool isOn)
{
	m_SmartDashboard_AutonTest_Valve = false;
	SmartDashboard::PutBoolean("Test_Auton", false);
	//FreezeArm(isOn);
	m_controller->GetUIController_RW()->SetAutoPilot(false);
	GetEventMap()->Event_Map["StopAutonAbort"].Fire();
	ClearGoal();
	//LockPosition(false);
}

Goal *TestAutonDefaultGoalCallback(FRC2019_Robot *Robot)
{ 
	return FRC2019_Goals::Get_FRC2019_Autonomous(Robot);
}

//No longer are these restricted to simulation
void FRC2019_Robot::TestAutonomous()
{
	m_SmartDashboard_AutonTest_Valve = true;
	SmartDashboard::PutBoolean("Test_Auton", true);
	Goal *oldgoal = ClearGoal();
	if (oldgoal)
		delete oldgoal;

	{
		Goal *goal = NULL;
		//Note: we may change how this gets implemented
		#if 0
		goal = FRC2019_Goals::Get_FRC2019_Autonomous(this);
		#else
		if (!m_TestAutonGoalCallback)
			m_TestAutonGoalCallback = TestAutonDefaultGoalCallback;
		//goal = FRC2019_Goals::Get_Sample_Goal(this);
		goal = m_TestAutonGoalCallback(this);
		#endif
		if (goal)
			goal->Activate(); //now with the goal(s) loaded activate it
		SetGoal(goal);
		//enable autopilot (note windriver does this in main)
		m_controller->GetUIController_RW()->SetAutoPilot(true);
	}
}

void FRC2019_Robot::GoalComplete()
{
	printf("Goals completed!\n");
	m_controller->GetUIController_RW()->SetAutoPilot(false);
}

  /***********************************************************************************************************************************/
 /*													FRC2019_Robot_Properties														*/
/***********************************************************************************************************************************/

FRC2019_Robot_Properties::FRC2019_Robot_Properties() : m_RobotControls(&s_ControlsEvents)
{
	const double c_OptimalAngleUp_r=DEG_2_RAD(70.0);
	const double c_OptimalAngleDn_r=DEG_2_RAD(50.0);
	const double c_ArmLength_m=1.8288;  //6 feet
	const double c_ArmToGearRatio=72.0/28.0;
	//const double c_GearToArmRatio=1.0/c_ArmToGearRatio;
	//const double c_PotentiometerToGearRatio=60.0/32.0;
	//const double c_PotentiometerToArmRatio=c_PotentiometerToGearRatio * c_GearToArmRatio;
	const double c_PotentiometerToArmRatio=36.0/54.0;
	//const double c_PotentiometerToGearRatio=c_PotentiometerToArmRatio * c_ArmToGearRatio;
	const double c_PotentiometerMaxRotation=DEG_2_RAD(270.0);
	const double c_GearHeightOffset=1.397;  //55 inches
	const double c_WheelDiameter=0.1524;  //6 inches
	const double c_MotorToWheelGearRatio=12.0/36.0;

	{
		Tank_Robot_Props props=m_TankRobotProps; //start with super class settings
		//Late assign this to override the initial default
		props.WheelDimensions=Vec2D(0.4953,0.6985); //27.5 x 19.5 where length is in 5 inches in, and width is 3 on each side
		props.WheelDiameter=c_WheelDiameter;
		props.LeftPID[1]=props.RightPID[1]=1.0; //set the I's to one... so it should be 1,1,0
		props.MotorToWheelGearRatio=c_MotorToWheelGearRatio;
		m_TankRobotProps=props;
	}

	FRC2019_Robot_Props props;
	props.OptimalAngleUp=c_OptimalAngleUp_r;
	props.OptimalAngleDn=c_OptimalAngleDn_r;
	props.ArmLength=c_ArmLength_m;
	props.ArmToGearRatio=c_ArmToGearRatio;
	props.PotentiometerToArmRatio=c_PotentiometerToArmRatio;
	props.PotentiometerMaxRotation=c_PotentiometerMaxRotation;
	props.GearHeightOffset=c_GearHeightOffset;
	props.MotorToWheelGearRatio=c_MotorToWheelGearRatio;
	m_FRC2019RobotProps=props;
}

//declared as global to avoid allocation on stack each iteration
const char * const g_FRC2019_Controls_Events[] = 
{
	"Claw_SetCurrentVelocity","Claw_Close",
	"Claw_Grip","Claw_Squirt",
	"Arm_SetCurrentVelocity","Arm_SetPotentiometerSafety",
	csz_Arm_SetPosRest,csz_Arm_SetPosHatch,
	"Arm_Rist","Arm_Advance","Arm_Retract",
	"Robot_CloseDoor",
	"TestAuton","StopAuton"
};

const char *FRC2019_Robot_Properties::ControlEvents::LUA_Controls_GetEvents(size_t index) const
{
	return (index<_countof(g_FRC2019_Controls_Events))?g_FRC2019_Controls_Events[index] : NULL;
}
FRC2019_Robot_Properties::ControlEvents FRC2019_Robot_Properties::s_ControlsEvents;

void FRC2019_Robot_Properties::LoadFromScript(Scripting::Script& script)
{
	const char* err=NULL;
	{
		double version;
		err=script.GetField("version", NULL, NULL, &version);
		if (!err)
			printf ("Version=%.2f\n",version);
	}
	m_ControlAssignmentProps.LoadFromScript(script);
	__super::LoadFromScript(script);
	err = script.GetFieldTable("robot_settings");
	if (!err) 
	{
		err = script.GetFieldTable("arm");
		if (!err)
		{
			m_ArmProps.LoadFromScript(script);
			script.Pop();
		}
		err = script.GetFieldTable("claw");
		if (!err)
		{
			m_ClawProps.LoadFromScript(script);
			script.Pop();
		}

		//This is the main robot settings pop
		script.Pop();
	}
	err = script.GetFieldTable("controls");
	if (!err)
	{
		m_RobotControls.LoadFromScript(script);
		script.Pop();
	}
}


//TODO implement once tank is enabled
#if 1
  /***********************************************************************************************************************************/
 /*													FRC2019_Robot_Control															*/
/***********************************************************************************************************************************/

void FRC2019_Robot_Control::ResetPos()
{
	if (m_Compressor)
	{
		//Enable this code if we have a compressor 
		m_Compressor->Stop();
		printf("RobotControl::ResetPos Compressor->Stop()\n");
		{
			printf("RobotControl::ResetPos Compressor->Start()\n");
			m_Compressor->Start();
		}
	}
}

__inline void CheckDisableSafety_FRC2019(size_t index, bool &SafetyLock)
{
	//return;
	std::string SmartLabel = csz_FRC2019_Robot_SpeedControllerDevices_Enum[index];
	SmartLabel[0] -= 32; //Make first letter uppercase
	//This section is extra control of each system while 3D positioning is operational... enable for diagnostics
	std::string VoltageArmSafety = SmartLabel + "Disable";
	const bool bVoltageArmDisable = SmartDashboard::GetBoolean(VoltageArmSafety.c_str());
	if (bVoltageArmDisable)
		SafetyLock = true;
}

void FRC2019_Robot_Control::UpdateVoltage(size_t index,double Voltage)
{
	bool SafetyLock=SmartDashboard::GetBoolean("SafetyLock_Arm");
	double VoltageScalar=1.0;

	switch (index)
	{
	case FRC2019_Robot::eArm:
		{
			#ifdef __EnableRobotArmDisable__
			CheckDisableSafety_FRC2019(index,SafetyLock);
			#endif
			#ifdef _Win32
			//Note: put index back in here if we have multiple pots
			m_Potentiometer.UpdatePotentiometerVoltage(SafetyLock?0.0:Voltage);
			m_Potentiometer.TimeChange();  //have this velocity immediately take effect
			#endif
		}
		break;
	}
	//Note this check will be needed if we follow the swerve drive model
	//if (index<FRC2019_Robot::eDriveOffset)
	{
		VoltageScalar=m_RobotProps.GetArmProps().GetRotaryProps().VoltageScalar;
		Voltage*=VoltageScalar;
		std::string SmartLabel=csz_FRC2019_Robot_SpeedControllerDevices_Enum[index];
		SmartLabel[0]-=32; //Make first letter uppercase
		SmartLabel+="Voltage";
		SmartDashboard::PutNumber(SmartLabel.c_str(),Voltage);
		if (SafetyLock)
			Voltage=0.0;
		PWMSpeedController_UpdateVoltage(index,Voltage);
	}
}

void FRC2019_Robot_Control::CloseSolenoid(size_t index,bool Close)
{
	switch (index)
	{
		case FRC2019_Robot::eDeployment:
			//DebugOutput("CloseDeploymentDoor=%d\n",Close);
			m_Deployment=Close;
			SmartDashboard::PutBoolean("Deployment",m_Deployment);
			break;
		case FRC2019_Robot::eClaw:
			//DebugOutput("CloseClaw=%d\n",Close);
			m_Claw=Close;
			SmartDashboard::PutBoolean("Claw",m_Claw);
			//This was used to test error with the potentiometer
			//m_Potentiometer.SetBypass(Close);
			break;
		case FRC2019_Robot::eRist:
			//DebugOutput("CloseRist=%d\n",Close);
			m_Rist=Close;
			SmartDashboard::PutBoolean("Wrist",m_Rist);
			break;
	}
}


FRC2019_Robot_Control::FRC2019_Robot_Control(bool UseSafety) : m_pTankRobotControl(&m_TankRobotControl),
	m_Deployment(false),m_Claw(false),m_Rist(false)
{
	//depreciated once we had smart dashboard
	//m_TankRobotControl.SetDisplayVoltage(false); //disable display there so we can do it here
}

FRC2019_Robot_Control::~FRC2019_Robot_Control()
{
	//Encoder_Stop(Curivator_Robot::eWinch);
	if (m_Compressor)
	{
		DestroyCompressor(m_Compressor);
		m_Compressor = nullptr;
	}
	m_FirstRun = false;
	//DestroyBuiltInAccelerometer(m_RoboRIO_Accelerometer);
	//m_RoboRIO_Accelerometer = NULL;
}

void FRC2019_Robot_Control::Reset_Rotary(size_t index)
{
	switch (index)
	{
		case FRC2019_Robot::eArm:
			m_KalFilter_Arm.Reset();
			#ifdef _Win32
			m_Potentiometer.ResetPos();
			#endif
			break;
	}
}

void FRC2019_Robot_Control::Initialize(const Entity_Properties *props)
{
	Tank_Drive_Control_Interface *tank_interface = m_pTankRobotControl;
	tank_interface->Initialize(props);

	const FRC2019_Robot_Properties *robot_props=dynamic_cast<const FRC2019_Robot_Properties *>(props);

	//For now robot_props can be NULL since the swerve robot is borrowing it
	if (robot_props)
	{
		#ifdef _Win32
		m_RobotProps=*robot_props;  //save a copy
		assert(robot_props);
		Rotary_Properties writeable_arm_props=robot_props->GetArmProps();
		m_ArmMaxSpeed=writeable_arm_props.GetMaxSpeed();
		//This is not perfect but will work for our simulation purposes
		writeable_arm_props.RotaryProps().EncoderToRS_Ratio=robot_props->GetFRC2019RobotProps().ArmToGearRatio;
		m_Potentiometer.Initialize(&writeable_arm_props);
		#endif
	}
	//Note: Initialize may be called multiple times so we'll only set this stuff up on first run
	if (!m_FirstRun)
	{
		m_FirstRun = true;
		//For now don't manage this here
		//SmartDashboard::PutBoolean("Test_Auton", false);
		//Ensure we have this
		SmartDashboard::PutBoolean("SafetyLock_Arm",false);

		//This one one must also be called for the lists that are specific to the robot
		RobotControlCommon_Initialize(robot_props->Get_ControlAssignmentProps());
		//This may return NULL for systems that do not support it
		//m_RoboRIO_Accelerometer = CreateBuiltInAccelerometer();
		m_Compressor = CreateCompressor();
		ResetPos(); //must be called after compressor is created
		#ifdef __EnableRobotArmDisable__
		//TODO change to reflect all pots that need safety
		for (size_t i=0;i<1;i++)
		{
			const char * const Prefix=csz_FRC2019_Robot_SpeedControllerDevices_Enum[i];
			string ContructedName;
			ContructedName=Prefix;
			ContructedName[0]-=32; //Make first letter uppercase
			ContructedName+="Disable";
			#ifdef Robot_TesterCode
			const bool DisableDefault=false;
			#else
			const bool DisableDefault=true;
			#endif
			SmartDashboard::PutBoolean(ContructedName.c_str(),DisableDefault);
		}
		#endif
	}
}

void FRC2019_Robot_Control::Robot_Control_TimeChange(double dTime_s)
{
	#ifdef _Win32
	m_Potentiometer.SetTimeDelta(dTime_s);
	#endif
	//depreciated once we had smart dashboard
	//display voltages
	//DOUT2("l=%f r=%f a=%f r=%f D%dC%dR%d\n",m_TankRobotControl.GetLeftVoltage(),m_TankRobotControl.GetRightVoltage(),m_ArmVoltage,m_RollerVoltage,
	//	m_Deployment,m_Claw,m_Rist
	//	);
	//These are no longer placed here
	//SmartDashboard::PutNumber("ArmVoltage",m_ArmVoltage);
	//SmartDashboard::PutNumber("RollerVoltage",m_RollerVoltage);
}

//void Robot_Control::UpdateVoltage(size_t index,double Voltage)
//{
//}

double FRC2019_Robot_Control::GetRotaryCurrentPorV(size_t index)
{
	double result=0.0;

	switch (index)
	{
		case FRC2019_Robot::eArm:
		{
			#ifndef _Win32
			result = 0.0;
			//TODO port from curivator
			#else
			result=(m_Potentiometer.GetPotentiometerCurrentPosition()) + 0.0;
			//Now to normalize it
			const Ship_1D_Props &shipprops = m_RobotProps.GetArmProps().GetShip_1D_Props();
			const double NormalizedResult = (result - shipprops.MinRange) / (shipprops.MaxRange - shipprops.MinRange);
			const char * const Prefix = csz_FRC2019_Robot_SpeedControllerDevices_Enum[index];
			std::string ContructedName;
			ContructedName = Prefix, ContructedName += "_Raw";
			SmartDashboard::PutNumber(ContructedName.c_str(), result);  //this one is a bit different as it is the selected units we use
			ContructedName = Prefix, ContructedName += "Pot_Raw";
			SmartDashboard::PutNumber(ContructedName.c_str(), NormalizedResult);
			#endif
		}
		break;
	}
	return result;
}
#endif