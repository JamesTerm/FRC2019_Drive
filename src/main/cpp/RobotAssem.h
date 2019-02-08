#pragma once
class RobotAssem_Internal; //forward declare... 
class Ship_Tester;

class RobotAssem
{
public:
    //Setup path to load robot lua, must be a valid path and name
	void RobotAssem_init(const char *RobotLua, Configuration::ActiveCollection *Collection=nullptr);
	//Provide updates from main loop
	void Update(double dTime_s);
	#ifdef _Win32
	//Give simulation parent access to robot
	Ship_Tester *GetRobot();
	#endif
private:
	std::shared_ptr<RobotAssem_Internal> m_p_RobotAssem; //a pointer to implementation idiom (using shared_ptr allows declaration to be hidden from destructor)
};
