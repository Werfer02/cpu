#include "screen.hpp"

#include <iostream>

#include "coututils.hpp"

void Screen::init(){
    data.fill(" ");
    screenReady = true;
}
void Screen::write(int x, int y, int r, int g, int b){
    if(!screenReady) init();
    data[(y * SimConfig::SCREEN_W) + x] = ansi::rgb(r, g, b) + "█" + ansi::reset;
}
void Screen::print(){
    for(int i = 0; i < SimConfig::SCREEN_H; i++){
        for(int j = 0; j < SimConfig::SCREEN_W; j++){
            std::cout << data[(i*SimConfig::SCREEN_W) + j];
        }
        std::cout << "\n";
    }
}