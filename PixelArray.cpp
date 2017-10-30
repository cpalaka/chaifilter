#include "PixelArray.h"

PixelArray& PixelArray::operator =(const PixelArray& toCopy) {
    if(this == &toCopy) return *this;

    xdim = toCopy.xdim;
    ydim = toCopy.ydim;
    arr = toCopy.arr;
    lastPixsSet = toCopy.lastPixsSet;
    return *this;
}

//setup affected-pixel vector (lastPixsSet) so we can compare later
void PixelArray::prepareLine(int x1, int y1, int x2, int y2, sf::Color color) {
    //Construct bounding box around line
    sf::Rect<float> area((x1<x2?x1:x2), (y1<y2?y2:y1), (x1<x2?x2-x1:x1-x2), (y1<y2?y2-y1:y1-y2));
    
    //get line equation y=m(x-x`) + y`
    float m = (float)(y2-y1)/(x2-x1);

    if(area.width > area.height) {
        if(x1 > x2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }

        for(float x = area.left+0.5; x < area.left+area.width; ++x) {
            float y = m*(x-x1) + y1;
            lastPixsSet.push_back(pix(sf::Vector2u(x,y), color));
        }
    } else {
        if(y1 > y2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }

        for(float y = area.top - area.height; y < area.top; ++y) {
            float x = (y-y1)/m + x1;
            lastPixsSet.push_back(pix(sf::Vector2u(x,y), color));
        }
    }
}

//do the distance measure with pixels in the AABB bounding box. mainly for testing and curiosity
long double PixelArray::measureDistance_AABB(sf::Image& img, int id, sf::Rect<int> area) {
    long double distance = 0;
    std::vector<sf::Color> oldpix(arr);
    if(id) drawPixsSet();

    //measure all pixels
    for(int i = area.left; i<(area.left+area.width); ++i) {
        for(int j = area.top; j>=(area.top-area.height); --j) {
            distance += dfn( (id)? getPixel(i, j) : oldpix[xdim*(ydim-j-1) + i], img.getPixel(i, j));
        }
    }
    return distance;
}

//do the distance measure with pixels only on the line
float PixelArray::measureDistance_line(sf::Image& img, int id) {
    float distance = 0;

    //measure only pixels of recently drawn line
    for(const auto& i: lastPixsSet) {
        distance += dfn( (id)?i.c : getPixel(i.loc.x, i.loc.y), img.getPixel(i.loc.x, i.loc.y));
    }
    return distance;
}