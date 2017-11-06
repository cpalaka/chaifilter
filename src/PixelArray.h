#ifndef __PIXEL_ARRAY_H__
#define __PIXEL_ARRAY_H__

#pragma once
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

//Pixel only representation of image/texture meant to work with SFML
class PixelArray {
public:
    // Constructors
    PixelArray() : xdim(0), ydim(0){};

    PixelArray(int dimX, int dimY, sf::Color clearColor = sf::Color::White) : 
        xdim(dimX), ydim(dimY), 
        arr(std::vector<sf::Color>(dimX*dimY,clearColor)){};
        
    PixelArray(sf::Vector2u size, sf::Color color = sf::Color::White) : PixelArray(size.x, size.y, color){};

    inline PixelArray & operator =(const PixelArray& toCopy) {
        if(this == &toCopy) return *this;
        xdim = toCopy.xdim;
        ydim = toCopy.ydim;
        arr = toCopy.arr;
        return *this;
    }

    inline friend bool operator ==(const PixelArray &p1, const PixelArray &p2); 
    
    //Getters/Setters
    inline sf::Color getPixel(int x, int y) const {
        assert(x<xdim && y<ydim && x>=0 && y>=0);
        return arr[y*xdim + x + 1];  ////origin at top left
    };

    inline void setPixel(int x, int y, sf::Color color) {
        assert(x<xdim && y<ydim && x>=0 && y>=0);
        arr[y*xdim + x + 1] = color;//origin at top left
    };

    inline sf::Uint8* getByteArray() const { return (sf::Uint8*)arr.data(); };
    inline sf::Color* getColorArray() { return arr.data(); };
    inline std::vector<sf::Color>& getColorVector() { return arr; }
    inline int getPixelArraySizeX() const { return xdim; };
    inline int getPixelArraySizeY() const { return ydim; };
    
private:
    //Member Data
    std::vector<sf::Color> arr;
    int xdim;
    int ydim;
};

inline bool operator ==(const PixelArray &p1, const PixelArray &p2) {
    return ( (p1.arr == p2.arr) && (p1.xdim == p2.xdim) && (p1.ydim == p2.ydim));
}

#endif//__PIXEL_ARRAY_H__