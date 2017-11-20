# ChaiFilter

![input LETSGO](/output/ball-line(50)-iter10000000-k++=20df=euclid.bmp)

## Explanation
This program is my implementation of this simple image processing algorithm:
- load an image (source image)
- get a list of all the colors in the source image
- create 2 blank images (image1 and image2)
- draw a random polygon or circle on image1 using a random color from source image
- compare image1 to the source image
- if it's closer in color to the source image than image2, copy image1 to image2; if not, copy image2 to image1 and continue drawing more random shapes and comparing
- post the results and bits of code

## Build Instructions

Only requirements are SFML2.4 and the MingW C++ compiler on Windows and EasyBMP which is included with the source.

#### Command Line/Visual Studio Code

Build the executable in Visual Studio Code by following these instructions -

* In **`c_cpp_properties.json`**, depending on your platform ("name"), change the `includePath` and `browse.path` json lists to include paths to both your C++ compiler and SFML headers
* Update **`tasks.json`** with your standard C++ compiler build command. For example, mine is:

` g++ src/main.cpp src/cfEngine.cpp src/easybmp/EasyBMP.o -I"D:/Code/C++/lib/SFML-2.4.2/include" -L"D:/Code/C++/lib/SFML-2.4.2/lib" -lsfml-graphics -lsfml-window -lsfml-system -std=c++17 -g -o chaiFilter`

This will allow you to press Ctrl+Shift+B in your Visual Studio Code editor to automatically build your executable in your directory.

## Installation

Download this repo and extract it somewhere.
Run install.bat.

## Usage
Keep the desired input images in the `images` folder and then execute chaiFilter on your favorite commandline.
Currently only supports windows and tested on Powershell and command prompt.

To execute:
* Powershell - `./chaifilter ...`
* Command Prompt - `chaifilter.exe ...`

#### Command Line Options

* **`-h`**

    The help command. Use this to display all the options in your command line.

* **`Full Image Name`**

    >example: inputpic.jpg

* **`-t GraphicType arg1`**

    ##### **GraphicType**:*string*
    * `line`
        - **arg1**:*integer* - Maximum line length
    * `circle`
        - **arg1**:*integer* - Maximum circle radius

    >blank arguments will cause errors
    >use `-t` twice with both line and circle parameters for a mix of circles and lines
    
    >examples: `-t line 50`, `-t rect 100 40`

* **`-m lineToCircle outlineToFill`**
    Used to control level of mixing between lines and circles and circle outlines and filled circles.

    ##### **lineToCircle**:*integer*
        - number between 0-100 controlling amount of lines.
        >setting this to 100 will produce only lines

    ##### **outlineToFill**:*integer*
        - number between 0-100 controlling amount of outlined circles.
        > lower yields more filled

* **`-f`**

    Use filled shapes.

* **`-i numberOfIterations`**

    ##### **numberOfIterations**:*integer*

    >Omit in `visual mode` for an infinite render loop

* **`-v`**

    Flag for `visual mode` or continuous rendering of the algorithm output onto a window.

    >Warning: takes a long time

* **`-d distanceFunction`**

    ##### **distanceFunction**:*string*
        * `euclideanDistance`
        * `taxiCabDistance`

    >Defaults to euclidean distance.

* **`-AABB`**

    Flag for using an AABB box to compare pixels instead of per-pixel. ** *Not recommended.* **

* **`-k.. numberOfClusters`**

    ##### **numberOfClusters**:*integer*

    Use this option to do the chaiFilter algorithm with only *numberOfClusters* most important colors instead of all unique colors. This is implemented in the standard way - kmeans clustering.

    * `k`: do kmeans with k=numberOfClusters
    * `k+` : do kmeans with k=numberOfClusters and initialize with kmeans++
    * `kv`: do kmeans with k=numberOfClusters and render in window
    * `kv+` / `k+v`: do kmeans with k=numberOfClusters and render in window with kmeans++ algo for initialization

* **`-w`**

Flag for outputting the result as a bmp file in `outputs/`.

* **`-p`**

Flag for showing time taken by different functions in cfEngine. Simple profiler.
>Not implemented yet.

## Example Usage

##### `./chanfilter inputpic.jpg -t lines 50 -i 1000000 -v`

##### `./chanfilter inputpic.jpg -kv 25 -w`

##### `./chanfilter inputpic.jpg -t rect 70 70 -i 1000000 -k 15 -w -d taxi`

## How to read output files

You may have noticed that the output bmp file names detail the specific parameters of the algorithm used. It should be fairly easy to interpret, however, to make things clear:
* df - distance function
* iter - number of iterations
* line(n) - lines with max line length of n
* circle(n) - circles with max radius of n
* mixed(x,y) - mix of lines and circles with line to circle ratio set to x and outlined circle to filled circle ratio set to y

## Examples

![kitty no kmeans](output/kitty-line(60)-iter2000000-df=euclid.bmp)

![kitty with kmeans](output/kitty-line(50)-iter10000000-k=50df=euclid.bmp)

![transcend-wallpaper](output/transcend-line(50)-iter10000000-k=50df=euclid.bmp)

![anime-girl](output/animegirl2-line(50)-iter2000000-df=euclid.bmp)

![anime-pic](output/animepic1-line(50)-iter10000000-k=50df=euclid.bmp)

![seashore](output/seashore-line(50)-iter10000000-df=euclid.bmp)

![Output image of me in Chicago with friends](output/chictrip-line(50)-iter10000000-df=euclid.bmp)

## THIS IS A WIP AND UNFINISHED