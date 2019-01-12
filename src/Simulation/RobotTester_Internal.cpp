#include "pch.h"
#include "Robot_Tester.h"


class RobotTester_Internal
{
private:
	bool m_IsStreaming=false;
	std::future<void> m_TaskState;

public:
	bool GetIsStreaming() const { return m_IsStreaming; }


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
	void operator() (const void*); //TODO work out how to call this directly
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


void RobotTester_Internal::operator() (const void*)
{
	static int test = 0;
	printf("testing %d\n", test++);
	MySleep(1);
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
	if (!m_IsStreaming)
	{
		m_IsStreaming = true;
		m_TaskState = reallyAsync(task_proc, this);
	}
}


void RobotTester::RobotTester_init()
{
	m_p_RobotTester = std::make_shared<RobotTester_Internal>();
}

//Testing static library access
#include "../main/cpp/Base/Time_Type.h"
#pragma comment (lib,"x64/debug/RobotLibraries")


void RobotTester::Test()
{
	time_type Test = time_type::get_current_time();
	//TODO instantiate robot here
	//might as well start it
	m_p_RobotTester->StartStreaming();
}
