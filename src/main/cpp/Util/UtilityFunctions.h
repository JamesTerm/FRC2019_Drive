/****************************** Header ******************************\
Class Name: -
File Name: UtilityFunctions.h
Summary: File of static utility functions for auto use.
Project: FRC2019CPP
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ryan Cooper, Dylann Ruiz
Comments added by Chris Weeks
Email: cooper.ryan@centaurisoft.org, ruizdylann@gmail.com
\********************************************************************/

#ifndef SRC_UTIL_UTILITYFUNCTIONS_H_
#define SRC_UTIL_UTILITYFUNCTIONS_H_


#include "Config/ActiveCollection.h"
#include <string>
#include <WPILib.h>
#include "Util/LoopChecks.h"

using namespace std;
using namespace Configuration;
using namespace Components;

/**********************************DRIVE METHODS**********************/
static void SetDrive(double left, double right, ActiveCollection *activeCollection) //set left and right motor power. range: [0,1]
{
	VictorSPItem *left_0 = activeCollection->GetVictor("Left_0"); //creates pointers to motor objects. This robot has three left motors and three right motors
	VictorSPItem *left_1 = activeCollection->GetVictor("Left_1");
	VictorSPItem *left_2 = activeCollection->GetVictor("Left_2");
	VictorSPItem *right_0 = activeCollection->GetVictor("Right_0");
	VictorSPItem *right_1 = activeCollection->GetVictor("Right_1");
	VictorSPItem *right_2 = activeCollection->GetVictor("Right_2");

	left_0->Set(left); //sets left and right motors to desired power
	left_1->Set(left);
	left_2->Set(left);
	right_0->Set(right);
	right_1->Set(right);
	right_2->Set(right);
}
static void StopDrive(ActiveCollection *activeCollection) //sets drive power to zero
{
	SetDrive(0, 0, activeCollection);
}
static void DriveWithTimer(double left, double right, double sec, ActiveCollection *activeCollection) //drives for desired amount of time at specified power.
{																									//this method may not drive straight
	SetDrive(left, right, activeCollection);

	Wait(sec);

	StopDrive(activeCollection);
}

/***********************OPERATOR METHODS************************/
static void SetIntake(double power, ActiveCollection *activeCollection) //sets intake motors to desired power. range [0,1]
{
	VictorSPItem *intakeLeft = activeCollection->GetVictor("RightIntake"); //creates pointers to motor objects
	VictorSPItem *intakeRight = activeCollection->GetVictor("LeftIntake");

	intakeLeft->Set(power); //sets motors to desired power
	intakeRight->Set(power);
}
static void StopIntake(ActiveCollection *activeCollection) //sets intake pwoer to zero
{
	SetIntake(0, activeCollection);
}
static void SetArm(double power, ActiveCollection *activeCollection) //sets arm motor power. range: [0,1]
{
	VictorSPItem *dropIntake = activeCollection->GetVictor("IntakeDrop"); //creates pointer to motor object
	dropIntake->Set(power); //sets motor to desired power
}
static void StopArm(ActiveCollection *activeCollection) //sets arm power to zero
{
	SetArm(0, activeCollection);
}
static void SetArmTimer(double power, double sec, ActiveCollection *activeCollection) //sets arm at specified power for specified amount of seconds
{
	SetArm(power, activeCollection);
	Wait(sec);

	StopArm(activeCollection);
}
static void DropCube(ActiveCollection *activeCollection) //sets arm and intake motors at predetermined powers/times designed to drop the cube reliably
{
	SetArm(-.8, activeCollection);
	Wait(.25);
	StopArm(activeCollection);

	SetIntake(-.3, activeCollection);
	Wait(1);

	StopIntake(activeCollection);
}
static void DropCube2(ActiveCollection *activeCollection) //small interation on DropCube. Wait is smaller
{
	SetArm(-.8, activeCollection);
	Wait(.25);
	StopArm(activeCollection);

	SetIntake(-.3, activeCollection);
	Wait(.5);

	StopIntake(activeCollection);
}
static void SetElevator(double power, ActiveCollection *activeCollection) //sets elevator 
{
	TalonSRXItem *lift = activeCollection->GetTalon("Lift"); //gets lift motor pointer

	lift->Set(power); //sets lift power
}
static void StopElevator(ActiveCollection *activeCollection) //sets elevator power to zero
{
	SetElevator(0, activeCollection);
}

/********************************AUTON METHODS********************************/
static void SlowStop(double left, double right, ActiveCollection *activeCollection) //sets motor power and decreases it over time until robot comes to a stop
{
	while ( (abs(right)) > .05 && (abs(left) > .05) ) //while motors still have significant power
	{
		left /= 1.02; //decrease by small amount
		right /= 1.02;

		SetDrive(left, right, activeCollection); //set drive to new power

		Wait(.005);
	}
	StopDrive(activeCollection);
}

/* DriveForward
 * This method uses two PID loops to drive straight and the requesred encoder distance
 * The first PID loop runs for the 60% of requested distance, using the navx to correct angle
 * the second PID loop runs for the remaining distance, using the encoder to stop at requested distance. The robot will not correct for angle during this portion.
 * 
 * What is a PID loop?
 * A PID loop is a feedback loop that uses an input stream to control an output stream. In our case, the NavX angle controls left/right motor speeds.
 * 	P - Proportion
 * 		outputs a value proportional to error (desired value minus expected value). 
 6 		For our code: We want to be facing at the same direction the whole time, so any deviation from that value is error.  
 9  I - Integral
 6 		The general term for integral is area under a graph. If you think of a graph of time versus error, integral would be the area between
 9 		the x-axis and the graph. Integral is used for error correction. Integral is not always neccessary, so you may see its scalar value (ki)
 * 		set to zero. 
 * 		For our code: Say we wanted to drive a specified distance and our robot was alrealy only 1 encoder tick away. The kp and kd values would not
 * 		provide enough power to push that last bit, because the values are so small and in the real world there's friction and stuff that would stop it.
 * 		The ki value, will observe error over time and, if it is high enough, provide that small boost that the robot needs.
 *  D - Derivative
 * 		Another way to think of derivative is slope. Derivate is the slope of the line tangent to a graph at a specific point.
 * 		Going back to the graph described above, derivitive will find how steep our error line is, aka the rate of change of error.
 * 		If we did not have kd, then kp would always overshoot the desired value, then start swinging back towards it, and overshoot again,
 * 		kinda like a pendulum trying to be at 0 degrees, it would always overshoot. Think of the kd value like a finger pushing against the pendulum.
 * 		When the pendulum isn't moving, the finger doesn't push at all, but the faster the pendulum moves, the more kd pushes back. Once tuned properly, kd
 * 		will stop the pendulum exactly at zero degrees, without it overshooting or pushing the pendulum back away. 
 * 		For our code: with only ki, the robot would oscillate around zero degress while driving forward and never stay there. With kd, the derivate will push against
 * 		ki to hopefully stop turning exactly at zero degrees.
 * 		derivative = current error - error prior. 
 */
static void DriveForward(double dist, double power, ActiveCollection *activeCollection)
{
		NavX *navx = activeCollection->GetNavX(); //gets navx from active collection
		navx->Reset(); //resets navx value
		EncoderItem *enc0 = activeCollection->GetEncoder("enc0"); //gets encoder from active collection
		enc0->Reset(); //resets encoder value
		Wait(.25);

		//cout << "initial angle = " << navx->GetAngle() << " initial enc = " << enc0->Get() << endl;

		double dist0 = dist*.6; //distance for first PID loop

		double left, right; //variables for left and right power

		double setPoint = navx->GetAngle(), currentValue = navx->GetAngle(), enc = enc0->Get(); //setPoint: init angle, currentValue: current angle, enc: encoder value

		double kp = .02, ki = 0, kd = .00005; //P, I, and D scalars. THESE ARE THE NUMBERS YOU TUNE
		double errorPrior = 0, error, integ = 0, deriv, output; //error prior: error from prior loop iteration, error: current error, output: value to be added or subtracted from motors

		//pid loop #1
		while(enc < dist0 &&_IsAutononomous()) //while we are less than 60% of desired dist and still in auton
		{
			error = setPoint - currentValue; //set error value (P)
			integ = integ + error; //add error to integral (I)
			deriv = (error - errorPrior); //find current derivative (D)
			output = (kp*error) + (ki*integ) + (kd*deriv); //set output to PID times their respective scalars
			errorPrior = error; //set errorPrior for next iteration of loop

			//cout << "output = " << output << endl;

			left = power + output; //set left motor to desired power + output
			right = power - output; //set right motor to desired power - output (+ and - to make robot turn slightly)

			SetDrive(left, right, activeCollection); //setDrive to new powers

			currentValue = navx->GetAngle(); //get new angle for next iter
			enc = enc0->Get(); //get new encoder distance for next iter

			Wait(.05);
			//cout<<"angle = " << currentValue << "	left = " << left << "	right = " << right << endl;
		}

		power = .1;
		while(enc < dist && _IsAutononomous())
		{
			error = setPoint - currentValue;
			integ = integ + error;
			deriv = (error - errorPrior);
			output = (kp*error) + (ki*integ) + (kd*deriv);
			errorPrior = error;

			//cout << "output = " << output << endl;

			left = power + output;
			right = power - output;

			SetDrive(left, right, activeCollection);

			currentValue = navx->GetAngle();
			enc = enc0->Get();

			Wait(.05);
			//cout<<"angle = " << currentValue << "	left = " << left << "	right = " << right << endl;
		}
		StopDrive(activeCollection);
}
static void Turn(double target, ActiveCollection *activeCollection)
{
	NavX *navx = activeCollection->GetNavX();
	navx->Reset();
	Wait(.25);

	//cout << "initial angle = " << navx->GetAngle() << endl;

	double killTime = .5, elapsedTime = 0;

	double left, right, power = 0;

	double currentValue = navx->GetAngle();

	double kp = .0046, ki = 0, kd = .0046;
	double allow = 2, errorPrior = 0, error, integ = 0, deriv, output;

	while(((currentValue > target + allow) || (currentValue < target - allow)) && (elapsedTime < killTime) && _IsAutononomous())
	{
		error = target - currentValue;
		integ = integ + error;
		deriv = (error - errorPrior);
		output = (kp*error) + (ki*integ) + (kd*deriv);
		errorPrior = error;

		//cout << "output = " << output << endl;

		left = power + output;
		right = power - output;

		SetDrive(left, right, activeCollection);

		currentValue = navx->GetAngle();

		Wait(.04);
		elapsedTime += .04;
		//cout<<"angle = " << currentValue << "	left = " << left << "	right = " << right << endl;
	}
	StopDrive(activeCollection);
	Wait(.25);
	currentValue = navx->GetAngle();
	//cout << "final angle = " << currentValue << endl;
}
static void DriveLong(double dist, double power, ActiveCollection *activeCollection)
{
	NavX *navx = activeCollection->GetNavX();
	navx->Reset();
	EncoderItem *enc0 = activeCollection->GetEncoder("enc0");
	enc0->Reset();
	Wait(.25);

	//cout << "initial angle = " << navx->GetAngle() << " initial enc = " << enc0->Get() << endl;

	double dist0 = dist*.75;

	double left, right;

	double setPoint = navx->GetAngle(), currentValue = navx->GetAngle(), enc = enc0->Get();

	double kp = .02, ki = 0, kd = .000005;
	double errorPrior = 0, error, integ = 0, deriv, output;

	while(enc < dist0 &&_IsAutononomous())
	{
		error = setPoint - currentValue;
		integ = integ + error;
		deriv = (error - errorPrior);
		output = (kp*error) + (ki*integ) + (kd*deriv);
		errorPrior = error;

		//cout << "output = " << output << endl;

		left = power + output;
		right = power - output;

		SetDrive(left, right, activeCollection);

		currentValue = navx->GetAngle();
		enc = enc0->Get();

		Wait(.05);
		//cout<<"angle = " << currentValue << "	left = " << left << "	right = " << right << endl;
	}

	power = .20;
	while(enc < dist && _IsAutononomous())
	{
		error = setPoint - currentValue;
		integ = integ + error;
		deriv = (error - errorPrior);
		output = (kp*error) + (ki*integ) + (kd*deriv);
		errorPrior = error;

		//cout << "output = " << output << endl;

		left = power + output;
		right = power - output;

		SetDrive(left, right, activeCollection);

		currentValue = navx->GetAngle();
		enc = enc0->Get();

		Wait(.05);
		//cout<<"angle = " << currentValue << "	left = " << left << "	right = " << right << endl;
	}
	StopDrive(activeCollection);
}
static void TurnShort(double target, ActiveCollection *activeCollection)
{
	NavX *navx = activeCollection->GetNavX();
	navx->Reset();
	Wait(.25);

	//cout << "initial angle = " << navx->GetAngle() << endl;

	double killTime = .2, elapsedTime = 0;

	double left, right, power = 0;

	double currentValue = navx->GetAngle();

	double kp = .0052, ki = 0, kd = .0223;
	double allow = 2, errorPrior = 0, error, integ = 0, deriv, output;

	while(((currentValue > target + allow) || (currentValue < target - allow)) && (elapsedTime < killTime) && _IsAutononomous())
	{
		error = target - currentValue;
		integ = integ + error;
		deriv = (error - errorPrior);
		output = (kp*error) + (ki*integ) + (kd*deriv);
		errorPrior = error;

		//cout << "output = " << output << endl;

		left = power + output;
		right = power - output;

		SetDrive(left, right, activeCollection);

		currentValue = navx->GetAngle();

		Wait(.04);
		elapsedTime += .04;
		//cout<<"angle = " << currentValue << "	left = " << left << "	right = " << right << endl;
	}
	StopDrive(activeCollection);
	Wait(.25);
	currentValue = navx->GetAngle();
	//cout << "final angle = " << currentValue << endl;
}
static void TurnLong(double target, ActiveCollection *activeCollection)
{
	NavX *navx = activeCollection->GetNavX();
	navx->Reset();
	Wait(.25);

	//cout << "initial angle = " << navx->GetAngle() << endl;

	double killTime = .75, elapsedTime = 0;

	double left, right, power = 0;

	double currentValue = navx->GetAngle();

	double kp = .0041, ki = 0, kd = .03;
	double allow = 2, errorPrior = 0, error, integ = 0, deriv, output;

	while(((currentValue > target + allow) || (currentValue < target - allow)) && (elapsedTime < killTime) && _IsAutononomous())
	{
		error = target - currentValue;
		integ = integ + error;
		deriv = (error - errorPrior);
		output = (kp*error) + (ki*integ) + (kd*deriv);
		errorPrior = error;

		//cout << "output = " << output << endl;

		left = power + output;
		right = power - output;

		SetDrive(left, right, activeCollection);

		currentValue = navx->GetAngle();

		Wait(.04);
		elapsedTime += .04;
		//cout<<"angle = " << currentValue << "	left = " << left << "	right = " << right << endl;
	}
	StopDrive(activeCollection);
	Wait(.25);
	currentValue = navx->GetAngle();
	//cout << "final angle = " << currentValue << endl;
}
static void Turn2(double target, ActiveCollection *activeCollection)
{
	NavX *navx = activeCollection->GetNavX();
	navx->Reset();
	Wait(.25);

	//cout << "initial angle = " << navx->GetAngle() << endl;

	double killTime = 1, elapsedTime = 0;

	double left, right, power = 0;

	double currentValue = navx->GetAngle();

	double kp = .0052, ki = 0, kd = .0223;
	double allow = 3, errorPrior = 0, error, integ = 0, deriv, output;

	while(((currentValue > target + allow) || (currentValue < target - allow)) && (elapsedTime < killTime) && _IsAutononomous())
	{
		error = target - currentValue;
		integ = integ + error;
		deriv = (error - errorPrior);
		output = (kp*error) + (ki*integ) + (kd*deriv);
		errorPrior = error;

		//cout << "output = " << output << endl;

		left = power + output;
		right = power - output;

		SetDrive(left, right, activeCollection);

		currentValue = navx->GetAngle();

		Wait(.04);
		elapsedTime += .04;
		//cout<<"angle = " << currentValue << "	left = " << left << "	right = " << right << endl;
	}
	StopDrive(activeCollection);
	Wait(.25);
	currentValue = navx->GetAngle();
	//cout << "final angle = " << currentValue << endl;
}
static void TurnNoReset(double target, ActiveCollection *activeCollection)
{
	NavX *navx = activeCollection->GetNavX();
	Wait(.25);

	//cout << "initial angle = " << navx->GetAngle() << endl;

	double killTime = .5, elapsedTime = 0;

	double left, right, power = 0;

	double currentValue = navx->GetAngle();

	double kp = .0052, ki = 0, kd = .0223;
	double allow = 2, errorPrior = 0, error, integ = 0, deriv, output;

	while(((currentValue > target + allow) || (currentValue < target - allow)) && (elapsedTime < killTime) && _IsAutononomous())
	{
		error = target - currentValue;
		integ = integ + error;
		deriv = (error - errorPrior);
		output = (kp*error) + (ki*integ) + (kd*deriv);
		errorPrior = error;

		//cout << "output = " << output << endl;

		left = power + output;
		right = power - output;

		SetDrive(left, right, activeCollection);

		currentValue = navx->GetAngle();

		Wait(.04);
		elapsedTime += .04;
		//cout<<"angle = " << currentValue << "	left = " << left << "	right = " << right << endl;
	}
	StopDrive(activeCollection);
	Wait(.25);
	currentValue = navx->GetAngle();
	//cout << "final angle = " << currentValue << endl;
}
#endif /* SRC_UTIL_UTILITYFUNCTIONS_H_ */
