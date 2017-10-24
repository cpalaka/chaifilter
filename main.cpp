
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics/Color.hpp>

#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <cmath>

//const int windowX = 500, windowY = 500; 
std::vector<sf::Color> getUniqueColorsFromImage(sf::Image image, unsigned int &xdim, unsigned int &ydim);
float getDiff(sf::Image a, sf::Image b, sf::Rect<int> area);
float euclideanDistance(int r1, int g1, int b1, int a1, int r2, int g2, int b2, int a2);

int main(int argc, char* argv[]) {
    srand(time(0));
    
    sf::RenderTexture texA, texB;
    sf::Vector2u img_size;
    sf::Image pic;
 
    pic.loadFromFile("pic.jpg");
    
    std::vector<sf::Color> vec = getUniqueColorsFromImage(pic, img_size.x, img_size.y);

    texA.create(img_size.x, img_size.y);
    texB.create(img_size.x, img_size.y);

    while(1) {
        sf::VertexArray line(sf::Lines, 2);

        sf::Color chosen = vec[rand()%vec.size()];
        int x1 = rand()%img_size.x, y1 = rand()%img_size.y;
        int x2 = rand()%img_size.x, y2 = rand()%img_size.y;

        line[0] = sf::Vertex(sf::Vector2f(x1,y1), chosen);
        line[1] = sf::Vertex(sf::Vector2f(x2,y2), chosen);
        texA.draw(line);

        //implement line drawing alogo

        std::cout<< "("<<x1<<","<<y1<<")\n";
        std::cout<< "("<<x2<<","<<y2<<")\n";
        sf::Rect<int> area((x1<x2?x1:x2),(y1<y2?y2:y1),(x1<x2?x2-x1:x1-x2),(y1<y2?y2-y1:y1-y2));

        std::cout<<"top: "<<area.top<<", left:"<<area.left<<", width:"<<area.width<<",height:"<<area.height<<std::endl;

        float picAdiff = getDiff(pic, texA.getTexture().copyToImage(), area);
        
        float picBdiff = getDiff(pic, texB.getTexture().copyToImage(), area);
        std::cout<<picAdiff<<" "<<picBdiff<<std::endl;

        if( picAdiff < picBdiff) {
            //copy texA to texB
            std::cout<<"It better be this.\n";
            texB = texA;
        } else {
            //copy texB to texA
            std::cout<<"Time for some debugging.\n";
        }
    }

    texA.display();
    
    sf::RenderWindow window(sf::VideoMode(img_size.x, img_size.y), "SFML works!");
    while (window.isOpen()) 
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

        }
        
        window.clear();
        window.draw(sf::Sprite(texA.getTexture()));
        window.display();
    }

    int n;
    std::cin>>n;
    return 0;
}

std::vector<sf::Color> getUniqueColorsFromImage(sf::Image image, unsigned int &xdim, unsigned int &ydim) {
    std::vector<sf::Color > colorvec;  

    xdim = image.getSize().x;
    ydim = image.getSize().y;

    const sf::Uint8* pixels = image.getPixelsPtr();
    for(int i = 0; i < (image.getSize().x * image.getSize().y)*4; i+=4) {
        sf::Color col(pixels[i], pixels[i+1], pixels[i+2], pixels[i+3]);
        colorvec.push_back(col);
    }

    auto del_repeats = std::unique(colorvec.begin(), colorvec.end(), [](auto a, auto b){ return (a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a);});
    return std::vector<sf::Color>(colorvec.begin(), del_repeats);
}

float getDiff(sf::Image a, sf::Image b, sf::Rect<int> area) {
    float distance = 0;
    
    for(int i = area.left; i<(area.left+area.width); ++i) {
        for(int j = area.top; j>=(area.top-area.height); --j) {
            //std::cout<<i<<","<<j<<std::endl;
            sf::Color col_A = a.getPixel(i, j);
            sf::Color col_B = b.getPixel(i, j);

            distance += euclideanDistance(col_A.r, col_A.g, col_A.b, col_A.a, col_B.r, col_B.g, col_B.b, col_B.a);
        }
    } 
    return distance;
}

float euclideanDistance(int r1, int g1, int b1, int a1, int r2, int g2, int b2, int a2) {
     return sqrt(pow(r1-r2, 2) + pow(g1-g2, 2) + pow(b1-b2, 2) + pow(a1-a2, 2));
}