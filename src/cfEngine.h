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

//Available types of graphics to be drawn using this algorithm
enum GraphicType {
    LINE = 0,
    CIRCLE,
    NONE,
};

//Available distance functions
enum distFunc {
    EUCLID,
    TAXICAB,
};

typedef float (*distance_func)(sf::Color,sf::Color);

//Main engine that runs the chan filter algorithm
class cfEngine {
public:
/*
 *  --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *                                               ~~ PUBLIC MEMBER FUNCTIONS ~~
 *  --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 */
    cfEngine();
    void init(sf::Color initialFill = sf::Color::White);                                // Initialize engine structures
    void runAlgo();                                                                     // Run the algorithm
    bool configureEngineSettings(const int argc, const char* argv []);                  // Setup enginer parameters
private:
/*
 *  --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *                                               ~~ PRIVATE MEMBER FUNCTIONS ~~
 *  --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 */
    //Main engine driver functions
    void algo_loop();                                                                   // Main algorithm loop. Called by runAlgo()
    void render_loop();                                                                 // Render Function used for visual mode. Updates window each iteration.
    void render_pixelArray();                                                           // Render function used to draw pixelArray to window.

    //Output to .bmp file functions
    void savePixelArrayToFile();                                                        // Save algorithm output to a BMP with detailed filename NOTE: uses EasyBMP
    void saveKmeansOutputToFile();                                                      // Save kmeans output to a BMP with detailed filename NOTE: uses EasyBMP
    std::string generateDetailedOutputString();                                         // Generates a detailed filename string from engine parameters

    //Line drawing algos
    void constructLine_naive(int x1, int y1, int x2, int y2, sf::Color color);          // Construct line raster on pixel array using my own naive algorithm
    void constructLine_bresenham(int x1, int y1, int x2, int y2, sf::Color color);      // Construct line raster on pixel array using Bresenham's line algorithm
    void constructCircle(int x0, int y0, int radius, sf::Color color);                  // Construct circle raster on pixel array

    //Pixel comparison functions
    long double measureAverageDistance_AABB(int id, sf::Rect<int> area);                // AABB bounding-box graphic comparison
    float measureAverageDistance_perPixel(int id);                                      // per-pixel graphic comparison
    
    //run kmeans algo on availableColors and output to kmeansColors
    void kmeans();                                                                      // Do kmeans clustering algorithm on availableColors in the RGB color space
    void showKmeansResult();                                                            // Show kmeans colors in window
    std::vector<sf::Color> doKmeansPlus();                                              // Do kmeans ++ initialization before kmeans clustering

    //sort color vector by hue
    void sortColorVector(std::vector<sf::Color>& vec);                                  // Sorts vectors of RGBA colors by converting to HSL space and sorting by hue
    util::HSL rgbToHsl(sf::Color c);                                                    // RGB to HSL conversion NOTE: copypasta from stackoverflow
    sf::Color hslToRgb(util::HSL hsl);                                                  // HSL to RGB conversion NOTE: copypasta from stackoverflow
    float hueToRgb(float p, float q, float t);                                          // Hue to RGB conversion NOTE: copypasta from stackoverflow - internal function used in hslToRgb()

    //help/debug functions/misc
    void printUsage();                                                                  // Print command line options NOTE: not implemented
    double callProfiler(std::string s);                                                 // Record runtime to profiler map
    void profilerOutput();                                                              // Print profiler output to terminal
    std::vector<sf::Color> getUniqueColorsFromImage();                                  // Removes all duplicate colors in vector
    inline void clearPixelBuffers() { pixBuffer.clear();pixBufferTemp.clear(); };       // Clears both pixelbuffers after each iteration so they can be used in the next iteration

    //'draw' the graphic to the pixelarray from the pixels present in pixBuffer
    inline void drawGraphic(bool flag = false) {
        for(const auto& i: pixBuffer) {
            if(flag) pixBufferTemp.push_back(util::pix(sf::Vector2u(i.loc.x, i.loc.y), pixelArray.getPixel(i.loc.x, i.loc.y)));
            if(i.loc.x >= 0 && i.loc.y >= 0 && i.loc.x < resolution.x && i.loc.y < resolution.y) {
                pixelArray.setPixel(i.loc.x, i.loc.y, i.c);
            }
        }
    }
    
    //undo the previously drawn line, saved in pixBufferTemp
    inline void undoGraphic() {
        for(const auto& i: pixBufferTemp) {
            pixelArray.setPixel(i.loc.x, i.loc.y, i.c);
        }
    }
/*
 *  --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *                                               ~~ PRIVATE MEMBER DATA ~~
 *  --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 */

    //Key engine objects
    PixelArray pixelArray;                                      // PixelArray object to which each pixel is saved before being drawn to screen/.bmp
    sf::Image inputpic;                                         // SFML representation of input image
    sf::Texture texture;                                        // SFML texture used to render to window
    sf::Sprite sprite;                                          // SFML sprite used to render to window
    std::vector<struct util::pix> pixBuffer, pixBufferTemp;     // The buffer where the graphic's pixels are stored to be compared
    std::vector<sf::Color> availableColors;                     // Vector of unique colors from input image
    std::vector<sf::Color> kmeansColors;                        // Vector of k-means colors

    //Key object properties
    sf::Vector2u resolution;                                    // stores the image width and height
    std::string input_filename;                                 // complete filename with file extension

    //Profiler
    std::map<std::string, double> profileResults;               // Profile results stored in map (string -> time taken)
    bool profileCode;                                           // 1: profile; 0: don't

    //General aLgo properties
    GraphicType graphicType;                                    // Chosen graphictype
    distFunc df;                                                // Distance function to use
    distance_func dfn;                                          // Function pointer to distance function in util.h
    int num_of_iter;                                            // Number of iterations (default: INT_MAX)
    
    //Line properties
    int max_line_length;                                        // Upper line length limit (default: 250)

    //Circle properties
    int max_radius;                                             // Upper circle radius limit (default: 100)
    bool isCircleFill;                                          // 1: filled; 0: outline

    //Mixed(line+circle) properties
    int lineToCircleRatio;                                      // Represents how many more lines get drawn rather than circles -> L in ratio L:C where L+C = 100
    int outlineToFillRatio;                                     // Represents how many more filled circles get drawn rather than outlines -> O in ratio O:F where O+F = 100
    bool isMixed;                                               // 1: Mix of lines and circles ; 0: not mixed
    bool notDrawn;                                              // Tries to control the skipping of filled circles in the random graphic selection process - 1: last randomized circle considered was not drawn ; 0: it was drawn
    
    //K-means clustering properties
    int num_clusters;                                           // Number of kmeans clusters (default: 0)
    bool useKmeans;                                             // 1: use ; 0: dont use
    bool showKMeansResult;                                      // 1: show colors in window ; 0: skip
    bool iskmeansSorted;                                        // internal flag used to skip redundant sorts
    bool useKmeansplus;                                         // 1: Perform kmeans++ initialization ; 0: don't

    //miscellaneous algorithm properties
    bool distanceIsInversed;                                    // Use inverse euclidean distance
    bool useAABBToCompare;                                      // Use AABB bounding-box comparison instead of per-pixel - 1: AABB ; 0: per-pixel NOTE: extremely slow
    bool visualMode;                                            // Controls whether each graphic drawn is rendered to window or not - 1: show ; 0: don't show NOTE: extremely slow
    
    //output file properties
    bool writeToBMPDetailed;                                    // Detailed output file names - 1: on ; 0: off
    bool writeToBMPNumbered;                                    // Numbered output file names - 1: on ; 0: off NOTE: not implemented
};

#endif //__CF__ENGINE_H__