/****************************** Header ******************************\
Class Name: -
File Name: UtilityFunctions.h
Summary: File of static utility functions for auto use.
Project: FRC2019CPP
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ryan Cooper, Dylann Ruiz
Email: cooper.ryan@centaurisoftware.co, ruizdylann@gmail.com
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
static void SetDrive(double left, double right, ActiveCollection *activeCollection)
{
	VictorSPItem *left_0 = activeCollection->GetVictor("Left_0");
	VictorSPItem *left_1 = activeCollection->GetVictor("Left_1");
	VictorSPItem *left_2 = activeCollection->GetVictor("Left_2");
	VictorSPItem *right_0 = activeCollection->GetVictor("Right_0");
	VictorSPItem *right_1 = activeCollection->GetVictor("Right_1");
	VictorSPItem *right_2 = activeCollection->GetVictor("Right_2");

	left_0->Set(left);
	left_1->Set(left);
	left_2->Set(left);
	right_0->Set(right);
	right_1->Set(right);
	right_2->Set(right);
}
static void StopDrive(ActiveCollection *activeCollection)
{
	SetDrive(0, 0, activeCollection);
}
static void DriveWithTimer(double left, double right, double sec, ActiveCollection *activeCollection)
{
	SetDrive(left, right, activeCollection);

	Wait(sec);

	StopDrive(activeCollection);
}

/***********************OPERATOR METHODS************************/
static void SetIntake(double power, ActiveCollection *activeCollection)
{
	VictorSPItem *intakeLeft = activeCollection->GetVictor("RightIntake");
	VictorSPItem *intakeRight = activeCollection->GetVictor("LeftIntake");

	intakeLeft->Set(power);
	intakeRight->Set(power);
}
static void StopIntake(ActiveCollection *activeCollection)
{
	SetIntake(0, activeCollection);
}
static void SetArm(double power, ActiveCollection *activeCollection)
{
	VictorSPItem *dropIntake = activeCollection->GetVictor("IntakeDrop");
	dropIntake->Set(power);
}
static void StopArm(ActiveCollection *activeCollection)
{
	SetArm(0, activeCollection);
}
static void SetArmTimer(double power, double sec, ActiveCollection *activeCollection)
{
	SetArm(power, activeCollection);
	Wait(sec);

	StopArm(activeCollection);
}
static void DropCube(ActiveCollection *activeCollection)
{
	SetArm(-.8, activeCollection);
	Wait(.25);
	StopArm(activeCollection);

	SetIntake(-.3, activeCollection);
	Wait(1);

	StopIntake(activeCollection);
}
static void DropCube2(ActiveCollection *activeCollection)
{
	SetArm(-.8, activeCollection);
	Wait(.25);
	StopArm(activeCollection);

	SetIntake(-.3, activeCollection);
	Wait(.5);

	StopIntake(activeCollection);
}
static void SetElevator(double power, ActiveCollection *activeCollection)
{
	TalonSRXItem *lift = activeCollection->GetTalon("Lift");

	lift->Set(power);
}
static void StopElevator(ActiveCollection *activeCollection)
{
	SetElevator(0, activeCollection);
}

/********************************FUN BOYS********************************/
static void SlowStop(double left, double right, ActiveCollection *activeCollection)
{
	while ( (abs(right)) > .05 && (abs(left) > .05) )
	{
		left /= 1.02;
		right /= 1.02;

		SetDrive(left, right, activeCollection);

		Wait(.005);
	}
	StopDrive(activeCollection);
}
static void DriveForward(double dist, double power, ActiveCollection *activeCollection)
{
		NavX *navx = activeCollection->GetNavX();
		navx->Reset();
		EncoderItem *enc0 = activeCollection->GetEncoder("enc0");
		enc0->Reset();
		Wait(.25);

		//cout << "initial angle = " << navx->GetAngle() << " initial enc = " << enc0->Get() << endl;

		double dist0 = dist*.6;

		double left, right;

		double setPoint = navx->GetAngle(), currentValue = navx->GetAngle(), enc = enc0->Get();

		double kp = .02, ki = 0, kd = .00005;
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
