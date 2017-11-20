/*
 *      CHAI FILTER 
 *          -a work by Chaitanya Palaka
 * 
 *  What it is:
 *      - an image approximation algorithm using randomly drawn shapes/textures
 *      - check out readme for further information
 * 
 */
 
#include "cfEngine.h"
#include "PixelArray.h"
#include "Util.h"

int main(const int argc, const char* argv[]) {
    srand(time(0)); 
    cfEngine cf;

    //Setup critical parameters before calling init. 
    //Engine configuration via command line options is MANDATORY.
    if(!cf.configureEngineSettings(argc, argv)) {
        exit(1);
    }

    //initialize the engine and run the algorithm
    cf.init(sf::Color::Black);
    cf.runAlgo();

    //??????
    //Profit!
    return 0;
}
