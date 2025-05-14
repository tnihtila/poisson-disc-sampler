// Created         : 2025-05-14T15:06:43+0300
// Author          : Timo Nihtila <timo.nihtila@gmail.com>

#ifndef POISSONDISCSAMPLING_H
#define POISSONDISCSAMPLING_H

#include <vector>
#include <random>
#include <set>
#include <map>
#include "Point.h"

class Point;

class PoissonDiscSampling
{
public:
    PoissonDiscSampling();
    ~PoissonDiscSampling();
    std::vector<Point> GeneratePoints(size_t numPositions,
                                      const std::vector<std::vector<double>>& mapData,
                                      double resolution,
                                      size_t numSamplesBeforeRejection);
    bool IsValid(const Point &candidate,
                 int xi,
                 int yi,
                 double radius,
                 std::vector<Point> &points,
                 std::vector<std::vector<int>> &grid);
    std::vector<Point> GetBorderPoints();
    std::vector<double> GetLimits();

private:
    double Rnd01 ();

    bool IsInside (int xi, int yi);
    bool IsInside(const Point& p, double marginSqr);
    Point GetRandomPoint(double marginSqr);

    std::uniform_real_distribution<double> m_unif01d;
    std::mt19937 m_gen;
    std::vector<std::vector<int>> m_areaMap;
    std::vector<std::vector<std::set<int>>> m_borderDirsPerPixel;
    std::map<int, std::pair<int, int>> m_directionToXYOffset;
    std::vector<Point> m_borderPoints;
    double m_resolution;
    double m_width;
    double m_height;
    std::vector<double> m_limits;
};

#endif // POISSONDISCSAMPLING_H
