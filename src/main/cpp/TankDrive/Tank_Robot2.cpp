
#include "../Base/Base_Includes.h"
#include <math.h>
#include <assert.h>
#include "../Base/Vec2d.h"
#include "../Base/Misc.h"
#include "../Base/Event.h"
#include "../Base/EventMap.h"
#include "../Base/Script.h"
#include "../Common/Entity_Properties.h"
#include "../Common/Physics_1D.h"
#include "../Common/Physics_2D.h"
#include "../Common/Entity2D.h"
#include "../Common/Goal.h"
#include "../Common/Ship.h"
#include "../Common/AI_Base_Controller.h"
#include "../Common/Vehicle_Drive.h"
#include "../Common/PIDController.h"
#include "../Common/Poly.h"

#include "../Base/Joystick.h"
#include "../Base/JoystickBinder.h"
#include "../Common/UI_Controller.h"
#ifndef _Win32
#include <frc/WPILib.h>
#include "../Common/InOut_Interface.h"
#else
#include "../Common/Ship_1D.h"
#include "../Common/Ship.h"
#include "../Common/AI_Base_Controller.h"
#include "../Common/Vehicle_Drive.h"
#include "../Common/PIDController.h"
#include "../Common/Poly.h"
#include "../Common/Robot_Control_Interface.h"
#include "../Common/Rotary_System.h"
#include "../Common/Calibration_Testing.h"
#endif
#include "../Common/Debug.h"
#include "../Common/Robot_Control_Common.h"
#include "Tank_Robot2.h"

#include "../Common/SmartDashboard.h"

using namespace Framework::Base;
using namespace std;

  /***********************************************************************************************************************************/
 /*																Tank_Robot2															*/
/***********************************************************************************************************************************/


void Tank_Robot2::TimeChange(double dTime_s)
{
	SmartDashboard::PutNumber("LeftVoltage", m_Velocity[0]);
	SmartDashboard::PutNumber("RightVoltage", m_Velocity[1]);
	//TODO pass to speed controllers here
}

const char *csz_Joystick_SetLeftVelocity = "Joystick_SetLeftVelocity";
const char *csz_Joystick_SetRightVelocity = "Joystick_SetRightVelocity";

void Tank_Robot2::BindAdditionalEventControls(bool Bind)
{
	Entity2D_Kind::EventMap *em = m_pParent->GetEventMap();
	if (Bind)
	{
		em->EventValue_Map[csz_Joystick_SetLeftVelocity].Subscribe([&](double value)
		{
			m_Controller_Velocity[0] = m_Velocity[0] = value;
		});
		em->EventValue_Map[csz_Joystick_SetRightVelocity].Subscribe([&](double value)
		{
			m_Controller_Velocity[1] = m_Velocity[1] = value;
		});
	}
}

void Tank_Robot2::BindAdditionalUIControls(bool Bind, void *joy, void *key)
{
	m_TankRobotProps->Get_RobotControls().BindAdditionalUIControls(Bind, joy, key);
}


  /***********************************************************************************************************************************/
 /*													Tank_Robot2_Properties															*/
/***********************************************************************************************************************************/

//declared as global to avoid allocation on stack each iteration
const char * const g_Tank_Robot2_Controls_Events[] =
{
	csz_Joystick_SetLeftVelocity,csz_Joystick_SetRightVelocity
};

const char *Tank_Robot2_Properties::ControlEvents::LUA_Controls_GetEvents(size_t index) const
{
	return (index < _countof(g_Tank_Robot2_Controls_Events)) ? g_Tank_Robot2_Controls_Events[index] : NULL;
}
Tank_Robot2_Properties::ControlEvents Tank_Robot2_Properties::s_ControlsEvents;

Tank_Robot2_Properties::Tank_Robot2_Properties() : m_RobotControls(&s_ControlsEvents)
{
}

void Tank_Robot2_Properties::LoadFromScript(Scripting::Script& script)
{
	const char* err = NULL;
	m_ControlAssignmentProps.LoadFromScript(script);
	err = script.GetFieldTable("controls");
	if (!err)
	{
		m_RobotControls.LoadFromScript(script);
		script.Pop();
	}
}
