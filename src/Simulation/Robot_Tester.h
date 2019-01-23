#pragma once

class RobotTester_Internal; //forward declare... 

class RobotTester
{
public:
	void RobotTester_init(void);  // allows the declaration to remain here
	void Test(int test);
	void StartStreaming();
	void StopStreaming();
	void SetGameMode(int mode);  //currently 0=auton 1=tele and 2=test
private:
	std::shared_ptr<RobotTester_Internal> m_p_RobotTester; //a pimpl idiom (using shared_ptr allows declaration to be hidden from destructor)
};
