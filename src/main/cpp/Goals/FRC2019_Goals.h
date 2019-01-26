#include <iostream>
#include "Goal.h"

/* Goal_Timer
 * This goal is a basic goal that will fail after requested amount of time.
 * Other goals should extend from this goal to implement time out features.
 * This goal will also be used to track the time remaining during auton (besides 2019 sandstorm lol... this may be different)
 * This goal also requires active collection because all goals extending it will require it.
 */
class Goal_Timer : public AtomicGoal
{
  public:
    Goal_Timer(ActiveCollection *activeCollection, double timeOut)
    {
        m_Status = eInactive;
        m_timeOut = timeOut;
        m_activeCollection = activeCollection;
        m_currentTime = 0;
    }
    virtual void Activate();
    virtual Goal::Goal_Status Process(double dTime);
    virtual void Terminate();

  protected:
    ActiveCollection *m_activeCollection;
    double m_currentTime;
    double m_timeOut;
};

/* Goal_DriveWithTimer
 * This goal implements Goal_Timer to drive at specified speed until time runs out.
 * This is the only goal that will return eCompleted after a time out
 */
class Goal_DriveWithTimer : public Goal_Timer
{
  public:
    Goal_DriveWithTimer(ActiveCollection *activeCollection, double leftSpeed, double rightSpeed, double timeOut) : Goal_Timer(activeCollection, timeOut)
    {
        cout << "constructor " << m_timeOut << endl;
        m_leftSpeed = leftSpeed;
        m_rightSpeed = rightSpeed;
    }
    virtual Goal::Goal_Status Process(double dTime);
    void Terminate();

  private:
    double m_leftSpeed, m_rightSpeed;
};

/* Goal_WaitThenDrive
 * This is just a test composite goal. Unlikely it will be used IRL
 */
class Goal_WaitThenDrive : public Generic_CompositeGoal
{
  public:
    Goal_WaitThenDrive(ActiveCollection *activeCollection, double leftSpeed, double rightSpeed, double waitTime, double driveTime) : Generic_CompositeGoal(activeCollection)
    {
        m_activeCollection = activeCollection;
        AddSubgoal(new Goal_Timer(m_activeCollection, waitTime));
        AddSubgoal(new Goal_DriveWithTimer(m_activeCollection, leftSpeed, rightSpeed, driveTime));
    }
    virtual void Activate() {m_Status = eActive;}
    Goal::Goal_Status Process(double dTime) {return Generic_CompositeGoal::Process(dTime);}
};

/* Goal_Turn
 * This goal uses the navx to turn to a specified degree
 * 
 */
//TODO PID implemenation
class Goal_Turn : public Goal_Timer
{
  public:
    Goal_Turn(ActiveCollection *activeCollection, double angle, double timeOut) : Goal_Timer(activeCollection, timeOut)
    {
        m_angle = angle;
        m_navx = m_activeCollection->GetNavX();
    }
    Goal_Turn(ActiveCollection *activeCollection, double angle) : Goal_Turn(activeCollection, angle, 3.0) {}
    virtual void Activate();
    virtual Goal::Goal_Status Process(double dTime);
    virtual void Terminate();

  private:
    double m_angle;
    double m_power;
    NavX *m_navx;
};

/* Goal_DriveStraight
 * This goal uses encoder to drive a specified amount forward and navx to stay straight
 */
class Goal_DriveStraight : public Goal_Timer
{
  public:
    Goal_DriveStraight(ActiveCollection *activeColelction, double dist, double power, double timeOut) : Goal_Timer(activeColelction, timeOut)
    {
        m_dist = dist;
        m_encLeft = m_activeCollection->GetEncoder("enc0");
        m_encRight = m_activeCollection->GetEncoder("enc1");
        m_leftPower = m_rightPower = power;
    }
    Goal_DriveStraight(ActiveCollection *activeCollection, double dist, double power) : Goal_DriveStraight(activeCollection, dist, power, 7.0) {}
    virtual void Activate();
    virtual Goal::Goal_Status Process(double dTime);
    virtual void Terminate();

  private:
    double m_dist;
    double m_leftPower, m_rightPower;
    EncoderItem *m_encLeft, *m_encRight;
};