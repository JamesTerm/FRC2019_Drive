#pragma once

class FRC2019_Robot_Properties;
class FRC2019_Control_Interface :	//public Tank_Drive_Control_Interface,
									public Robot_Control_Interface,
									public Rotary_Control_Interface
{
public:
	//This is primarily used for updates to dashboard and driver station during a test build
	virtual void Robot_Control_TimeChange(double dTime_s)=0;
	//We need to pass the properties to the Robot Control to be able to make proper conversions.
	//The client code may cast the properties to obtain the specific data 
	virtual void Initialize(const FRC2019_Robot_Properties *props)=0;
	#ifdef Robot_TesterCode
	virtual void BindAdditionalEventControls(bool Bind,GG_Framework::Base::EventMap *em,IEvent::HandlerList &ehl)=0;
	#endif
};

struct FRC2019_Robot_Props
{
public:
	double hatch_deploy_time;
	double cargo_deploy_time;
	double hatch_grab_deploy_time;

	double cargo1_height;
	double cargo2_height;
	double cargo3_height;

	struct Autonomous_Properties
	{
		size_t AutonTest;
		//void ShowAutonParameters(); //This will show SmartDashboard variables if ShowParameters is true
		bool ShowParameters;   //If true ShowAutonParameters will populate SmartDashboard with autonomous parameters
	} Autonomous_Props;

	//Manipulate time slice logic of which system to use with these
	bool using_ac_drive = false;     
	bool using_ac_operator = false;
	bool using_ac_elevator = false;
};

class FRC2019_Robot_Properties
{
	public:
		FRC2019_Robot_Properties();
		virtual void LoadFromScript(Scripting::Script& script);
		
		const Rotary_Pot_Properties &GetArmProps() const {return m_ArmProps;}
		const Rotary_Properties &GetClawProps() const {return m_ClawProps;}
		const FRC2019_Robot_Props &GetFRC2019RobotProps() const {return m_FRC2019RobotProps;}
		FRC2019_Robot_Props &GetFRC2019RobotProps_rw() { return m_FRC2019RobotProps; }
		const Tank_Robot2_Properties &GetDrive_Properties() const { return m_driveProps; }
		const LUA_Controls_Properties &Get_RobotControls() const {return m_RobotControls;}
		virtual const char *SetUpGlobalTable(Scripting::Script& script) { return script.GetGlobalTable("Robot2019"); }
		//note derived class will populate these properties because of where it is in the script 
		const frc::Control_Assignment_Properties &Get_ControlAssignmentProps() const { return m_ControlAssignmentProps; }
	protected:
		frc::Control_Assignment_Properties m_ControlAssignmentProps;
	private:
		Tank_Robot2_Properties m_driveProps;
		Rotary_Pot_Properties m_ArmProps;
		Rotary_Properties m_ClawProps;
		FRC2019_Robot_Props m_FRC2019RobotProps;

		class ControlEvents : public LUA_Controls_Properties_Interface
		{
			protected: //from LUA_Controls_Properties_Interface
				virtual const char *LUA_Controls_GetEvents(size_t index) const; 
		};
		static ControlEvents s_ControlsEvents;
		LUA_Controls_Properties m_RobotControls;

};


const char * const csz_FRC2019_Robot_SpeedControllerDevices_Enum[] =
{
	"arm","roller"
};

//Note: rotary systems share the same index as their speed controller counterpart
//this can be any kind of sensor (e.g. pot, encoder etc) that get resolved in robot control
const char * const csz_FRC2019_Robot_AnalogInputs_Enum[] =
{
	"arm_pot"
};

const char * const csz_FRC2019_Robot_Solenoid_Enum[] =
{
	"wedge","intake","hatch","hatch_grab"
};

const char * const csz_FRC2019_Robot_BoolSensorDevices_Enum[] =
{
	"elevator_min","elevator_max"
};

///This is a specific robot that is a robot common and is composed of an arm, it provides addition methods to control the arm, and applies updates to
///the Robot_Control_Interface
class FRC2019_Robot : public RobotCommon
{
	public:
		enum SolenoidDevices
		{
			eWedgeDeploy,
			eIntakeDeploy,
			eHatchDeploy,
			eHatchGrabDeploy
		};
		enum SpeedControllerDevices
		{
			eArm,eRoller
		};
		static SpeedControllerDevices GetSpeedControllerDevices_Enum(const char *value)
		{
			return Enum_GetValue<SpeedControllerDevices>(value, csz_FRC2019_Robot_SpeedControllerDevices_Enum, _countof(csz_FRC2019_Robot_SpeedControllerDevices_Enum));
		}
		enum AnalogInputs
		{
			eArmPot
		};
		static AnalogInputs GetAnalogInputs_Enum(const char *value)
		{
			return Enum_GetValue<AnalogInputs>(value, csz_FRC2019_Robot_AnalogInputs_Enum, _countof(csz_FRC2019_Robot_AnalogInputs_Enum));
		}

		static SolenoidDevices GetSolenoidDevices_Enum(const char *value)
		{
			return Enum_GetValue<SolenoidDevices>(value, csz_FRC2019_Robot_Solenoid_Enum, _countof(csz_FRC2019_Robot_Solenoid_Enum));
		}

		enum BoolSensorDevices
		{
			eElevatorMin,
			eElevatorMax
		};

		static BoolSensorDevices GetBoolSensorDevices_Enum(const char *value)
		{
			return Enum_GetValue<BoolSensorDevices>(value, csz_FRC2019_Robot_BoolSensorDevices_Enum, _countof(csz_FRC2019_Robot_BoolSensorDevices_Enum));
		}

		FRC2019_Robot(const char EntityName[],FRC2019_Control_Interface *robot_control,bool UseEncoders=false);
		IEvent::HandlerList ehl;
		virtual void Initialize(Entity2D_Kind::EventMap& em, const FRC2019_Robot_Properties *props=NULL);
		virtual void ResetPos();
		virtual void TimeChange(double dTime_s);
		void CloseDeploymentDoor(bool Close);

		const FRC2019_Robot_Properties &GetRobotProps() const;
		FRC2019_Robot_Props::Autonomous_Properties &GetAutonProps();

		//Keep ability to share the drive resources with extenal allocators
		void SetDriveExternalPWMSpeedControllerHook(std::function<void *(size_t, size_t, const char *, const char*,bool &)> callback) 
		{	m_drive.SetDriveExternalPWMSpeedControllerHook(callback);
		}
		const char *HandlePWMHook_GetActiveName(const char *Name)
		{	return m_drive.HandlePWMHook_GetActiveName(Name);
		}
		//TODO test roller using is angular to be true
		class Robot_Claw : public Rotary_Velocity_Control
		{
			public:
				Robot_Claw(FRC2019_Robot *parent,Rotary_Control_Interface *robot_control);
				IEvent::HandlerList ehl;
				//Using meaningful terms to assert the correct direction at this level
				void Grip(bool on);
				void Squirt(bool on);
			protected:
				//Intercept the time change to send out voltage
				virtual void TimeChange(double dTime_s);
				virtual void BindAdditionalEventControls(bool Bind);
			private:
				#ifndef _Win32
				typedef Rotary_Velocity_Control __super;
				#endif
				//events are a bit picky on what to subscribe so we'll just wrap from here
				void SetRequestedVelocity_FromNormalized(double Velocity) {__super::SetRequestedVelocity_FromNormalized(Velocity);}
				FRC2019_Robot * const m_pParent;
				bool m_Grip,m_Squirt;
		};
		class Robot_Arm_Manager;  //forward declare
		class Robot_Arm : public Rotary_Position_Control
		{
			public:
				Robot_Arm(FRC2019_Robot *parent,Rotary_Control_Interface *robot_control);
				IEvent::HandlerList ehl;
				virtual void ResetPos();
				void Initialize(Base::EventMap& em, const Entity1D_Properties *props);  //initialize Robot_Arm_Manager once we have the eventmap
			protected:
				//Intercept the time change to obtain current height as well as sending out the desired velocity
				virtual void BindAdditionalEventControls(bool Bind);
				//events are a bit picky on what to subscribe so we'll just wrap from here... also great place for breakpoint
				void SetRequestedVelocity_FromNormalized(double Velocity);

				void SetPotentiometerSafety(bool DisableFeedback) {__super::SetPotentiometerSafety(DisableFeedback);}
				virtual void TimeChange(double dTime_s);
				//limit switch implementation
				virtual bool DidHitMinLimit() const;
				virtual bool DidHitMaxLimit() const;
			private:
				#ifndef _Win32
				typedef Rotary_Position_Control __super;
				#endif
				friend Robot_Arm_Manager;
				FRC2019_Robot * const m_pParent;
				bool m_Advance, m_Retract;
				std::shared_ptr<Robot_Arm_Manager> m_RobotArmManager; //pointer to implementation
		};

		//Accessors needed for setting goals
		Robot_Arm &GetArm() {return m_Arm;}
		Robot_Claw &GetClaw() {return m_Claw;}
		//Allow external code to switch the default goal
		void SetTestAutonCallbackGoal(std::function<Goal *(FRC2019_Robot *Robot)> callback) { m_TestAutonGoalCallback = callback; }
	protected:  //from RobotCommon
		virtual void BindAdditionalEventControls(bool Bind);
		virtual void BindAdditionalUIControls(bool Bind, void *joy, void *key);
		virtual bool IsDriverMoving() const { return m_drive.IsDriverMoving(); }
	private:
		Tank_Robot2 m_drive = this;  //we are agregatting instead of inheriting
		FRC2019_Robot_Properties m_RobotProps;
		#ifndef _Win32
		typedef  RobotCommon __super;
		#endif
		FRC2019_Control_Interface * const m_RobotControl;
		Robot_Arm m_Arm;
		Robot_Claw m_Claw;
		bool m_VoltageOverride;  //when true will kill voltage
		std::function<Goal *(FRC2019_Robot *Robot)> m_TestAutonGoalCallback=nullptr;  //pointer to function to obtain goal to use for TestAutonomous to use
		//No longer are these restricted to simulation
		void TestAutonomous();
		void StopAuton(bool isOn);
		void GoalComplete();
		bool m_SmartDashboard_AutonTest_Valve = false; //Value used to detect change of AutonTest CheckBox
};


namespace FRC2019_Goals
{
	Goal *Get_FRC2019_Autonomous(FRC2019_Robot *Robot);
}

class FRC2019_Robot_Control : public frc::RobotControlCommon, public FRC2019_Control_Interface
{
	public:
		FRC2019_Robot_Control(bool UseSafety = true);
		virtual ~FRC2019_Robot_Control();
		//This is called per enabled session to enable (on not) things dynamically (e.g. compressor)
		void ResetPos();
		FRC2019_Control_Interface &AsControlInterface() { return *this; }
		const FRC2019_Robot_Properties &GetRobotProps() const { return m_RobotProps; }
		//Give access to set hooks in the drive as well
		//void SetDriveExternalPWMSpeedControllerHook(std::function<void *(size_t, size_t, const char *, const char*,bool &)> callback) 
		//{
		//	m_TankRobotControl.SetExternalPWMSpeedControllerHook(callback);
		//}
	protected: //from Robot_Control_Interface
		virtual void UpdateVoltage(size_t index, double Voltage);
		virtual bool GetBoolSensorState(size_t index) const;
		virtual void CloseSolenoid(size_t index,bool Close);
		virtual void OpenSolenoid(size_t index,bool Open) {CloseSolenoid(index,!Open);}
		virtual bool GetIsSolenoidOpen(size_t index) const;

	protected: //from Rotary Interface
		virtual void Reset_Rotary(size_t index=0); 
		//pacify this by returning its current value
		virtual double GetRotaryCurrentPorV(size_t index);
		virtual void UpdateRotaryVoltage(size_t index, double Voltage) { UpdateVoltage(index, Voltage); }

		protected: //from RobotControlCommon
			virtual size_t RobotControlCommon_Get_PWMSpeedController_EnumValue(const char *name) const
			{	return FRC2019_Robot::GetSpeedControllerDevices_Enum(name);
			}
			virtual size_t RobotControlCommon_Get_DigitalInput_EnumValue(const char *name) const
			{  return FRC2019_Robot::GetBoolSensorDevices_Enum(name);
			}
			virtual size_t RobotControlCommon_Get_AnalogInput_EnumValue(const char *name) const
			{	return FRC2019_Robot::GetAnalogInputs_Enum(name);
			}
			virtual size_t RobotControlCommon_Get_DoubleSolenoid_EnumValue(const char *name) const
			{	return FRC2019_Robot::GetSolenoidDevices_Enum(name);
			}

	protected: //from FRC2019_Control_Interface
		//Will reset various members as needed (e.g. Kalman filters)
		virtual void Robot_Control_TimeChange(double dTime_s);
		virtual void Initialize(const FRC2019_Robot_Properties *props);

	protected:
		FRC2019_Robot_Properties m_RobotProps;  //saves a copy of all the properties
		//Tank_Robot_Control m_TankRobotControl;
		//Tank_Drive_Control_Interface * const m_pTankRobotControl;  //This allows access to protected members
		frc::Compressor *m_Compressor=nullptr;
		//frc::Accelerometer *m_RoboRIO_Accelerometer=nullptr;   <---for reference
		//Base::EventMap* m_EventMap=nullptr;  <---TODO see if we need this

	private:
		double GetPotValue(size_t index);
		//Note: these may be arrayed if we have more pots
		KalmanFilter m_KalFilter_Arm;
		Averager<double, 5> m_Averager_Arm;
		#ifdef _Win32
		Potentiometer_AM2619_3590 m_Potentiometer; //simulate a real potentiometer for calibration testing
		#endif
		bool m_FirstRun = false;
		bool m_ElevatorMin, m_ElevatorMax;
};

