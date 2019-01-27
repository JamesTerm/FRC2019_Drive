#include "FRC2019_Goals.h"

using namespace std;

/////////////////////////Goal_Timer/////////////////////////
void Goal_Timer::Activate()
{
    m_Status = eActive;

}
Goal::Goal_Status Goal_Timer::Process(double dTime)
{
    if (m_Status == eActive)
    {
        m_currentTime += dTime;
        if (m_currentTime >= m_timeOut)
        {
            Terminate();
            return eCompleted;
        }
        
        return eActive;
    }
    else
    {
        m_Status = eInactive;
        return m_Status;
    }
}
void Goal_Timer::Terminate()
{
    m_Status = eCompleted;
}
/////////////////////////Goal_DriveWithTimer/////////////////////////
Goal::Goal_Status Goal_DriveWithTimer::Process(double dTime)
{
    if (m_Status == eActive)
    {
        m_currentTime += dTime;
        SetDrive(m_leftSpeed, m_rightSpeed, m_activeCollection);
        if (m_currentTime >= m_timeOut)
        {

            
            Terminate();
            return eCompleted;
        }
        return eActive;
    }
    else
    {
        return m_Status;
    }
}

void Goal_DriveWithTimer::Terminate()
{
    StopDrive(m_activeCollection);
    m_Status = eCompleted;
}
//TODO first thing tomorrow (1/26) check to see if code works in order
/////////////////////////Goal_WaitThenDrive/////////////////////////
void Goal_WaitThenDrive::Activate()
{
   
    //TODO dont be backwards
    AddSubgoal(new Goal_DriveWithTimer(m_activeCollection, m_leftSpeed, m_rightSpeed, m_driveTime));
    AddSubgoal(new Goal_Timer(m_activeCollection, m_waitTime));
    m_Status = eActive;
}

/////////////////////////Goal_Turn/////////////////////////
void Goal_Turn::Activate()
{
    Goal_Timer::Activate();
    m_navx->Reset();
}

Goal::Goal_Status Goal_Turn::Process(double dTime)
{
    if (m_Status == eActive)
    {
        m_currentTime += dTime;
        if (m_currentTime > m_timeOut)
            return eFailed;

        //TODO PID
        m_power = (m_angle - m_navx->GetAngle()) / 90; //temporary proportional scale for angle.
        if (m_power > 1.0)
            m_power = 1.0; //temp
        else if (m_power < -1.0)
            m_power = -1.0; //temp

        if (m_navx->GetAngle() < m_angle)
        {
            SetDrive(m_power, -m_power, m_activeCollection);
            return eActive;
        }
        else
        {
            Terminate();
            return eCompleted;
        }
    }
    else
    {
        return m_Status;
    }
}

void Goal_Turn::Terminate()
{
    StopDrive(m_activeCollection);
    m_Status = eInactive;
}

/////////////////////////Goal_DriveStraight/////////////////////////
void Goal_DriveStraight::Activate()
{
    Goal_Timer::Activate();
    m_encLeft->Reset();
    m_encRight->Reset();
}

Goal::Goal_Status Goal_DriveStraight::Process(double dTime)
{
    if (m_Status = eActive)
    {
        m_currentTime += dTime;
        if (m_currentTime > m_timeOut)
            return eFailed;
        //TODO PID
        // double error = 
    }
    else
    {
        return m_Status;
    }
    
}

void Goal_DriveStraight::Terminate()
{
    StopDrive(m_activeCollection);
    m_Status = eInactive;
}