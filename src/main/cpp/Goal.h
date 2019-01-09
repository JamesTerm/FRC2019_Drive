#pragma once

#include "GoalManager.h"
#include "Util/UtilityFunctions.h"

#pragma region Abstract goal classes
class Goal
{
  public:
	Goal() {}
	Goal(string name, GoalManager* gm)
	{
		m_name = name;
		m_goalManager = gm;
	}
	string getName() { return m_name; }
	GoalManager *getGoalManager() { return m_goalManager; }

	virtual void execute() = 0;

  protected:
	string m_name;
	GoalManager* m_goalManager;
};

class AtomicGoal : public Goal
{
  public:
	AtomicGoal() {}
	AtomicGoal(string name, GoalManager* gm) : public Goal(name, gm) {}
	void execute() const;

  private:
};

class CompositeGoal : public Goal
{
  public:
	void execute() const;

  private:
};
#pragma endregion

#pragma region Implemented Goal Classes
class DriveForwardGoal : public AtomicGoal
{
  public:
	DriveForwardGoal(string name, GoalManager* gm, double distance, double power, Configuration::ActiveCollection* activeCollection) : public AtomicGoal(name, gm)
	{
		m_distance = distance;
		m_power = power;
		m_activeCollection = activeCollection;
	}
	void execute()
	{
		//TODO do this without DriveForward(...). This is temp
		DriveForward(m_distance, m_power, m_activeCollection);
	}
  private:
	double m_distance;
	double m_power;
	ActiveCollection* m_activeCollection;
};
#pragma endregion