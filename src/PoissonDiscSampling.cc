// Created         : 2025-05-14T15:06:43+0300
// Author          : Timo Nihtila <timo.nihtila@gmail.com>

#include "PoissonDiscSampling.h"
#include "Point.h"
#include <cmath>
#include <iostream>

using namespace std;

PoissonDiscSampling::PoissonDiscSampling():
    m_areaMap(),
    m_borderDirsPerPixel(),
    m_directionToXYOffset(),
    m_borderPoints(),
    m_resolution(),
    m_width(),
    m_height(),
    m_limits(4)
{
    m_unif01d = std::uniform_real_distribution<double>(0.0, 1.0);
    m_directionToXYOffset[0] = std::make_pair(1, 0); // east
    m_directionToXYOffset[1] = std::make_pair(1, -1); // south-east
    m_directionToXYOffset[2] = std::make_pair(0, -1); // south
    m_directionToXYOffset[3] = std::make_pair(-1, -1); // south-west
    m_directionToXYOffset[4] = std::make_pair(-1, 0); // west
    m_directionToXYOffset[5] = std::make_pair(-1, 1); // north-west
    m_directionToXYOffset[6] = std::make_pair(0, 1); // north
    m_directionToXYOffset[7] = std::make_pair(1, 1); // north-east
}

PoissonDiscSampling::~PoissonDiscSampling()
{
}

double PoissonDiscSampling::Rnd01()
{
    return m_unif01d(m_gen);
}

bool PoissonDiscSampling::IsInside (int xi, int yi)
{
    return xi >= 0 && xi < m_areaMap.size() &&
        yi >= 0 && yi < m_areaMap[xi].size() &&
        m_areaMap[xi][yi];
}

bool PoissonDiscSampling::IsInside(const Point& p, double marginSqr)
{
    if (p.x < m_limits[0] ||
        p.x > m_limits[1] ||
        p.y < m_limits[2] ||
        p.y > m_limits[3])
    {
        return false;
    }

    bool ret = IsInside (static_cast<int>((p.x - m_limits[0]) / m_resolution),
                         static_cast<int>((p.y - m_limits[2]) / m_resolution));

    if (ret && marginSqr > 0)
    {
        for (const auto& pt : m_borderPoints)
        {
            auto sqrDst = p.DistSqr (pt);
            if (sqrDst < marginSqr)
            {
                return false;
            }
        }
    }

    return ret;
}

Point PoissonDiscSampling::GetRandomPoint(double marginSqr)
{
    Point p;
    do
    {
        p.x = m_limits[0] + Rnd01() * m_width;
        p.y = m_limits[2] + Rnd01() * m_height;

    } while (!IsInside (p, marginSqr));
    return p;
}

std::vector<Point> PoissonDiscSampling::GetBorderPoints()
{
    return m_borderPoints;
}

std::vector<double> PoissonDiscSampling::GetLimits()
{
    return m_limits;
}

std::vector<Point> PoissonDiscSampling::GeneratePoints(size_t numPositions,
                                                       const std::vector<std::vector<double>>& mapData,
                                                       double resolution,
                                                       size_t numSamplesBeforeRejection)
{
    m_resolution = resolution;
    m_limits[0] = std::numeric_limits<double>::max();
    m_limits[1] = -std::numeric_limits<double>::max();
    m_limits[2] = std::numeric_limits<double>::max();
    m_limits[3] = -std::numeric_limits<double>::max();
    auto halfReso = resolution/2;

    // Find dominance area limits
    for (auto i = 0; i < mapData.size (); i++)
    {
        if (static_cast<int>(mapData[i][2]))
        {
            m_limits[0] = std::min(m_limits[0], mapData[i][0] - halfReso);
            m_limits[1] = std::max(m_limits[1], mapData[i][0] + halfReso);
            m_limits[2] = std::min(m_limits[2], mapData[i][1] - halfReso);
            m_limits[3] = std::max(m_limits[3], mapData[i][1] + halfReso);
        }
    }

    m_width = m_limits[1] - m_limits[0];
    m_height = m_limits[3] - m_limits[2];
    auto xPixels = static_cast<int>(m_width / resolution);
    auto yPixels = static_cast<int>(m_height / resolution);
    std::cout << "resolution: " << resolution << " width: " << m_width << " height: " << m_height << std::endl;

    m_areaMap.resize(xPixels);
    m_borderDirsPerPixel.resize(xPixels);
    for (auto i = 0u; i < xPixels; ++i)
    {
        m_areaMap[i].resize(yPixels, 0);
        // Set of possible pixel directions occupied by own cell
        // dominance pixel, clockwise (0=east, 1=south-east, 2=south, ... )
        m_borderDirsPerPixel[i].resize(yPixels, std::set<int>({0,1,2,3,4,5,6,7}));
    }

    // Create 2D dominance map
    for (auto i = 0; i < mapData.size (); i++)
    {
        if (static_cast<int>(mapData[i][2]))
        {
            auto xi = static_cast<int>((mapData[i][0] - m_limits[0]) / resolution);
            auto yi = static_cast<int>((mapData[i][1] - m_limits[2]) / resolution);
            m_areaMap[xi][yi] = static_cast<int>(mapData[i][2]);
        }
    }

    // Find border points
    for (auto xi = 0; xi < static_cast<int>(m_areaMap.size()); xi++)
    {
        for (auto yi = 0; yi < static_cast<int>(m_areaMap[xi].size()); yi++)
        {
            for (auto dir = 0u; dir < m_directionToXYOffset.size(); dir++)
            {
                if (IsInside (xi + m_directionToXYOffset[dir].first,
                              yi + m_directionToXYOffset[dir].second))
                {
                    m_borderDirsPerPixel[xi][yi].erase(dir);
                }
            }

            if (!m_borderDirsPerPixel[xi][yi].empty())
            {
                auto centerX = m_limits[0] + halfReso + xi * resolution;
                auto centerY = m_limits[2] + halfReso + yi * resolution;
                for (auto it : m_borderDirsPerPixel[xi][yi])
                {
                    auto x = centerX + m_directionToXYOffset[it].first * halfReso;
                    auto y = centerY + m_directionToXYOffset[it].second * halfReso;
                    std::cout << "Center x: " << centerX << " y: " << centerY
                        << " Border point x: " << x << " y: " << y << " (xi: " << xi << " yi: " << yi << ") xoff: " <<  m_directionToXYOffset[it].first << " yoff: " <<  m_directionToXYOffset[it].second<<std::endl;

                    m_borderPoints.emplace_back(Point (x, y));
                }
            }
        }
    }

    std::vector<Point> points;

    double radius = std::max(m_width, m_height);
    double radiusDecrement = resolution / 50;


    while (true)
    {
        double radiusSqr = std::pow (radius, 2);
        double halfRadiusSqr = std::pow (radius / 2, 2);
        points.clear ();
        double cellSize = radius / sqrt(2.0);

        std::vector<std::vector<int>> grid(ceil(m_width / cellSize), std::vector<int>(ceil(m_height / cellSize), 0));
        std::vector<Point> spawnPoints;
        Point p = GetRandomPoint(0);
        spawnPoints.emplace_back (p);

        while (!spawnPoints.empty () && points.size () < numPositions)
        {
            int spawnIndex = static_cast<int> (Rnd01 () * spawnPoints.size ());
            Point spawnCenter = spawnPoints[spawnIndex];
            bool candidateAccepted = false;

            for (size_t i = 0; i < numSamplesBeforeRejection; i++)
            {
                double angle = Rnd01 () * 3.1415926535 * 2;
                Point dir (std::sin (angle), std::cos (angle));
                Point candidate = spawnCenter + dir * (radius + Rnd01 () * radius);
                auto xi = static_cast<int> ((candidate.x - m_limits[0]) / cellSize);
                auto yi = static_cast<int> ((candidate.y - m_limits[2]) / cellSize);

                if (IsInside(candidate, halfRadiusSqr) && IsValid(candidate, xi, yi, radiusSqr, points, grid))
                {
                    points.emplace_back (candidate);
                    spawnPoints.emplace_back (candidate);
                    grid[xi][yi] = points.size ();
                    candidateAccepted = true;
                    break;
                }
            }
            if (!candidateAccepted)
            {
                spawnPoints.erase (spawnPoints.begin () + spawnIndex);
            }
        }

        if (points.size () == numPositions || radius <= radiusDecrement)
        {
            std::cout << "break: radius: " << radius << " points: " << points.size() << " / " << numPositions << std::endl;
            break;
        }
        else
        {
            std::cout << "radius: " << radius << " points: " << points.size() << " / " << numPositions << std::endl;
            radius -= radiusDecrement;
        }
    }
    return points;
}

bool PoissonDiscSampling::IsValid (const Point& candidate,
                                   int xi,
                                   int yi,
                                   double radiusSqr,
                                   std::vector<Point>& points,
                                   std::vector<std::vector<int>>& grid)
{
    int searchStartX = std::max(0, xi - 2);
    int searchEndX = std::min(xi + 2, static_cast<int>(grid.size()) - 1);
    int searchStartY = std::max(0, yi - 2);
    int searchEndY = std::min(yi + 2, static_cast<int>(grid[0].size()) - 1);
    Point mp;
    auto minDst = std::numeric_limits<double>::max();
    for (int x = searchStartX; x <= searchEndX; x++)
    {
        for (int y = searchStartY; y <= searchEndY; y++)
        {
            int pointIndex = grid[x][y]-1;
            if (pointIndex != -1)
            {
                float sqrDst = candidate.DistSqr(points[pointIndex]);
                if (sqrDst < radiusSqr)
                {
                    return false;
                }
            }
        }
    }
    return true;
}
