// Created         : 2025-05-14T15:06:43+0300
// Author          : Timo Nihtila <timo.nihtila@gmail.com>

#include "Point.h"
#include <iostream>
#include <math.h>

using namespace std;

Point::Point() : x(0), y(0)
{
}

Point::Point(double x, double y) : x(x), y(y)
{
}

Point::~Point()
{
}

Point::Point(const Point &source) : x(source.x), y(source.y)
{
}

Point &Point::operator=(const Point &source)
{
    if (this != &source)
    {
        x = source.x;
        y = source.y;
    }
    return *this;
}

Point &Point::operator+(const Point &source)
{
    x += source.x;
    y += source.y;
    return *this;
}

Point &Point::operator-(const Point &source)
{
    x -= source.x;
    y -= source.y;
    return *this;
}

Point &Point::operator*(double r)
{
    x *= r;
    y *= r;
    return *this;
}

double Point::DistSqr(const Point &b) const
{
    return pow((x - b.x), 2) + pow((y - b.y), 2);
}

std::ostream & operator << (std::ostream &os, const Point& p)
{
    os << "P(" << p.x << "," << p.y << ")";
    return os;
}
