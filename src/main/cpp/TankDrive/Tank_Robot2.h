#pragma once
#include "../Base/Vec2d.h"
#include "../Base/Misc.h"
#include "../Base/Event.h"
#include "../Base/EventMap.h"
#include "../Common/Entity_Properties.h"

struct Tank_Robot2_Props
{
	bool ReverseSteering;  //This will fix if the wiring on voltage has been reversed (e.g. voltage to right turns left side)
};

const char * const csz_Tank_Robot2_SpeedControllerDevices_Enum[] =
{
	"left_drive_1","left_drive_2","left_drive_3","right_drive_1","right_drive_2","right_drive_3"
};

//TODO move to a more generic location, will need to do so when we support other drives (e.g. swerve, butterfly etc...)
//All common traits can be added here, this allows simulation to have access
class RobotCommon
{
private:
	Entity2D_Kind::EventMap* m_eventMap;
protected:
	virtual void Initialize(Entity2D_Kind::EventMap& em) { m_eventMap = &em; }
public:
	Entity2D_Kind::EventMap* GetEventMap() { return m_eventMap; }
	//Robot implements... client code calls
	virtual void BindAdditionalEventControls(bool Bind) = 0;
	virtual void BindAdditionalUIControls(bool Bind, void *joy, void *key) = 0;
};




class Tank_Robot2_Properties
{
public:

	Tank_Robot2_Properties();
	virtual void LoadFromScript(Scripting::Script& script);
	const Tank_Robot2_Props &GetTankRobotProps() const { return m_TankRobotProps; }
	const frc::Control_Assignment_Properties &Get_ControlAssignmentProps() const { return m_ControlAssignmentProps; }
	const LUA_Controls_Properties &Get_RobotControls() const { return m_RobotControls; }
protected:
	frc::Control_Assignment_Properties m_ControlAssignmentProps;
	Tank_Robot2_Props m_TankRobotProps;

private:
	class ControlEvents : public LUA_Controls_Properties_Interface
	{
	protected: //from LUA_Controls_Properties_Interface
		virtual const char *LUA_Controls_GetEvents(size_t index) const;
	};
	static ControlEvents s_ControlsEvents;
	LUA_Controls_Properties m_RobotControls;
};

//This manages all drive inputs and directs them to the speed controllers
class Tank_Robot2
{
	public:
		//For velocity [0] is port side (left), and and [1] is starboard (right)
		using Vec2d = Framework::Base::Vec2d;
		enum SpeedControllerDevices
		{
			eLeftDrive1,
			eLeftDrive2,
			eLeftDrive3,
			eRightDrive1,
			eRightDrive2,
			eRightDrive3
		};

		static SpeedControllerDevices GetSpeedControllerDevices_Enum (const char *value)
		{	return Enum_GetValue<SpeedControllerDevices> (value,csz_Tank_Robot2_SpeedControllerDevices_Enum,_countof(csz_Tank_Robot2_SpeedControllerDevices_Enum));
		}

		//I'm switching this up a bit... providing access to the parent class through a common type
		Tank_Robot2(RobotCommon *robot) : m_pParent(robot) {}
		//virtual ~Tank_Robot2();
		virtual void Initialize(const Tank_Robot2_Properties *props) { m_TankRobotProps = props; }

		//void Reset(bool ResetPosition=true);

		//This should be called last in robot's TimeChange, this will give any teleop autonomous code the opportunity to set the velocity
		//before transmitting the velocity to the controller
		virtual void TimeChange(double dTime_s);

		//I probably do not need this, depends on how to manage a gear change
		//void UpdateTankProps(const Tank_Robot_Props &TankProps);

		//Use this method to override the current velocity
		void SetVelocity(const Vec2d &velocity) { m_Velocity = velocity; }
		Vec2d GetVelocity() const { return m_Velocity; }

		//Use this method to determine if the drive is moving the controls
		bool IsDriverMoving() { return m_Controller_Velocity.length2() > 0.0; }
		//This is where all joystick events are subscribed
		virtual void BindAdditionalEventControls(bool Bind);
		virtual void BindAdditionalUIControls(bool Bind, void *joy, void *key);
protected:
		const Tank_Robot2_Props &GetTankRobotProps() const {return m_TankRobotProps->GetTankRobotProps();}
		//this may need to be overridden for robots that need it on for certain cases like 2012 needing it on for low gear
		//virtual bool GetUseAgressiveStop() const;

	private:
		#ifndef _Win32
		typedef  Ship_Tester __super;
		#endif
		RobotCommon * const m_pParent;
		const Tank_Robot2_Properties *m_TankRobotProps; //cached in the Initialize from specific robot

		//This velocity is captured from the controller in the form of left/right velocity, even FPS controls will be translated here
		Vec2D m_Controller_Velocity;
		//This will be the velocity applied to the controller on a time change
		Vec2d m_Velocity;
	public:
};

