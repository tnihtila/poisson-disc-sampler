// Created         : 2025-05-14T15:06:43+0300
// Author          : Timo Nihtila <timo.nihtila@gmail.com>

#include <iostream>
#include <sstream>
#include <fstream>
#include "PoissonDiscSampling.h"

#define cimg_display 1
#include "CImg.h"
using namespace cimg_library;

std::vector<std::string> SplitString(std::string str, char delim = ',')
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delim))
    {
        if (token.size())
        {
            tokens.emplace_back(token);
        }
    }

    return tokens;
}

void ParseFile(const std::string& filename,
               std::vector<std::vector<double>>& mapData)
{
    std::ifstream file;

    file.open(filename);
    if (file.is_open())
    {
        if (file.good())
        {
            std::string line;
            while (getline(file,line))
            {
                auto fields = SplitString(line);
                mapData.emplace_back(std::vector<double>());
                for (const auto &it : fields)
                {
                    mapData.back().emplace_back(std::stod(it));
                }
            }
        }
    }
}

// Example call
// ./pos ../input/area.csv 6 300 500

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <map_file> <num_positions> <num_attempts_before_rejection>\n";
        return EXIT_FAILURE;
    }

    std::string filename = argv[1];

    // Optional: check that file exists
    std::ifstream infile(filename);
    if (!infile.good())
    {
        std::cerr << "Error: Cannot open file '" << filename << "'\n";
        return EXIT_FAILURE;
    }

    // Number of generated positions
    auto numPositions = 0;

    // Number of alloweds failures to generate a new position before quitting
    auto numAttemptsBeforeRejection = 0;

    try
    {
        numPositions = std::stoull(argv[2]);
        numAttemptsBeforeRejection = std::stoull(argv[3]);
    }
    catch (const std::invalid_argument&)
    {
        std::cerr << "Error: One of the numeric arguments is not a valid number.\n";
        return EXIT_FAILURE;
    } catch (const std::out_of_range&)
    {
        std::cerr << "Error: One of the numeric arguments is out of range.\n";
        return EXIT_FAILURE;
    }

    if (numPositions == 0)
    {
        std::cerr << "Error: Number of generated positions must be greater than zero.\n";
        return EXIT_FAILURE;
    }

    if (numAttemptsBeforeRejection == 0)
    {
        std::cerr << "Error: Number of attempts before rejection must be greater than zero.\n";
        return EXIT_FAILURE;
    }

    std::vector<std::vector<double>> mapData;

    ParseFile(std::string(argv[1]), mapData);

    double resolution = std::max(mapData[1][0] - mapData[0][0],
                                 mapData[1][1] - mapData[0][1]);

    PoissonDiscSampling pds;
    auto evenlyDistributedPts = pds.GeneratePoints(numPositions, mapData, resolution, numAttemptsBeforeRejection);

    auto borderPoints = pds.GetBorderPoints();
    auto limits = pds.GetLimits();
    auto width = (limits[1] - limits[0]);
    auto height = (limits[3] - limits[2]);
    auto scale = std::max(width, height);

    static int white[] = {255,255,255};
    static int red[] = {255,100,100};
    static int green[] = {100,255,100};

    auto imageSize = 800;
    auto margin = 20;
    auto plotSize = imageSize - 2*margin;

    CImg<double> image1(imageSize, imageSize, 1, 3, 1);

    for (auto i = 0; i < mapData.size (); i++)
    {
        if (mapData[i][2])
        {
            Point p1(margin + (((mapData[i][0] - 0.5*resolution) - limits[0]) / scale) * plotSize,
                     imageSize - (margin + (((mapData[i][1] - 0.5*resolution) - limits[2]) / scale) * plotSize));
            Point p2(margin + (((mapData[i][0] + 0.5*resolution) - limits[0]) / scale) * plotSize,
                     imageSize - (margin + (((mapData[i][1] - 0.5*resolution) - limits[2]) / scale) * plotSize));
            Point p3(margin + (((mapData[i][0] + 0.5*resolution) - limits[0]) / scale) * plotSize,
                     imageSize - (margin + (((mapData[i][1] + 0.5*resolution) - limits[2]) / scale) * plotSize));
            Point p4(margin + (((mapData[i][0] - 0.5*resolution) - limits[0]) / scale) * plotSize,
                     imageSize - (margin + (((mapData[i][1] + 0.5*resolution) - limits[2]) / scale) * plotSize));
            image1.draw_line(p1.x, p1.y, p2.x, p2.y, green, 1);
            image1.draw_line(p2.x, p2.y, p3.x, p3.y, green, 1);
            image1.draw_line(p3.x, p3.y, p4.x, p4.y, green, 1);
            image1.draw_line(p4.x, p4.y, p1.x, p1.y, green, 1);
        }
    }

    for (auto& p : borderPoints)
    {
        p.x = margin + ((p.x - limits[0]) / scale) * plotSize;
        p.y = imageSize - (margin + ((p.y - limits[2])/ scale) * plotSize);
        image1.draw_circle(static_cast<int>(p.x), static_cast<int>(p.y), 2, red, 1);
    }

    for (auto& p : evenlyDistributedPts)
    {
        p.x = margin + ((p.x - limits[0]) / scale) * plotSize;
        p.y = imageSize - (margin + ((p.y - limits[2]) / scale) * plotSize);
        image1.draw_circle(static_cast<int>(p.x), static_cast<int>(p.y), 3, white, 1, 1);
    }

    CImgDisplay disp1(image1, "Positions");
    disp1.move(50,50);
    disp1.show();
    while(true)
    {
        CImgDisplay::wait(disp1);
    }
}
