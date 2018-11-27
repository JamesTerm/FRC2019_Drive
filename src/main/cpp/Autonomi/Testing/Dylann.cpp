/*
 * Dylann.cpp
 *
 *  Created on: Mar 3, 2018
 *      Author: Dylann Ruiz
 */

#include <string>
#include <WPILib.h>
#include <fstream>
#include <thread>
#include <iostream>

#include <Autonomi/Testing/Dylann.h>
#include <UtilityFunctions.h>
#include "Config/ActiveCollection.h"
#include "Config/Config.h"
#include "Components/VictorSPItem.h"
#include "Components/EncoderItem.h"
#include "Components/TalonSRXItem.h"
#include <vector>
#include "Util/LoopChecks.h"

using namespace std;
using namespace Autonomi;
using namespace Components;

Dylann::Dylann(ActiveCollection *_activeCollection)
{
	activeCollection = _activeCollection;
}

void Dylann::Start()
{
	/*
	CorrectiveDrive(1300, .5, activeCollection);

	Turn(70, activeCollection);*/

	//Turn(70, activeCollection);


	/*NavX* navx = activeCollection->GetNavX();
	navx->Reset();

	double straight = navx->GetAngle();
Wait(2);

	Turn(70, straight, activeCollection);*/
	/*cout << "hi" << endl;
	SetElevator(1, activeCollection);
	Wait(1.5);

	DropCube(activeCollection);

	StopElevator(activeCollection);*/

	/*NavX* navx = activeCollection->GetNavX();
	navx->Reset();
	EncoderItem* enc0 = activeCollection->GetEncoder("enc0");
	enc0->Reset();

	double enc, distance = 1500, current, straight, power = .75, master, slave, diff, inc, gain = .002;

	enc = enc0->Get();

	master = power;
	slave = power;

	straight = navx->GetAngle();
	current = navx->GetAngle();

	diff = straight - current;

	cout << "initial angle = " << current << endl;
	while(enc < distance && _IsAutononomous())
	{
		SetDrive(master, slave, activeCollection);

		inc = diff * gain;

		slave -= inc;

		if (slave > 1) slave = 1;
		if (slave < -1) slave = -1;

		SetDrive(master, slave, activeCollection);

		current = navx->GetAngle();
		enc = enc0->Get();

		diff = straight - current;
		cout << "current angle = " << current << " slave power = " << slave << " encoder = " << enc << endl;
		Wait(.05);
	}
	cout << "final angle = " << current << " slave power = " << slave << " encoder = " << enc << endl;

	StopDrive(activeCollection);
	Wait(.5);
	current = navx->GetAngle();
	cout << "actual final angle = " << current << " slave power = " << slave << " encoder = " << enc << endl;




	double target = 90, error;
	gain = .005;

	current = navx->GetAngle();
	error = target - current;
	diff = target - current;

	cout << "initial angle= " << navx->GetAngle() << endl;

	while((abs(diff) > 3) && _IsAutononomous())
	{
		current = navx->GetAngle();
		error = target - current;
		power = error * gain;
		diff = target - current;

		if (abs(power) < .09) power = .09*sign(power);

		cout << "error = " << error << " power = " << power << " angle = " << current << endl;
		SetDrive(power, -power, activeCollection);

		Wait(.045);
	}
	SetDrive( ((-1 *sign(power))*.75), ((-1 *sign(-power))*.75), activeCollection);
	Wait(.1);
	cout << "final angle= " << current << endl;
	StopDrive(activeCollection);
	Wait(2);
	current = navx->GetAngle();
	cout << "actual final angle= " << current << endl;
	*/
	/*activeCollection->GetNavX()->Reset();
	Turn(30,activeCollection->GetNavX()->GetAngle(), activeCollection);*/
	//ThiccTurnLong(180, activeCollection);

	//ThiccDriveLong(900, .75, activeCollection);

//	Gay(20, 750, .25, activeCollection);

	/*EncoderItem *enc0 = activeCollection->GetEncoder("enc0");
	for (int i = 0; i < 25; i ++)
	{
		enc0->Reset();
		Wait(.25);

		DriveWithTimer(.5, .5, 5, activeCollection);

		cout << "encoder value = " << enc0->Get() << endl;
	}*/

	//hard code turn idk if its left or right lol

	double target = 85;

	NavX *navx = activeCollection->GetNavX();
		navx->Reset();
		Wait(.25);

		//cout << "initial angle = " << navx->GetAngle() << endl;

		double killTime = .5, elapsedTime = 0;

		double left, right, power = 0;

		double currentValue = navx->GetAngle();

		double kp = .02, ki = 0, kd = .000005;
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

