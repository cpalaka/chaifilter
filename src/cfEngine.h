#ifndef __CF_ENGINE_H__
#define __CF_ENGINE_H__

#pragma once
#include <utility>
#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>
#include <cstring>
#include <cstdlib>

#include "PixelArray.h"
#include "Util.h"

enum GraphicType {
    LINE,
    CIRCLE,
    POLYGON,
    SPRITE,
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
    void render_pixelArray();
    //Line drawing algos
    void constructLine_naive(int x1, int y1, int x2, int y2, sf::Color color);

    long double measureAverageDistance_AABB(int id, sf::Rect<int> area);
    float measureAverageDistance_perPixel(int id);//id = 1 for with line, id = 0 for without
    
    inline void drawLine(bool flag = false) {
        for(const auto& i: pixBuffer) {
            if(flag) pixBufferTemp.push_back(util::pix(sf::Vector2u(i.loc.x, i.loc.y), pixelArray.getPixel(i.loc.x, i.loc.y)));
            pixelArray.setPixel(i.loc.x, i.loc.y, i.c);
        }
    }
    
    //undo the previously drawn line, saved in pixBufferTemp
    inline void undoLine() {
        for(const auto& i: pixBufferTemp) {
            pixelArray.setPixel(i.loc.x, i.loc.y, i.c);
        }
    }

    inline void clearPixelBuffers() { pixBuffer.clear();pixBufferTemp.clear(); };
    //TODO: implememt bresenhams algo
    //TODO: implement xiaolin wu algo

    //Member data
    PixelArray pixelArray;
    sf::Image inputpic;
    std::string input_filename;

    sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2u resolution;
    GraphicType graphicType;

    int num_of_iter;

    std::vector<struct util::pix> pixBuffer, pixBufferTemp;

    //GraphicType=Line settings
    int max_line_length;

    std::vector<sf::Color> availableColors;
    std::vector<sf::Color> kmeansColors;

    distance_func dfn;
    bool distanceIsInversed;
    bool useAABBToCompare;
    bool visualMode;
};

#endif //__CF__ENGINE_H__