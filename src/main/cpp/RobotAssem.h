#pragma once
class RobotAssem_Internal; //forward declare... 
class RobotCommon;

//Temporary during the transition of using LUA
//#define __UseXMLConfig__

class RobotAssem
{
public:
    //Setup path to load robot lua, must be a valid path and name
	void RobotAssem_init(const char *RobotLua,Framework::Base::EventMap *pEventMap, Configuration::ActiveCollection *Collection=nullptr);
	//Provide updates from main loop
	void Update(double dTime_s);
	#ifdef _Win32
	//Give simulation parent access to robot
	RobotCommon *GetRobot();
	#endif
	bool get_using_ac_drive() const;
	bool get_using_ac_operator() const;
	bool get_using_ac_elevator() const;
private:
	std::shared_ptr<RobotAssem_Internal> m_p_RobotAssem; //a pointer to implementation idiom (using shared_ptr allows declaration to be hidden from destructor)
};
