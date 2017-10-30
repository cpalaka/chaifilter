//TODO: profile entire code and see what is slowing things down so much
#include "PixelArray.h"

#define DEBUG 0
#define VISUAL 0 //set this to draw the algo's progress to the window (a lot slower)
#define VIEWCTR 0 //set this to view the line counter on the console (a little slower)
#define ITERATIONS 20000000

std::vector<sf::Color> getUniqueColorsFromImage(const sf::Image &image);
inline float euclideanDistance(sf::Color c1, sf::Color c2);
inline float euclideanDistance(sf::Vector2u p1, sf::Vector2u p2);
void algo_loop(sf::Image &img, PixelArray &pixelArray, std::vector<sf::Color> &uniqueColors);

//TODO: implement more distance functions
//TODO: implement color segmenting <-- urgent
//NOTE: it could be worth multithreading this

distance_func PixelArray::dfn = euclideanDistance;

int main(int argc, char* argv[]) {
    srand(time(0));

    sf::Image pic;
    pic.loadFromFile("../images/pic.jpg");
    sf::Vector2u img_resolution = pic.getSize();
    std::cout<<"Image size: "<<img_resolution.x<<" x "<<img_resolution.y<<"\n";
    std::cout<<"Total number of pixels = "<<img_resolution.x*img_resolution.y<<"\n";
    
    std::vector<sf::Color> uniqueColors(getUniqueColorsFromImage(pic));
    //std::sort(uniqueColors.begin(), uniqueColors.end());

    std::cout<<"Number of unique colors in the picture: "<<uniqueColors.size()<<"\n";
/*
    std::cout<<"R G B A\n";
    for(const auto& col: uniqueColors) {
        std::cout<<(int)col.r<<" "<<(int)col.g<<" "<<(int)col.b<<" "<<(int)col.a<<"\n";
    }
*/
    sf::Texture texture;
    texture.create(img_resolution.x, img_resolution.y);

    sf::Sprite sprite;
    sprite.setTexture(texture);
    
    PixelArray pixelArray(img_resolution, sf::Color::White);

    //debugging clusterfuck coming through
#if !VISUAL
    int counter = 0;
    while(counter <= ITERATIONS) {
    #if VIEWCTR
        if ( counter% 30 == 0) system("cls");
    #endif
        if(counter%20000 == 0) std::cout<<counter<<"\n";
        algo_loop(pic, pixelArray, uniqueColors);
        counter++;
    #if VIEWCTR
        std::cout<<counter<<"\n";
    #endif
    }
#endif


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
        algo_loop(pic, pixelArray, uniqueColors);
#endif
        texture.update(pixelArray.getByteArray());
        window.draw(sprite);
        window.display();
    }
    //std::cout<<skipped<<std::endl;
    return 0;
}

void algo_loop(sf::Image& img, PixelArray& pixelArray, std::vector<sf::Color>& uniqueColors) {
    //custom lines
    /*sf::Color chosen = sf::Color::Blue;
    int x1 = 40, y1 = 100;
    int x2 = 550, y2 = 322;*/

    //static int skipped = 0;
    //randomized lines
    sf::Vector2u img_resolution = img.getSize();

    sf::Color chosen = uniqueColors[rand()%uniqueColors.size()];
    int x1, x2, y1, y2;
    float line_length;
    do {
        x1 = rand()%img_resolution.x; 
        y1 = rand()%img_resolution.y;
        x2 = rand()%img_resolution.x; 
        y2 = rand()%img_resolution.y;
        //std::cout<<"redo\n";
    } while((line_length = euclideanDistance(sf::Vector2u(x1, y1), sf::Vector2u(x2, y2))) >= 50.0);

    

    //save the lines' info (color and pixel locations) in an internal vector
    sf::Rect<int> area((x1<x2?x1:x2),(y1<y2?y2:y1),(x1<x2?x2-x1:x1-x2),(y1<y2?y2-y1:y1-y2));
    pixelArray.prepareLine(x1,y1,x2,y2,chosen);
    
  
    //NOTE: line only below

    float without = pixelArray.measureDistance_line(img, 0);
    float with = pixelArray.measureDistance_line(img, 1);

    //NOTE:AABB BELOW
    //PixelArray temp(pixelArray);

    //float without = pixelArray.measureDistance_AABB(img, 0, area);
    //float with = pixelArray.measureDistance_AABB(img, 1, area);
    
    //if drawing the line increases the overall distance measurement,
    if(with < without)  {
        pixelArray.drawLine();
        //std::cout<<"not good\n";
        //pixelArray = temp;
        //pixelArray.undoLine();
        //assert(pixelArray == temp);
    }
    //else skipped++;
#if DEBUG
    int n;
    std::cout<<"("<<x1<<","<<y1<<")"<<"("<<x2<<","<<y2<<")\n";
    std::cout<<"Euclidean distance of line: "<<line_length<<"\n";
    //check distance values if we use AABB bounding box instead
    //sf::Rect<int> area((x1<x2?x1:x2),(y1<y2?y2:y1),(x1<x2?x2-x1:x1-x2),(y1<y2?y2-y1:y1-y2));
    std::cout<<"top: "<<area.top<<", left:"<<area.left<<", width:"<<area.width<<",height:"<<area.height<<std::endl;
    std::cout<<(int)chosen.r<<" "<<(int)chosen.g<<" "<<(int)chosen.b<<" "<<(int)chosen.a<<"\n";
    std::cin >> n;
    //long double _without = pixelArray.measureDistance_AABB(pic, 0, area);
    //long double _with = pixelArray.measureDistance_AABB(pic, 1, area);
 
    //std::cout<<"(all)with: "<<_with<<" (all)without: "<<_without<<std::endl;
#endif

    pixelArray.clearLinePixels();
    pixelArray.clearPrevLinePixels();
    return;
}

std::vector<sf::Color> getUniqueColorsFromImage(const sf::Image &image) {
    std::vector<sf::Color> colorvec;  

    for(int i = 0; i < image.getSize().x; ++i) {
        for(int j = 0; j < image.getSize().y; ++j) {
            colorvec.push_back(image.getPixel(i, j));
        }
    }
    
    //NOTE: should i keep only unique colors or should i get all colors?
    auto del_repeats = std::unique(colorvec.begin(), colorvec.end(), [](auto a, auto b){ return a == b;});
    return std::vector<sf::Color>(colorvec.begin(), del_repeats);

    //return colorvec;
}

inline float euclideanDistance(sf::Vector2u p1, sf::Vector2u p2) {
    return sqrt(pow(p1.x-p2.x, 2) + pow(p1.y-p2.y, 2));
}

inline float euclideanDistance(sf::Color c1, sf::Color c2) {
    //NOTE: lets try kicking out the alpha part - keep it strictly RGB
    return sqrt(pow(c1.r-c2.r, 2) + pow(c1.g-c2.g, 2) + pow(c1.b-c2.b, 2));
}