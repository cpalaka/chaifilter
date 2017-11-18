#ifndef __CF_ENGINE_H__
#define __CF_ENGINE_H__

#pragma once
#include <utility>
#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>
#include <map>
#include <set>
#include <unordered_set>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <climits>
#include <cfloat>
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>

#include "PixelArray.h"
#include "Util.h"
#include "easybmp/EasyBMP.h"

enum GraphicType {
    LINE,
    CIRCLE,
    RECTANGLE,
    SPRITE,
    NONE,
};

enum distFunc {
    EUCLID,
    TAXICAB,
    COSINE,
    JACCARD,
};

typedef float (*distance_func)(sf::Color,sf::Color);

//Main engine that runs the chan filter algorithm and more
class cfEngine {
public:
    cfEngine();
    void init(sf::Color initialFill = sf::Color::White);
    void runAlgo();
    bool configureEngineSettings(const int argc, const char* argv []);
    
private:
    std::vector<sf::Color> getUniqueColorsFromImage();
    void printUsage();
    void render_loop();
    void algo_loop();
    void render_pixelArray();


    void savePixelArrayToFile();
    void saveKmeansOutputToFile();

    std::string generateDetailedOutputString();

    //Line drawing algos
    void constructLine_naive(int x1, int y1, int x2, int y2, sf::Color color);
    void constructLine_bresenham(int x1, int y1, int x2, int y2, sf::Color color);

    //AABB is a LOT slower
    long double measureAverageDistance_AABB(int id, sf::Rect<int> area);
    float measureAverageDistance_perPixel(int id);//id = 1 for with line, id = 0 for without
    
    //run kmeans algo on availableColors and output to kmeansColors
    void kmeans();
    void showKmeansResult();
    std::vector<sf::Color> doKmeansPlus();

    //sort color vector by hue
    void sortColorVector(std::vector<sf::Color>& vec);
    util::HSL rgbToHsl(sf::Color c);
    sf::Color hslToRgb(util::HSL hsl);
    float hueToRgb(float p, float q, float t);

    double callProfiler(std::string s);
    void profilerOutput();

    inline void drawGraphic(bool flag = false) {
        for(const auto& i: pixBuffer) {
            if(flag) pixBufferTemp.push_back(util::pix(sf::Vector2u(i.loc.x, i.loc.y), pixelArray.getPixel(i.loc.x, i.loc.y)));
            pixelArray.setPixel(i.loc.x, i.loc.y, i.c);
        }
    }
    
    //undo the previously drawn line, saved in pixBufferTemp
    inline void undoGraphic() {
        for(const auto& i: pixBufferTemp) {
            pixelArray.setPixel(i.loc.x, i.loc.y, i.c);
        }
    }

    inline void clearPixelBuffers() { pixBuffer.clear();pixBufferTemp.clear(); };
    
    //Member data
    PixelArray pixelArray;
    sf::Image inputpic;
    std::string input_filename;

    sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2u resolution;
    GraphicType graphicType;
    distFunc df;
    //util::Timer t;
    int num_of_iter;

    std::map<std::string, double> profileResults;
    std::vector<struct util::pix> pixBuffer, pixBufferTemp;

    //GraphicType=Line settings
    int max_line_length;

    std::vector<sf::Color> availableColors;
    std::vector<sf::Color> kmeansColors;

    distance_func dfn;

    //Option flags
    bool distanceIsInversed;
    bool useAABBToCompare;
    bool iskmeansSorted;
    bool visualMode;
    bool useKmeans;
    bool showKMeansResult;
    bool useKmeansplus;
    bool writeToBMPDetailed;
    bool writeToBMPNumbered;
    bool profileCode;
    int num_clusters;//k
};

#endif //__CF__ENGINE_H__