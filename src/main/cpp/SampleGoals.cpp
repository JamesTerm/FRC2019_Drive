//It looks vast, but if you type ctrl k-0 to collapse it is quite small (use ctrl k-j to expand)
//For VS2017 its ctrl m-o to collapse (m-p to expand)
//You'll want to keep the Auton_Smart_ inline methods as a tool to quickly work with SmartDashboard

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
#endif
#include "Common/Debug.h"
//TODO enable robot control
#include "Common/Robot_Control_Common.h"
#include "TankDrive/Tank_Robot.h"


//#define __UsingTankDrive__
#ifdef Robot_TesterCode
namespace Robot_Tester
{
#include "CommonUI.h"
#ifdef __UsingTankDrive__
#include "Tank_Robot_UI.h"
#else
#include "Swerve_Robot_UI.h"
#endif
#include "FRC2019_Robot.h"
}

using namespace Robot_Tester;
using namespace GG_Framework::Base;
using namespace osg;
using namespace std;

const double Pi=M_PI;
const double Pi2=M_PI*2.0;

#else

#include "FRC2019_Robot.h"
#include "Common/SmartDashboard.h"
using namespace Framework::Base;
using namespace std;
#endif

#define __UsingTankDrive__

enum AutonType
{
	eDoNothing,
	eJustMoveForward,
	eJustRotate,
	eSimpleMoveRotateSequence,
	eTestBoxWayPoints,
	eTestArm,
	//autons for 2019
	eOnePieceAuto, 
	eTwoPieceAuto, 
	//end autons
	eNoAutonTypes
};

/* GOALS FOR 2019
 * DriveForward  //super basic, no fluff, just needs to get past line no matter what
 * 	- Drive past hab line
 * 		+ JustMoveForward vovers this
 * OnePieceAuto //goal can be told whether to do hatch or cargo
 * 	- Drive to location
 * 		+ I think James has this implemented already, not sure.
 *  - Place Hatch -OR-
 * 		+ line up with cargo bay
 * 		+ release (??? idk how robot works yet)
 * 	- Place Cargo
 * 		+ move arm down
 * 		+ (possibly) lift elevator
 * 		+ outtake cargo (motors)
 * TwoPieceAuto //Dream lol
 * 	- OnePieceAuto
 * 	- Drive to location
 * 	- Intake Hatch -OR-
 * 	- Intake Cargo
 * 	- OnePieceAuto (if waypoint stuff is accurate enough for this, we can call OnePieceAuto again)
 */



  /***********************************************************************************************************************************/
 /*															Sample_Goals															*/
/***********************************************************************************************************************************/

__inline bool Auton_Smart_GetSingleValue_Bool(const char *SmartName,bool default_value)
{
	bool result=default_value;
	SmartDashboard::SetDefaultBoolean(SmartName,default_value);
	result=SmartDashboard::GetBoolean(SmartName);
	return result;
}

__inline void Auton_Smart_GetMultiValue_Bool(size_t NoItems,const char * const SmartNames[],bool * const SmartVariables[])
{
	for (size_t i=0;i<NoItems;i++)
	{
		SmartDashboard::SetDefaultBoolean(SmartNames[i],*(SmartVariables[i]));
		*(SmartVariables[i])=SmartDashboard::GetBoolean(SmartNames[i]);
	}
}


__inline double Auton_Smart_GetSingleValue(const char *SmartName,double default_value)
{
	double result=default_value;
	SmartDashboard::SetDefaultNumber(SmartName,default_value);
	result=SmartDashboard::GetNumber(SmartName);
	return result;
}

__inline void Auton_Smart_GetMultiValue(size_t NoItems,const char * const SmartNames[],double * const SmartVariables[])
{
	for (size_t i=0;i<NoItems;i++)
	{
		SmartDashboard::SetDefaultNumber(SmartNames[i],*(SmartVariables[i]));
		*(SmartVariables[i])=SmartDashboard::GetNumber(SmartNames[i]);
	}
}


//No need to compute these every clock cycle
const double c_TestTimeLimit = 30.0*60.0; //level 1 30 minutes <--arbitrary
const double c_AutonomousTimeLimit = 15.0; //15 seconds
const double c_TeleOpTimeLimit = (2.0 * 60.0) + 15.0;   //2:15
const double c_TimerTable[3] = { c_TestTimeLimit,c_AutonomousTimeLimit,c_TeleOpTimeLimit };
const char *const csz_TimerTable[3] = { "Test","Autonomous","Tele" };

//Use this class as an example template, and create your own... at the very bottom MainGoal->AddSubgoal() put your goal there
//Collapse this to elimate most of the clutter.  Some goals and calls to parts not on this robot have been disabled, but
//can be traced to determine how to integrate them 1/11/19 -James
class Sample_Goals_Impl : public AtomicGoal
{
	private:
		FRC2019_Robot &m_Robot;
		double m_Timer=0.0;
		bool m_GameClock = true;
		enum ClockMode
		{
			eTest,    //Like Tele but a good long time for testing goals for a long period of time
			eAuton,  //test auton short time
			eTele,  //Tele cycle of game
		} m_ClockMode = eAuton,m_PrevClockMode= eAuton;

		class SetUpProps
		{
		protected:
			Sample_Goals_Impl *m_Parent;
			FRC2019_Robot &m_Robot;
			FRC2019_Robot_Props::Autonomous_Properties m_AutonProps;
			Entity2D_Kind::EventMap &m_EventMap;
		public:
			SetUpProps(Sample_Goals_Impl *Parent)	: m_Parent(Parent),m_Robot(Parent->m_Robot),m_EventMap(*m_Robot.GetEventMap())
			{	
				m_AutonProps=m_Robot.GetRobotProps().GetFRC2019RobotProps().Autonomous_Props;
			}
		};
		class goal_clock : public AtomicGoal
		{
		private:
			Sample_Goals_Impl *m_Parent;
		public:
			goal_clock(Sample_Goals_Impl *Parent)	: m_Parent(Parent) {	m_Status=eInactive;	}
			void Activate()  
			{
				ClockMode &_ClockMode = m_Parent->m_ClockMode;
				//Keep logic simple... if we are not doing the game we are in dev mode
				bool DevMode = !m_Parent->m_GameClock;
				DevMode= Auton_Smart_GetSingleValue_Bool("DevMode", false); 
				m_Parent->m_GameClock = !DevMode;
				//While in dev mode we can pick which kind of clock to use
				if (DevMode)
				{
					const double dClockMode = Auton_Smart_GetSingleValue("ClockMode", 0.0);
					const int ReallyAreYouKiddingme = (int)dClockMode;		//Gah
					_ClockMode = (ClockMode)ReallyAreYouKiddingme;
				}
				else
					_ClockMode = eAuton;   //Game mode: Set it explicitly for multisession use-case

				//setup default mode
				SmartDashboard::PutString("GameMode", csz_TimerTable[_ClockMode]);
				m_Status=eActive;	
			}
			Goal_Status Process(double dTime_s)
			{
				//reference parent member vars for easier reading
				double &Timer=m_Parent->m_Timer;
				ClockMode &_ClockMode = m_Parent->m_ClockMode;
				ClockMode &_PrevClockMode = m_Parent->m_PrevClockMode;

				const double CurrentTimeLimit =c_TimerTable[_ClockMode];
				if (m_Status==eActive)
				{
					SmartDashboard::PutNumber("Timer", CurrentTimeLimit-Timer);
					//Increment the timer... should be the only write to it!!
					Timer+=dTime_s;
					if (Timer >= CurrentTimeLimit)
					{
						if (!m_Parent->m_GameClock)
							m_Status = eCompleted;
						else
						{
							assert(_ClockMode != eTest);  //duh
							if (_ClockMode == eAuton)
							{
								Timer = 0.0;  //reset the clock
								_ClockMode = eTele;  //and switch to teleop
							}
							else
								m_Status = eCompleted;
						}
					}
				}
				//All clockmode writes are final... see if they changed... for flood control
				if (_PrevClockMode != _ClockMode)
					SmartDashboard::PutString("GameMode", csz_TimerTable[_ClockMode]);
				//updated previous for next cycle
				_PrevClockMode = _ClockMode;
				return m_Status;
			}
			void Terminate() {	m_Status=eFailed;	}
		};

		class goal_watchdog : public AtomicGoal
		{
		private:
			Sample_Goals_Impl *m_Parent;
		public:
			goal_watchdog(Sample_Goals_Impl *Parent)	: m_Parent(Parent) {	m_Status=eInactive;	}
			void Activate()  {	m_Status=eActive;	}
			Goal_Status Process(double dTime_s)
			{
				if (m_Status==eActive)
				{
					//May want to have a safety lock
					const bool SafetyLock = false;
					//bool SafetyLock=SmartDashboard::GetBoolean("SafetyLock_Arm") || SmartDashboard::GetBoolean("SafetyLock_Drive");
					if (SafetyLock)
						m_Status=eFailed;
				}
				return m_Status;
			}
			void Terminate() {	m_Status=eFailed;	}
		};


		MultitaskGoal m_Primer;

		static Goal * Move_Straight(Sample_Goals_Impl *Parent,double length_ft)
		{
			FRC2019_Robot *Robot=&Parent->m_Robot;
			//Construct a way point
			WayPoint wp;
			const Vec2d Local_GoalTarget(0.0,Feet2Meters(length_ft));
			wp.Position=Local_GoalTarget;
			wp.Power=1.0;
			//Now to setup the goal
			const bool LockOrientation=true;
			#ifdef __UsingTankDrive__
			const double PrecisionTolerance=Robot->GetRobotProps().GetTankRobotProps().PrecisionTolerance;
			#else
			const double PrecisionTolerance=Robot->GetRobotProps().GetSwerveRobotProps().PrecisionTolerance;
			#endif
			Goal_Ship_MoveToPosition *goal_drive=NULL;
			goal_drive=new Goal_Ship_MoveToRelativePosition(Robot->GetController(),wp,true,LockOrientation,PrecisionTolerance);
			return goal_drive;
		}

		static Goal * Rotate(Sample_Goals_Impl *Parent,double Degrees)
		{
			FRC2019_Robot *Robot=&Parent->m_Robot;
			return new Goal_Ship_RotateToRelativePosition(Robot->GetController(),DEG_2_RAD(Degrees));
		}

		// static Goal * Move_TurretPosition(Sample_Goals_Impl *Parent,double Angle_Deg, bool RelativePosition=false)
		// {
		// 	FRC2019_Robot *Robot=&Parent->m_Robot;
		// 	FRC2019_Robot::Robot_Arm &Arm=Robot->GetTurret();
		// 	const double PrecisionTolerance=Robot->GetRobotProps().GetRotaryProps(FRC2019_Robot::eTurret).GetRotaryProps().PrecisionTolerance;
		// 	Goal_Rotary_MoveToPosition *goal_arm=NULL;
		// 	const double position=Angle_Deg;
		// 	if (!RelativePosition)
		// 		goal_arm=new Goal_Rotary_MoveToPosition(Arm,DEG_2_RAD(position),PrecisionTolerance);
		// 	else
		// 		goal_arm=new Goal_Rotary_MoveToRelativePosition(Arm,DEG_2_RAD(position),PrecisionTolerance);
		// 	return goal_arm;
		// }
		static Goal * Move_ArmPosition(Sample_Goals_Impl *Parent, double length_in)
		{
			FRC2019_Robot *Robot = &Parent->m_Robot;
			FRC2019_Robot::Robot_Arm &Arm = Robot->GetArm();
			const double PrecisionTolerance = Robot->GetRobotProps().GetArmProps().GetRotaryProps().PrecisionTolerance;
			Goal_Rotary_MoveToPosition *goal_arm = NULL;
			const double position = length_in;
			goal_arm = new Goal_Rotary_MoveToPosition(Arm, position, PrecisionTolerance);
			return goal_arm;
		}

		class SetArmWaypoint : public Generic_CompositeGoal, public SetUpProps
		{
		public:
			SetArmWaypoint(Sample_Goals_Impl *Parent, bool AutoActivate = false) : Generic_CompositeGoal(AutoActivate), SetUpProps(Parent)
			{
				if (!AutoActivate)
					m_Status = eActive;
			}
			virtual void Activate()
			{
				const char * const MoveSmartVar = "ArmHeight";
				double height_in = Auton_Smart_GetSingleValue(MoveSmartVar, 6.0); //should be a safe default

				AddSubgoal(new Goal_Wait(0.500));
				AddSubgoal(Move_ArmPosition(m_Parent, height_in));
				AddSubgoal(new Goal_Wait(0.500));  //allow time for mass to settle
				m_Status = eActive;
			}
		};

		class RobotQuickNotify : public AtomicGoal, public SetUpProps
		{
		private:
			std::string m_EventName;
			bool m_IsOn;
		public:
			RobotQuickNotify(Sample_Goals_Impl *Parent,const char *EventName, bool On)	: SetUpProps(Parent),m_EventName(EventName),m_IsOn(On)
				{	m_Status=eInactive;	
				}
			virtual void Activate() {m_Status=eActive;}
			virtual Goal_Status Process(double dTime_s)
			{
				ActivateIfInactive();
				m_EventMap.EventOnOff_Map[m_EventName.c_str()].Fire(m_IsOn);
				m_Status=eCompleted;
				return m_Status;
			}
		};

		class RobotArmHoldStill : public Generic_CompositeGoal, public SetUpProps
		{
		public:
			RobotArmHoldStill(Sample_Goals_Impl *Parent)	: Generic_CompositeGoal(true),SetUpProps(Parent) {	m_Status=eInactive;	}
			virtual void Activate()
			{
				AddSubgoal(new RobotQuickNotify(m_Parent,"Robot_LockPosition",false));
				AddSubgoal(new RobotQuickNotify(m_Parent,"Robot_FreezeArm",false));
				AddSubgoal(new Goal_Wait(0.2));
				AddSubgoal(new RobotQuickNotify(m_Parent,"Robot_FreezeArm",true));
				AddSubgoal(new Goal_Wait(0.4));
				AddSubgoal(new RobotQuickNotify(m_Parent,"Robot_LockPosition",true));
				m_Status=eActive;
			}
		};

		//Drive Tests----------------------------------------------------------------------
		class MoveForward : public Generic_CompositeGoal, public SetUpProps
		{
		public:
			MoveForward(Sample_Goals_Impl *Parent, bool AutoActivate=false)	: Generic_CompositeGoal(AutoActivate),SetUpProps(Parent) 
			{	
				if(!AutoActivate) 
					m_Status=eActive;	
			}
			virtual void Activate()
			{
				const char * const MoveSmartVar="TestMove";
				double DistanceFeet=Auton_Smart_GetSingleValue(MoveSmartVar,1.0); //should be a safe default

				AddSubgoal(new Goal_Wait(0.500));
				AddSubgoal(Move_Straight(m_Parent,DistanceFeet));
				AddSubgoal(new Goal_Wait(0.500));  //allow time for mass to settle
				m_Status=eActive;
			}
		};

		class RotateWithWait : public Generic_CompositeGoal, public SetUpProps
		{
		public:
			RotateWithWait(Sample_Goals_Impl *Parent, bool AutoActivate=false)	: Generic_CompositeGoal(AutoActivate),SetUpProps(Parent)
			{	
				if(!AutoActivate) 
					m_Status=eActive;	
			}
			virtual void Activate()
			{
				const char * const RotateSmartVar="TestRotate";
				const double RotateDegrees=Auton_Smart_GetSingleValue(RotateSmartVar,45.0); //should be a safe default

				AddSubgoal(new Goal_Wait(0.500));
				AddSubgoal(Rotate(m_Parent,RotateDegrees));
				AddSubgoal(new Goal_Wait(0.500));  //allow time for mass to settle
				m_Status=eActive;
			}
		};

		class TestMoveRotateSequence : public Generic_CompositeGoal, public SetUpProps
		{
		public:
			TestMoveRotateSequence(Sample_Goals_Impl *Parent)	: SetUpProps(Parent),m_pParent(Parent) {	m_Status=eActive;	}
			virtual void Activate()
			{
				double dNoIterations=4.0;
				double TestRotateDeg=90.0;
				double TestMoveFeet=1.0;
				const char * const SmartNames[]={"TestMoveRotateIter","TestRotate","TestMove"};
				double * const SmartVariables[]={&dNoIterations,&TestRotateDeg,&TestMoveFeet};
				Auton_Smart_GetMultiValue(3,SmartNames,SmartVariables);
				size_t NoIterations=(size_t)dNoIterations;

				for (size_t i=0;i<NoIterations;i++)
				{
					AddSubgoal(new MoveForward(m_pParent,true));
					AddSubgoal(new RotateWithWait(m_pParent,true));
				}
				m_Status=eActive;
			}
		private:
			Sample_Goals_Impl *m_pParent;
		};

		static Goal * GiveRobotSquareWayPointGoal(Sample_Goals_Impl *Parent)
		{
			FRC2019_Robot *Robot=&Parent->m_Robot;
			const char * const LengthSetting="TestDistance_ft";
			const double Length_m=Feet2Meters(Auton_Smart_GetSingleValue(LengthSetting,Feet2Meters(1)));

			std::list <WayPoint> points;
			struct Locations
			{
				double x,y;
			} test[]=
			{
				{Length_m,Length_m},
				{Length_m,-Length_m},
				{-Length_m,-Length_m},
				{-Length_m,Length_m},
				{0,0}
			};
			for (size_t i=0;i<_countof(test);i++)
			{
				WayPoint wp;
				wp.Position[0]=test[i].x;
				wp.Position[1]=test[i].y;
				wp.Power=0.5;
				points.push_back(wp);
			}
			//Now to setup the goal
			Goal_Ship_FollowPath *goal=new Goal_Ship_FollowPath(Robot->GetController(),points,false,true);
			return goal;
		}


		//Arm Tests----------------------------------------------------------------------

		//2019 utility goals
	//TODO: implement all "_util" goals
		class IntakeHatch_util : public AtomicGoal, public SetUpProps
		{
		public:
			IntakeHatch_util(Sample_Goals_Impl *parent) : SetUpProps(parent)
			{
				m_Status = eInactive;
			}
			virtual void Activate()
			{
				SmartDashboard::PutString("GoalMessage", "Intake Hatch util");
				m_Status = eActive;
			}
			virtual Goal_Status Process()
			{
				SmartDashboard::PutString("GoalMessage", "Intake Hatch util");
			}
		};
		class IntakeCargo_util : public AtomicGoal, public SetUpProps
		{
		public:
			IntakeCargo_util(Sample_Goals_Impl *parent) : SetUpProps(parent)
			{
				m_Status = eInactive;
			}
			virtual void Activate()
			{
				m_Status = eActive;
				SmartDashboard::PutString("GoalMessage", "Intake Cargo util");
			}
			virtual Goal_Status Process()
			{
				SmartDashboard::PutString("GoalMessage", "Intake Cargo util");
			}
		};
		class OuttakeHatch_util : public AtomicGoal, public SetUpProps
		{
		public:
			OuttakeHatch_util(Sample_Goals_Impl *parent) : SetUpProps(parent)
			{
				m_Status = eInactive;
			}
			virtual void Activate()
			{
				m_Status = eActive;
			}
			//Note: you must put parameter in to properly override
			virtual Goal_Status Process(double dTime_s)
			{
				ActivateIfInactive();
				SmartDashboard::PutString("GoalMessage", "Outtake Hatch util");
				//we'll probably fire an event here (if it's just this then use RobotQuickNotify)
				//m_EventMap.EventOnOff_Map[m_EventName.c_str()].Fire(m_IsOn);
				m_Status = eCompleted;
				return m_Status;
			}
		};
		class OuttakeCargo_util : public AtomicGoal, public SetUpProps
		{
		public:
			OuttakeCargo_util(Sample_Goals_Impl *parent) : SetUpProps(parent)
			{
				m_Status = eInactive;
			}
			virtual void Activate()
			{
				m_Status = eActive;
				SmartDashboard::PutString("GoalMessage", "Outtake Cargo util");
			}
			virtual Goal_Status Process()
			{

			}
		};

		class IntakeHatch : public Generic_CompositeGoal, public SetUpProps
		{
		public:
			IntakeHatch(Sample_Goals_Impl *parent) : SetUpProps(parent)
			{
				m_Status = eInactive;
			}
			virtual void Activate()
			{
				m_Status = eActive;
				SmartDashboard::PutString("GoalMessage", "Intake Hatch");
				AddSubgoal(new Goal_Wait(.5));
				//AddSubgoal(new IntakeHatch_util(m_Parent));
				AddSubgoal(new Goal_Wait(3.0)); //replace with correct goal once implemented
				AddSubgoal(new Goal_Wait(.5));
			}
		};
		class IntakeCargo : public Generic_CompositeGoal, public SetUpProps
		{
		public:
			IntakeCargo(Sample_Goals_Impl *parent) : SetUpProps(parent)
			{
				m_Status = eInactive;
			}
			virtual void Activate()
			{
				m_Status = eActive;
				SmartDashboard::PutString("GoalMessage", "Intake Cargo");
				AddSubgoal(new Goal_Wait(.5));
				//AddSubgoal(new IntakeCargo_util(m_Parent));
				AddSubgoal(new Goal_Wait(3.0)); //replace with correct goal once implemented
				AddSubgoal(new Goal_Wait(.5));
			}
		};

		class OuttakeHatch : public Generic_CompositeGoal, public SetUpProps
		{
		public:
			OuttakeHatch(Sample_Goals_Impl *parent) : SetUpProps(parent), Generic_CompositeGoal(true)
			{
				m_Status = eInactive;
			}
			virtual void Activate()
			{
				SmartDashboard::PutString("GoalMessage", "Outtake Hatch");
				m_Status = eActive;
				AddSubgoal(new Goal_Wait(.5));
				AddSubgoal(new OuttakeHatch_util(m_Parent));
				AddSubgoal(new Goal_Wait(1.0)); //replace with correct goal once implemented
				AddSubgoal(new Goal_Wait(.5));
			}
		};
		class OuttakeCargo : public Generic_CompositeGoal, public SetUpProps
		{
		public:
			OuttakeCargo(Sample_Goals_Impl *parent) : SetUpProps(parent)
			{
				m_Status = eInactive;
				SmartDashboard::PutString("GoalMessage", "Outtake Cargo");
			}
			virtual void Activate()
			{
				m_Status = eActive;
				AddSubgoal(new Goal_Wait(.5));
				//AddSubgoal(new OuttakeCargo_util(m_Parent));
				AddSubgoal(new Goal_Wait(1.0)); //replace with correct goal once implemented
				AddSubgoal(new Goal_Wait(.5));
			}
		};

		//2019 auton goals

		enum Game_Piece
		{
			eHatch,
			eCargo
		};
		class OnePieceAuto : public Generic_CompositeGoal, public SetUpProps
		{
		public:
			OnePieceAuto(Sample_Goals_Impl *parent, Game_Piece gamePiece) : SetUpProps(parent),Generic_CompositeGoal(true)
			{
				m_Status = eInactive;
				m_gamePiece = gamePiece;
			}
			virtual void Activate()
			{
				m_Status = eActive;
				//TODO goals to get to location
				if (m_gamePiece == eHatch)
					AddSubgoal(new OuttakeHatch(m_Parent));
				else if (m_gamePiece == eCargo)
					AddSubgoal(new OuttakeCargo(m_Parent));
			}

		protected:
			Game_Piece m_gamePiece;
		};

		class TwoPieceAuto : public Generic_CompositeGoal, public SetUpProps
		{
		public:
			TwoPieceAuto(Sample_Goals_Impl *parent, Game_Piece gamePiece, Game_Piece gamePiece2) : SetUpProps(parent)
			{
				m_Status = eInactive;
				m_gamePiece = gamePiece;
				m_gamePiece2 = gamePiece2;
			}
			virtual void Activate()
			{
				m_Status = eActive;

				AddSubgoal(new OnePieceAuto(m_Parent, m_gamePiece));
				//TODO add remaining subgoals to get to next game piece
				if (m_gamePiece2 == eHatch)
					AddSubgoal(new IntakeHatch(m_Parent));
				else if (m_gamePiece2 == eCargo)
					AddSubgoal(new IntakeCargo(m_Parent));
				AddSubgoal(new OnePieceAuto(m_Parent, m_gamePiece2));
			}

		protected:
			Game_Piece m_gamePiece, m_gamePiece2;
		};

	  public:
		Sample_Goals_Impl(FRC2019_Robot &robot) : m_Robot(robot), m_Timer(0.0), 
			m_Primer(false)  //who ever is done first on this will complete the goals (i.e. if time runs out)
		{
			m_Status=eInactive;
		}
		void Activate() 
		{
			//ensure arm is unfrozen... as we are about to move it
			m_Robot.GetEventMap()->EventOnOff_Map["Robot_FreezeArm"].Fire(false);
			m_Primer.AsGoal().Terminate();  //sanity check clear previous session
			typedef FRC2019_Robot_Props::Autonomous_Properties Autonomous_Properties;
			//pull parameters from SmartDashboard
			Autonomous_Properties &auton=m_Robot.GetAutonProps();
			//auton.ShowAutonParameters();  //Grab again now in case user has tweaked values

			AutonType AutonTest = (AutonType)auton.AutonTest;
			const char * const AutonTestSelection="AutonTest";
			#if defined _Win32
			try
			{
				AutonTest=(AutonType)((size_t)SmartDashboard::GetNumber(AutonTestSelection));
			}
			catch (...)
			{
				//set up some good defaults for a small box
				SmartDashboard::PutNumber(AutonTestSelection,(double)auton.AutonTest);
			}
			#else
			#if !defined __USE_LEGACY_WPI_LIBRARIES__
			SmartDashboard::SetDefaultNumber(AutonTestSelection,0.0);
			AutonTest=(AutonType)((size_t)SmartDashboard::GetNumber(AutonTestSelection));
			#else
			//for cRIO checked in using zero in lua (default) to prompt the variable and then change to -1 to use it
			if (auton.AutonTest!=(size_t)-1)
			{
				SmartDashboard::PutNumber(AutonTestSelection,(double)0.0);
				SmartDashboard::PutBoolean("TestVariables_set",false);
			}
			else
				AutonTest=(AutonType)((size_t)SmartDashboard::GetNumber(AutonTestSelection));
			#endif
			#endif

			printf("Testing=%d \n",AutonTest);
			
			//TODO get piece type from SmartDashboard. for now, just assigned manually
			Game_Piece gamePiece = eHatch;
			Game_Piece gamePiece2 = eCargo;
			switch(AutonTest)
			{
			case eJustMoveForward:
				m_Primer.AddGoal(new MoveForward(this));
				break;
			case eJustRotate:
				m_Primer.AddGoal(new RotateWithWait(this));
				break;
			case eSimpleMoveRotateSequence:
				m_Primer.AddGoal(new TestMoveRotateSequence(this));
				break;
			case eTestBoxWayPoints:
				m_Primer.AddGoal(GiveRobotSquareWayPointGoal(this));
				break;
			case eTestArm:
				m_Primer.AddGoal(new SetArmWaypoint(this));
				break;
			case eOnePieceAuto:
				m_Primer.AddGoal(new OnePieceAuto(this, gamePiece));
				break;
			case eTwoPieceAuto:
				m_Primer.AddGoal(new TwoPieceAuto(this, gamePiece, gamePiece2));
				break;
			case eDoNothing:
			case eNoAutonTypes: //grrr windriver and warning 1250
				break;
			}
			m_Primer.AddGoal(new goal_clock(this));
			m_Primer.AddGoal(new goal_watchdog(this));
			m_Status=eActive;
		}

		Goal_Status Process(double dTime_s)
		{
			ActivateIfInactive();
			if (m_Status==eActive)
				m_Status=m_Primer.AsGoal().Process(dTime_s);
			return m_Status;
		}
		void Terminate() 
		{
			m_Primer.AsGoal().Terminate();
			m_Status=eFailed;
		}
};

Goal *FRC2019_Goals::Get_Sample_Goal(FRC2019_Robot *Robot)
{
	Goal_NotifyWhenComplete *MainGoal=new Goal_NotifyWhenComplete(*Robot->GetEventMap(),"Complete","Failed");
	//SmartDashboard::PutNumber("Sequence",1.0);  //ensure we are on the right sequence
	//Inserted in reverse since this is LIFO stack list
	MainGoal->AddSubgoal(new Sample_Goals_Impl(*Robot));
	//MainGoal->AddSubgoal(goal_waitforturret);
	return MainGoal;
}
