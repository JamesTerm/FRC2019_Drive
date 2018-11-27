/*
 * RealNibbaShit.cpp
 *
 *  Created on: Apr 2, 2018
 *      Author: Dylann Ruiz
 */

#include <Autonomi/Testing/RealNibbaShit.h>
#include <UtilityFunctions.h>
#include "Config/ActiveCollection.h"
#include "Config/Config.h"
#include "Components/VictorSPItem.h"
#include <string>
#include <WPILib.h>

using namespace std;
using namespace Autonomi;
using namespace Components;

RealNibbaShit::RealNibbaShit(ActiveCollection *_activeCollection)
{
	activeCollection = _activeCollection;
}

void RealNibbaShit::Start()
{
/*	NavX *navx = activeCollection->GetNavX();
	navx->Reset();
	EncoderItem *enc0 = activeCollection->GetEncoder("enc0");
	enc0->Reset();
	Wait(.25);

	cout << "initial angle = " << navx->GetAngle() << " initial enc = " << enc0->Get() << endl;

	double dist = 3000;
	double dist0 = dist*.6;

	double left, right, power = .75;

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

		cout << "output = " << output << endl;

		left = power + output;
		right = power - output;

		SetDrive(left, right, activeCollection);

		currentValue = navx->GetAngle();
		enc = enc0->Get();

		Wait(.05);
		cout<<"angle = " << currentValue << "	left = " << left << "	right = " << right << endl;
	}

	power = .10;
	while(enc < dist && _IsAutononomous())
	{
		error = setPoint - currentValue;
		integ = integ + error;
		deriv = (error - errorPrior);
		output = (kp*error) + (ki*integ) + (kd*deriv);
		errorPrior = error;

		cout << "output = " << output << endl;

		left = power + output;
		right = power - output;

		SetDrive(left, right, activeCollection);

		currentValue = navx->GetAngle();
		enc = enc0->Get();

		Wait(.05);
		cout<<"angle = " << currentValue << "	left = " << left << "	right = " << right << endl;
	}
	StopDrive(activeCollection);
*/
	NavX *navx = activeCollection->GetNavX();
		navx->Reset();
		Wait(.25);

		cout << "initial angle = " << navx->GetAngle() << endl;

		double killTime = .75, elapsedTime = 0;

		double left, right, power = 0;

		double target = 90, currentValue = navx->GetAngle();

		double kp = .0046, ki = 0, kd = .0005;
		double allow = 1, errorPrior = 0, error, integ = 0, deriv, output;

		while(((currentValue > target + allow) || (currentValue < target - allow))&& (elapsedTime < killTime)  &&_IsAutononomous())
		{
			error = target - currentValue;
			integ = integ + error;
			deriv = (error - errorPrior);
			output = (kp*error) + (ki*integ) + (kd*deriv);
			errorPrior = error;

			cout << "output = " << output << endl;

			left = power + output;
			right = power - output;

			SetDrive(left, right, activeCollection);

			currentValue = navx->GetAngle();

			Wait(.04);
			elapsedTime += .04;
			cout<<"angle = " << currentValue << "	left = " << left << "	right = " << right << endl;
		}
		StopDrive(activeCollection);
		Wait(1);
		currentValue = navx->GetAngle();
		cout << "final angle = " << currentValue << endl;

	//angle drive

	/*NavX *navx = activeCollection->GetNavX();
	navx->Reset();
	EncoderItem *enc0 = activeCollection->GetEncoder("enc0");
	enc0->Reset();
	Wait(.25);

	cout << "initial angle = " << navx->GetAngle() << " initial enc = " << enc0->Get() << endl;

	double dist = 500;
	double dist0 = dist*.6;

	double left, right, power = .75;

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

		cout << "output = " << output << endl;

		left = power + output;
		right = power - output;

		SetDrive(left, right, activeCollection);

		currentValue = navx->GetAngle();
		enc = enc0->Get();

		Wait(.05);
		cout<<"angle = " << currentValue << "	left = " << left << "	right = " << right << endl;
	}

	power = .10;
	while(enc < dist && _IsAutononomous())
	{
		error = setPoint - currentValue;
		integ = integ + error;
		deriv = (error - errorPrior);
		output = (kp*error) + (ki*integ) + (kd*deriv);
		errorPrior = error;

		cout << "output = " << output << endl;

		left = power + output;
		right = power - output;

		SetDrive(left, right, activeCollection);

		currentValue = navx->GetAngle();
		enc = enc0->Get();

		Wait(.05);
		cout<<"angle = " << currentValue << "	left = " << left << "	right = " << right << endl;
	}
	StopDrive(activeCollection);
	*/

}
