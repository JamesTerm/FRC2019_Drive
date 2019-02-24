#pragma once
#include "../Base/Vec2d.h"
#include "../Base/Misc.h"
#include "../Base/Event.h"
#include "../Base/EventMap.h"
#include "../Common/Entity_Properties.h"

struct Tank_Robot2_Props
{
	bool HasEncoders;
	bool IsOpen;
	bool PID_Console_Dump;
	bool UseAggressiveStop;
	bool ReverseSteering;  //This will fix if the wiring on voltage has been reversed (e.g. voltage to right turns left side)
	//Different robots may have the encoders flipped or not which must represent the same direction of both treads
	//for instance the hiking viking has both of these false, while the admiral has the right encoder reversed
	bool LeftEncoderReversed, RightEncoderReversed;
	double WheelDiameter;
	double VoltageScalar_Left, VoltageScalar_Right;		//Used to handle reversed voltage wiring
	struct gear_props
	{
		double MotorToWheelGearRatio;  //Used to interpolate RPS of the encoder to linear velocity
		double LeftPID[3]; //p,i,d
		double RightPID[3]; //p,i,d
		//This cannot be avoided when working with encoders as they need to convert velocity
		double MaxSpeed;  //linear speed in meters per second
	};
	//We usually have one or 2 gears, for seasons with 1 gear... use high
	gear_props high_gear;
	gear_props low_gear;
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
	RobotCommon *AsRobotCommon() { return this; }
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
class Tank_Robot2_Control;
struct Tank_Robot2_Ancillary;
//This manages all drive inputs and directs them to the speed controllers
class Tank_Robot2
{
	public:
		//For velocity [0] is port side (left), and and [1] is starboard (right)
		using Vec2d = Framework::Base::Vec2d;
		//I'm switching this up a bit... providing access to the parent class through a common type
		Tank_Robot2(RobotCommon *robot);
		//virtual ~Tank_Robot2();
		virtual void Initialize(const Tank_Robot2_Properties *props);

		//void Reset(bool ResetPosition=true);

		//This should be called last in robot's TimeChange, this will give any teleop autonomous code the opportunity to set the velocity
		//before transmitting the velocity to the controller
		virtual void TimeChange(double dTime_s);

		//I probably do not need this, depends on how to manage a gear change
		//void UpdateTankProps(const Tank_Robot_Props &TankProps);

		//Use this method to determine if the drive is moving the controls
		bool IsDriverMoving() const 
			{ return m_Controller_Voltage.length2() > 0.0; 
			}
		//This is where all joystick events are subscribed
		virtual void BindAdditionalEventControls(bool Bind);
		virtual void BindAdditionalUIControls(bool Bind, void *joy, void *key);

		void SetDriveExternalPWMSpeedControllerHook(std::function<void *(size_t, size_t, const char *, const char*, bool &)> callback);
		const char *HandlePWMHook_GetActiveName(const char *Name);
		virtual void ResetPos();
		Entity2D_Kind::EventMap *GetEventMap() { return m_pParent->GetEventMap(); }
protected:
		const Tank_Robot2_Props &GetTankRobotProps() const {return m_TankRobotProps->GetTankRobotProps();}
		//this may need to be overridden for robots that need it on for certain cases like 2012 needing it on for low gear
		//virtual bool GetUseAgressiveStop() const;

	private:
		RobotCommon * const m_pParent;
		const Tank_Robot2_Properties *m_TankRobotProps; //cached in the Initialize from specific robot
		std::shared_ptr<Tank_Robot2_Control> m_DriveControl;
		std::shared_ptr<Tank_Robot2_Ancillary> m_Ancillary;

		//This velocity is captured from the controller in the form of left/right velocity, even FPS controls will be translated here
		Vec2d m_Controller_Voltage;
		Vec2d m_External_Voltage;  //this is used by auton
	public:
};

