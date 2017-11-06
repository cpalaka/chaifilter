/*
 *      CHAN FILTER 
 *          -a work by Chaitanya Palaka, inspired by 4chan's gentoomen
 * 
 */

//TODO: profile entire code and see what is slowing things down so much
//TODO: implement more distance functions

//TODO: implement color segmenting <-- urgent

//NOTE: it could be worth multithreading this

//TODO: think about how i can write about this stuff in a blog post(or multiple) for example, line algo - SFML - setup in vscode, etc

//TODO: add more shapes that can be drawn

/*NOTE: when doing kmeans to segment colors, instead of getting only the k colors, add
*       a proportional number of that color with respect to its size (cluster size)
*/

//TODO: add feature where not only can you make the image out of shapes, but other images

//TODO: lines, polygons, circles,etc
#include "cfEngine.h"
#include "PixelArray.h"
#include "Util.h"

void algo_loop(const sf::Image &img, PixelArray &pixelArray,const  std::vector<sf::Color> &chosenColors);

void render_loop(const sf::Vector2u, const sf::Image &, PixelArray &, const std::vector<sf::Color> &, sf::Texture &, sf::Sprite &, unsigned int &counter);

int main(const int argc, const char* argv[]) {
    srand(time(0)); 

    cfEngine cf;
    cf.init(sf::Color::Blue);
    if(!cf.configureEngineSettings(argc, argv)) {
        exit(1);
    }
    cf.runAlgo();
    //std::cout<<type<<std::endl;
   /*
    //debugging clusterfuck coming through
#if !VISUAL
    counter = 0;
    while(counter <= ITERATIONS) {
    #if VIEWCTR
        if ( counter% 30 == 0) system("cls");
    #endif
        if(counter%10000 == 0) std::cout<<counter<<"\n";
        algo_loop(pic, pixelArray, chosenColors);
        counter++;
    #if VIEWCTR
        std::cout<<counter<<"\n";
    #endif
    }
#endif
    int n;
    unsigned int loopCounter = 0;
    render_loop(img_resolution, pic, pixelArray, chosenColors, texture, sprite, loopCounter);
    std::cout<<"Total loops: "<<loopCounter<<"\n";
    std::cin>>n;
    */
    return 0;
}

void render_loop(const sf::Vector2u img_resolution, const sf::Image & pic, PixelArray &pixelArray, const std::vector<sf::Color>& chosenColors, sf::Texture &texture, sf::Sprite &sprite, unsigned int &counter) {
    //int counter = 0;
    sf::RenderWindow window(sf::VideoMode(img_resolution.x, img_resolution.y), "ChanFilter");
    while (window.isOpen()) 
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        window.clear(sf::Color::Black);
#if VISUAL
        //algo_loop(pic, pixelArray, chosenColors);
        //our algo loop
        
        //if(counter%100000 == 0) std::cout<<counter<<"\n";
#endif
        texture.update(pixelArray.getByteArray());
        window.draw(sprite);
        window.display();

        counter++;
        int n;
#if DEBUG
        if(counter == 2) std::cin>>n;
#endif
    }
}

void algo_loop(const sf::Image& img, PixelArray& pixelArray, const std::vector<sf::Color>& chosenColors) {
    //randomized lines
    sf::Vector2u img_resolution = img.getSize();
    static int algo_loop_counter = 0;
    int permittedLength = 1000;

    sf::Color chosen = chosenColors[rand()%chosenColors.size()];
    int x1, x2, y1, y2;
    float line_length;

    //some stuipid shit
    if(algo_loop_counter % 1000 == 0 && permittedLength >= 10) permittedLength-=algo_loop_counter;

    //for lines - have 'max line length' setting,
    //different shapes will have different settings'
    //so maybe we need a settings class/struct?
    do {
        x1 = rand()%img_resolution.x; 
        y1 = rand()%img_resolution.y;
        x2 = rand()%img_resolution.x; 
        y2 = rand()%img_resolution.y;
    } while((line_length = util::euclideanDistance(sf::Vector2u(x1, y1), sf::Vector2u(x2, y2))) >= 65);

    //save the lines' info (color and pixel locations) in an internal vector
    sf::Rect<int> area((x1<x2?x1:x2),(y1<y2?y2:y1),(x1<x2?x2-x1:x1-x2),(y1<y2?y2-y1:y1-y2));
    pixelArray.constructLine_naive(x1,y1,x2,y2,chosen);
    
    //NOTE: line only below

    float without = pixelArray.measureAverageDistance_perPixel(img, 0);
    float with = pixelArray.measureAverageDistance_perPixel(img, 1);

    //NOTE:AABB BELOW 
    //TODO: FIX THIS
    //PixelArray temp(pixelArray);

    //float without = pixelArray.measureAverageDistance_AABB(img, 0, area);
    //float with = pixelArray.measureAverageDistance_AABB(img, 1, area);
    
    //TODO: implement this in a better way, jesus
    //if drawing the line increases the overall distance measurement,
    if(with <= without)  {
        pixelArray.drawLine();
        //std::cout<<"not good\n";
        //pixelArray = temp;
        //pixelArray.undoLine();
        //assert(pixelArray == temp);
        
    } else {
        //save one of the calculation so w
    }
#if DEBUG
    int n;
    std::cout<<"("<<x1<<","<<y1<<")"<<"("<<x2<<","<<y2<<")\n";
    std::cout<<"Euclidean inverse distance of line: "<<line_length<<"\n";
    //check distance values if we use AABB bounding box instead
    //sf::Rect<int> area((x1<x2?x1:x2),(y1<y2?y2:y1),(x1<x2?x2-x1:x1-x2),(y1<y2?y2-y1:y1-y2));
    std::cout<<"top: "<<area.top<<", left:"<<area.left<<", width:"<<area.width<<",height:"<<area.height<<std::endl;
    std::cout<<(int)chosen.r<<" "<<(int)chosen.g<<" "<<(int)chosen.b<<" "<<(int)chosen.a<<"\n";
    std::cout<<"d with line: "<<with<<" d without line: "<<without<<std::endl;
#endif

    pixelArray.clearPixelBuffers();
    algo_loop_counter++;
    return;
}

