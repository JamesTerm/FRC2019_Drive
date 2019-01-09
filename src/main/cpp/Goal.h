#pragma once


public class Goal
{
	public:
		string getName() {return m_name;}
		virtual void execute() = 0;
	private:
		string m_name;

}

public class AtomicGoal : public Goal
{
	public:

	private:
	
}

public class CompositeGoal : public Goal
{
	public:
		virtual Goal getNextGoal() = 0;
	private:
	
}