// Created         : 2025-05-14T15:06:43+0300
// Author          : Timo Nihtila <timo.nihtila@gmail.com>

#ifndef POINT_H
#define POINT_H
#include <ostream>

class Point
{
public:
    Point();
    Point(double x, double y);
    Point(const Point&);
    ~Point();
    double DistSqr(const Point& b) const;
    Point& operator=(const Point& source);
    Point& operator+(const Point& source);
    Point& operator-(const Point& source);
    Point& operator*(double r);

    double x;
    double y;
};

std::ostream & operator << (std::ostream &os, const Point& p);


#endif // POINT_H
