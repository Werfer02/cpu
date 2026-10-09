#include "app.hpp"

#include <iostream>

#include "argparser.hpp"
#include "util.hpp"
#include "assembler.hpp"

void App::configure(int argc, char** argv){
    argparser::Args args = argparser::parseArgv(argc, argv);

    if(args.noDashArgs.size() == 2 && args.doubleDashArgs.empty() && args.singleDashArgs.empty()){ // 1 no dash parameter passed, assume just a path to instruction file
        inputProgramPath = args.noDashArgs[1];
        
    } else if(args.hasSingleDashArg("I")){

        inputProgramPath = args.singleDashArgs["I"];

    } else if(args.hasSingleDashArg("B")){

        inputProgramPath = args.singleDashArgs["B"];
        binaryInput = true;

    } else { // fallback
        inputProgramPath = "tests/testlooploop.inst";
    }

    if(args.hasSingleDashArg("A")){
        binaryOutputPath = args.singleDashArgs["A"];
        wantsToOutputAssembly = true;
        wantsToRunProgram = false;
    } 

    if(args.hasDoubleDashArg("run")) wantsToRunProgram = true;

    if(args.hasDoubleDashArg("print-registers")) wantsToPrintRegisters = true;

    if(args.hasDoubleDashArg("print-heap-before")) wantsToPrintHeapBefore = true;
    if(args.hasDoubleDashArg("print-heap-after")) wantsToPrintHeapAfter = true;
    if(args.hasDoubleDashArg("print-heap")) wantsToPrintHeapAfter = true;
}

void App::run(){
    std::vector<byte> program = {};
    if(binaryInput){
        program = readBinaryFromFile(inputProgramPath);
    } else {
        Assembler as(s.getLangConfig());
        program = as.instructionsToBinary(as.readInstructionsFromFile(inputProgramPath));
    }
    
    if(wantsToOutputAssembly) writeBinaryToFile(binaryOutputPath, program);

    if(wantsToPrintHeapBefore) s.printHeap();
    if(wantsToRunProgram) {
        address a = s.allocate(program.size()*sizeof(byte));
        if(a){
            s.write(program.data(), a, program.size() * sizeof(byte));
            s.run(a);
        } else{
            printError("error allocating, got address 0\n");
        }
        s.printScreen();
    }
    if(wantsToPrintHeapAfter) s.printHeap();

    if(wantsToPrintRegisters) s.printRegisters();
}