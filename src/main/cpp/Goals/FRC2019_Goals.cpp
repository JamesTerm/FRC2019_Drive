#include "Goal.h"

using namespace std;

class Goal_DoNothing : public AtomicGoal
{
  public:
    Goal_DoNothing(double timeOut)
    {
        m_Status = eInactive;
        m_timeOut = timeOut;
    }
    void Activate()
    {
        m_Status = eActive;
    }
    Goal::Goal_Status Process(double dTime_s)
    {
        if (m_Status == eActive)
        {
            m_time += dTime_s;
            cout << m_time << endl;
            if (m_time >= m_timeOut)
            {
                return eCompleted;
                Terminate();
            }
        }
        else
        {
            return eInactive;
        }
    }

  private:
    double m_time = 0;
    double m_timeOut;
};