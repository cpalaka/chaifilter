#include "cfEngine.h"
cfEngine::cfEngine() :
    num_of_iter(0),
    distanceIsInversed(false),
    useAABBToCompare(false),
    max_line_length(50),
    dfn(NULL),
    visualMode(false)
{}

void cfEngine::init(sf::Color initialFill) {
    inputpic.loadFromFile("../images/" + input_filename);
    resolution = inputpic.getSize();

    availableColors = getUniqueColorsFromImage();
    texture.create(resolution.x, resolution.y);
    sprite.setTexture(texture);
    pixelArray = PixelArray(resolution, initialFill);

    //chan filter init debug info
    std::cout<<"Image size: "<<resolution.x<<" x "<<resolution.y<<"\n";
    std::cout<<"Total number of pixels = "<<resolution.x*resolution.y<<"\n";
    std::cout<<"Size of availableColors: "<<availableColors.size()<<"\n";
}

bool cfEngine::configureEngineSettings(const int argc,const char* argv []) {
    //Handle command line options
    //filename -v(isual)/not -t line/circle/polygon/sprite number -i(terations) number(-1 for infinity) -d(istance) taxi/euclid/inverse(default euclid) -AABB(if not, then perpixel) 
    bool toption = false;
    bool distanceMetricSpecified = false;
    if(argc <= 1) {
        std::cout<<"ChanFilter requires atleast the image filename from the 'images' folder\n";
        printUsage();
        return 0;
    } else {
        int args = 1;
        while(args < argc) {
            //command line options with -
            if(argv[args][0] == '-') {
                switch(argv[args][1]) {
                    case 't':
                    {
                        toption = true;
                        switch(argv[args+1][0]) {
                            case 'L':
                            case 'l':
                            {
                                graphicType = LINE;
                                max_line_length = atoi(argv[args+2]);
                                args++;
                                break;
                            }
                            case 'C':
                            case 'c':
                            {
                                graphicType = CIRCLE;
                                break;
                            }
                            case 'P':
                            case 'p':
                            {
                                graphicType = POLYGON;
                                break;
                            }
                            case 'S':
                            case 's':
                            {
                                graphicType = SPRITE;
                                break;
                            }
                            default:
                            {
                                std::cout<<"Invalid GraphicType.\n";
                                printUsage();
                                return 0;
                            }
                        }
                        args++;
                        break;
                    }
                    case 'i':
                    {
                        num_of_iter = atoi(argv[args+1]);
                        args++;
                        break;
                    }
                    case 'd':
                    {
                        distanceMetricSpecified = true;
                        if(argv[args+1][0] == 'e') {
                            dfn = util::euclideanDistanceSquared;
                        } else
                        if(argv[args+1][0] == 't') {
                            dfn = util::taxiCabDistance;
                        } else
                        if(argv[args+1][0] == 'i') {
                            dfn == util::euclideanDistanceInverse;
                            distanceIsInversed = true;
                        }
                        args++;
                        break;
                    }
                    case 'A':
                    {
                        useAABBToCompare = true;
                        break;
                    }
                    case 'v':
                    {
                        visualMode = true;
                        break;
                    }
                    default:
                    {
                        std::cout<<"Option doesn't exist. Yet.\n";
                        printUsage();
                        return 0;
                    }
                }
            } else { //only image filename
                //TODO:check that filename ends with .jpg/.jpeg/.bmp/.png/.gif
                input_filename = std::string(argv[args]);
            }
            args++;
        }
    }

    if(!toption) {
        std::cout<<"You didn't set the GraphicType(-t) option. You can select either line/LINE, circle/CIRCLE, polygon/POLY, or sprite/SPR\n";
        printUsage();
        return 0;
    }
    if(!distanceMetricSpecified) {
        dfn = util::euclideanDistanceSquared;
    }
    return 1;
}

void cfEngine::printUsage() {
    //print the settings options usage in a fancy way
}

std::vector<sf::Color> cfEngine::getUniqueColorsFromImage() {
    std::vector<sf::Color> colorvec;  
    for(int i = 0; i < inputpic.getSize().x; ++i) {
        for(int j = 0; j < inputpic.getSize().y; ++j) {
           colorvec.push_back(inputpic.getPixel(i, j));
        }
    }
        
    auto del_repeats = std::unique(colorvec.begin(), colorvec.end(), [](auto a, auto b){ return a == b;});
    return std::vector<sf::Color>(colorvec.begin(), del_repeats);
}

void cfEngine::algo_loop() {
    //choose a random color from availableColors
    sf::Color chosen = availableColors[rand()%availableColors.size()];
    sf::Rect<int> area;

    //choose random shape and construct it on pixelArray
    switch(graphicType) {
        case LINE:
        {
            int x1, x2, y1, y2;
            do {
                x1 = rand()%resolution.x; 
                y1 = rand()%resolution.y;
                x2 = rand()%resolution.x; 
                y2 = rand()%resolution.y;
            } while(util::euclideanDistance(sf::Vector2u(x1, y1), sf::Vector2u(x2, y2)) >= max_line_length);
            
            area = sf::Rect<int>((x1<x2?x1:x2),(y1<y2?y2:y1),(x1<x2?x2-x1:x1-x2),(y1<y2?y2-y1:y1-y2));
            constructLine_naive(x1, y1, x2, y2, chosen);
            break;
        }
        case CIRCLE:
        {
        
            break;
        }
        case POLYGON:
        {

            break;
        }
        case SPRITE:
        {

        
            break;
        }
        default:
            break;
    }

    if(useAABBToCompare) {
        PixelArray temp(pixelArray);

        long double without = measureAverageDistance_AABB(0, area);
        long double with = measureAverageDistance_AABB(1, area);

        if(distanceIsInversed) {
            if(with < without) {
                undoGraphic();
                assert(pixelArray == temp);
            }
        } else {
            if(with > without) {
                undoGraphic();
                assert(pixelArray == temp);
            }
        }
    } else {
        float without = measureAverageDistance_perPixel(0);
        float with = measureAverageDistance_perPixel(1);

        if(distanceIsInversed) {
            if(with > without) {
                drawGraphic();
            }
        } else {
            if(with < without) {
                drawGraphic();
            }
        } 
    }

    //clear pixel buffers
    clearPixelBuffers();

}

void cfEngine::runAlgo() {
    //if(num_of_iter == 0) <= do algo infinite
    //runs algo loop or render loop
    if(visualMode) {
        render_loop();
    } else {
        //loop
        int count = 0;
        while(count < num_of_iter) {
            algo_loop();
            count++;
        }

        render_pixelArray();
    }
}

//setup affected-pixel vector (pixBuffer) so we can compare later
void cfEngine::constructLine_naive(int x1, int y1, int x2, int y2, sf::Color color) {
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
            int y = floor(m*(x-x1) + y1);
            pixBuffer.push_back(util::pix(sf::Vector2u(floor(x),y), color));
        }
    } else {
        if(y1 > y2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }

        for(float y = area.top - area.height; y < area.top; ++y) {
            int x = floor((y-y1)/m + x1);
            pixBuffer.push_back(util::pix(sf::Vector2u(x,floor(y)), color));
        }
    }
}

//do the distance measure with pixels in the AABB bounding box. mainly for testing and curiosity
long double cfEngine::measureAverageDistance_AABB(int id, sf::Rect<int> area) {
    long double distance = 0;
    std::vector<sf::Color> oldpix(pixelArray.getColorVector());
    if(id) drawGraphic(true);

    //measure all pixels
    for(int i = area.left; i<(area.left+area.width); ++i) {
        for(int j = area.top-area.height; j< area.top; ++j) {     
            distance += dfn( (id)? pixelArray.getPixel(i, j) : oldpix[j*resolution.x + i + 1], inputpic.getPixel(i,j));
        }
    }
    return distance/area.width*area.height;
}

//do the distance measure with pixels only on the line
float cfEngine::measureAverageDistance_perPixel(int id) {
    float distance = 0;
    //measure only pixels of recently drawn line
    for(const auto& i: pixBuffer) {
        distance += dfn( (id)?i.c : pixelArray.getPixel(i.loc.x, i.loc.y), inputpic.getPixel(i.loc.x, i.loc.y));
    }
    return distance/pixBuffer.size();
}

void cfEngine::render_loop() {
    //int counter = 0;
    sf::RenderWindow window(sf::VideoMode(resolution.x, resolution.y), "ChanFilter");
    while (window.isOpen()) 
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        window.clear(sf::Color::Black);
        algo_loop();
        texture.update(pixelArray.getByteArray());
        window.draw(sprite);
        window.display();

    }
}

void cfEngine::render_pixelArray() {
    sf::RenderWindow window(sf::VideoMode(resolution.x, resolution.y), "ChanFilter");
    while (window.isOpen()) 
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        window.clear(sf::Color::Black);
        texture.update(pixelArray.getByteArray());
        window.draw(sprite);
        window.display();
    }
}