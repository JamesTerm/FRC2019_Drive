#include "pch.h"
#include "Robot_Tester.h"

//Stubbed out for now
class FRC_2019_Control : public FRC_2019_Control_Interface
{
public:
	//from Tank_Drive_Control_Interface,
	virtual void Tank_Drive_Control_TimeChange(double dTime_s) {}
	//We need to pass the properties to the Robot Control to be able to make proper conversions.
	//The client code may cast the properties to obtain the specific data 
	virtual void Initialize(const Entity_Properties *props) {}
	virtual void Reset_Encoders() {}

	//Encoders populate this with current velocity of motors
	virtual void GetLeftRightVelocity(double &LeftVelocity, double &RightVelocity) { LeftVelocity = RightVelocity = 0.0; }
	virtual void UpdateLeftRightVoltage(double LeftVoltage, double RightVoltage) {}

	//from Robot_Control_Interface,
	virtual void UpdateVoltage(size_t index, double Voltage) {}
	//Having both Open and Close makes it easier to make the desired call without applying the not operator
	virtual void OpenSolenoid(size_t index, bool Open = true) {}
	virtual void CloseSolenoid(size_t index, bool Close = true) { OpenSolenoid(index, !Close); }
	virtual bool GetIsSolenoidOpen(size_t index) const { return false; }
	virtual bool GetIsSolenoidClosed(size_t index) const { return !GetIsSolenoidOpen(index); }
	/// \ret true if contact is made 
	virtual bool GetBoolSensorState(size_t index) const { return false; }

	//from  Rotary_Control_Interface
	virtual void Reset_Rotary(size_t index = 0) {}
	virtual double GetRotaryCurrentPorV(size_t index = 0) { return 0.0; }
	virtual void UpdateRotaryVoltage(size_t index, double Voltage) {}

	//This is primarily used for updates to dashboard and driver station during a test build
	virtual void Robot_Control_TimeChange(double dTime_s) {}
	//We need to pass the properties to the Robot Control to be able to make proper conversions.
	//The client code may cast the properties to obtain the specific data 
	//virtual void Initialize(const Entity_Properties *props)	{}
};

class RobotTester_Internal
{
private:
	bool m_IsStreaming=false;
	std::future<void> m_TaskState;
	FRC_2019_Control m_Control;
	FRC2019_Robot_Properties m_RobotProps;
	FRC2019_Robot *m_pRobot=nullptr;
	//Framework::UI::JoyStick_Binder m_JoyBinder;
	UI_Controller *m_pUI;

	Framework::Base::EventMap m_EventMap;
public:
	bool GetIsStreaming() const { return m_IsStreaming; }
	void InitRobot();

	void StartStreaming();
	void StopStreaming()
	{
		if (m_IsStreaming)
		{
			m_IsStreaming = false;
			//join... wait for thread to close:
			size_t TimeOut = 0;
			while (m_TaskState.wait_for(std::chrono::milliseconds(500)) != std::future_status::ready)
			{
				printf("waiting %d\n", TimeOut++);  //put else where so I can monitor both
			}
			//Close any other resources here
		}
	}
	RobotTester_Internal()
	{
	}
	~RobotTester_Internal()
	{
		StopStreaming();
		delete m_pRobot;  //checks for null implicitly 
		m_pRobot = nullptr;
	}
	void operator() (const void*); //TODO work out how to call this directly
};

void RobotTester_Internal::InitRobot()
{
	SmartDashboard::init();
	const bool UseEncoders = false;
	m_pRobot = new FRC2019_Robot("Curivator_Robot", &m_Control, UseEncoders);
	{
		Framework::Scripting::Script script;
		script.LoadScript("../main/cpp/FRC2019Robot.lua", true);
		script.NameMap["EXISTING_ENTITIES"] = "EXISTING_SHIPS";
		m_RobotProps.SetUpGlobalTable(script);
		m_RobotProps.LoadFromScript(script);
		//m_Joystick.SetSlotList(m_RobotProps.Get_RobotControls());
		m_pRobot->Initialize(m_EventMap, &m_RobotProps);
	}
	//The script must be loaded before initializing control since the wiring assignments are set there
	//m_Control.AsControlInterface().Initialize(&m_RobotProps);
				//Bind the ship's eventmap to the joystick
	//m_JoyBinder.SetControlledEventMap(m_pRobot->GetEventMap());

	//To to bind the UI controller to the robot
	AI_Base_Controller *controller = m_pRobot->GetController();
	assert(controller);
	//m_pUI = new UI_Controller(m_JoyBinder, controller);
	m_pUI = new UI_Controller(nullptr, controller);
	if (controller->Try_SetUIController(m_pUI))
	{
		//Success... now to let the entity set things up
		m_pUI->HookUpUI(true);
	}
	else
	{
		m_pUI->Set_AI_Base_Controller(NULL);   //no luck... flush ship association
		assert(false);
	}

	//start in auton (can manage this later)
	SmartDashboard::PutBoolean("Test_Auton", true);
	SmartDashboard::PutNumber("AutonTest", 0.0);
}


template<typename F, typename... Ts>
inline std::future<typename std::result_of<F(Ts...)>::type> reallyAsync(F&& f, Ts&&... params) // return future for asynchronous call to f(params...)
{
	return std::async(std::launch::async, std::forward<F>(f), std::forward<Ts>(params)...);
}

__inline void MySleep(double Seconds)
{
	const int time_ms = (int)(Seconds*1000.0);
	std::this_thread::sleep_for(std::chrono::milliseconds(time_ms));
}


void RobotTester_Internal::operator() (const void*)
{
	//static int test = 0;
	//printf("testing %d\n", test++);
	const double synthetic_delta = 0.01;
	if (m_pRobot)
		m_pRobot->TimeChange(synthetic_delta);
	MySleep(synthetic_delta);
}

void task_proc(RobotTester_Internal *instance)
{
	printf("starting task_proc()\n");
	void *dummy_ptr = nullptr;
	while (instance->GetIsStreaming())
	{
		//no spin management here:... delegate to client
		(*instance)(dummy_ptr);
	}
	printf("ending task_proc()\n");
}

void RobotTester_Internal::StartStreaming()
{
	//Here is an example where lambda is useful, this avoids the need to pass an instance and call both the operator() and task_proc
	//I've left them it to compare
	#if 0
	if (!m_IsStreaming)
	{
		m_IsStreaming = true;
		m_TaskState = reallyAsync(task_proc, this);
	}
	#else
	if (!m_IsStreaming)
	{
		m_IsStreaming = true;
		//TODO make a reallyAsync that doesn't require an instance
		m_TaskState = reallyAsync(
			[&](void *dummy)
		{
			printf("starting task_proc()\n");
			void *dummy_ptr = nullptr;
			while (m_IsStreaming)
			{
				const double synthetic_delta = 0.01;
				if (m_pRobot)
					m_pRobot->TimeChange(synthetic_delta);
				MySleep(synthetic_delta);
			}
			printf("ending task_proc()\n");
		}
		, this);
	}
	#endif
}


void RobotTester::RobotTester_init()
{
	m_p_RobotTester = std::make_shared<RobotTester_Internal>();
}

#include "../main/cpp/Base/Time_Type.h"
#pragma comment (lib,"x64/debug/RobotLibraries")

//using namespace Framework::Base;

void RobotTester::Test()
{
	//time_type Test = time_type::get_current_time();
	m_p_RobotTester->InitRobot();
	//might as well start it
	m_p_RobotTester->StartStreaming();
}
