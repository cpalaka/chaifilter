#ifndef __UTIL_H__
#define __UTIL_H__

#pragma once
#include <SFML/Graphics.hpp>

namespace util {

//Object to represent a pixel in a PixelArray (position + color)
struct pix {
    pix(sf::Vector2u position, sf::Color color) : loc(position), c(color) {};
    sf::Vector2u loc;
    sf::Color c;
    inline friend bool operator ==(const util::pix &a, const util::pix &b);
};

inline bool operator ==(const util::pix &a, const util::pix &b) {
    return ( (a.loc == b.loc) && (a.c == b.c));
}

inline float Q_rsqrt( float number );
inline float euclideanDistance(const sf::Vector2u p1, const sf::Vector2u p2);
inline float euclideanDistanceInverse(const sf::Color c1, const sf::Color c2);
inline float euclideanDistanceSquared(const sf::Color c1, const sf::Color c2);
inline float taxiCabDistance(const sf::Color c1, const sf::Color c2);

inline float euclideanDistance(const sf::Vector2u p1, const sf::Vector2u p2) {
    return sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y));
}

inline float euclideanDistanceInverse(const sf::Color c1, const sf::Color c2) {
    //NOTE: lets try kicking out the alpha part - keep it strictly RGB
    return  util::Q_rsqrt(((int)c1.r-c2.r)*((int)c1.r-c2.r) + ((int)c1.g-c2.g)*((int)c1.g-c2.g) + ((int)c1.b-c2.b)*((int)c1.b-c2.b));
}

inline float euclideanDistanceSquared(const sf::Color c1, const sf::Color c2) {
    return ((int)c1.r-c2.r)*((int)c1.r-c2.r) + ((int)c1.g-c2.g)*((int)c1.g-c2.g) + ((int)c1.b-c2.b)*((int)c1.b-c2.b);
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