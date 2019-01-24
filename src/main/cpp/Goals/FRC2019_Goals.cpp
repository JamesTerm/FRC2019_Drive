#include "FRC2019_Goals.h"

using namespace std;

#if 0
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
#endif
void Goal_DoNothing::Activate()
{
    m_Status = eActive;
}
Goal::Goal_Status Goal_DoNothing::Process(double dTime_s)
{
    if (m_Status == eActive)
    {
        m_currentTime += dTime_s;
        cout << m_currentTime << endl;
        if (m_currentTime >= m_timeOut)
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