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
#define __EnableRobotArmDisable__
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
} // namespace Robot_Tester

using namespace Robot_Tester;
using namespace GG_Framework::Base;
using namespace osg;
using namespace std;

const double Pi = M_PI;
const double Pi2 = M_PI * 2.0;

#else

#include "FRC2019_Robot.h"
#include "Common/SmartDashboard.h"
using namespace Framework::Base;
using namespace std;
#endif

#define __UsingTankDrive__

enum AutonType
{
	eDoNothing,					//0
	eJustMoveForward,			//1
	eJustRotate,				//2
	eSimpleMoveRotateSequence,	//3
	eTestBoxWayPoints,			//4
	eTurretTracking,			//5
	eDriveTracking,				//6
	//autons for 2019			
	eOnePieceAuto,				//7
	eTwoPieceAuto,				//8
	//end autons
	eNoAutonTypes				//9
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

//TODO Move into Misc
//The way this works is that we keep track of the sign of each entry and all numbers that go it are negative
//This way the priority makes the sort in reverse and the top number ends up being the floor number
//Since we have both number sets on the same side of zero the result of the sign is also averaged using a blend average
//where the smoothing value is the reciprocal of the sample size
class Priority_Averager_floor
{
  private:
	// Priority queue using operator < for ordering
	//std::priority_queue<double, vector<double>,std::less<double>> m_queue;
	struct NumberSign
	{
		NumberSign(double number)
		{
			if (number > 0)
			{
				Number = number * -1.0;
				sign = false;
			}
			else
			{
				Number = number;
				sign = true; //for zero we want this to be true as well
			}
		}
//This method is not needed, but kept here to illustrate how to obtain the numbers original value
#if 0
		double GetNumber()
		{
			if (sign)
				return Number;
			else
				return Number*-1.0;
		}
#endif
		double Number;
		bool sign;
		bool operator>(const NumberSign &rhs) const { return Number > rhs.Number; }
		bool operator<(const NumberSign &rhs) const { return Number < rhs.Number; }
		bool operator==(const NumberSign &rhs) const { return ((Number == rhs.Number) && (sign == rhs.sign)); }
	};
	std::priority_queue<NumberSign> m_queue;
	const size_t m_SampleSize;
	const double m_PurgePercent;

	double m_CurrentBadApple_Percentage;
	size_t m_Iteration_Counter;
	void flush()
	{
		while (!m_queue.empty())
			m_queue.pop();
	}
	Blend_Averager<double> m_SignInfluence; //have some weight on which sign to use
  public:
	Priority_Averager_floor(size_t SampleSize, double PurgePercent) : m_SampleSize(SampleSize), m_PurgePercent(PurgePercent),
																	  m_CurrentBadApple_Percentage(0.0), m_Iteration_Counter(0), m_SignInfluence(1.0 / (double)SampleSize)
	{
	}
	double operator()(double newItem)
	{
		m_queue.push(NumberSign(newItem));
		//This is a bit convoluted...
		//The sign influence does a blend average of the sign of the new entry and depending on this result will impact the final sign of our answer
		const double sign_influence = (m_SignInfluence((newItem > 0) ? 1.0 : -1.0) > 0) ? -1.0 : 1.0;
		double ret = m_queue.top().Number * sign_influence;
		if (m_queue.size() > m_SampleSize)
			m_queue.pop();
		//Now to manage when to purge the bad apples
		m_Iteration_Counter++;
		if ((m_Iteration_Counter % m_SampleSize) == 0)
		{
			m_CurrentBadApple_Percentage += m_PurgePercent;
			//printf(" p=%.2f ",m_CurrentBadApple_Percentage);
			if (m_CurrentBadApple_Percentage >= 1.0)
			{
				//Time to purge all the bad apples
				flush();
				m_queue.push(NumberSign(ret)); //put one good apple back in to start the cycle over
				m_CurrentBadApple_Percentage -= 1.0;
				//printf(" p=%.2f ",m_CurrentBadApple_Percentage);
			}
		}
		return ret;
	}
};

/***********************************************************************************************************************************/
/*															FRC2019_Goals															*/
/***********************************************************************************************************************************/

#ifdef Robot_TesterCode
const double FRC2019Goal_StartingPosition[4] = {13.0, 4.0, 60.0, 5.0};
const double FRC2019Goal_HoverPosition[4] = {39.0, 0.0, 90.0, 45.0};
const double FRC2019Goal_PickupPosition[4] = {39.0, -20.0, 90.0, 45.0};
#else
const double FRC2019Goal_StartingPosition[4] = {18.0, 4.0, 70.0, 5.0};
const double FRC2019Goal_HoverPosition[4] = {25.0, 0.0, 90.0, 45.0};
const double FRC2019Goal_PickupPosition[4] = {25.0, -5.0, 90.0, 45.0};
#endif

__inline bool Auton_Smart_GetSingleValue_Bool(const char *SmartName, bool default_value)
{
	bool result = default_value;
//Can't use try catch on cRIO since Thunder RIO has issue with using catch(...)
//RoboRio uses SetDefault*() to accomplish same effect
//Simulation can use try catch method, but we could modify smart dashboard to allow using the new method
#if defined _Win32
	try
	{
		result = SmartDashboard::GetBoolean(SmartName);
	}
	catch (...)
	{
		//set up some good defaults for a small box
		SmartDashboard::PutBoolean(SmartName, default_value);
	}
#else
#if !defined __USE_LEGACY_WPI_LIBRARIES__
	SmartDashboard::SetDefaultBoolean(SmartName, default_value);
	result = SmartDashboard::GetBoolean(SmartName);
#else
	//for cRIO checked in using zero in lua (default) to prompt the variable and then change to -1 to use it
	if (!SmartDashboard::GetBoolean("TestVariables_set"))
		SmartDashboard::PutBoolean(SmartName, default_value);
	else
		result = SmartDashboard::GetBoolean(SmartName);
#endif
#endif
	return result;
}

__inline void Auton_Smart_GetMultiValue_Bool(size_t NoItems, const char *const SmartNames[], bool *const SmartVariables[])
{
	//Remember can't do this on cRIO since Thunder RIO has issue with using catch(...)
#if defined _Win32
	for (size_t i = 0; i < NoItems; i++)
	{
		try
		{
			*(SmartVariables[i]) = SmartDashboard::GetBoolean(SmartNames[i]);
		}
		catch (...)
		{
			//I may need to prime the pump here
			SmartDashboard::PutBoolean(SmartNames[i], *(SmartVariables[i]));
		}
	}
#else
#if !defined __USE_LEGACY_WPI_LIBRARIES__
	for (size_t i = 0; i < NoItems; i++)
	{
		SmartDashboard::SetDefaultBoolean(SmartNames[i], *(SmartVariables[i]));
		*(SmartVariables[i]) = SmartDashboard::GetBoolean(SmartNames[i]);
	}
#else
	for (size_t i = 0; i < NoItems; i++)
	{
		if (SmartDashboard::GetBoolean("TestVariables_set"))
			*(SmartVariables[i]) = SmartDashboard::GetBoolean(SmartNames[i]);
		else
			SmartDashboard::PutBoolean(SmartNames[i], *(SmartVariables[i]));
	}
#endif
#endif
}

__inline double Auton_Smart_GetSingleValue(const char *SmartName, double default_value)
{
	double result = default_value;
//Can't use try catch on cRIO since Thunder RIO has issue with using catch(...)
//RoboRio uses SetDefault*() to accomplish same effect
//Simulation can use try catch method, but we could modify smart dashboard to allow using the new method
#if defined _Win32
	try
	{
		result = SmartDashboard::GetNumber(SmartName);
	}
	catch (...)
	{
		//set up some good defaults for a small box
		SmartDashboard::PutNumber(SmartName, default_value);
	}
#else
#if !defined __USE_LEGACY_WPI_LIBRARIES__
	SmartDashboard::SetDefaultNumber(SmartName, default_value);
	result = SmartDashboard::GetNumber(SmartName);
#else
	//for cRIO checked in using zero in lua (default) to prompt the variable and then change to -1 to use it
	if (!SmartDashboard::GetBoolean("TestVariables_set"))
		SmartDashboard::PutNumber(SmartName, default_value);
	else
		result = SmartDashboard::GetNumber(SmartName);
#endif
#endif
	return result;
}

__inline void Auton_Smart_GetMultiValue(size_t NoItems, const char *const SmartNames[], double *const SmartVariables[])
{
//Remember can't do this on cRIO since Thunder RIO has issue with using catch(...)
#if defined _Win32
	for (size_t i = 0; i < NoItems; i++)
	{
		try
		{
			*(SmartVariables[i]) = SmartDashboard::GetNumber(SmartNames[i]);
		}
		catch (...)
		{
			//I may need to prime the pump here
			SmartDashboard::PutNumber(SmartNames[i], *(SmartVariables[i]));
		}
	}
#else
#if !defined __USE_LEGACY_WPI_LIBRARIES__
	for (size_t i = 0; i < NoItems; i++)
	{
		SmartDashboard::SetDefaultNumber(SmartNames[i], *(SmartVariables[i]));
		*(SmartVariables[i]) = SmartDashboard::GetNumber(SmartNames[i]);
	}
#else
	for (size_t i = 0; i < NoItems; i++)
	{
		if (SmartDashboard::GetBoolean("TestVariables_set"))
			*(SmartVariables[i]) = SmartDashboard::GetNumber(SmartNames[i]);
		else
			SmartDashboard::PutNumber(SmartNames[i], *(SmartVariables[i]));
	}
#endif
#endif
}

//Use this class as an example template, and create your own... at the very bottom MainGoal->AddSubgoal() put your goal there
//Collapse this to elimate most of the clutter.  Some goals and calls to parts not on this robot have been disabled, but
//can be traced to determine how to integrate them 1/11/19 -James
class FRC2019_Goals_Impl : public AtomicGoal
{
  private:
	FRC2019_Robot &m_Robot;
	double m_Timer;

	class SetUpProps
	{
	  protected:
		FRC2019_Goals_Impl *m_Parent;
		FRC2019_Robot &m_Robot;
		FRC2019_Robot_Props::Autonomous_Properties m_AutonProps;
		Entity2D_Kind::EventMap &m_EventMap;

	  public:
		SetUpProps(FRC2019_Goals_Impl *Parent) : m_Parent(Parent), m_Robot(Parent->m_Robot), m_EventMap(*m_Robot.GetEventMap())
		{
			m_AutonProps = m_Robot.GetRobotProps().GetFRC2019RobotProps().Autonomous_Props;
		}
	};
	class goal_clock : public AtomicGoal
	{
	  private:
		FRC2019_Goals_Impl *m_Parent;

	  public:
		goal_clock(FRC2019_Goals_Impl *Parent) : m_Parent(Parent) { m_Status = eInactive; }
		void Activate() { m_Status = eActive; }
		Goal_Status Process(double dTime_s)
		{
			const double AutonomousTimeLimit = 30.0 * 60.0; //level 1 30 minutes
			double &Timer = m_Parent->m_Timer;
			if (m_Status == eActive)
			{
				SmartDashboard::PutNumber("Timer", AutonomousTimeLimit - Timer);
				Timer += dTime_s;
				if (Timer >= AutonomousTimeLimit)
					m_Status = eCompleted;
			}
			return m_Status;
		}
		void Terminate() { m_Status = eFailed; }
	};

	class goal_watchdog : public AtomicGoal
	{
	  private:
		FRC2019_Goals_Impl *m_Parent;

	  public:
		goal_watchdog(FRC2019_Goals_Impl *Parent) : m_Parent(Parent) { m_Status = eInactive; }
		void Activate() { m_Status = eActive; }
		Goal_Status Process(double dTime_s)
		{
			if (m_Status == eActive)
			{
				//May want to have a safety lock
				const bool SafetyLock = false;
				//bool SafetyLock=SmartDashboard::GetBoolean("SafetyLock_Arm") || SmartDashboard::GetBoolean("SafetyLock_Drive");
				if (SafetyLock)
					m_Status = eFailed;
			}
			return m_Status;
		}
		void Terminate() { m_Status = eFailed; }
	};

	MultitaskGoal m_Primer;
	bool m_IsHot;
	bool m_HasSecondShotFired;

	static Goal *Move_Straight(FRC2019_Goals_Impl *Parent, double length_ft)
	{
		FRC2019_Robot *Robot = &Parent->m_Robot;
		//Construct a way point
		WayPoint wp;
		const Vec2d Local_GoalTarget(0.0, Feet2Meters(length_ft));
		wp.Position = Local_GoalTarget;
		wp.Power = 1.0;
		//Now to setup the goal
		const bool LockOrientation = true;
#ifdef __UsingTankDrive__
		const double PrecisionTolerance = Robot->GetRobotProps().GetTankRobotProps().PrecisionTolerance;
#else
		const double PrecisionTolerance = Robot->GetRobotProps().GetSwerveRobotProps().PrecisionTolerance;
#endif
		Goal_Ship_MoveToPosition *goal_drive = NULL;
		goal_drive = new Goal_Ship_MoveToRelativePosition(Robot->GetController(), wp, true, LockOrientation, PrecisionTolerance);
		return goal_drive;
	}

	static Goal *Rotate(FRC2019_Goals_Impl *Parent, double Degrees)
	{
		FRC2019_Robot *Robot = &Parent->m_Robot;
		return new Goal_Ship_RotateToRelativePosition(Robot->GetController(), DEG_2_RAD(Degrees));
	}

	// static Goal * Move_TurretPosition(FRC2019_Goals_Impl *Parent,double Angle_Deg, bool RelativePosition=false)
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

	// static Goal * Move_ArmAndBucket(FRC2019_Goals_Impl *Parent,double length_in,double height_in,double Bucket_Angle_Deg,double Clasp_Angle_Deg,
	// 	double length_in_speed=1.0,double height_in_speed=1.0,double Bucket_Angle_Deg_speed=1.0,double Clasp_Angle_Deg_speed=1.0)
	// {
	// 	MultitaskGoal *goal=new MultitaskGoal(true);
	// 	//I could have added both multi task goals here, but its easier to debug keeping it more flat lined
	// 	goal->AddGoal(Move_ArmXPosition(Parent,length_in));
	// 	goal->AddGoal(Move_ArmYPosition(Parent,height_in));
	// 	goal->AddGoal(Move_BucketAngle(Parent,Bucket_Angle_Deg,Bucket_Angle_Deg_speed));
	// 	goal->AddGoal(Move_ClaspAngle(Parent,Clasp_Angle_Deg));
	// 	return goal;
	// }

	class RobotQuickNotify : public AtomicGoal, public SetUpProps
	{
	  private:
		std::string m_EventName;
		bool m_IsOn;

	  public:
		RobotQuickNotify(FRC2019_Goals_Impl *Parent, const char *EventName, bool On) : SetUpProps(Parent), m_EventName(EventName), m_IsOn(On)
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

	class RobotArmHoldStill : public Generic_CompositeGoal, public SetUpProps
	{
	  public:
		RobotArmHoldStill(FRC2019_Goals_Impl *Parent) : Generic_CompositeGoal(true), SetUpProps(Parent) { m_Status = eInactive; }
		virtual void Activate()
		{
			AddSubgoal(new RobotQuickNotify(m_Parent, "Robot_LockPosition", false));
			AddSubgoal(new RobotQuickNotify(m_Parent, "Robot_FreezeArm", false));
			AddSubgoal(new Goal_Wait(0.2));
			AddSubgoal(new RobotQuickNotify(m_Parent, "Robot_FreezeArm", true));
			AddSubgoal(new Goal_Wait(0.4));
			AddSubgoal(new RobotQuickNotify(m_Parent, "Robot_LockPosition", true));
			m_Status = eActive;
		}
	};

	//Drive Tests----------------------------------------------------------------------
	class MoveForward : public Generic_CompositeGoal, public SetUpProps
	{
	  public:
		MoveForward(FRC2019_Goals_Impl *Parent, bool AutoActivate = false) : Generic_CompositeGoal(AutoActivate), SetUpProps(Parent)
		{
			if (!AutoActivate)
				m_Status = eActive;
		}
		virtual void Activate()
		{
			const char *const MoveSmartVar = "TestMove";
			double DistanceFeet = Auton_Smart_GetSingleValue(MoveSmartVar, 1.0); //should be a safe default

			AddSubgoal(new Goal_Wait(0.500));
			AddSubgoal(Move_Straight(m_Parent, DistanceFeet));
			AddSubgoal(new Goal_Wait(0.500)); //allow time for mass to settle
			m_Status = eActive;
		}
	};

	class RotateWithWait : public Generic_CompositeGoal, public SetUpProps
	{
	  public:
		RotateWithWait(FRC2019_Goals_Impl *Parent, bool AutoActivate = false) : Generic_CompositeGoal(AutoActivate), SetUpProps(Parent)
		{
			if (!AutoActivate)
				m_Status = eActive;
		}
		virtual void Activate()
		{
			const char *const RotateSmartVar = "TestRotate";
			const double RotateDegrees = Auton_Smart_GetSingleValue(RotateSmartVar, 45.0); //should be a safe default

			AddSubgoal(new Goal_Wait(0.500));
			AddSubgoal(Rotate(m_Parent, RotateDegrees));
			AddSubgoal(new Goal_Wait(0.500)); //allow time for mass to settle
			m_Status = eActive;
		}
	};

	class TestMoveRotateSequence : public Generic_CompositeGoal, public SetUpProps
	{
	  public:
		TestMoveRotateSequence(FRC2019_Goals_Impl *Parent) : SetUpProps(Parent), m_pParent(Parent) { m_Status = eActive; }
		virtual void Activate()
		{
			double dNoIterations = 4.0;
			double TestRotateDeg = 90.0;
			double TestMoveFeet = 1.0;
			const char *const SmartNames[] = {"TestMoveRotateIter", "TestRotate", "TestMove"};
			double *const SmartVariables[] = {&dNoIterations, &TestRotateDeg, &TestMoveFeet};
			Auton_Smart_GetMultiValue(3, SmartNames, SmartVariables);
			size_t NoIterations = (size_t)dNoIterations;

			for (size_t i = 0; i < NoIterations; i++)
			{
				AddSubgoal(new MoveForward(m_pParent, true));
				AddSubgoal(new RotateWithWait(m_pParent, true));
			}
			m_Status = eActive;
		}

	  private:
		FRC2019_Goals_Impl *m_pParent;
	};

	static Goal *GiveRobotSquareWayPointGoal(FRC2019_Goals_Impl *Parent)
	{
		FRC2019_Robot *Robot = &Parent->m_Robot;
		const char *const LengthSetting = "TestDistance_ft";
		const double Length_m = Feet2Meters(Auton_Smart_GetSingleValue(LengthSetting, Feet2Meters(1)));

		std::list<WayPoint> points;
		struct Locations
		{
			double x, y;
		} test[] =
			{
				{Length_m, Length_m},
				{Length_m, -Length_m},
				{-Length_m, -Length_m},
				{-Length_m, Length_m},
				{0, 0}};
		for (size_t i = 0; i < _countof(test); i++)
		{
			WayPoint wp;
			wp.Position[0] = test[i].x;
			wp.Position[1] = test[i].y;
			wp.Power = 0.5;
			points.push_back(wp);
		}
		//Now to setup the goal
		Goal_Ship_FollowPath *goal = new Goal_Ship_FollowPath(Robot->GetController(), points, false, true);
		return goal;
	}

	//Arm Tests----------------------------------------------------------------------

	//--Vision Tracking
	//This class is a helper class to be aggregated into a goal; it feed/translates the camera vectors into yaw angle, and filters the coordinates
	class VisionTracking
	{
	  public:
		VisionTracking() : m_X_PriorityAverager(10, 0.30), m_Z_PriorityAverager(10, 0.30)
		{
		}
		void Process(double dTime_s)
		{
			//Now to manage the actual vector feed
			double ZRawPositon = SmartDashboard::GetNumber("Z Position");
			//Sanity check... if the distance is negative it must be invalid
			//		--for now, given the probably of false positives... limit the range to 2 meters
			if ((ZRawPositon < 0) && (ZRawPositon > -2.0))
			{
				double RawPositon = SmartDashboard::GetNumber("X Position");
				RawPositon = m_X_PriorityAverager(RawPositon);
				RawPositon = m_X_KalmanFilter(RawPositon);
				RawPositon = m_X_Averager.GetAverage(RawPositon);
				SmartDashboard::PutNumber("X_Pos_Average", RawPositon); //Observe *no* false positives
				//leave negative going in... this will give me the least value
				ZRawPositon = m_Z_PriorityAverager(ZRawPositon);
				ZRawPositon *= -1; //invert from Zed
				ZRawPositon = m_Z_KalmanFilter(ZRawPositon);
				ZRawPositon = m_Z_Averager.GetAverage(ZRawPositon);
				SmartDashboard::PutNumber("Z_Pos_Average", ZRawPositon); //Observe *no* false positives
				const double YawAngle = atan(RawPositon / ZRawPositon);  //opposite/adjacent
				SmartDashboard::PutNumber("YawAngle", RAD_2_DEG(YawAngle));
				SmartDashboard::PutNumber("DriveDistance", ZRawPositon); //follow suit with yaw, so we can test it separately
			}
			else
			{
				//If it's not able to detect it then we zero out, which will stop all motion until we can detect it again
				SmartDashboard::PutNumber("YawAngle", 0.0);
				SmartDashboard::PutNumber("DriveDistance", 0.0); //we check for 0.0 to obtain the z_offset
			}
		}

	  private:
		//filter out noise!
		KalmanFilter m_X_KalmanFilter, m_Z_KalmanFilter;
		Averager<double, 5> m_X_Averager, m_Z_Averager;
		Priority_Averager_floor m_X_PriorityAverager;
		Priority_Averager m_Z_PriorityAverager;
	};

	class TurretTracking : public AtomicGoal, public SetUpProps
	{
	  public:
		TurretTracking(FRC2019_Goals_Impl *Parent) : SetUpProps(Parent)
		{
			Activate(); //no need to delay activation
		}
		virtual void Activate()
		{
			// FRC2019_Robot::Robot_Arm &Arm=m_Robot.GetTurret();
			// m_Position=Arm.GetActualPos(); //start out on the actual position
			m_LatencyCounter = 0.0;
			m_Status = eActive;
			m_IsTargeting = false;
			SmartDashboard::PutBoolean("Main_Is_Targeting", false);
		}
		virtual Goal_Status Process(double dTime_s)
		{
			if (m_Status == eActive)
			{
				{
					const char *const SmartVar = "EnableVision";
					bool EnableVision = Auton_Smart_GetSingleValue_Bool(SmartVar, false);
					if (m_IsTargeting)
					{
						if (!EnableVision)
						{
							SmartDashboard::PutBoolean("Main_Is_Targeting", false);
							m_IsTargeting = false;
						}
					}
					else
					{
						if (EnableVision)
						{
							SmartDashboard::PutBoolean("Main_Is_Targeting", true);
							m_IsTargeting = true;
						}
					}
					if (EnableVision)
						m_Vision.Process(dTime_s);
				}
				const char *const SmartVar = "EnableTurret";
				bool EnableTurret = Auton_Smart_GetSingleValue_Bool(SmartVar, false);
				if (EnableTurret)
				{
					double YawAngle = 0.0;
					double TrackLatency = 0.25;   //default high seconds
					double YawScaleFactor = 0.35; //ability to tune adjustment intensity... default half to under estimate avoid oscillation
					double YawTolerance = 0.4;	//degrees tolerance before taking action
					const char *const SmartNames[] = {"YawAngle", "TrackLatency", "YawScaleFactor", "YawTolerance"};
					double *const SmartVariables[] = {&YawAngle, &TrackLatency, &YawScaleFactor, &YawTolerance};
					Auton_Smart_GetMultiValue(4, SmartNames, SmartVariables);

					m_LatencyCounter += dTime_s;
					if (m_LatencyCounter > TrackLatency)
					{
						// FRC2019_Robot::Robot_Arm &Arm=m_Robot.GetTurret();
						// const double YawAngleRad=DEG_2_RAD(YawAngle);
						// m_Position=Arm.GetActualPos()+(YawAngleRad*YawScaleFactor);  //set out new position
						// if (fabs(YawAngle)>YawTolerance*YawScaleFactor)
						// 	Arm.SetIntendedPosition(m_Position);
						m_LatencyCounter = 0.0;
					}
				}
			}
			return m_Status; //Just pass through m_Status
		}
		virtual void Terminate()
		{
			//pacify the set point
			// FRC2019_Robot::Robot_Arm &Arm=m_Robot.GetTurret();
			// Arm.SetIntendedPosition(Arm.GetActualPos());
			m_Status = eInactive; //this goal never really completes
			SmartDashboard::PutBoolean("Main_Is_Targeting", false);
		}

	  private:
		VisionTracking m_Vision;
		double m_LatencyCounter;
		double m_Position;  //keep track of last position between each latency count
		bool m_IsTargeting; //Have a valve for targeting
	};

	class DriveTracking : public AtomicGoal, public SetUpProps
	{
	  public:
		DriveTracking(FRC2019_Goals_Impl *Parent) : SetUpProps(Parent)
		{
			Activate(); //no need to delay activation
		}
		virtual void Activate()
		{
			// FRC2019_Robot::Robot_Arm &Arm=m_Robot.GetTurret();
			// m_Position=Arm.GetActualPos(); //start out on the actual position
			m_LatencyCounter = 0.0;
			m_Status = eActive;
			m_IsTargeting = false;
			m_SAS_FloodControl = true; //assume we have aggressive stop by default
			SmartDashboard::PutBoolean("Main_Is_Targeting", false);
			const char *const CameraModeSmartVar = "ZedMode";
			double CurrentMode = Auton_Smart_GetSingleValue(CameraModeSmartVar, 0.0); //should be a safe default
			m_PriorCameraMode = CurrentMode;										  //mode to put back to when not tracking
		}

		//This is a short-term fix to handle yaw tracking on drive without the clicking.  The actual fix will become more clear once I start
		//working with the gyro
		void Set_UseAggresiveStop(bool UseAgressiveStop)
		{
			if (m_SAS_FloodControl != UseAgressiveStop)
			{
				//printf("AggresiveStop=%d\n",UseAgressiveStop);  //testing flooding
				SmartDashboard::PutBoolean("UseAggresiveStop", UseAgressiveStop); //monitor this
				//TODO add this function (see Curivator)
				//m_Robot.FRC2019_Robot_SetAggresiveStop(UseAgressiveStop);
				m_SAS_FloodControl = UseAgressiveStop;
			}
		}

		virtual Goal_Status Process(double dTime_s)
		{
			if (m_Status == eActive)
			{
				bool EnableVision = false;
				bool EnableDriveYaw = false;
				bool EnableDrive = false;
				{
					const char *const SmartNames[] = {"EnableVision", "EnableDriveYaw", "EnableDrive"};
					bool *const SmartVariables[] = {&EnableVision, &EnableDriveYaw, &EnableDrive};
					Auton_Smart_GetMultiValue_Bool(3, SmartNames, SmartVariables);
				}
				if (m_IsTargeting)
				{
					if (!EnableVision)
					{
						SmartDashboard::PutBoolean("Main_Is_Targeting", false);
						SmartDashboard::PutNumber("ZedMode", m_PriorCameraMode);
						m_IsTargeting = false;
					}
				}
				else
				{
					if (EnableVision)
					{
						SmartDashboard::PutBoolean("Main_Is_Targeting", true);
						SmartDashboard::PutNumber("ZedMode", 2); //put to track rock
						m_IsTargeting = true;
					}
				}
				if (EnableVision)
					m_Vision.Process(dTime_s);

				double YawAngle = 0.0;
				double TrackLatency = 0.25;   //default high seconds
				double YawScaleFactor = 0.35; //ability to tune adjustment intensity... default half to under estimate avoid oscillation
				double YawTolerance = 0.15;   //degrees tolerance before taking action, also threshold before advancing drive
				{
					const char *const SmartNames[] = {"YawAngle", "TrackLatency", "YawScaleFactor", "YawTolerance"};
					double *const SmartVariables[] = {&YawAngle, &TrackLatency, &YawScaleFactor, &YawTolerance};
					Auton_Smart_GetMultiValue(4, SmartNames, SmartVariables);
				}
				bool LatencyIntervalTriggered = false; //share the latency check with the drive
				if (EnableDriveYaw)
				{
					m_LatencyCounter += dTime_s;
					if (m_LatencyCounter > TrackLatency)
					{
						LatencyIntervalTriggered = true;
						const double YawAngleRad = DEG_2_RAD(YawAngle);
						m_Position = m_Robot.GetAtt_r() + (YawAngleRad * YawScaleFactor); //set out new position
						//Note: we needn't normalize here as that happens implicitly
						//printf("rotation=%.2f\n",RAD_2_DEG(m_Position));
						if (fabs(YawAngle) > YawTolerance)
						{
							Set_UseAggresiveStop(false); //avoid clicking while tracking yaw (short term solution)
							m_Robot.GetController()->SetIntendedOrientation(m_Position);
						}
						m_LatencyCounter = 0.0;
					}
				}
				if (EnableDrive)
				{
					double DriveDistance = 1.0;		 //start with same value as the z offset
					double DriveScaleFactor = 0.35;  //ability to tune adjustment intensity... default to 1/4 under estimate avoid oscillation
					double DriveTolerance = 0.05;	//degrees tolerance before taking action, also threshold before advancing drive
					double Z_Offset = DriveDistance; //used to tune how close we wish to be away from our target
					{
						const char *const SmartNames[] = {"DriveDistance", "DriveScaleFactor", "DriveTolerance", "Z_Offset"};
						double *const SmartVariables[] = {&DriveDistance, &DriveScaleFactor, &DriveTolerance, &Z_Offset};
						Auton_Smart_GetMultiValue(4, SmartNames, SmartVariables);
					}
					//Note: It can handle whatever yaw is being set to
					{
						if (!EnableDriveYaw)
						{
							m_LatencyCounter += dTime_s;
							LatencyIntervalTriggered = (m_LatencyCounter > TrackLatency);
						}
						if (LatencyIntervalTriggered)
						{

							m_LatencyCounter = 0; //may be redundant if drive yaw is active
							//If we are exactly 0.0 its because we don't have a lock on the target setting it to Z_offset will
							//make it stop
							if (DriveDistance == 0.0)
								DriveDistance = Z_Offset;
							const double distance_m = (DriveDistance - Z_Offset) * DriveScaleFactor;
							if (fabs(distance_m) > DriveTolerance)
							{
								Set_UseAggresiveStop(true);
								//expand out the functionality to drive forward or reverse using Goal_Ship_MoveToRelativePosition
								WayPoint wp;
								const Vec2d Local_GoalTarget(0.0, distance_m); //where postive if forward and negative is reverse
								wp.Power = 1.0;
								//set up all the other fields
								const Vec2d &pos = m_Robot.GetPos_m();
								const Vec2d Global_GoalTarget = LocalToGlobal(m_Robot.GetAtt_r(), Local_GoalTarget);
								wp.Position = Global_GoalTarget + pos;
								//set the trajectory point
								double lookDir_radians = atan2(Local_GoalTarget[0], Local_GoalTarget[1]);
								const Vec2d LocalTrajectoryOffset(sin(lookDir_radians), cos(lookDir_radians));
								const Vec2d GlobalTrajectoryOffset = LocalToGlobal(m_Robot.GetAtt_r(), LocalTrajectoryOffset);
								const Vec2d TrajectoryPoint(wp.Position + GlobalTrajectoryOffset);
								Vec2d Temp(0, 0);
								m_Robot.GetController()->DriveToLocation(TrajectoryPoint, wp.Position, wp.Power, dTime_s, &Temp, true);
							}
							else
							{
								//we're good for now
								m_Robot.GetController()->SetShipVelocity(0.0);
							}
						}
					}
				}
			}
			return m_Status; //Just pass through m_Status
		}
		virtual void Terminate()
		{
			//stop it
			m_Robot.GetController()->SetIntendedOrientation(m_Robot.GetAtt_r());
			m_Robot.GetController()->SetShipVelocity(0.0);
			Set_UseAggresiveStop(true); //back to default
			m_Status = eInactive;		//this goal never really completes
			SmartDashboard::PutBoolean("Main_Is_Targeting", false);
			SmartDashboard::PutNumber("ZedMode", m_PriorCameraMode);
		}

	  private:
		VisionTracking m_Vision;
		double m_LatencyCounter;
		double m_Position;		  //keep track of last position between each latency count
		double m_PriorCameraMode; //keep track of camera mode prior to activation
		bool m_IsTargeting;		  //Have a valve for targeting
		bool m_SAS_FloodControl;  //Limit writes to the properties
	};
	//2019 utility goals

	//TODO: implement all "_util" goals
	class IntakeHatch_util : public AtomicGoal, public SetUpProps
	{
	  public:
		IntakeHatch_util(FRC2019_Goals_Impl *parent) : SetUpProps(parent)
		{
			m_Status = eInactive;
		}
		virtual void Activate()
		{
			SmartDashboard::PutString("GoalMessage","Intake Hatch util");
			m_Status = eActive;
		}
		virtual Goal_Status Process()
		{
			SmartDashboard::PutString("GoalMessage","Intake Hatch util");
		}
	};
	class IntakeCargo_util : public AtomicGoal, public SetUpProps
	{
	  public:
		IntakeCargo_util(FRC2019_Goals_Impl *parent) : SetUpProps(parent)
		{
			m_Status = eInactive;
		}
		virtual void Activate()
		{
			m_Status = eActive;
			SmartDashboard::PutString("GoalMessage","Intake Cargo util");
		}
		virtual Goal_Status Process()
		{
			SmartDashboard::PutString("GoalMessage","Intake Cargo util");
		}
	};
	class OuttakeHatch_util : public AtomicGoal, public SetUpProps
	{
	  public:
		OuttakeHatch_util(FRC2019_Goals_Impl *parent) : SetUpProps(parent)
		{
			m_Status = eInactive;
		}
		virtual void Activate()
		{
			m_Status = eActive;
		}
		virtual Goal_Status Process()
		{
			SmartDashboard::PutString("GoalMessage","Outtake Hatch util");
		}
	};
	class OuttakeCargo_util : public AtomicGoal, public SetUpProps
	{
	  public:
		OuttakeCargo_util(FRC2019_Goals_Impl *parent) : SetUpProps(parent)
		{
			m_Status = eInactive;
		}
		virtual void Activate()
		{
			m_Status = eActive;
SmartDashboard::PutString("GoalMessage","Outtake Cargo util");
		}
		virtual Goal_Status Process()
		{

		}
	};

	class IntakeHatch : public Generic_CompositeGoal, public SetUpProps
	{
	  public:
		IntakeHatch(FRC2019_Goals_Impl *parent) : SetUpProps(parent)
		{
			m_Status = eInactive;
			Activate();
		}
		virtual void Activate()
		{
			m_Status = eActive;
			SmartDashboard::PutString("GoalMessage","Intake Hatch");
			AddSubgoal(new Goal_Wait(.5));
			//AddSubgoal(new IntakeHatch_util(m_Parent));
			AddSubgoal(new Goal_Wait(3.0)); //replace with correct goal once implemented
			AddSubgoal(new Goal_Wait(.5));
		}
	};
	class IntakeCargo : public Generic_CompositeGoal, public SetUpProps
	{
	  public:
		IntakeCargo(FRC2019_Goals_Impl *parent) : SetUpProps(parent)
		{
			m_Status = eInactive;
			Activate();
		}
		virtual void Activate()
		{
			m_Status = eActive;
SmartDashboard::PutString("GoalMessage","Intake Cargo");
			AddSubgoal(new Goal_Wait(.5));
			//AddSubgoal(new IntakeCargo_util(m_Parent));
			AddSubgoal(new Goal_Wait(3.0)); //replace with correct goal once implemented
			AddSubgoal(new Goal_Wait(.5));
		}
	};

	class OuttakeHatch : public Generic_CompositeGoal, public SetUpProps
	{
	  public:
		OuttakeHatch(FRC2019_Goals_Impl *parent) : SetUpProps(parent)
		{
			m_Status = eInactive;
			Activate();
		}
		virtual void Activate()
		{
			m_Status = eActive;
			SmartDashboard::PutString("GoalMessage","Outtake Hatch");
			printf("wait1");
			AddSubgoal(new Goal_Wait(.5));
			//AddSubgoal(new OuttakeHatch_util(m_Parent));
			printf("wait2");
			AddSubgoal(new Goal_Wait(4.0)); //replace with correct goal once implemented
			printf("wait3");
			AddSubgoal(new Goal_Wait(.5));
		}
	};
	class OuttakeCargo : public Generic_CompositeGoal, public SetUpProps
	{
	  public:
		OuttakeCargo(FRC2019_Goals_Impl *parent) : SetUpProps(parent)
		{
			m_Status = eInactive;
			Activate();
			
		}
		virtual void Activate()
		{
			m_Status = eActive;
			SmartDashboard::PutString("GoalMessage","Outtake Cargo");
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
		OnePieceAuto(FRC2019_Goals_Impl *parent, Game_Piece gamePiece) : SetUpProps(parent)
		{
			printf("constructor\n");
			m_Status = eInactive;
			m_gamePiece = gamePiece;
		}
		virtual void Activate()
		{
			m_Status = eActive;
			printf("activate\n");
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
		TwoPieceAuto(FRC2019_Goals_Impl *parent, Game_Piece gamePiece, Game_Piece gamePiece2) : SetUpProps(parent)
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
	FRC2019_Goals_Impl(FRC2019_Robot &robot) : m_Robot(robot), m_Timer(0.0),
											   m_Primer(false) //who ever is done first on this will complete the goals (i.e. if time runs out)
	{
		m_Status = eInactive;
	}
	void Activate()
	{
		//ensure arm is unfrozen... as we are about to move it
		m_Robot.GetEventMap()->EventOnOff_Map["Robot_FreezeArm"].Fire(false);
		m_Primer.AsGoal().Terminate(); //sanity check clear previous session
		typedef FRC2019_Robot_Props::Autonomous_Properties Autonomous_Properties;
		//pull parameters from SmartDashboard
		Autonomous_Properties &auton = m_Robot.GetAutonProps();
		//auton.ShowAutonParameters();  //Grab again now in case user has tweaked values

		AutonType AutonTest = (AutonType)auton.AutonTest;
		const char *const AutonTestSelection = "AutonTest";
#if defined _Win32
		try
		{
			AutonTest = (AutonType)((size_t)SmartDashboard::GetNumber(AutonTestSelection));
		}
		catch (...)
		{
			//set up some good defaults for a small box
			SmartDashboard::PutNumber(AutonTestSelection, (double)auton.AutonTest);
		}
#else
#if !defined __USE_LEGACY_WPI_LIBRARIES__
		SmartDashboard::SetDefaultNumber(AutonTestSelection, 0.0);
		AutonTest = (AutonType)((size_t)SmartDashboard::GetNumber(AutonTestSelection));
#else
		//for cRIO checked in using zero in lua (default) to prompt the variable and then change to -1 to use it
		if (auton.AutonTest != (size_t)-1)
		{
			SmartDashboard::PutNumber(AutonTestSelection, (double)0.0);
			SmartDashboard::PutBoolean("TestVariables_set", false);
		}
		else
			AutonTest = (AutonType)((size_t)SmartDashboard::GetNumber(AutonTestSelection));
#endif
#endif

		printf("Testing=%d \n", AutonTest);

		//TODO get piece type from SmartDashboard. for now, just assigned manually
		Game_Piece gamePiece = eHatch;
		Game_Piece gamePiece2 = eCargo;
		switch (AutonTest)
		{
		case eJustMoveForward:	//1
			m_Primer.AddGoal(new MoveForward(this));
			break;
		case eJustRotate: //2
			m_Primer.AddGoal(new RotateWithWait(this));
			break;
		case eSimpleMoveRotateSequence: //3
			m_Primer.AddGoal(new TestMoveRotateSequence(this));
			break;
		case eTestBoxWayPoints: //4
			m_Primer.AddGoal(GiveRobotSquareWayPointGoal(this));
			break;
		case eTurretTracking: //5
			m_Primer.AddGoal(new TurretTracking(this));
			break;
		case eDriveTracking: //6
			m_Primer.AddGoal(new DriveTracking(this));
			break;
		case eOnePieceAuto: //7
			printf("test\n");
			m_Primer.AddGoal(new OnePieceAuto(this, gamePiece));
			break;
		case eTwoPieceAuto: //8
			m_Primer.AddGoal(new TwoPieceAuto(this, gamePiece, gamePiece2));
			break;
		case eDoNothing: //0
		case eNoAutonTypes: //grrr windriver and warning 1250
			break;
		}
		m_Primer.AddGoal(new goal_clock(this));
		m_Primer.AddGoal(new goal_watchdog(this));
		m_Status = eActive;
	}

	Goal_Status Process(double dTime_s)
	{
		ActivateIfInactive();
		if (m_Status == eActive)
			m_Status = m_Primer.AsGoal().Process(dTime_s);
		return m_Status;
	}
	void Terminate()
	{
		m_Primer.AsGoal().Terminate();
		m_Status = eFailed;
	}
};

Goal *FRC2019_Goals::Get_FRC2019_Autonomous(FRC2019_Robot *Robot)
{
	Goal_NotifyWhenComplete *MainGoal = new Goal_NotifyWhenComplete(*Robot->GetEventMap(), "Complete", "Failed");
	//SmartDashboard::PutNumber("Sequence",1.0);  //ensure we are on the right sequence
	//Inserted in reverse since this is LIFO stack list
	MainGoal->AddSubgoal(new FRC2019_Goals_Impl(*Robot));
	//MainGoal->AddSubgoal(goal_waitforturret);
	return MainGoal;
}
