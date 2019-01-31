#include "networktables/NetworkTable.h"
#include "networktables/NetworkTableEntry.h"
#include "networktables/NetworkTableInstance.h"

using namespace std;

class VisionTarget
{
    public:
    VisionTarget(int _x, int _y, int _radius, int _area)
    {
        X = _x;
        Y = _y;
        Radius = _radius;
        Area = _area;
    }
    int getX() {return X;}
    int getY() {return Y;}
    int getRadius() {return Radius;}
    int getArea() {return Area;}
    private:
    int X,Y,Radius,Area;
};