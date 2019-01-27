#include "../Constants.h"
using namespace Util;
class Distance
{
    public:
    const double toEncoderTicks() {return m_value * inATick();}
    const double toInches() {return m_value * inAnInch();}
    const double toFeet() {return m_value * inAFoot();}
    const double toYards() {return m_value * inAYard();}

    virtual double inATick() = 0;

    const double inAnInch() {return inATick() * TICKS_PER_INCH;}
    const double inAFoot() {return inAnInch() * 12.0;}
    const double inAYard() {return inAFoot() * 3.0;}
    protected:
    double m_value;
};

class EncoderTicks : public Distance
{
    public:
    EncoderTicks(double value) {m_value = value;}
    
    double inATick() {return 1.0;}
};

class Inches : public Distance
{
    public:
    Inches(double value) {m_value = value;}

    double inATick() {return 1.0 / TICKS_PER_INCH;}

};
class Feet : public Distance
{
    public:
    Feet(double value) {m_value = value;}
    double inATick() {return 1.0 / 12.0 / TICKS_PER_INCH;}
};

class Yards : public Distance
{
    public:
    Yards(double value) {m_value = value;}
    double inATick() {return 1.0 / 3.0 / 12.0 / TICKS_PER_INCH;}
};