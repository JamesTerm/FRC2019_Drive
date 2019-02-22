
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
#include "TankDrive/Tank_Robot2.h"
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
		em->EventOnOff_Map["Claw_Grip"].Subscribe(ehl, *this, &FRC2019_Robot::Robot_Claw::Grip);
		em->EventOnOff_Map["Claw_Squirt"].Subscribe(ehl, *this, &FRC2019_Robot::Robot_Claw::Squirt);
	}
	else
	{
		em->EventValue_Map["Claw_SetCurrentVelocity"].Remove(*this, &FRC2019_Robot::Robot_Claw::SetRequestedVelocity_FromNormalized);
		em->EventOnOff_Map["Claw_Grip"]  .Remove(*this, &FRC2019_Robot::Robot_Claw::Grip);
		em->EventOnOff_Map["Claw_Squirt"]  .Remove(*this, &FRC2019_Robot::Robot_Claw::Squirt);
	}
}

  /***********************************************************************************************************************************/
 /*													FRC2019_Robot::Robot_Arm_Manager												*/
/***********************************************************************************************************************************/

//#define __UseArmManual__
const char *csz_Arm_IntakeDeploy_manual = "Arm_IntakeDeploy_manual";
const char *csz_Arm_HatchDeploy_manual = "Arm_HatchDeploy_manual";
const char *csz_Arm_HatchGrabDeploy_manual = "Arm_HatchGrabDeploy_manual";

class FRC2019_Robot::Robot_Arm_Manager
{
private:
	Robot_Arm *m_pParent;
	enum GoalList
	{
		eIntake,
		eHatch,
		eHatchGrab,
		eNoGoals
	};
	bool m_GoalActive[eNoGoals];
	bool m_IsIntakeDeployed=false;
	bool m_IsHatchDeployed=false;
	bool m_IsHatchGrabExpanded = false;
	//This is a short scope cache from activate to process... to monitor the desired change
	bool m_IntendedIntakeDeployed = false;
	bool m_IntendedHatchDeployed = false;
	bool m_IntendedHatchGrabExpanded = false;
	//Goals--- literally able to copy these as-is

	FRC2019_Robot &m_Robot;
	class SetUpProps
	{
	protected:
		Robot_Arm_Manager *m_Parent;
		FRC2019_Robot &m_Robot;
		Entity2D_Kind::EventMap &m_EventMap;
		const FRC2019_Robot_Props &m_props;
	public:
		SetUpProps(Robot_Arm_Manager *Parent) : m_Parent(Parent), m_Robot(Parent->m_Robot), m_EventMap(*m_Robot.GetEventMap()), 
			m_props(m_Robot.GetRobotProps().GetFRC2019RobotProps())
		{
		}
	};
	//This makes it easy to notify by name
	class RobotQuickNotify : public AtomicGoal, public SetUpProps
	{
	private:
		std::string m_EventName;
		bool m_IsOn;
	public:
		RobotQuickNotify(Robot_Arm_Manager *Parent, const char *EventName, bool On) : SetUpProps(Parent), m_EventName(EventName), m_IsOn(On)
		{
			m_Status = eInactive;
		}
		virtual void Activate() { m_Status = eActive; }
		virtual Goal_Status Process(double dTime_s)
		{
			ActivateIfInactive();
			m_EventMap.EventOnOff_Map[m_EventName.c_str()].Fire(m_IsOn);
			m_Status = eCompleted;
			return m_Status;
		}
	};

	//#define __UseSimpleHatchGrip__

	class GoalIntake : public Generic_CompositeGoal, public SetUpProps
	{
	public:
		GoalIntake(Robot_Arm_Manager *Parent) : Generic_CompositeGoal(false), SetUpProps(Parent) 
			{ 	m_Status = eInactive; 
			}
		virtual void Activate(bool IsStowed)
		{
			//Check to see if we are already active... then check the direction... if we are in the same direction then its merely flood control
			//otherwise we have to terminate and refresh with these new goals going in the other direction
			if (m_Status == eActive)
			{
				if (m_Parent->m_IsIntakeDeployed == !IsStowed)
				{
					m_Status = eInactive;  //mark inactive... because the work is already done
					m_Parent->m_GoalActive[eIntake] = false;
					return;  //no work to do
				}
				else
					Terminate();
			}
			m_Parent->m_IntendedIntakeDeployed = !IsStowed;
			//for the automated hatch grip... any other input will override it
			#ifndef __UseSimpleHatchGrip__
			if (m_Parent->m_GoalActive[eHatchGrab])
				m_Parent->m_GoalHatchGrip.Terminate();
			#endif
			//Keep this around for test purposes
			//m_Parent->mutual_exlude_other_goals(eIntake);
			AddSubgoal(new Goal_Wait(m_props.cargo_deploy_time));
			AddSubgoal(new RobotQuickNotify(m_Parent, csz_Arm_IntakeDeploy_manual, !IsStowed));
			m_Status = eActive;
			m_Parent->m_GoalActive[eIntake] = true;
		}
		virtual Goal_Status Process(double dTime_s)
		{
			if (m_Status == eActive)
			{
				//so the logic here... we can always stow, but to deploy the hatch must be stowed
				bool can_process = true;
				if (m_Parent->m_IntendedIntakeDeployed)
				{
					if ((m_Parent->m_IsHatchDeployed) || (m_Parent->m_GoalActive[eHatch]))
						can_process = false;
				}
				if (can_process)
				{
					m_Status = Generic_CompositeGoal::Process(dTime_s);
					m_Parent->m_GoalActive[eIntake] = m_Status == eActive;
				}
			}
			return m_Status;
		}
		virtual void Terminate()
		{
			m_Parent->m_GoalActive[eIntake] = false;
			//since we are interrupted ensure the cached vars reflect the current state
			//m_Parent->m_IsIntakeDeployed=m_Robot.m_RobotControl->GetIsSolenoidClosed(eIntakeDeploy);
			Generic_CompositeGoal::Terminate();
		}
	};

	class GoalHatch : public Generic_CompositeGoal, public SetUpProps
	{
	public:
		GoalHatch(Robot_Arm_Manager *Parent) : Generic_CompositeGoal(false), SetUpProps(Parent) 
			{	m_Status = eInactive; 
			}
		virtual void Activate(bool IsStowed)
		{
			//Check to see if we are already active... then check the direction... if we are in the same direction then its merely flood control
			//otherwise we have to terminate and refresh with these new goals going in the other direction
			if (m_Status == eActive)
			{
				if (m_Parent->m_IsHatchDeployed == !IsStowed)
				{
					m_Status = eInactive;  //mark inactive... because the work is already done
					m_Parent->m_GoalActive[eHatch] = false;
					return;  //no work to do
				}
				else
					Terminate();
			}
			m_Parent->m_IntendedHatchDeployed = !IsStowed;
			//for the automated hatch grip... any other input will override it
			#ifndef __UseSimpleHatchGrip__
			if (m_Parent->m_GoalActive[eHatchGrab])
				m_Parent->m_GoalHatchGrip.Terminate();
			#endif
			//Keep this around for test purposes
			//m_Parent->mutual_exlude_other_goals(eHatch);
			AddSubgoal(new Goal_Wait(m_props.hatch_deploy_time));
			AddSubgoal(new RobotQuickNotify(m_Parent, csz_Arm_HatchDeploy_manual, !IsStowed));
			m_Status = eActive;
			m_Parent->m_GoalActive[eHatch] = true;
		}
		virtual Goal_Status Process(double dTime_s)
		{
			if (m_Status == eActive)
			{
				//so the logic here... we can always stow, but to deploy the intake must be stowed
				bool can_process = true;
				if (m_Parent->m_IntendedHatchDeployed)
				{
					if ((m_Parent->m_IsIntakeDeployed) || (m_Parent->m_GoalActive[eIntake]))
						can_process = false;
				}
				if (can_process)
				{
					m_Status = Generic_CompositeGoal::Process(dTime_s);
					m_Parent->m_GoalActive[eHatch] = m_Status == eActive;
				}
			}
			return m_Status;
		}
		virtual void Terminate()
		{
			m_Parent->m_GoalActive[eHatch] = false;
			//since we are interrupted ensure the cached vars reflect the current state
			//m_Parent->m_IsHatchDeployed = m_Robot.m_RobotControl->GetIsSolenoidClosed(eHatchDeploy);
			Generic_CompositeGoal::Terminate();
		}
	};

	class GoalHatchGrip : public Generic_CompositeGoal, public SetUpProps
	{
	private:
		bool m_LastDeployStowed = false; //Cache during the deploy session if the hatch was stowed as we retain the last state when we stow the grabber
	public:
		GoalHatchGrip(Robot_Arm_Manager *Parent) : Generic_CompositeGoal(false), SetUpProps(Parent)
		{
			m_Status = eInactive;
		}
		virtual void Activate(bool IsStowed)
		{
			//Check to see if we are already active... then check the direction... if we are in the same direction then its merely flood control
			//otherwise we have to terminate and refresh with these new goals going in the other direction
			if (m_Status == eActive)
			{
				//See if we need to add new goals... 
				//First see if the current state is the requested state
				//Next see if we are deployed... that implies everything is set
				//finally see if last session was deployed on the hatch
				if ((m_Parent->m_IsHatchGrabExpanded == !IsStowed)&&((!IsStowed)||!m_LastDeployStowed))
				{
					m_Status = eInactive;  //mark inactive... because the work is already done
					m_Parent->m_GoalActive[eHatchGrab] = false;
					return;  //no work to do
				}
				else
				{
					//See if we are terminating to stow the expansion, and then if this session didn't have the hatch deployed
					if ((IsStowed)&&(m_LastDeployStowed))
						m_EventMap.EventOnOff_Map[csz_Arm_HatchDeploy_manual].Fire(false);  //stow it as it was before now, before termination
					Terminate();
				}
			}

			m_Parent->m_IntendedHatchGrabExpanded = !IsStowed;
			const double HatchDeployTime = m_props.hatch_deploy_time;

			//We'll override the other goals if using the automated version, note it's easier to trace code if we handle this before adding any new goals
			#ifndef __UseSimpleHatchGrip__
			m_Parent->mutual_exlude_other_goals(eHatchGrab);
			#endif

			#ifdef __UseSimpleHatchGrip__
			//Go ahead add these goals... but we may add more keep reading
			AddSubgoal(new Goal_Wait(m_props.hatch_grab_deploy_time));
			AddSubgoal(new RobotQuickNotify(m_Parent, csz_Arm_HatchGrabDeploy_manual, !IsStowed));
			#else	
			if (!IsStowed)
			{
				m_LastDeployStowed = !m_Parent->m_IsHatchDeployed; //cache this... this will be the state we return when stowing the grabber

				//Go ahead add these goals... but we may add more keep reading
				AddSubgoal(new Goal_Wait(m_props.hatch_grab_deploy_time));
				AddSubgoal(new RobotQuickNotify(m_Parent, csz_Arm_HatchGrabDeploy_manual, !IsStowed));

				//This one is more automated it will add to stow the intake and deploy the hatch as needed
				//This is in reverse so we start with the hatch being deployed
				if (!m_Parent->m_IsHatchDeployed)
				{
					//rather than activate the other goals (messy and could cause recursion)-- just add the goals here
					AddSubgoal(new Goal_Wait(HatchDeployTime));
					AddSubgoal(new RobotQuickNotify(m_Parent, csz_Arm_HatchDeploy_manual, true));
					m_Parent->m_IsHatchDeployed = true;
				}
				//Now to see about the intake... it must be stowed
				if (m_Parent->m_IsIntakeDeployed)
				{
					AddSubgoal(new Goal_Wait(m_props.cargo_deploy_time));
					AddSubgoal(new RobotQuickNotify(m_Parent, csz_Arm_IntakeDeploy_manual, false));
					m_Parent->m_IsIntakeDeployed = false;
				}
			}
			else
			{
				//for stowing... let's stow the hatch as well
				if ((m_Parent->m_IsHatchDeployed)&&(m_LastDeployStowed))
				{
					AddSubgoal(new Goal_Wait(HatchDeployTime));
					AddSubgoal(new RobotQuickNotify(m_Parent, csz_Arm_HatchDeploy_manual, false));
					m_Parent->m_IsHatchDeployed = false;
				}
				//Go ahead add these goals... but we may add more keep reading
				AddSubgoal(new Goal_Wait(m_props.hatch_grab_deploy_time));
				AddSubgoal(new RobotQuickNotify(m_Parent, csz_Arm_HatchGrabDeploy_manual, !IsStowed));
			}
			#endif
			//go ahead and activate
			m_Status = eActive;
			m_Parent->m_GoalActive[eHatchGrab] = true;
		}
		virtual Goal_Status Process(double dTime_s)
		{
			if (m_Status == eActive)
			{
				//This is the safe logic... it makes sure its safe to deploy just like hatch (it can deploy whether or not the hatch is)
				#ifdef __UseSimpleHatchGrip__
				//so the logic here... we can always stow, but to deploy the intake must be stowed
				bool can_process = true;
				if (m_Parent->m_IsHatchGrabExpanded)
				{
					if ((m_Parent->m_IsIntakeDeployed) || (m_Parent->m_GoalActive[eIntake]))
						can_process = false;
				}
				if (can_process)
				{
					m_Status = Generic_CompositeGoal::Process(dTime_s);
					m_Parent->m_GoalActive[eHatchGrab] = m_Status == eActive;
				}
				#else
				//The goals should protect any issues... so we can just process them
				m_Status = Generic_CompositeGoal::Process(dTime_s);
				m_Parent->m_GoalActive[eHatchGrab] = m_Status == eActive;
				#endif
			}
			return m_Status;
		}
		virtual void Terminate()
		{
			m_Parent->m_GoalActive[eHatchGrab] = false;
			#if 0
			//since we are interrupted ensure the cached vars reflect the current state
			//we've taken control of all of them so we'll need to update all of them
			m_Parent->m_IsHatchGrabExpanded = m_Robot.m_RobotControl->GetIsSolenoidClosed(eHatchGrabDeploy);
			m_Parent->m_IsHatchDeployed = m_Robot.m_RobotControl->GetIsSolenoidClosed(eHatchDeploy);
			m_Parent->m_IsIntakeDeployed = m_Robot.m_RobotControl->GetIsSolenoidClosed(eIntakeDeploy);
			#endif
			Generic_CompositeGoal::Terminate();
		}
	};

	GoalIntake m_GoalIntake=this;
	GoalHatch m_GoalHatch=this;
	GoalHatchGrip m_GoalHatchGrip = this;
	//There should never be a reason to terminate, but I'm keeping around in case we run into a situation where it becomes necessary
	void mutual_exlude_other_goals(GoalList active_goal)
	{
		for (size_t i=0;i< eNoGoals;i++)
		{
			if (i == active_goal) continue;
			else
			{
				switch (i)
				{
				case eIntake:
					m_GoalIntake.Terminate();
					break;
				case eHatch:
					m_GoalHatch.Terminate();
					break;
				}
			}
		}
	}

public: 
	Robot_Arm_Manager(Robot_Arm *parent) : m_pParent(parent),m_Robot(*m_pParent->m_pParent)
	{
		for (size_t i = 0; i < eNoGoals; i++)
			m_GoalActive[i] = false;
	}
	void TimeChange(double dTime_s)
	{
		m_GoalIntake.Process(dTime_s);
		m_GoalHatch.Process(dTime_s);
		m_GoalHatchGrip.Process(dTime_s);
	}
	void ResetPos()
	{
		//Update solenoids to use the accessor functions properly
		CloseIntake_manual(false);
		CloseHatchDeploy_manual(false);
		CloseHatchGrabDeploy_manual(false);
	}

	void CloseIntake_manual(bool Close)
	{
		m_pParent->m_pParent->m_RobotControl->CloseSolenoid(eIntakeDeploy, Close);
		m_IsIntakeDeployed = Close;
	}
	void CloseHatchDeploy_manual(bool Close)
	{
		m_pParent->m_pParent->m_RobotControl->CloseSolenoid(eHatchDeploy, Close);
		m_IsHatchDeployed = Close;
	}
	void CloseHatchGrabDeploy_manual(bool Close)
	{
		m_pParent->m_pParent->m_RobotControl->CloseSolenoid(eHatchGrabDeploy, Close);
		m_IsHatchGrabExpanded = Close;
	}
	void CloseIntake(bool Close)
	{
		#ifdef __UseArmManual__
		CloseIntake_manual(Close);
		#else
		m_GoalIntake.Activate(!Close);
		#endif
	}
	void CloseHatchDeploy(bool Close)
	{
		#ifdef __UseArmManual__
		CloseHatchDeploy_manual(Close);
		#else
		m_GoalHatch.Activate(!Close);
		#endif
	}
	void CloseHatchGrabDeploy(bool Close)
	{
		#ifdef __UseArmManual__
		CloseHatchGrabDeploy_manual(Close);
		#else
		m_GoalHatchGrip.Activate(!Close);
		#endif
	}
};

  /***********************************************************************************************************************************/
 /*													FRC2019_Robot::Robot_Arm														*/
/***********************************************************************************************************************************/

FRC2019_Robot::Robot_Arm::Robot_Arm(FRC2019_Robot *parent,Rotary_Control_Interface *robot_control) : 
	Rotary_Position_Control("Arm",robot_control,eArm),m_pParent(parent),m_Advance(false),m_Retract(false)
{
}

void FRC2019_Robot::Robot_Arm::Initialize(Base::EventMap& em, const Entity1D_Properties *props)
{
	__super::Initialize(em,props);
	//Hold off on creating this until now... now that we have an eventmap
	m_RobotArmManager = std::make_shared<FRC2019_Robot::Robot_Arm_Manager>(this);
}

void FRC2019_Robot::Robot_Arm::TimeChange(double dTime_s)
{
	const double Accel=m_Ship_1D_Props.ACCEL;
	const double Brake=m_Ship_1D_Props.BRAKE;

	//Get in my button values now use xor to only set if one or the other is true (not setting automatically zero's out)
	if (m_Advance ^ m_Retract)
		SetCurrentLinearAcceleration(m_Advance?Accel:-Brake);

	m_RobotArmManager->TimeChange(dTime_s);
	if (!m_pParent->m_RobotProps.GetFRC2019RobotProps().using_ac_elevator)
		__super::TimeChange(dTime_s);
	}

bool FRC2019_Robot::Robot_Arm::DidHitMinLimit() const
{
	return m_pParent->m_RobotControl->GetBoolSensorState(eElevatorMin);
}
bool FRC2019_Robot::Robot_Arm::DidHitMaxLimit() const
{
	return m_pParent->m_RobotControl->GetBoolSensorState(eElevatorMax);
}

void FRC2019_Robot::Robot_Arm::ResetPos()
{
	m_RobotArmManager->ResetPos();
	__super::ResetPos();
}

void FRC2019_Robot::Robot_Arm::SetRequestedVelocity_FromNormalized(double Velocity)
{ 
	//An example of tracing variables when something isn't working
	//SmartDashboard::PutNumber("TestArmPot", Velocity);
	__super::SetRequestedVelocity_FromNormalized(Velocity); 
}

//Declare event names for arm
const char *csz_Arm_SetPosRest = "Arm_SetPosRest";
const char *csz_Arm_SetPosCargo1 = "Arm_SetPosCargo1";
const char *csz_Arm_SetPosCargo2 = "Arm_SetPosCargo2";
const char *csz_Arm_SetPosCargo3 = "Arm_SetPosCargo3";
const char *csz_Arm_IntakeDeploy = "Arm_IntakeDeploy";
const char *csz_Arm_HatchDeploy = "Arm_HatchDeploy";
const char *csz_Arm_HatchGrabDeploy = "Arm_HatchGrabDeploy";

void FRC2019_Robot::Robot_Arm::BindAdditionalEventControls(bool Bind)
{
	Base::EventMap *em=GetEventMap(); //grrr had to explicitly specify which EventMap
	const FRC2019_Robot_Props &props = m_pParent->m_RobotProps.GetFRC2019RobotProps();

	if (Bind)
	{
		em->EventValue_Map["Arm_SetCurrentVelocity"].Subscribe(ehl,*this, &FRC2019_Robot::Robot_Arm::SetRequestedVelocity_FromNormalized);
		em->EventOnOff_Map["Arm_SetPotentiometerSafety"].Subscribe(ehl,*this, &FRC2019_Robot::Robot_Arm::SetPotentiometerSafety);

		em->Event_Map[csz_Arm_SetPosRest].Subscribe([&]()
			{	SetIntendedPosition(1.0);  //inches from ground
			});
		em->Event_Map[csz_Arm_SetPosCargo1].Subscribe([&]()
			{	SetIntendedPosition(props.cargo1_height);
			});
		em->Event_Map[csz_Arm_SetPosCargo2].Subscribe([&]()
		{	SetIntendedPosition(props.cargo2_height);
		});
		em->Event_Map[csz_Arm_SetPosCargo3].Subscribe([&]()
		{	SetIntendedPosition(props.cargo3_height);
		});

		em->EventOnOff_Map["Arm_Advance"].Subscribe([&](bool on)
			{	m_Advance = on;
			});
		em->EventOnOff_Map["Arm_Retract"].Subscribe([&](bool on)
			{	m_Retract = on;
			});

		em->EventOnOff_Map[csz_Arm_IntakeDeploy].Subscribe([&](bool on)
			{	m_RobotArmManager->CloseIntake(on);
			});
		em->EventOnOff_Map[csz_Arm_HatchDeploy].Subscribe([&](bool on)
			{	m_RobotArmManager->CloseHatchDeploy(on);
			});
		em->EventOnOff_Map[csz_Arm_HatchGrabDeploy].Subscribe([&](bool on)
			{	m_RobotArmManager->CloseHatchGrabDeploy(on);
			});
		em->EventOnOff_Map[csz_Arm_IntakeDeploy_manual].Subscribe([&](bool on)
		{	m_RobotArmManager->CloseIntake_manual(on);
		});
		em->EventOnOff_Map[csz_Arm_HatchDeploy_manual].Subscribe([&](bool on)
		{	m_RobotArmManager->CloseHatchDeploy_manual(on);
		});
		em->EventOnOff_Map[csz_Arm_HatchGrabDeploy_manual].Subscribe([&](bool on)
		{	m_RobotArmManager->CloseHatchGrabDeploy_manual(on);
		});
	}
	else
	{
		//Note: I'm not going to worry about removing V2 events because these will auto cleanup
		em->EventValue_Map["Arm_SetCurrentVelocity"].Remove(*this, &FRC2019_Robot::Robot_Arm::SetRequestedVelocity_FromNormalized);
		em->EventOnOff_Map["Arm_SetPotentiometerSafety"].Remove(*this, &FRC2019_Robot::Robot_Arm::SetPotentiometerSafety);
	}
}

  /***********************************************************************************************************************************/
 /*															FRC2019_Robot															*/
/***********************************************************************************************************************************/
FRC2019_Robot::FRC2019_Robot(const char EntityName[],FRC2019_Control_Interface *robot_control,bool UseEncoders) : 
	//Tank_Robot(EntityName,robot_control,UseEncoders), 
	m_RobotControl(robot_control), m_Arm(this,robot_control), m_Claw(this,robot_control)
{
}

void FRC2019_Robot::Initialize(Entity2D_Kind::EventMap& em, const FRC2019_Robot_Properties *props)
{
	__super::Initialize(em);
	m_RobotControl->Initialize(props);

	const FRC2019_Robot_Properties *RobotProps=props;
	assert(RobotProps);
	m_RobotProps=*RobotProps;  //Copy all the properties (we'll need them for high and low gearing)

	m_Arm.Initialize(em,RobotProps?&RobotProps->GetArmProps():NULL);
	m_Claw.Initialize(em,RobotProps?&RobotProps->GetClawProps():NULL);
	m_drive.Initialize(&RobotProps->GetDrive_Properties());
}
void FRC2019_Robot::ResetPos()
{
	m_drive.ResetPos();
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
	//Will keep this disabled for now unless we really want it back...
	//for simulation we'll need to put the checkbox as its version of smartdashboard requires a put
	#if 0
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
	#endif

	//For the simulated code this must be first so the simulators can have the correct times
	m_RobotControl->Robot_Control_TimeChange(dTime_s);
	//__super::TimeChange(dTime_s);
	if (!m_RobotProps.GetFRC2019RobotProps().using_ac_operator)
	{
		m_Arm.AsEntity1D().TimeChange(dTime_s);
		m_Claw.AsEntity1D().TimeChange(dTime_s);
	}

	//saving drive for last... other autonomous functionality can alter the drive before this point
	if (!m_RobotProps.GetFRC2019RobotProps().using_ac_drive)
		m_drive.TimeChange(dTime_s);
}

void FRC2019_Robot::CloseDeploymentDoor(bool Close)
{
	if (!m_RobotProps.GetFRC2019RobotProps().using_ac_operator)
		m_RobotControl->CloseSolenoid(eWedgeDeploy,Close);
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
	//__super::BindAdditionalEventControls(Bind);
	m_drive.BindAdditionalEventControls(Bind);
}

void FRC2019_Robot::BindAdditionalUIControls(bool Bind,void *joy,void *key)
{
	m_RobotProps.Get_RobotControls().BindAdditionalUIControls(Bind,joy,key);
	m_drive.BindAdditionalUIControls(Bind,joy,key);  //call drive for more control assignments
}

void FRC2019_Robot::StopAuton(bool isOn)
{
	//m_SmartDashboard_AutonTest_Valve = false;
	//SmartDashboard::PutBoolean("Test_Auton", false);
	////FreezeArm(isOn);
	//m_controller->GetUIController_RW()->SetAutoPilot(false);
	//GetEventMap()->Event_Map["StopAutonAbort"].Fire();
	//ClearGoal();
	////LockPosition(false);
}

Goal *TestAutonDefaultGoalCallback(FRC2019_Robot *Robot)
{ 
	return FRC2019_Goals::Get_FRC2019_Autonomous(Robot);
}

//No longer are these restricted to simulation
void FRC2019_Robot::TestAutonomous()
{
	//m_SmartDashboard_AutonTest_Valve = true;
	//SmartDashboard::PutBoolean("Test_Auton", true);
	//Goal *oldgoal = ClearGoal();
	//if (oldgoal)
	//	delete oldgoal;

	//{
	//	Goal *goal = NULL;
	//	//Note: we may change how this gets implemented
	//	#if 0
	//	goal = FRC2019_Goals::Get_FRC2019_Autonomous(this);
	//	#else
	//	if (!m_TestAutonGoalCallback)
	//		m_TestAutonGoalCallback = TestAutonDefaultGoalCallback;
	//	//goal = FRC2019_Goals::Get_Sample_Goal(this);
	//	goal = m_TestAutonGoalCallback(this);
	//	#endif
	//	if (goal)
	//		goal->Activate(); //now with the goal(s) loaded activate it
	//	SetGoal(goal);
	//	//enable autopilot (note windriver does this in main)
	//	m_controller->GetUIController_RW()->SetAutoPilot(true);
	//}
}

void FRC2019_Robot::GoalComplete()
{
	//printf("Goals completed!\n");
	//m_controller->GetUIController_RW()->SetAutoPilot(false);
}

  /***********************************************************************************************************************************/
 /*													FRC2019_Robot_Properties														*/
/***********************************************************************************************************************************/

FRC2019_Robot_Properties::FRC2019_Robot_Properties() : m_RobotControls(&s_ControlsEvents)
{
	const double c_WheelDiameter=0.1524;  //6 inches
	const double c_MotorToWheelGearRatio=12.0/36.0;

	//{
	//	Tank_Robot_Props props=m_TankRobotProps; //start with super class settings
	//	//Late assign this to override the initial default
	//	props.WheelDimensions=Vec2D(0.4953,0.6985); //27.5 x 19.5 where length is in 5 inches in, and width is 3 on each side
	//	props.WheelDiameter=c_WheelDiameter;
	//	props.LeftPID[1]=props.RightPID[1]=1.0; //set the I's to one... so it should be 1,1,0
	//	props.MotorToWheelGearRatio=c_MotorToWheelGearRatio;
	//	m_TankRobotProps=props;
	//}

	FRC2019_Robot_Props props;
	//Note: setting defaults to arbitrary numbers to ensure lua is properly loaded
	props.hatch_deploy_time = 1.0;
	props.cargo_deploy_time = 1.0;
	props.hatch_grab_deploy_time = 1.0;

	props.cargo1_height = 5.0;
	props.cargo2_height = 30;
	props.cargo3_height = 60;

	m_FRC2019RobotProps=props;
}

//declared as global to avoid allocation on stack each iteration
const char * const g_FRC2019_Controls_Events[] = 
{
	"Claw_SetCurrentVelocity",
	"Claw_Grip","Claw_Squirt",
	"Arm_SetCurrentVelocity","Arm_SetPotentiometerSafety",
	csz_Arm_SetPosRest,csz_Arm_HatchGrabDeploy,
	csz_Arm_SetPosCargo1,csz_Arm_SetPosCargo2,csz_Arm_SetPosCargo3,
	csz_Arm_IntakeDeploy,csz_Arm_HatchDeploy,"Arm_Advance","Arm_Retract",
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
	m_driveProps.LoadFromScript(script);
	err = script.GetFieldTable("robot_settings");
	if (!err) 
	{
		FRC2019_Robot_Props &props = m_FRC2019RobotProps;
		double fValue;
		err = script.GetField("cargo1_height", NULL, NULL, &fValue);
		if (!err)
			props.cargo1_height = fValue;
		err = script.GetField("cargo2_height", NULL, NULL, &fValue);
		if (!err)
			props.cargo2_height = fValue;
		err = script.GetField("cargo3_height", NULL, NULL, &fValue);
		if (!err)
			props.cargo3_height = fValue;
		err = script.GetField("cargo_deploy_time", NULL, NULL, &fValue);
		if (!err)
			props.cargo_deploy_time= fValue;
		err = script.GetField("hatch_deploy_time", NULL, NULL, &fValue);
		if (!err)
			props.hatch_deploy_time = fValue;
		err = script.GetField("hatch_grab_deploy_time", NULL, NULL, &fValue);
		if (!err)
			props.hatch_grab_deploy_time = fValue;
		std::string sTest;
		err = SCRIPT_TEST_BOOL_YES_NoDefault(props.using_ac_drive,"using_ac_drive");
		err = SCRIPT_TEST_BOOL_YES_NoDefault(props.using_ac_operator, "using_ac_operator");
		err = SCRIPT_TEST_BOOL_YES_NoDefault(props.using_ac_elevator, "using_ac_elevator");

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
		//Do the real work
		PWMSpeedController_UpdateVoltage(index,Voltage);
	}
}

bool FRC2019_Robot_Control::GetBoolSensorState(size_t index) const
{
	bool ret=false;
	switch (index)
	{
	case FRC2019_Robot::eElevatorMin:
		ret = m_ElevatorMin;
		break;
	case FRC2019_Robot::eElevatorMax:
		ret = m_ElevatorMax;
		break;
	}
	return ret;
}

void FRC2019_Robot_Control::CloseSolenoid(size_t index,bool Close)
{
	switch (index)
	{
		case FRC2019_Robot::eWedgeDeploy:
			SmartDashboard::PutBoolean("Wedge", Close);
			break;
		case FRC2019_Robot::eHatchDeploy:
			SmartDashboard::PutBoolean("Hatch", Close);
			break;
		case FRC2019_Robot::eHatchGrabDeploy:
			SmartDashboard::PutBoolean("HatchGrip", Close);
			break;
		case FRC2019_Robot::eIntakeDeploy:
			SmartDashboard::PutBoolean("Intake", Close);
			break;
	}
	//do the real work
	Solenoid_Close(index, Close);
}

bool FRC2019_Robot_Control::GetIsSolenoidOpen(size_t index) const
{
	return Solenoid_GetIsOpen(index);
}


FRC2019_Robot_Control::FRC2019_Robot_Control(bool UseSafety)
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

void FRC2019_Robot_Control::Initialize(const FRC2019_Robot_Properties *props)
{
	//Tank_Drive_Control_Interface *tank_interface = m_pTankRobotControl;
	//tank_interface->Initialize(props);

	const FRC2019_Robot_Properties *robot_props=props;

	//For now robot_props can be NULL since the swerve robot is borrowing it
	if (robot_props)
	{
		#ifdef _Win32
		m_RobotProps=*robot_props;  //save a copy
		assert(robot_props);
		Rotary_Properties writeable_arm_props=robot_props->GetArmProps();
		m_Potentiometer.Initialize(&writeable_arm_props);
		//Now to compute the scale down, there is 0-61.5 native range, and this account for 93% of the potentiometer, so we'll multiply this together
		const double native_range_coef = 1 / 61.5;
		const double pot_usage = 0.93;
		//The amount of offset represents how much lead padding we give, ideally the safest is a centered 93% 
		const double pot_offset = 3.5 / 100.0;
		m_Potentiometer.SetNormalizedProperties(native_range_coef*pot_usage, pot_offset);
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
	//setup the actual potentiometer
	Analog_TimeChange(FRC2019_Robot::eArmPot, dTime_s, m_Potentiometer.GetActualPotPosition());
	#endif
	//Update limit switches on each time slice, this avoids multiple calls and can ensure reliable up to date monitoring on when they become triggered
	m_ElevatorMin = BoolSensor_GetState(FRC2019_Robot::eElevatorMin);
	m_ElevatorMax = BoolSensor_GetState(FRC2019_Robot::eElevatorMax);
	SmartDashboard::PutBoolean("LimitElevatorMin", m_ElevatorMin);
	SmartDashboard::PutBoolean("LimitElevatorMax", m_ElevatorMax);
}


//This is becoming a standard way to work with potentiometers, I'll try to see if I can make it more generic, so that it can be called from any
//instance
double FRC2019_Robot_Control::GetPotValue(size_t index)
{
	double result = 0.0;
	//In the simulation we can choose to view the values as they are, or a simpler native mode
	//Ideally we want to view it as they are, but allow to flip if things are not working properly
	//(e.g. especially with a new sensor)
	//#ifndef _Win32
	#if 1
	double raw_value = (double)Analog_GetAverageValue(index);
	raw_value = m_KalFilter_Arm(raw_value);  //apply the Kalman filter
	raw_value = m_Averager_Arm.GetAverage(raw_value); //and Ricks x element averager


	double PotentiometerRaw_To_Arm;

	const double HiRange = m_RobotProps.GetArmProps().GetRotary_Pot_Properties().PotMaxValue;
	const double LowRange = m_RobotProps.GetArmProps().GetRotary_Pot_Properties().PotMinValue;
	//If this is true, the value is inverted with the negative operator
	const bool FlipRange = m_RobotProps.GetArmProps().GetRotary_Pot_Properties().IsFlipped;

	PotentiometerRaw_To_Arm = raw_value - LowRange;//zeros the potentiometer
	PotentiometerRaw_To_Arm = PotentiometerRaw_To_Arm / (HiRange - LowRange);//scales values from 0 to 1 with +- .001

	//Clip Range
	//I imagine .001 corrections will not be harmful for when in use.
	//if (PotentiometerRaw_To_Arm < 0) PotentiometerRaw_To_Arm = 0;//corrects .001 or less causing a negative value
	//if (PotentiometerRaw_To_Arm > 1 || PotentiometerRaw_To_Arm > .999) PotentiometerRaw_To_Arm = 1;//corrects .001 or lass causing value greater than 1

	//TODO see if we need a ratio multiply here... otherwise range is from 0-1 for full motion

	if (FlipRange)
		PotentiometerRaw_To_Arm = 1.0 - PotentiometerRaw_To_Arm;

	const char * const Prefix = csz_FRC2019_Robot_SpeedControllerDevices_Enum[index];
	string ContructedName;
	ContructedName = Prefix, ContructedName += "_Raw";
	SmartDashboard::PutNumber(ContructedName.c_str(), raw_value);
	ContructedName = Prefix, ContructedName += "Pot_Raw";
	SmartDashboard::PutNumber(ContructedName.c_str(), PotentiometerRaw_To_Arm);
	const double Tolerance = m_RobotProps.GetArmProps().GetRotary_Pot_Properties().PotLimitTolerance;
	//Potentiometer safety, if we lose wire connection it will be out of range in which case we turn on the safety (we'll see it turned on)
	if (raw_value > HiRange + Tolerance || raw_value < LowRange - Tolerance)
	{
		std::string SmartLabel = csz_FRC2019_Robot_SpeedControllerDevices_Enum[index];
		SmartLabel[0] -= 32; //Make first letter uppercase
		ContructedName = SmartLabel + "Disable";
		SmartDashboard::PutBoolean(ContructedName.c_str(), true);
		//TODO see if we want this
		//const bool mainSafetyLock = SmartDashboard::GetBoolean("SafetyLock_Arm") || SmartDashboard::GetBoolean("SafetyLock_Drive");
		//if (!mainSafetyLock)
		//	m_EventMap->Event_Map["Failed"].Fire();
	}

	//Now to compute the result... we start with the normalized value and give it the appropriate offset and scale
	//the offset is delegated in script in the final scale units, and the scale is the total range in radians
	result = PotentiometerRaw_To_Arm;
	//get scale
	const Ship_1D_Props &shipprops = m_RobotProps.GetArmProps().GetShip_1D_Props();
	//SmartDashboard::PutNumber("Arm_ScaleTest",shipprops.MaxRange-shipprops.MinRange);
	result *= shipprops.MaxRange - shipprops.MinRange;  //compute the total distance in radians
	result *= m_RobotProps.GetArmProps().GetRotaryProps().EncoderToRS_Ratio;
	//get offset... Note: scale comes first since the offset is of that scale
	result += m_RobotProps.GetArmProps().GetRotary_Pot_Properties().PotentiometerOffset;
	#else
	result = (m_Potentiometer.GetPotentiometerCurrentPosition()) + 0.0;
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
	return result;
}

double FRC2019_Robot_Control::GetRotaryCurrentPorV(size_t index)
{
	double result=0.0;

	switch (index)
	{
		case FRC2019_Robot::eArm:
			result = GetPotValue(index);
		break;
	}
	return result;
}

//Keeping around for any quick testing goals

  /***********************************************************************************************************************************/
 /*														FRC2019_Goals																*/
/***********************************************************************************************************************************/

Goal *FRC2019_Goals::Get_FRC2019_Autonomous(FRC2019_Robot *Robot)
{
	Goal_NotifyWhenComplete *MainGoal = new Goal_NotifyWhenComplete(*Robot->GetEventMap(), "Complete", "Failed");
	//SmartDashboard::PutNumber("Sequence",1.0);  //ensure we are on the right sequence
	//Inserted in reverse since this is LIFO stack list
	//MainGoal->AddSubgoal(new FRC2019_Goals_Impl(*Robot));
	//MainGoal->AddSubgoal(goal_waitforturret);
	return MainGoal;
}
