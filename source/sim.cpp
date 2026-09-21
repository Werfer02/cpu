#include "sim.hpp"

#include <iostream>
#include <vector>

#include "util.hpp"
#include "coututils.hpp"

address Sim::useBlock(address blockAddress, int bytes){
    int blockSize = heap[blockAddress];
    
    if(blockSize >= bytes + 3){ // bytes + metadata + at least 1 usable byte ? then split into 2 blocks
        heap[blockAddress+2+bytes] = blockSize - (bytes + 2); // second block with leftover length
        heap[blockAddress+2+bytes+1] = false; // mark as free
        
        heap[blockAddress] = bytes;  
        heap[blockAddress+1] = true; // first block
        return blockAddress+2;	
    }
    else { // if not enough space to make another usable block, just take up this whole block
        heap[blockAddress+1] = true;
        return blockAddress+2;
    }	
}

void Sim::initScreen(){
    regScreen.fill(" ");
    screenReady = true;
}

Sim::Sim(){
    initScreen();
}

LangConfig Sim::getLangConfig(){
    return config.langConfig;
}

address Sim::allocate(byte bytes){
    int i = 0;
    int mergeCandidate = -1;
    
    while(i < config.HEAP_SIZE){
        if(heap[i] == 0){ // this must mean free, size of a block is never zero, always jumping to size field
            if(i + bytes < config.HEAP_SIZE){
                heap[i] = bytes;
                heap[i+1] = true;
                return i+2;
            }
            i++;
        }
        else if(heap[i] >= bytes){ // always jumping to size field
            if(heap[i+1] == false){ // this must mean its free
                return useBlock(i, bytes);
            } else{ // this must mean its occupied
                i += heap[i] + 2; // jump to next size field or free
            }
        }
        else if(heap[i] < bytes){ // always jumping to size field
            if(heap[i+1] == false) { // this block is too small but free so look to merge it
                if(mergeCandidate > -1){ // if previous block is merge candidate, merge
                    heap[mergeCandidate] = heap[mergeCandidate] + heap[i] + 2;
                    if(heap[mergeCandidate] >= bytes){ // if enough space stop looking
                        return useBlock(mergeCandidate, bytes);
                    }
                }
                else { // otherwise make this the merge candidate
                    mergeCandidate = i; 
                }
            } else{
                mergeCandidate = -1;
            }
            i += heap[i] + 2; // jump to next size field or free
        }
    }
    printError("couldnt allocate\n");
    return 0;
}

void Sim::free(address tofree){
    heap[tofree-1] = false;
}

void Sim::write(const void* memory, address location, int size){
    const byte* src = (const byte*)memory;
    for(int i = 0; i < size; i++){
        heap[location + i] = src[i];
    }
}

void Sim::setProgStart(address a){
    progStart = a;
}

void Sim::run(address a){
    setProgStart(a);
    run();
}

void Sim::run(){
    std::cout << "< program started >\n";
    progCounter = progStart;
    while(progCounter < config.HEAP_SIZE){
        byte b = heap[progCounter];
        if (b == config.langConfig.operationInfoMap[operation::PROG_END].binary) break;

        if(config.langConfig.byteToOperation.find(b) == config.langConfig.byteToOperation.end()){
            printError({"no operation for byte: '", std::to_string(b), "'\n"});
            return;
        }
        operation op = config.langConfig.byteToOperation[b];
        switch(op){
            case operation::TEST : {
                std::cout << "test\n";
                break;
            }
            case operation::ECHO : {
                std::string echostr;
                progCounter++;
                int echolen = *getReg(heap[progCounter]);
                progCounter++;
                address echostart = *getReg(heap[progCounter]);
                address echoCounter = echostart;
                while(echoCounter < echostart + echolen) {
                    echostr.push_back(heap[echoCounter]);
                    echoCounter++;
                }
                std::cout << "echo: " << echostr << "\n";
                break;
            }
            case operation::JUMPREG : {
                progCounter++;
                //std::cout << "jump to: " << (int)heap[progCounter] << "\n";
                progCounter = *getReg(heap[progCounter]) + progStart;
                progCounter--; // go back one because loop goes forward one
                break;
            }
            case operation::WRITEHEAP : {
                progCounter++;
                address writeTo = *getReg(heap[progCounter]);
                progCounter++;
                heap[writeTo] = *getReg(heap[progCounter]);
                break;
            }
            case operation::READHEAP : {
                progCounter++;
                byte* reg = getReg(heap[progCounter]);
                progCounter++;
                *reg = heap[*getReg(heap[progCounter])];;
                break;
            }
            case operation::JUMPZEROREG : {
                progCounter++;
                byte* reg = getReg(heap[progCounter]);
                progCounter++;
                address jumpTo = *getReg(heap[progCounter]);
                if(*reg == 0){
                    //std::cout << "jump to (zero): " << (int)jumpTo << "\n";
                    progCounter = progStart + jumpTo;
                    progCounter--; // go back one because loop goes forward one
                }
                break;
            }
            case operation::WRITEREG : {
                progCounter++;
                //std::cout << "writereg: " << (int)heap[progCounter] << "\n";
                byte* reg = getReg(heap[progCounter]);
                progCounter++;
                *reg = heap[progCounter];
                break;
            }
            case operation::ADD : {
                progCounter++;
                byte* firstReg  = getReg(heap[progCounter]);
                progCounter++;
                byte* secondReg = getReg(heap[progCounter]);
                *firstReg += *secondReg;
                break;
            }
            case operation::SUB : {
                progCounter++;
                byte* firstReg  = getReg(heap[progCounter]);
                progCounter++;
                byte* secondReg = getReg(heap[progCounter]);
                *firstReg -= *secondReg;
                break;
            }
            case operation::WRITESCREEN : {
                screenWritten = true;
                progCounter++;
                int x = *getReg(heap[progCounter]);
                progCounter++;
                int y = *getReg(heap[progCounter]);
                regScreen[(y * config.SCREEN_W) + x] = ansi::rgb(regPixelR, regPixelG, regPixelB) + "█" + ansi::reset;
                break;
            }
            case operation::JUMP : {
                progCounter++;
                progCounter = progStart + heap[progCounter];
                progCounter--; // go back one because loop goes forward one
                break;
            }
            case operation::JUMPZERO : {
                progCounter++;
                byte* reg = getReg(heap[progCounter]);
                progCounter++;
                address jumpTo = heap[progCounter];
                if(*reg == 0){
                    //std::cout << "jump to (zero): " << (int)jumpTo << "\n";
                    progCounter = progStart + jumpTo;
                    progCounter--; // go back one because loop goes forward one
                }
                break;
            }
            case operation::LABEL : {
                break;
            }
            case operation::ROM : { // at start of rom
                //std::cout << "at rom: " << (int)progCounter << "\n";
                progCounter++;
                progCounter += heap[progCounter] + 1; // skip data and this byte
                //std::cout << "now at byte: " << (int)progCounter << "\n";
                progCounter--; // go back one because loop goes forward one
                break;
            }
            case operation::GET : {
                progCounter++;
                byte* toReg = getReg(heap[progCounter]);
                progCounter++;
                byte infoByte = *getReg(heap[progCounter]);
                *toReg = getInfo(infoByte);
                break;
            }
        }
        progCounter++;
    }
    std::cout << "< program finished >\n";

}

byte Sim::getInfo(byte infoByte){
    switch(infoByte){
        case 'S' : {
            return progStart;
        }
        case 'C' : {
            return progCounter;
        }
        default : {
            printError({"no info for byte: '", std::to_string((char)infoByte), "'\n"});
            return 0;
        }
    }
}

byte* Sim::getReg(byte index){
    switch(index) {
        case 0   : return &reg0;
        case 1   : return &reg1;
        case 2   : return &reg2;
        case 3   : return &reg3;
        case 'R' : return &regPixelR;
        case 'G' : return &regPixelG;
        case 'B' : return &regPixelB;
        default : {
            printError({"no register: '", std::to_string((char)index), "'\n"});
            return nullptr;
        }
    }
}

void Sim::printHeap(){
    int emptyCounter = 0;
    for(int i = 0; i < config.HEAP_SIZE; i++){
        char c = heap[i];

        if (c == '\0') {
            emptyCounter++;
            if(emptyCounter < 4){
                std::cout << i << ": " << "\\0\n";	
            }
            else if(emptyCounter == 4) {
                std::cout << ". . .\n";
                continue;
            }
        }
        else {
            emptyCounter = 0;
            
            std::cout << i << ": ";

            if (std::isprint(c)) std::cout << "'" << c << "'";
            else if (c == '\n') std::cout << "\\n";
            else std::cout << (int)c;
            
            std::cout << "\n";
        }
        
    }
}

void Sim::printRegisters(){
    std::cout << "reg0: " << (int)reg0 << "\n";
    std::cout << "reg1: " << (int)reg1 << "\n";
    std::cout << "reg2: " << (int)reg2 << "\n";
    std::cout << "reg3: " << (int)reg3 << "\n";
    std::cout << "regPixelR: " << (int)regPixelR << "\n";
    std::cout << "regPixelG: " << (int)regPixelG << "\n";
    std::cout << "regPixelB: " << (int)regPixelB << "\n";
}

void Sim::printScreen(){
    if(!screenWritten){
        printInfo("screen has not been written to, not printing\n");
        return;
    }
    for(int i = 0; i < config.SCREEN_H; i++){
        for(int j = 0; j < config.SCREEN_W; j++){
            std::cout << regScreen[(i*config.SCREEN_W) + j];
        }
        std::cout << "\n";
    }
}
