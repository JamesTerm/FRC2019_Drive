/****************************** Header ******************************\
Class Name: (multiple classes)
File Name: FRC2019_Goals.h
Summary: All implemented Goals go here. Region statements categorize
types
Project: BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Chris Weeks
Email: chrisrweeks@aol.com
\********************************************************************/
#pragma once

#include <iostream>
#include "Goal.h"
#include "../Util/Units/Distances.h"

//?HINT ctrl+k then ctrl+0 will collapse all regions
//?ctrl+k then ctrl+j will uncollapse all regions

//Atomic Goals go in this region
#pragma region AtomicGoals

  //Time based goals go in this region
  #pragma region TimerGoals
    /* Goal_Wait
    * This goal will wait a specified amount of time then return eCompleted. Use it for delay features.
    ! NOTE: This goal is different then Goal_TimeOut
    */














   //up for change
   class Goal_OneHatch : public CompositeGoal
   {
     public: 
      Goal_OneHatch(ActiveCollection *activeCollection, double timeOut)
     {
       m_timeOut = timeOut;
     }

     virtual void Activate();
     private:
       
       ActiveCollection *m_activeCollection;
             double m_timeOut;

   };




   class Goal_Hatch : public AtomicGoal
   {
     public:
     Goal_Hatch(ActiveCollection *activeCollection, double timeOut)
     {
       m_Status = eActive;
       m_timeOut = timeOut;
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

   
  





   //gabe stuff^^

















    class Goal_Wait : public AtomicGoal
    {
    public:
      Goal_Wait(ActiveCollection *activeCollection, double timeOut)
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
    /* Goal_TimeOut
     * This goal will wait a specified amount of time then return eFailed. Use to time out a goal if it takes too long
     ! NOTE: This goal is different than Goal_Wait
     */
    class Goal_TimeOut : public Goal_Wait
    {
      public:
      Goal_TimeOut(ActiveCollection *activeCollection, double timeOut) : Goal_Wait(activeCollection, timeOut) {}
      virtual Goal::Goal_Status Process(double dTime);
      void Terminate();
    };
    /* Goal_DriveWithTimer
    * This goal implements Goal_Wait to drive at specified speed until time runs out.
    */
    class Goal_DriveWithTimer : public Goal_Wait
    {
    public:
      Goal_DriveWithTimer(ActiveCollection *activeCollection, double leftSpeed, double rightSpeed, double timeOut) : Goal_Wait(activeCollection, timeOut)
      {
        m_leftSpeed = leftSpeed;
        m_rightSpeed = rightSpeed;
      }
      virtual Goal::Goal_Status Process(double dTime);
      void Terminate();

    private:
      double m_leftSpeed, m_rightSpeed;
    };
  #pragma endregion

  //Goals that use data to determine completion go here
  #pragma region FeedbackLoopGoals
    /* Goal_Turn
    * This goal uses the navx to turn to a specified degree
    * 
    */
    //TODO PID implemenation
    class Goal_Turn : public Goal_Wait
    {
    public:
      Goal_Turn(ActiveCollection *activeCollection, double angle, double timeOut) : Goal_Wait(activeCollection, timeOut)
      {
        m_target = angle;
        m_navx = m_activeCollection->GetNavX();
      }
      Goal_Turn(ActiveCollection *activeCollection, double angle) : Goal_Turn(activeCollection, angle, 3.0) {} //default timeOut is 3 seconds
      virtual void Activate();
      virtual Goal::Goal_Status Process(double dTime);
      virtual void Terminate();

    private:
      double m_target;
      double m_power;
      const double m_freedom = 5; //code can be within +/- 5 degrees of target
      NavX *m_navx;

      double kp = 0, ki = 0, kd = 0;
      double bias = 0; //bias is a constant that is added to the output. We most likely will not use it but it is here if needed.
      double propError, integ, deriv;
      double errorPrior;
    };

    /* Goal_DriveStraight
    * This goal uses encoder to drive a specified amount forward and navx to stay straight
    */
    class Goal_DriveStraight : public Goal_Wait
    {
    public:
      //*standard constructor
      Goal_DriveStraight(ActiveCollection *activeColelction, double dist, double power, double timeOut) : Goal_Wait(activeColelction, timeOut)
      {
        m_distTarget = dist;
        m_encLeft = m_activeCollection->GetEncoder("enc0");
        m_encRight = m_activeCollection->GetEncoder("enc1");
        m_leftPower = m_rightPower = power;
      }
      //*optional constructor without timeOut, calls standard with default timeOut value
      Goal_DriveStraight(ActiveCollection *activeCollection, double dist, double power) : Goal_DriveStraight(activeCollection, dist, power, 7.0) {}
      //*constructor that takes a distance object as a parameter
      Goal_DriveStraight(ActiveCollection *activeCollection, Distance *dist, double power, double timeOut) : Goal_DriveStraight(activeCollection, dist->inATick(), power, timeOut) {}
      //*constructor that takes distance object and uses default timeOut value
      Goal_DriveStraight(ActiveCollection *activeCollection, Distance *dist, double power) : Goal_DriveStraight(activeCollection, dist, power, 7.0) {}

      virtual void Activate();
      virtual Goal::Goal_Status Process(double dTime);
      virtual void Terminate();



    private:
      double m_distTarget;
      double m_distTraveled = 0;
      double m_leftPower, m_rightPower;
      EncoderItem *m_encLeft, *m_encRight;

      double kp = 0, ki = 0, kd = 0;
      double bias = 0; //bias is a constant that is added to the output. We most likely will not use it but it is here if needed.
      double error, integ, deriv;
      double errorPrior;
    };
  #pragma endregion
#pragma endregion

//Composite Goals go here
#pragma region CompositeGoals

  //utility-style goals go here
  #pragma region utility

  #pragma endregion

  //auton goals and other complex goals go here
  #pragma region complex
    /* Goal_WaitThenDrive
    * This is just a test composite goal. Unlikely it will be used IRL
    */
    class Goal_WaitThenDrive : public CompositeGoal
    {
    public:
      Goal_WaitThenDrive(ActiveCollection *activeCollection, double leftSpeed, double rightSpeed, double waitTime, double driveTime)
      {
        m_activeCollection = activeCollection;
        m_leftSpeed = leftSpeed;
        m_rightSpeed = rightSpeed;
        m_waitTime = waitTime;
        m_driveTime = driveTime;
      }
      virtual void Activate();

    private:
      ActiveCollection *m_activeCollection;
      double m_leftSpeed, m_rightSpeed, m_waitTime, m_driveTime;
    };
  #pragma endregion

#pragma endregion

//multitask goals go here
#pragma region MultitaskGoals

#pragma endregion