#pragma once
class AutonMain_Internal; //forward declare... 

class AutonMain
{
public:
    //Setup path to load robot lua, must be a valid path and name
    //TODO add active collection to hook into robot control
	void AutonMain_init(const char *RobotLua);
	//Provide updates from main loop
	void Update(double dTime_s);
private:
	std::shared_ptr<AutonMain_Internal> m_p_AutonMain; //a pointer to implementation idiom (using shared_ptr allows declaration to be hidden from destructor)
};
