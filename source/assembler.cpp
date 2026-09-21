#include "assembler.hpp"

#include <fstream>
#include <sstream>
#include <unordered_map>

#include "util.hpp"

Assembler::Assembler(const LangConfig& cfg){
    langConfig = cfg;
}

std::vector<instruction> Assembler::readInstructionsFromFile(const std::string& path){
    std::ifstream file(path);
    std::vector<instruction> instructions;
    std::stringstream linestream;
    bool hasROM = false;

    int ROMcounter = 0;

    for(std::string line; std::getline(file, line, ';');){
        linestream.clear();
        linestream.str(line);
        std::string word;

        if(ROMcounter > 0){ // if reading rom
            //if(line[0] == '\n') line = line.substr(1); // skip first char (newline from formatting)
            if(line.length() == 1 && !isdigit(line[0])){ // if single non digit char
                instructions[instructions.size() - 1].operands.push_back({operandType::BYTE, (byte)line[0]});
                ROMcounter--;
            } else try { 								 // try convert whole line to number
                size_t numendpos = 0;
                int num = (byte)std::stoi(line, &numendpos);
                if(numendpos == line.length()){			 // if whole line consumed as number
                    instructions[instructions.size() - 1].operands.push_back({operandType::BYTE, (byte)num});
                    ROMcounter--;
                } else throw std::invalid_argument("stoi");
            }
            catch (std::invalid_argument){				 // write line byte by byte
                for(byte c : line){
                    if (ROMcounter == 0) break;
                    instructions[instructions.size() - 1].operands.push_back({operandType::BYTE, c});
                    ROMcounter--;
                }
            }
        } else if(linestream >> word){ // first word
            if(stringToOperation.find(word) != stringToOperation.end()){
                instruction i;
                i.op = stringToOperation[word];
                if(i.op == operation::ROM){
                    if(hasROM){
                        printError("more than one ROM instruction found\n");
                        return {};
                    }
                    linestream >> word;

                    int a;
                    if(word.length() == 1 && !isdigit(word[0])){
                        a = word[0];
                    } else {
                        a = std::stoi(word);
                    }
                    if(a > BYTE_MAX){
                        printError({"byte operand too big: ", std::to_string(a), "\n"});
                        return {};
                    } else i.operands.push_back({operandType::BYTE, (byte)a});

                    hasROM = true;
                    ROMcounter = a;
                }
                else while(linestream >> word && i.operands.size() < langConfig.operationInfoMap[i.op].operands.size()){ // rest of words up to max operands
                    switch(langConfig.operationInfoMap[i.op].operands[i.operands.size()]){ // no -1 because looking for next operand
                        case operandType::BYTE : {
                            int a; 
                            if(word.length() == 1 && !isdigit(word[0])){
                                a = word[0];
                            } else {
                                a = std::stoi(word);
                            }
                            if(a > BYTE_MAX){
                                printError({"byte operand too big: '", std::to_string(a), "'\n"});
                                return {};
                            } else i.operands.push_back({operandType::BYTE, (byte)a});
                            break;
                        }
                        case operandType::LABEL : {
                            i.operands.push_back({operandType::LABEL, 0, word});
                            break;
                        }
                    }
                }
                instructions.push_back(i);
            }
            else {
                printError({"unknown operation: \"", word, "\"\n"});
            }
        }
    }
    file.close();
    return instructions;
}

std::vector<byte> Assembler::instructionsToBinary(const std::vector<instruction>& instructions){
    std::vector<byte> prog;
    
    int position = 0;
    int labelcounter = 0;
    std::unordered_map<std::string, address> labelsdefined;
    std::unordered_map<int, address> labelsused;
    std::unordered_map<int, std::string> indextolabel;
    for(auto i : instructions){
        if(position >= BYTE_MAX) {
            printError({"program too long, addressable limit: '", std::to_string(BYTE_MAX), "'\n"});
            return {0};
        }
        if(i.op == operation::LABEL){
            if(i.operands.empty()){
                printError("empty label\n");
                return{0};
            }
            labelsdefined[i.operands[0].stringvalue] = position;
            continue;
        }
        else if(i.op == operation::ROM){
            prog.push_back(langConfig.operationInfoMap[i.op].binary);
            prog.push_back(i.operands[0].bytevalue);
            for(int b = 1; b <= i.operands[0].bytevalue; b++){ // skip first operand, use it as length, need the <= !!!
                prog.push_back(i.operands[b].bytevalue);
            }
            position += 2 + i.operands[0].bytevalue; // operation + length byte + length of rom
            continue;
        }
        prog.push_back(langConfig.operationInfoMap[i.op].binary);
        position++;
        for(auto op : i.operands) {
            switch(op.type){
                case operandType::BYTE : {
                    prog.push_back(op.bytevalue);
                    position++;
                    break;
                }
                case operandType::LABEL : {
                    labelsused.insert({labelcounter, position});
                    indextolabel.insert({labelcounter, op.stringvalue});
                    prog.push_back(0);
                    labelcounter++;
                    position++;
                    break;
                }
            }
        }
    }
    for(auto l : labelsused){
        if(labelsdefined.find(indextolabel[l.first]) == labelsdefined.end()){
            printError({"undefined label used: ", std::to_string(l.first), "\n"});
        } else {
            prog[l.second] = labelsdefined[indextolabel[l.first]];
        }
    }
    return prog;
}