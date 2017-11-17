#ifndef __UTIL_H__
#define __UTIL_H__

#pragma once
#include <SFML/Graphics.hpp>
#include <cstdint>
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;

namespace util {

struct Timer {
    Timer() { };
    inline void start() {
        a = Clock::now();
    }
    inline double stop() {
        //b = Clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - a).count();
    }
    std::chrono::time_point<Clock, std::chrono::nanoseconds> a;
};

//Object to represent a pixel in a PixelArray (position + color)
struct pix {
    pix(sf::Vector2u position, sf::Color color) : loc(position), c(color) {};
    sf::Vector2u loc;
    sf::Color c;
    inline friend bool operator ==(const util::pix &a, const util::pix &b);
};

//used to sort the colors by hue
struct HSL {
    HSL(float _h, float _s, float _l, int _id) : h(_h), s(_s), l(_l), id(_id) {};
    float h;
    float s;
    float l;
    int id;
};

struct Color {
    Color(uint8_t _r, uint8_t _g, uint8_t _b) : r(_r), g(_g), b(_b) {};
    Color(sf::Color c) : r((uint8_t)c.r), g((uint8_t)c.g), b((uint8_t)c.b) {};
    inline friend bool operator ==(const util::Color &a, const util::Color &b);
    inline friend bool operator <(const util::Color &a, const util::Color &b);
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

inline bool operator ==(const util::pix &a, const util::pix &b) {
    return ( (a.loc == b.loc) && (a.c == b.c));
}

inline bool operator ==(const util::Color &a, const util::Color &b) {
    return ( (a.r == b.r) && (a.g == b.g) == (a.b == b.b) );
}

inline bool operator <(const util::Color &a, const util::Color &b) {
    return ( (int)a.r + a.g + a.b < (int)b.r + b.g + b.b );
}

inline float Q_rsqrt( float number );
inline float euclideanDistance(const sf::Vector2u p1, const sf::Vector2u p2);
inline float euclideanDistanceInverse(const sf::Color c1, const sf::Color c2);//NOTE: not functional
inline float euclideanDistanceSquared(const sf::Color c1, const sf::Color c2);
inline float taxiCabDistance(const sf::Color c1, const sf::Color c2);

inline float euclideanDistance(const sf::Vector2u p1, const sf::Vector2u p2) {
    return sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y));
}

inline float euclideanDistanceInverse(const sf::Color c1, const sf::Color c2) {
    return  util::Q_rsqrt(((float)c1.r-c2.r)*((float)c1.r-c2.r) + ((float)c1.g-c2.g)*((float)c1.g-c2.g) + ((float)c1.b-c2.b)*((float)c1.b-c2.b));
}

inline float euclideanDistanceSquared(const sf::Color c1, const sf::Color c2) {
    return ((float)c1.r-c2.r)*((float)c1.r-c2.r) + ((float)c1.g-c2.g)*((float)c1.g-c2.g) + ((float)c1.b-c2.b)*((float)c1.b-c2.b);
}

inline float taxiCabDistance(const sf::Color c1, const sf::Color c2) {
    return (abs(c1.r - c2.r) + abs(c1.g - c2.g) + abs(c1.b - c2.b));
}

//Yup, i did it.
inline float Q_rsqrt( float number )
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck? 
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}
}

#endif//__UTIL_H__