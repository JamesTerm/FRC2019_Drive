#pragma once

#ifdef _Win32
//https://stackoverflow.com/questions/2132747/warning-c4251-when-building-a-dll-that-exports-a-class-containing-an-atlcstrin
#pragma warning ( disable : 4251 )

#include <map>
#include <string>
//we could include this if Sendable.h would declare the iTable class without including it
//#include "SmartDashboard/NamedSendable.h"
//#include "NetworkTables/cpp/include/src/main/include/networktables2/type/ComplexData.h"
//#include "NetworkTables/cpp/include/src/main/include/ErrorBase.h"

class NamedSendable
{
public:

	/**
	 * @return the name of the subtable of SmartDashboard that the Sendable object will use
	 */
	virtual std::string GetName() = 0;
};

class ITable;
class Sendable;

class SmartDashboard //: public SensorBase
{
public:
	static void init();
	static void shutdown();

	static void PutData(std::string key, Sendable *data);
	static void PutData(NamedSendable *value);
	//static Sendable* GetData(std::string keyName);

	static void PutBoolean(std::string keyName, bool value);
	static bool GetBoolean(std::string keyName);

	static void PutNumber(std::string keyName, double value);
	static double GetNumber(std::string keyName);

	static void PutString(std::string keyName, std::string value);
	static int GetString(std::string keyName, char *value, unsigned int valueLen);
	static std::string GetString(std::string keyName);

	//static void PutValue(std::string keyName, ComplexData& value);
	//static void RetrieveValue(std::string keyName, ComplexData& value);

	///set that network tables should be a client This must be called before initialize or GetTable
	static void SetClientMode();
	///set that network tables should be a server This must be called before initialize or GetTable
	static void SetServerMode();
	///set the team the robot is configured for (this will set the ip address that network tables will connect to in client mode)
	///This must be called before initialize or GetTable @param team the team number
	static void SetTeam(int team);
	/// @param address the address that network tables will connect to in client mode
	static void SetIPAddress(const char* address);
private:
	SmartDashboard();
	//virtual ~SmartDashboard();
	//DISALLOW_COPY_AND_ASSIGN(SmartDashboard);

	/** The {@link NetworkTable} used by {@link SmartDashboard} */
	static ITable* m_table;

	/**
	 * A map linking tables in the SmartDashboard to the {@link SmartDashboardData} objects
	 * they came from.
	 */
	static std::map<ITable *, Sendable *> m_tablesToData;
};

#else
#include "frc/SmartDashboard/SmartDashboard.h"

namespace SmartDashboard
{
inline void init()
{
    frc::SmartDashboard::init();
}

inline bool PutBoolean(wpi::StringRef keyName, bool value)
{
    return frc::SmartDashboard::PutBoolean(keyName,value);
}
inline bool PutNumber(wpi::StringRef keyName, double value)
{
    return frc::SmartDashboard::PutNumber(keyName,value);
}
inline bool PutString(wpi::StringRef keyName, wpi::StringRef value)
{
    return frc::SmartDashboard::PutString(keyName,value);
}

inline bool SetDefaultBoolean(wpi::StringRef key, bool defaultValue)
{
    return frc::SmartDashboard::SetDefaultBoolean(key,defaultValue);
}

inline bool SetDefaultNumber(wpi::StringRef key, double defaultValue)
{
    return frc::SmartDashboard::SetDefaultNumber(key,defaultValue);
}

//These have been depreciated in FRC, but we still support them by wrapping to the newer methods
inline bool GetBoolean(const char *keyName)
{
    return frc::SmartDashboard::GetBoolean(keyName,false);
}
inline double GetNumber(const char *keyName)
{
    return frc::SmartDashboard::GetNumber(keyName,0.0);
}
inline std::string GetString(const char *keyName)
{
    return frc::SmartDashboard::GetString(keyName,"");
}
}
#endif