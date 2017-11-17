#include "cfEngine.h"
cfEngine::cfEngine() :
    num_of_iter(INT_MAX),
    distanceIsInversed(false),
    useAABBToCompare(false),
    max_line_length(0),
    dfn(NULL),
    visualMode(false),
    useKmeans(false),
    num_clusters(0),
    graphicType(NONE),
    showKMeansResult(false),
    useKmeansplus(false),
    writeToBMPDetailed(false),
    writeToBMPNumbered(false),
    df(EUCLID),
    iskmeansSorted(false)
{}

void cfEngine::init(sf::Color initialFill) {
    if(!inputpic.loadFromFile("images/" + input_filename)) {
        exit(1);
    }
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
    //filename -v(isual)/not -t line/circle/polygon/sprite number -i(terations) number(-1 for infinity) -d(istance) taxi/euclid/inverse(default euclid) -AABB(if not, then perpixel) -k/v/+ num_clusters(do kmeans) -w(output file) -p (profile code and show results)
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
                                if(args+2 > argc-1 || argv[args+2][0] == '-') {std::cout<<"Options error.\n"; printUsage();exit(1);}
                                else {max_line_length = atoi(argv[args+2]);}
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
                            df = EUCLID;
                        } else
                        if(argv[args+1][0] == 't') {
                            dfn = util::taxiCabDistance;
                            df = TAXICAB;
                        } else
                        if(argv[args+1][0] == 'i') {// this shit is never used anyways
                            dfn == util::euclideanDistanceInverse;
                            distanceIsInversed = true;
                        } else
                        if(argv[args+1][0] == 'c') {
                            df = COSINE;
                        }else 
                        if(argv[args+1][0] == 'j') {
                            df = JACCARD;
                        }else {
                            std::cout<<"Options error.\n";
                            printUsage();
                            exit(1);
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
                        if(strlen(argv[args]) == 3) {
                            if(argv[args][2] == 'v') showKMeansResult = true; // for option -kv (kmeans visual)
                            if(argv[args][2] == '+') useKmeansplus = true; // for option -kv (kmeans visual)
                        }
                        if(strlen(argv[args]) == 4) useKmeansplus = true; // for option -kv+
                        num_clusters = atoi(argv[args+1]);
                        args++;
                        break;
                    }
                    case 'w':
                    {
                        writeToBMPDetailed = true;
                        break;
                    }
                    case 'o':
                    {
                        writeToBMPNumbered = true;

                    }
                    case 'h':
                    {
                        printUsage();
                        return 0;
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
    std::set<util::Color> uniquecolorset;

    for(int i = 0; i < inputpic.getSize().x; ++i) {
        for(int j = 0; j < inputpic.getSize().y; ++j) {
            uniquecolorset.insert(util::Color(inputpic.getPixel(i, j)));
        }
    }
    
    for(const auto &i: uniquecolorset) {
        colorvec.push_back(sf::Color(i.r, i.g, i.b));
    }
    
    return colorvec;
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

        util::Timer t;
        t.start();

        kmeans();

        double t_elapsed = t.stop();
        std::cout<<"Kmeans took "<<t_elapsed/1000000<<" milliseconds.\n";

        assert(num_clusters == kmeansColors.size());
        if(showKMeansResult) {
            showKmeansResult();
            saveKmeansOutputToFile();
        }
    }

    if(visualMode) {
        render_loop();
    } else {
        util::Timer t;
        
        //loop
        if(num_of_iter != INT_MAX) {
            int count = 0;

            t.start();

            while(count < num_of_iter) {
                algo_loop();
                count++;
            }

            double t_elapsed = t.stop();
            std::cout<<"Algo("<<num_of_iter<<") took "<<t_elapsed/1000000000<<" seconds.\n";


            render_pixelArray();
        }
    }

    if(writeToBMPDetailed) {
        savePixelArrayToFile();
    }
}

void cfEngine::savePixelArrayToFile() {
    //construct BMP file
    std::vector<sf::Color> vec = pixelArray.getColorVector();
    BMP output;
    output.SetSize(resolution.x, resolution.y);
    output.SetBitDepth(32);
    
    for(int i = 0; i < resolution.x; ++i) {
        for(int j = 0 ; j< resolution.y ; ++j) {
            sf::Color c = pixelArray.getPixel(i, j);
            output(i,j)->Red = c.r;
            output(i,j)->Green = c.g;
            output(i,j)->Blue = c.b;
            output(i,j)->Alpha = c.a;

        }
    }

    std::string outfile_name = generateDetailedOutputString();
    output.WriteToFile(outfile_name.c_str());
}

void cfEngine::saveKmeansOutputToFile() {
    int widthperColor = 30;//in pixels
    int height = 200;
    if(kmeansColors.empty()) {
        std::cout<<"error: Kmeans segmentation not specified. Nothing to draw.\n";
        return;
    }
    sortColorVector(kmeansColors);

    BMP output;
    output.SetSize(widthperColor*num_clusters, height);
    output.SetBitDepth(32);

    for(int c = 0; c < num_clusters; ++c) {
        sf::Color col = kmeansColors[c];
        for(int i = widthperColor*c; i < widthperColor*c + widthperColor; ++i) {
            for(int j = 0; j < height; ++j) {
                output(i, j)->Red = col.r;
                output(i,j)->Green = col.g;
                output(i,j)->Blue = col.b;
                output(i,j)->Alpha = 255;
            }
        }
    }

    std::string outfile_name = "output/" + input_filename + " k=" + std::to_string(num_clusters) + ".jpg";
    output.WriteToFile(outfile_name.c_str());
}

std::string cfEngine::generateDetailedOutputString() {
    std::string type;
    std::string dist_measure;
    std::string kmeansstr;
    int file_len = std::strcspn(input_filename.c_str(), ".");
    input_filename.resize(file_len, '0');
    switch(graphicType) {
        case LINE:
            type = "line";
            break;
        case CIRCLE:
            type = "circle";
            break;
        default:
            break;
    }

    switch(df) {
        case EUCLID:
            dist_measure = "euclid";
            break;
        case TAXICAB:
            dist_measure = "taxicab";
            break;
        case COSINE:
            dist_measure = "cosine";
            break;
        case JACCARD:
            dist_measure = "jaccard";
            break;
        default:
            break;
    }
    if(useKmeans) {
        kmeansstr = "k="+ std::to_string(num_clusters);
    }
    const std::string string = "output/"+input_filename+"-"+type+"("+ std::to_string(max_line_length) +")-iter"+ std::to_string(num_of_iter)+"-"+kmeansstr+"df="+dist_measure+".bmp";
    return string;
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

void cfEngine::constructLine_bresenham(int x0, int y0, int x1, int y1, sf::Color color) {
    float deltax = x1 - x0;
    float deltay = y1 - y0;
    float deltaerr = abs(deltay/deltax);
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
    int kmeans_max_iter = 50;

    std::vector<sf::Color> means, temp;
    std::vector<std::pair<sf::Color, int> > _ksets;

    if(useKmeansplus) {
        //TODO:use kmeans+ algo to initialize points

    } else {
        //randomly initialize points
        for(int i = 0; i < num_clusters; ++i) {
            means.push_back(availableColors[rand()%availableColors.size()]);
        }
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
            if(clusterSizeCounts[i] != 0) {
                temp.push_back(sf::Color((int)rsum/clusterSizeCounts[i], (int)gsum/clusterSizeCounts[i], (int)bsum/clusterSizeCounts[i], (int)asum/clusterSizeCounts[i]));
            } else {
                temp.push_back(means[i]);
            }
        }
        
        //break out of loop if it converges
        if(temp == means) {
            //std::cout<<"Broke out of loop at iter: "<<iter<<"\n";
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
    if(iskmeansSorted) {
        //std::cout<<"error: Kmeans Color Vector is already sorted.\n";
        return;
    } else {
        iskmeansSorted = true;

    }
    std::vector<util::HSL> colors;

    for(const auto& i: vec) {
        colors.push_back(rgbToHsl(i));
    }

    /*
    std::cout<<"HSL kmeans-\n";
    int x = 0;
    for(const auto& c: colors) {
        std::cout<<x<<". H="<<c.h<<" S="<<c.s<<" L="<<c.l<<"\t"<<"R="<<(int)kmeansColors[x].r<<" G="<<(int)kmeansColors[x].g<<" B="<<(int)kmeansColors[x].b<<"\n";
        x++;
    }
    */
    std::sort(colors.begin(), colors.end(), [](util::HSL a, util::HSL b) { 
        if(a.s == b.l && a.s == b.s) {
            return a.h < b.h ;
        } else {
            return (a.l > b.l);
        }
    });
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

    std::vector<float> v({r, g, b});
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

      if(max == r)      h = ((g - b) / d) + ( (g < b) ? 6 : 0);
      else if(max == g) h = ((b - r) / d) + 2.0;
      else if(max == b) h = ((r - g) / d) + 4.0;
      h = h/6;
    }

    util::HSL result(h, s, l, i);
    i++;
    return result;
}

sf::Color cfEngine::hslToRgb(util::HSL hsl) {
    float r, g, b;

    if(hsl.s == 0) {
        r = hsl.l;
        g = hsl.l;
        b = hsl.l;
    } else {
        float q = (hsl.l < 0.5) ? (hsl.l * (1 + hsl.s)) : (hsl.l + hsl.s - (hsl.l*hsl.s));
        float p = (2* hsl.l - q);
        r = hueToRgb(p, q, hsl.h + 1/3);
        g = hueToRgb(p, q, hsl.h);
        b = hueToRgb(p, q, hsl.h - 1/3);
    }

    return sf::Color(std::round(r*255), std::round(g*255), std::round(b*255));
}

float cfEngine::hueToRgb(float p, float q, float t) {
    if(t < 0) t += 1;
    if(t > 1) t -= 1;
    if(t < 1/6) return p + (q - p) * 6 * t;
    if(t < 1/2) return q;
    if(t < 2/3) return p + (q - p) * (2/3 - t) * 6;
    return p;
}