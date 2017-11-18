/*
 *      CHAN FILTER 
 *          -a work by Chaitanya Palaka, inspired by 4chan's gentoomen
 * 
 * 
 * 
 *  What it is:
 *      - an image approximation algorithm using randomly drawn shapes/textures
 * 
 * 
 */

//TODO: implement cosine similarity
//TODO: add more shapes: (solid fill)circles, squares --- sprites
//TODO: implememt bresenhams algo
 
#include "cfEngine.h"
#include "PixelArray.h"
#include "Util.h"

int main(const int argc, const char* argv[]) {
    srand(time(0)); 

    cfEngine cf;
    if(!cf.configureEngineSettings(argc, argv)) {
        exit(1);
    }
    cf.init(sf::Color::Black);
    cf.runAlgo();
    return 0;
}
