# ChaiFilter

![input LETSGO](/images/letsgo.jpg)![output LETSGO](output/letsgo-line(100)-iter2000000-df%3Deuclid.bmp)![output LETSGO](output/letsgo-line(50)-iter10000000-k%3D25df%3Deuclid.bmp)

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
The only installation instruction is to create two folders at the same directory level as the final executable named - `images` and `output`.

## Usage
Keep the desired input images in the `images` folder and then execute chaiFilter on your favorite commandline.

#### Command Line Options
* **`FULL IMAGE NAME`**

    >example: inputpic.jpg

* **`-t GraphicType arg1 arg2..`**

    ##### **GraphicType**:*string*
    * `line`
        - **arg1**:*integer* - Maximum line length
    * `circle`
        - **arg1**:*integer* - Maximum circle radius
    * `rect`
        - **arg1**:*integer* - Maximum line width
        - **arg2**:*integer* - Maximum rectangle height
    * `sprite`
        - *coming soon*

    >blank arguments will cause errors
    >examples: `-t line 50`, `-t rect 100 40`

* **`-i numberOfIterations`**

    ##### **numberOfIterations**:*integer*

    >Omit in `visual mode` for an infinite render loop

* **`-v`**

    Flag for `visual mode` or continuous rendering of the algorithm output onto a window.

    >Warning: takes a long time

* **`-d distanceFunction`**

    ##### **distanceFunction**:*string*
        * *euclideanDistance*
        * *taxiCabDistance*

* **`-AABB`**

    Flag for using an AABB box to compare pixels instead of per-pixel. ** *Not recommended.* **

* **`-k.. numberOfClusters`**

    ##### **numberOfClusters**:*integer*

    Use this option to do the chaiFilter algorithm with only *numberOfClusters* most important colors instead of all unique colors. This is implemented in the standard way - kmeans clustering.

    * `k`: do kmeans with k=numberOfClusters
    * `kv`: do kmeans with k=numberOfClusters and render in window
    * `kv+`: do kmeans with k=numberOfClusters and render in window with kmeans++ algo for initialization

* **`-w`**

Flag for outputting the result as a bmp file in `outputs/`.

* **`-p`**

Flag for showing time taken by different functions in cfEngine. Simple profiler.
>Not implemented yet.

## Example Usage

##### `./chanfilter inputpic.jpg -t lines 50 -i 1000000 -v`

##### `./chanfilter inputpic.jpg -kv 25 -w`

##### `./chanfilter inputpic.jpg -t rect 70 70 -i 1000000 -k 15 -w -d taxi`

## Examples
Input:
![Input image of me in Chicago with friends](https://raw.githubusercontent.com/cpalaka/chanfilter/master/images/chictrip.jpeg)

10million iterations, drawn with lines of max length of 50 pixels:
![Output image of me in Chicago with friends](https://raw.githubusercontent.com/cpalaka/chanfilter/master/output/chictrip-line(50)-iter10000000-df=euclid.bmp)

## THIS IS A WIP AND UNFINISHED