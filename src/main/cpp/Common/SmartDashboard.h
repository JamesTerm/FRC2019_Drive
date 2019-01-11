#pragma once
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
