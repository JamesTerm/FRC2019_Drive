#pragma once

#include "networktables/NetworkTable.h"
#include "networktables/NetworkTableEntry.h"
#include "networktables/NetworkTableInstance.h"

using namespace std;

class VisionTarget
{
    public:
    VisionTarget(int _x, int _y, int _radius)
    {
        X = _x;
        Y = _y;
        Radius = _radius;
        Area = 3.1415 * _radius * _radius;
        lowerBound = X - 20;
        upperBound = X + 20;
    }
    VisionTarget(int _x, int _radius) : VisionTarget(_x, 0, _radius) {}
    int getX() {return X;}
    int getY() {return Y;}
    int getRadius() {return Radius;}
    int getArea() {return Area;}
    int getLowerBound() {return lowerBound;}
    int getUpperBound() {return upperBound;}
    private:
    int X,Y,Radius,Area;
    int lowerBound, upperBound;
};