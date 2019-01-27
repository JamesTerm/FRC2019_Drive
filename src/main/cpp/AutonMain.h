#pragma once
class AutonMain_Internal; //forward declare... 
class Ship_Tester;

class AutonMain
{
public:
    //Setup path to load robot lua, must be a valid path and name
	void AutonMain_init(const char *RobotLua, Configuration::ActiveCollection *Collection=nullptr);
	//Provide updates from main loop
	void Update(double dTime_s);
	#ifdef _Win32
	//Give simulation parent access to robot
	Ship_Tester *GetRobot();
	#endif
private:
	std::shared_ptr<AutonMain_Internal> m_p_AutonMain; //a pointer to implementation idiom (using shared_ptr allows declaration to be hidden from destructor)
};
