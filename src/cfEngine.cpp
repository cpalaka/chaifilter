#include "cfEngine.h"
cfEngine::cfEngine() :
    num_of_iter(INT_MAX),
    distanceIsInversed(false),
    useAABBToCompare(false),
    max_line_length(50),
    dfn(NULL),
    visualMode(false),
    useKmeans(false),
    num_clusters(4),
    showKMeansResult(false)
{}

void cfEngine::init(sf::Color initialFill) {
    inputpic.loadFromFile("../images/" + input_filename);
    resolution = inputpic.getSize();

    availableColors = getUniqueColorsFromImage();
   
    //sortColorVector(availableColors);
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
    //filename -v(isual)/not -t line/circle/polygon/sprite number -i(terations) number(-1 for infinity) -d(istance) taxi/euclid/inverse(default euclid) -AABB(if not, then perpixel) -k/v num_clusters(do kmeans)
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
                    case 'k':
                    {
                        useKmeans = true;
                        if(strlen(argv[args]) == 3) showKMeansResult = true; // for option -kv (kmeans visual)
                        num_clusters = atoi(argv[args+1]);
                        args++;
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

    if(!toption && !useKmeans) {
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
    sf::Color chosen;
    if(useKmeans) {
        chosen = kmeansColors[rand()%kmeansColors.size()];
    } else {
        chosen = availableColors[rand()%availableColors.size()];
    }
    
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

    //compare + keep/drop newly drawn shape
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
            std::cout<<"hmm";
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
    //test
    if(useKmeans) {
        kmeans();
        //std::cout<<"Kmeans done. Size of kmeansColors: "<<kmeansColors.size()<<"\n";
        if(showKMeansResult) showKmeansResult();
    }

    if(visualMode) {
        render_loop();
    } else {
        //loop
        if(num_of_iter != INT_MAX) {
            int count = 0;
            while(count < num_of_iter) {
                algo_loop();
                count++;
            }

            render_pixelArray();
        }
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

void cfEngine::constructLine_bresenham() {

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
    int counter = 0;
    bool printCount = false;
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
        if(counter < num_of_iter) { 
            algo_loop();
        } else {
            if(!printCount) std::cout<<"Total loops: "<<counter<<"\n"; printCount = true;
        }

        texture.update(pixelArray.getByteArray());
        window.draw(sprite);
        window.display();
        counter++;
    }
    std::cout<<"Total loops: "<<counter<<"\n";
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

void cfEngine::kmeans() {
    int kmeans_max_iter = 10;

    std::vector<sf::Color> means, temp;
    std::vector<std::pair<sf::Color, int> > _ksets;

    //randomly initialize points
    for(int i = 0; i < num_clusters; ++i) {
        means.push_back(availableColors[rand()%availableColors.size()]);
    }
    
    int iter = 0;
    while(iter < kmeans_max_iter) {
        //assign each color to its closest kmeans color
        for(int i = 0; i < availableColors.size(); ++i) {
            float leastDist = FLT_MAX;
            int leastIndex = 0;

            for(int j = 0; j < num_clusters; ++j) {
                float dist = util::euclideanDistanceSquared(availableColors[i], means[j]);
                if(dist < leastDist) {
                    leastDist = dist;
                    leastIndex = j;
                }
            }
            _ksets.push_back(std::make_pair(availableColors[i], leastIndex));
        }

        //calculate new centroids
        for(int i = 0; i < num_clusters; ++i) {
            float rsum=0, gsum=0, bsum=0, asum=0;
            std::vector<int> clusterSizeCounts(num_clusters, 0); 
            for(int j = 0; j < _ksets.size(); ++j) {

                if(_ksets[j].second == i) {
                    rsum += (float)_ksets[j].first.r;
                    gsum += (float)_ksets[j].first.g;
                    bsum += (float)_ksets[j].first.b;
                    asum += (float)_ksets[j].first.a;
                    clusterSizeCounts[i]++;
                }
            }
            temp.push_back(sf::Color((int)rsum/clusterSizeCounts[i], (int)gsum/clusterSizeCounts[i], (int)bsum/clusterSizeCounts[i], (int)asum/clusterSizeCounts[i]));
        }
        
        //break out of loop if it converges
        if(temp == means) {
            std::cout<<"Broke out of loop at iter: "<<iter<<"\n";
            break;
        } else {
            means = std::vector<sf::Color>(temp);
            temp.clear();
        }
        iter++;
        _ksets.clear();
    }

    //store calculated means to kmeansColors
    kmeansColors = std::vector<sf::Color>(means);
}

void cfEngine::showKmeansResult() {
    sf::RenderWindow window(sf::VideoMode(1000, 300), "kMeans Result");
    std::vector<sf::RectangleShape> color_rects;

    //sort the vector by hue so it looks nice
    sortColorVector(kmeansColors);

    for(int i = 0; i<num_clusters ;++i) {
        sf::RectangleShape c;
        c.setSize(sf::Vector2f(1000/num_clusters, 300));
        c.setPosition((1000/num_clusters) * i, 0);
        c.setFillColor(kmeansColors[i]);
        color_rects.push_back(c);
    }
    
    while (window.isOpen()) 
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        window.clear(sf::Color::Black);
        for(int i = 0; i < num_clusters ; ++i) {
            window.draw(color_rects[i]);
        }
        window.display();
    }
}

void cfEngine::sortColorVector(std::vector<sf::Color>& vec) {
    std::vector<util::HSL> colors;

    for(const auto& i: vec) {
        colors.push_back(rgbToHsl(i));
    }

    std::sort(colors.begin(), colors.end(), [](util::HSL a, util::HSL b) { return a.h < b.h ;});
    std::vector<sf::Color> sortedRGB;

    for(int i = 0; i<colors.size(); ++i) {
        sortedRGB.push_back(vec[colors[i].id]);
    }

    //vec = sortedRGB;
    assert(vec.size() == sortedRGB.size());
    vec = std::vector<sf::Color>(sortedRGB);
}

util::HSL cfEngine::rgbToHsl(sf::Color c) {
    static int i = 0;
    float r = (float)c.r/255, g = (float)c.g/255, b = (float)c.b/255;

    std::vector<float> v({(float)c.r/255, (float)c.g/255, (float)c.b/255});
    std::sort(v.begin(), v.end());
    float max = v[2]; 
    float min = v[0];
    float h = (max + min)/2;
    float s = h;
    float l = h;

    if(max == min) {
      h = 0; // achromatic
      s=0;
    } else {
      float d = max - min;
      s = (l > 0.5) ? d / (2 - max - min) : d / (max + min);

      if(max == r) h = (g - b) / d + (g < b ? 6 : 0);
      else if(max == g) h = (b - r) / d + 2;
      else if(max == b) h = (r - g) / d + 4;
      h /= 6;
    }
    util::HSL result(h * 360, s * 100, l * 100, i);
    i++;
    return result;
}

/*
var sortedRgbArr = rgbArr.map(function(c, i) {
    // Convert to HSL and keep track of original indices
    return {color: rgbToHsl(c), index: i};
  }).sort(function(c1, c2) {
    // Sort by hue
    return c1.color[0] - c2.color[0];
  }).map(function(data) {
    // Retrieve original RGB color
    return rgbArr[data.index];
  });
  */