#include "pch.h"
#include "Robot_Tester.h"

//#define __TestGoalsDirect__
void SetCommandPromptCallback(std::function<std::string(void)> callback);  //allow this code to manipulate the command prompt
std::string DefaultCommandPrompt();  //set back for proper closing

#ifdef __TestGoalsDirect__ 
#include "../main/cpp/AutonMain.h"

class RobotTester_Internal
{
private:
	bool m_IsStreaming=false;
	std::future<void> m_TaskState;
	AutonMain m_Robot;
public:
	bool GetIsStreaming() const { return m_IsStreaming; }
	void InitRobot()
	{
		SmartDashboard::init();  //put here for this environment (needs to be in the most root area)... if we switch to execute robot this will be moved
		m_Robot.AutonMain_init("../main/cpp/FRC2019Robot.lua");
	}

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
	}

	//Give command ability to switch to different set of goals
	void HookSampleGoals()
	{
		m_Robot.Test("hook_samples");
	}

	//Keeping around to show benefit of lambda 
	//void operator() (const void*); //TODO work out how to call this directly
};


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

//Keeping this around to demonstraight the benefit of using a lambda
#if 0
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
#endif

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
			[](RobotTester_Internal *Instance)
		{
			printf("starting task_proc()\n");
			while (Instance->m_IsStreaming)
			{
				const double synthetic_delta = 0.01;
				Instance->m_Robot.Update(synthetic_delta);
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

void RobotTester::Test(int test)
{
	//time_type Test = time_type::get_current_time();
	m_p_RobotTester->InitRobot();
	if (test == 1)
		m_p_RobotTester->HookSampleGoals();
	//might as well start it
	m_p_RobotTester->StartStreaming();
}

//Not used for this mode
void RobotTester::SetGameMode(int mode)
{
}

#else

//This iteration works with the main robot code
#include "../main/cpp/Robot.h"
#pragma comment (lib,"x64/debug/RobotLibraries")
#pragma comment (lib,"x64/debug/TeleopLibraries")

const char * const csz_GameMode_Enum[] =
{
	"Auton","TeleOp","Test"
};


class RobotTester_Internal
{
private:
	bool m_IsStreaming = false;
	std::future<void> m_TaskState;
	Robot m_Robot;  //what power :)
	enum GameMode
	{
		eAuton,
		eTeleop,
		eTest
	} m_GameMode=eTest;
public:
	RobotTester_Internal()
	{
		SetCommandPromptCallback(
			[&](void)
		{
			std::string ret = csz_GameMode_Enum[m_GameMode];
			ret += '>';
			return ret;
		}
		);
	}
	~RobotTester_Internal()
	{
		SetCommandPromptCallback(DefaultCommandPrompt);
		StopStreaming();
	}

	void InitRobot()
	{
		m_Robot.RobotInit();
		//Setup callbacks here
		m_Robot.SetIsEnabledCallback(
			[&](void)
			{
				return m_IsStreaming;
			});
		m_Robot.SetIsGameModeCallback(
			[&](void)
			{
				return (int)m_GameMode;  //probably pedantic to cast as an int
			});
	}

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

	//bool GetIsStreaming() const { return m_IsStreaming; }
	void SetGameMode(int mode)
	{	m_GameMode = (GameMode)mode;
	}
	//Give command ability to switch to different set of goals
	void HookSampleGoals()
	{	m_Robot.TestCommand("hook_samples");
	}
};


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

void RobotTester_Internal::StartStreaming()
{
	if (!m_IsStreaming)
	{
		m_IsStreaming = true;
		//TODO make a reallyAsync that doesn't require an instance
		m_TaskState = reallyAsync(
			[](RobotTester_Internal *Instance)
		{
			printf("starting task_proc()\n");
			switch (Instance->m_GameMode)
			{
			case RobotTester_Internal::eAuton:
				Instance->m_Robot.Autonomous();
				break;
			case RobotTester_Internal::eTeleop:
				Instance->m_Robot.OperatorControl();
				break;
			case RobotTester_Internal::eTest:
				Instance->m_Robot.Test();
				break;
			}
			printf("ending task_proc()\n");
		}
		, this);
	}
}


void RobotTester::RobotTester_init()
{
	m_p_RobotTester = std::make_shared<RobotTester_Internal>();
	m_p_RobotTester->InitRobot();  //go ahead and init
}


void RobotTester::Test(int test)
{
	SetGameMode(2);
	if (test == 1)
		m_p_RobotTester->HookSampleGoals();
	m_p_RobotTester->StartStreaming();
}

void RobotTester::SetGameMode(int mode)
{
	m_p_RobotTester->SetGameMode(mode);
}

#endif

void RobotTester::StartStreaming()
{
	m_p_RobotTester->StartStreaming();
}
void RobotTester::StopStreaming()
{
	m_p_RobotTester->StopStreaming();
}
