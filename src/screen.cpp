#include "screen.hpp"

#include <iostream>

#include "coututils.hpp"

Screen::Screen(){
    data = std::vector<std::string>(width*height, " ");
}
Screen::Screen(int w, int h){
    width = w;
    height = h;

    data = std::vector<std::string>(width*height, " ");
}
void Screen::write(int x, int y, int r, int g, int b){
    data[(y * width) + x] = ansi::rgb(r, g, b) + "█" + ansi::reset;
}
void Screen::print(){
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            std::cout << data[(i*width) + j];
        }
        std::cout << "\n";
    }
}