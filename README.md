# ChanFilter

## Explanation
My attempt at a recent programming 'competition' on /g/ - a relatively simple image post-processing task - with the following algorithm:

- load an image (source image)
- get a list of all the colors in the source image
- create 2 blank images (image1 and image2)
- draw a random polygon or circle on image1 using a random color from source image
- compare image1 to the source image
- if it's closer in color to the source image than image2, copy image1 to image2; if not, copy image2 to image1 and continue drawing more random shapes and comparing
- post the results and bits of code

## Build Instructions

Only requirements are SFML2.4 and the MingW C++ compiler on Windows.

### Visual Studio Code

I didn't want to use an IDE like the regular Visual Studio 2017 for this so I chose instead to run it entirely in Visual Studio Code.
To get yours working the same way, follow these settings -

* In **`c_cpp_properties.json`**, depending on your platform ("name"), change the `includePath` and `browse.path` json lists to include paths to both your C++ compiler and SFML headers
* Update **`tasks.json`** with your build command. For example, mine is:

` g++ main.cpp -I"D:/Code/C++/lib/SFML-2.4.2/include" -L"D:/Code/C++/lib/SFML-2.4.2/lib" -lsfml-graphics -lsfml-window -lsfml-system -std=c++17 -g -o testmain`

This will allow you to press Ctrl+Shift+B in your Visual Studio Code editor to automatically build your executable in your directory.
> Remember to change your the `-I` and `-L` options to point to paths to the SFML `/include` and `/lib` directories on your specific system.

## THIS IS A WIP AND UNFINISHED