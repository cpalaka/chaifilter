#include "cfEngine.h"

cfEngine::cfEngine() :
    num_of_iter(INT_MAX),
    distanceIsInversed(false),
    useAABBToCompare(false),
    max_line_length(250),
    max_radius(100),
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
    iskmeansSorted(false),
    profileCode(false),
    isCircleFill(false),
    isMixed(false),
    lineToCircleRatio(50),
    outlineToFillRatio(50),
    notDrawn(false)
{}

void cfEngine::init(sf::Color initialFill) {
    if(profileCode) {
        callProfiler("init");
    }

    if(!inputpic.loadFromFile("images/" + input_filename)) {
        exit(1);
    }
    resolution = inputpic.getSize();
    availableColors = getUniqueColorsFromImage();
   
    texture.create(resolution.x, resolution.y);
    sprite.setTexture(texture);
    pixelArray = PixelArray(resolution, initialFill);

    //chan filter init debug info
    if(profileCode) {
        std::cout<<"Image size: "<<resolution.x<<" x "<<resolution.y<<"\n";
        std::cout<<"Total number of pixels = "<<resolution.x*resolution.y<<"\n";
        std::cout<<"Size of availableColors: "<<availableColors.size()<<"\n";
    }
    if(profileCode) profileResults.insert(std::pair<std::string, double>("init", callProfiler("init")));
}

std::vector<sf::Color> cfEngine::getUniqueColorsFromImage() {
    std::vector<sf::Color> colorvec;
    std::set<util::Color> uniquecolorset;

    for(int i = 0; i < inputpic.getSize().x; ++i) {
        for(int j = 0; j < inputpic.getSize().y; ++j) {
            uniquecolorset.insert(util::Color(inputpic.getPixel(i,j)));
        }
    }
    
    for(const auto &i: uniquecolorset) {
        colorvec.push_back(sf::Color(i.r, i.g, i.b, i.a));
    }
    return colorvec;
}

bool cfEngine::configureEngineSettings(const int argc,const char* argv []) {
    //Handle command line options
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
                                if(graphicType == NONE) {
                                    graphicType = LINE;
                                } else {
                                    isMixed = true;
                                }
                                if(args+2 > argc-1 || argv[args+2][0] == '-') {std::cout<<"Options error.\n"; printUsage();exit(1);}
                                else {max_line_length = atoi(argv[args+2]);}
                                args++;
                                break;
                            }
                            case 'C':
                            case 'c':
                            {
                                if(graphicType == NONE) {
                                    graphicType = CIRCLE;
                                } else {
                                     isMixed = true;
                                }
                                max_radius = atoi(argv[args+2]);
                                args++;
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
                            if(argv[args][2] == '+') useKmeansplus = true; // for option -k+ (kmeans+ no visual)
                        }
                        else if(strlen(argv[args]) == 4) { 
                            useKmeansplus = true; // for option -kv+ (kmeans+ visual)
                            showKMeansResult = true;
                        }
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
                        writeToBMPNumbered = true; //TODO: use this for animations/gifs
                        break;

                    }
                    case 'h':
                    {
                        printUsage();
                        return 0;
                    }
                    case 'p':
                    {
                        profileCode = true;
                        break;
                    }
                    case 'f':
                    {
                        isCircleFill = true;
                        break;
                    }
                    case 'm':
                    {
                        lineToCircleRatio = atoi(argv[args+1]);
                        outlineToFillRatio = atoi(argv[args+2]);
                        args+=2;
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
    //TODO:
}

void cfEngine::algo_loop() {
    sf::Color chosen;
    static int r;

    //1. choose a random color from availableColors
    if(useKmeans) {
        chosen = kmeansColors[rand()%kmeansColors.size()];
    } else {
        chosen = availableColors[rand()%availableColors.size()];
    }
    
    sf::Rect<int> area;

    //2. Choose random graphic and construct it on pixelArray
    if(!notDrawn && isMixed) {
        int selection = rand()%100;
        if(selection < lineToCircleRatio) {
            graphicType = LINE;
        } else {
            graphicType = CIRCLE;
            r = 2 + rand()%(max_radius-2);
        }

        if(graphicType == CIRCLE) {
            int circleSelection = rand()%100;
            if(circleSelection < outlineToFillRatio) {
                isCircleFill = false;
            } else {
                isCircleFill = true;
            }
        }
    }
    
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
            int x = rand()%resolution.x;
            int y = rand()%resolution.y;
            area = sf::Rect<int>(x-r, y-r, 2*r, 2*r);
            constructCircle(x, y, r, chosen);
            break;
        }
        default:
            break;
    }

    //3. Compare the 2 pixelArrays with and without graphic to input image
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
    
    //4. Remove or keep graphic based on distance function
        if(distanceIsInversed) {
            std::cout<<"hmm";
            if(with > without) {
                drawGraphic();
            }
        } else {
            if(with < without) {
                drawGraphic();
                notDrawn = false;
            } else {
                if(!notDrawn) { notDrawn = true; }
                else { notDrawn = false; }
            }
        } 
    }

    //5. clear pixel buffers for the next iteration
    clearPixelBuffers();
}

void cfEngine::runAlgo() {
    //Should we use kmeans?
    if(useKmeans) {
        kmeans();
        assert(num_clusters == kmeansColors.size());
        if(showKMeansResult) {
            showKmeansResult();
            saveKmeansOutputToFile();
        }
    }

    //Run algorithm is visual mode if flag is set
    if(visualMode) {
        render_loop();
    } else {  
    //Run algorithm num_of_iter times profiling runtime
        if(num_of_iter != INT_MAX) {
            int count = num_of_iter;
            if(profileCode) callProfiler("algo");

            while(count) {
                algo_loop();
                count--;
            }

            if(profileCode) profileResults.insert(std::pair<std::string, double>("algo", callProfiler("algo")));
            //render final constructed pixelArray
            render_pixelArray();
        }
    }

    //write bmp to file if flag is set
    if(writeToBMPDetailed) {
        savePixelArrayToFile();
    }

    //display profiler output if flag is set
    if(profileCode) profilerOutput();
}

void cfEngine::render_loop() {
    int counter = 0;
    bool printCount = false;
    sf::RenderWindow window(sf::VideoMode(resolution.x, resolution.y), "ChaiFilter");
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
    sf::RenderWindow window(sf::VideoMode(resolution.x, resolution.y), "ChaiFilter");
    texture.update(pixelArray.getByteArray());
    while (window.isOpen()) 
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.display();
    }
}

void cfEngine::savePixelArrayToFile() {
    std::vector<sf::Color> vec = pixelArray.getColorVector();
    //construct BMP file
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

    //generate output filename string and write to file
    std::string outfile_name = generateDetailedOutputString();
    output.WriteToFile(outfile_name.c_str());
}

void cfEngine::saveKmeansOutputToFile() {
    int widthperColor = 30;//in pixels
    int height = 200;
    std::string kmeansType = " k=";

    if(kmeansColors.empty()) {
        std::cout<<"error: Kmeans segmentation not specified. Nothing to draw.\n";
        return;
    }
    sortColorVector(kmeansColors);

    //construct bmp output file
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
                output(i,j)->Alpha = col.a;
            }
        }
    }

    //specify kmeans++ algo, if used
    if(useKmeansplus) kmeansType = " k++=";

    //generate output filename string and write to file
    std::string outfile_name = "output/" + input_filename + kmeansType + std::to_string(num_clusters) + ".jpg";
    output.WriteToFile(outfile_name.c_str());
}

std::string cfEngine::generateDetailedOutputString() {
    std::string filename; //final

    //truncate filetype (.jpg, .jpeg, .png, etc)
    int file_len = std::strcspn(input_filename.c_str(), ".");
    input_filename.resize(file_len, '0');

    //Get graphic type {line, circle, mixed} and generate string
    std::string type;
    if(isMixed) {
        type = "mixed";
    } else {
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
    }
    
    //Get distance measure and generate string
    std::string dist_measure;
    switch(df) {
        case EUCLID:
            dist_measure = "euclid";
            break;
        case TAXICAB:
            dist_measure = "taxicab";
            break;
        default:
            break;
    }

    //Get circle fill property and generate string
    std::string circlefill;
    if(isCircleFill) {
        circlefill = "-filled";
    } else {
        circlefill = "";
    }

    //Get kmeans property and generate string
    std::string kmeansstr;
    if(useKmeans) {
        if(useKmeansplus) {
            kmeansstr = "k++="+ std::to_string(num_clusters);
        } else {
            kmeansstr = "k="+ std::to_string(num_clusters);
        }
    }

    //combine all properties to construct final filename string
    if(type == "line") {
        filename = "output/"+input_filename+"-"+type+"("+ std::to_string(max_line_length) +")-iter"+ std::to_string(num_of_iter)+"-"+kmeansstr+"df="+dist_measure+".bmp";
    } else if(type == "circle") {
        filename = "output/"+input_filename+"-"+type+"("+ std::to_string(max_radius) +")"+ circlefill + " iter" + std::to_string(num_of_iter)+"-"+kmeansstr+"df="+dist_measure+".bmp";
    } else {
        filename = "output/"+input_filename+"-"+type+"(" + std::to_string(lineToCircleRatio)+ ","+ std::to_string(outlineToFillRatio)+ ")"+"-line("+ std::to_string(max_line_length) +")"+".circle("+ std::to_string(max_radius) +")"+ circlefill +" iter"+  std::to_string(num_of_iter)+"-"+kmeansstr+"df="+dist_measure+".bmp";
    }
    return filename;
}

void cfEngine::constructLine_naive(int x1, int y1, int x2, int y2, sf::Color color) {
    //Construct bounding box around line
    sf::Rect<float> area((x1<x2?x1:x2), (y1<y2?y2:y1), (x1<x2?x2-x1:x1-x2), (y1<y2?y2-y1:y1-y2));
    
    //get slope
    float m = (float)(y2-y1)/(x2-x1);

    if(area.width > area.height) {
        if(x1 > x2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }

        //add pixel to buffer
        for(float x = area.left+0.5; x < area.left+area.width; ++x) {
            int y = floor(m*(x-x1) + y1);
            pixBuffer.push_back(util::pix(sf::Vector2u(floor(x),y), color));
        }
    } else {
        if(y1 > y2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }

        //add pixel to buffer
        for(float y = area.top - area.height; y < area.top; ++y) {
            int x = floor((y-y1)/m + x1);
            pixBuffer.push_back(util::pix(sf::Vector2u(x,floor(y)), color));
        }
    }
}

void cfEngine::constructLine_bresenham(int x0, int y0, int x1, int y1, sf::Color color) {
    //NOTE: unfinished
    float deltax = x1 - x0;
    float deltay = y1 - y0;
    float deltaerr = abs(deltay/deltax);
}

void cfEngine::constructCircle(int x0, int y0, int radius, sf::Color color) {
    int x = radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y)
    {
        //filled circle
        if(isCircleFill) {
            constructLine_naive(x0 - x, y0 + y, x0 + x, y0 + y, color);
            constructLine_naive(x0 - y, y0 + x, x0 + y, y0 + x, color);
            constructLine_naive(x0 - x, y0 - y, x0 + x, y0 - y, color);
            constructLine_naive(x0 - y, y0 - x, x0 + y, y0 - x, color);
        } else {//circle outline
            pixBuffer.push_back(util::pix(sf::Vector2u(x0 + x, y0 + y), color));
            pixBuffer.push_back(util::pix(sf::Vector2u(x0 - x, y0 + y), color));

            pixBuffer.push_back(util::pix(sf::Vector2u(x0 + y, y0 + x), color));
            pixBuffer.push_back(util::pix(sf::Vector2u(x0 - y, y0 + x), color));

            pixBuffer.push_back(util::pix(sf::Vector2u(x0 + x, y0 - y), color));
            pixBuffer.push_back(util::pix(sf::Vector2u(x0 - x, y0 - y), color));

            pixBuffer.push_back(util::pix(sf::Vector2u(x0 - y, y0 - x), color));
            pixBuffer.push_back(util::pix(sf::Vector2u(x0 + y, y0 - x), color));
        }
        
        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0)
        {
            x--;
            dx += 2;
            err += (-radius << 1) + dx;
        }
    }

    //clean up pixbuffer for duplicate pixels
    std::set<util::pix> set;
    for(const auto &i: pixBuffer) {
        set.insert(i);
    }
    int before = pixBuffer.size();
    pixBuffer.clear();
        
    for(const auto &i: set) {
        pixBuffer.push_back(i);
    } 
}

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

float cfEngine::measureAverageDistance_perPixel(int id) {
    float distance = 0;
    //measure only pixels of recently drawn line
    for(const auto& i: pixBuffer) {
        if(i.loc.x >= 0 && i.loc.y >= 0 && i.loc.x < resolution.x && i.loc.y < resolution.y) {
            distance += dfn( (id)?i.c : pixelArray.getPixel(i.loc.x, i.loc.y), inputpic.getPixel(i.loc.x, i.loc.y));
        }
    }
    return distance/pixBuffer.size();
}

void cfEngine::kmeans() {
    int kmeans_max_iter = 50;
    std::vector<sf::Color> means, temp;
    std::vector<std::pair<sf::Color, int> > _ksets;

    //1. Initialization step
    //Should we use kmeans++ for cluster initialization or not?
    if(useKmeansplus) {
        if(profileCode) callProfiler("kmeansPlus");
        means = doKmeansPlus();
        if(profileCode) profileResults.insert(std::pair<std::string, double>("kmeansPlus", callProfiler("kmeansPlus"))); 
    } else {
        //randomly initialize points
        for(int i = 0; i < num_clusters; ++i) {
            means.push_back(availableColors[rand()%availableColors.size()]);
        }
    }
    
    if(profileCode) callProfiler("kmeans");

    //Loop til convergence of centroids
    int iter = 0;
    while(iter < kmeans_max_iter) {
        //2. Assignment step
        //Assign each color to its closest cluster
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

        //3. Update step
        //calculate new cluster centers
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
            std::cout<<"Broke out of loop at iter: "<<iter<<"\n";
            break;
        } else {
            means = std::vector<sf::Color>(temp);
            temp.clear();
        }
        iter++;
        _ksets.clear();
    }
    if(profileCode) profileResults.insert(std::pair<std::string, double>("kmeans", callProfiler("kmeans")));
    //store calculated means in kmeansColors
    kmeansColors = std::vector<sf::Color>(means);
}

// The kmeans++ algoright which I followed from Wikipedia:
//    1.Choose one center uniformly at random from among the data points.
//    2.For each data point x, compute D(x), the distance between x and the nearest center that has already been chosen.
//    3.Choose one new data point at random as a new center, using a weighted probability distribution where a point x is chosen with probability proportional to D(x)2.
//    4.Repeat Steps 2 and 3 until k centers have been chosen.
std::vector<sf::Color> cfEngine::doKmeansPlus() {
    std::vector<sf::Color> result;
    //Step 1
    sf::Color initial = availableColors[rand()%availableColors.size()];
    int count = 0;

    //Step 4 (loop k times)
    while(count < num_clusters) {
        std::vector<float> distances;
        sf::Color nearestCenter;

        //Step 2
        float sum = 0;
        for(int i = 0; i < availableColors.size(); ++i) {
            //get the nearest center which is already found
            if(result.empty()) { nearestCenter = initial; }
            else {
                float lowestDist = FLT_MAX;
                for(const auto &c: result) {
                    float d = util::euclideanDistanceSquared(c, availableColors[i]);
                    if( d < lowestDist) {
                        lowestDist = d;
                        nearestCenter = c;
                    }
                }
            }

            sum += util::euclideanDistanceSquared(nearestCenter, availableColors[i]);
            distances.push_back(sum);
        }

        //Step 3
        float sample = rand()%(int)sum;
        int index = 0;
        assert(availableColors.size() == distances.size());
        for(const auto &i: distances) {
            if(i > sample) break;
            index++;
        }
        result.push_back(availableColors[index]);
        count++;
    }
    assert(result.size() == num_clusters);
    return result;
}

void cfEngine::showKmeansResult() {
    std::string kmeansType = "kMeans Result";
    if(useKmeansplus) kmeansType = "kMeans++ Result";
    sf::RenderWindow window(sf::VideoMode(1000, 300), kmeansType);
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
    std::vector<sf::Color> sortedRGB;
    
    if(iskmeansSorted) {//dont sort if it's already sorted
        return;
    } else {
        iskmeansSorted = true;
    }

    //convert colors from rgb to hsl
    for(const auto& i: vec) {
        colors.push_back(rgbToHsl(i));
    }

    //sort hsl colors by hue and luminosity
    //NOTE: kinda hackish way to sort colors in HSL space by 
    //      sorting groups of colors with the same luminosity and saturation by luminosity
    //      and within those groups, sorting by hue
    std::sort(colors.begin(), colors.end(), [](util::HSL a, util::HSL b) { 
        if(a.l == b.l && a.s == b.s) {
            return a.h < b.h ;
        } else {
            return (a.l > b.l);
        }
    });

    for(int i = 0; i<colors.size(); ++i) {
        sortedRGB.push_back(vec[colors[i].id]);
    }

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

void cfEngine::profilerOutput() {
    for(const auto &i: profileResults) {
        std::cout<<i.first<<": "<<i.second/1000<<" seconds \n";
    }
}

double cfEngine::callProfiler(std::string s) {
    static util::Timer t;
    return t.trigger()/1000000;//returns milliseconds
}