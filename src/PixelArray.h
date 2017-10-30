#ifndef __PIXEL_ARRAY_H__
#define __PIXEL_ARRAY_H__

#include <utility>
#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>
#include <cstring>
#include <cstdlib>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>


typedef float (*distance_func)(sf::Color,sf::Color);

typedef struct pix {
    pix(sf::Vector2u position, sf::Color color) : loc(position), c(color) {};
    sf::Vector2u loc;
    sf::Color c;
    inline friend bool operator ==(const pix &a, const pix &b);
} pix;

inline bool operator ==(const pix &a, const pix &b) {
    return ( (a.loc == b.loc) && (a.c == b.c));
}

//Pixel only representation of image/texture meant to work with SFML
class PixelArray {
public:
    PixelArray() : xdim(0), ydim(0){};

    PixelArray(int dimX, int dimY, sf::Color clearColor = sf::Color::White) : 
        xdim(dimX), ydim(dimY), 
        arr(std::vector<sf::Color>(dimX*dimY,clearColor)){};
        
    PixelArray(sf::Vector2u size, sf::Color color = sf::Color::White) : PixelArray(size.x, size.y, color){};

    PixelArray & operator =(const PixelArray& toCopy);
    inline friend bool operator ==(const PixelArray &p1, const PixelArray &p2);
    
    inline sf::Color getPixel(int x, int y) const {
        assert(x<xdim && y<ydim && x>=0 && y>=0);
        return arr[xdim*(ydim-y-1) + x];
    };

    inline void setPixel(int x, int y, sf::Color color) {
        assert(x<xdim && y<ydim && x>=0 && y>=0);
        arr[xdim*(ydim-y-1) + x] = color;
    };

    inline sf::Uint8* getByteArray() const {
        return (sf::Uint8*)arr.data();
    };

    inline sf::Color* getColorArray() {
        return arr.data();
    };

    inline int getPixelArraySizeX() const {
        return xdim;
    };

    inline int getPixelArraySizeY() const {
        return ydim;
    };

    void prepareLine(int x1, int y1, int x2, int y2, sf::Color color);
    
    long double measureDistance_AABB(sf::Image& img, int id, sf::Rect<int> area);
    float measureDistance_line(sf::Image& img, int id);//id = 1 for with line, id = 0 for without
    
    inline void drawLine(bool flag = false) {
        for(const auto& i: linePixels) {
            if(flag) prev_linePixels.push_back(pix(sf::Vector2u(i.loc.x, i.loc.y), getPixel(i.loc.x, i.loc.y)));
            setPixel(i.loc.x, i.loc.y, i.c);
        }
    }

    
    //undo the previously drawn line, saved in prev_linePixels
    void undoLine() {
        for(const auto& i: prev_linePixels) {
            setPixel(i.loc.x, i.loc.y, i.c);
        }
    }

    inline void clearLinePixels() { linePixels.clear();};
    inline void clearPrevLinePixels() { prev_linePixels.clear();};

    

    //TODO: implememt bresenhams algo
    //TODO: implement xiaolin wu algo

private:
    std::vector<pix> linePixels, prev_linePixels;
    std::vector<sf::Color> arr;
    int xdim;
    int ydim;

    static distance_func dfn;
};


inline bool operator ==(const PixelArray &p1, const PixelArray &p2) {
    return ( (p1.arr == p2.arr) && (p1.xdim == p2.xdim) && (p1.ydim == p2.ydim));
}


#endif//__PIXEL_ARRAY_H__