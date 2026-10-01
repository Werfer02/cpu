#include "alu.hpp"

void ALU::setFlags(){
    isZero = 0, isPos = 0, isNeg = 0;
    result > 0 ? isPos = 1 : (result < 0 ? isNeg = 1 : isZero = 1);
}

byte ALU::add(byte a, byte b){
    result = a+b;
    setFlags();
    return result;
}
byte ALU::subtract(byte a, byte b){
    result = a-b;
    setFlags();
    return result;
}
byte ALU::multiply(byte a, byte b){
    result = a*b;
    setFlags();
    return result;
}
byte ALU::divide(byte a, byte b){
    result = a/b;
    setFlags();
    return result;
}

byte ALU::getResult(){
    return result;
}

flag ALU::getZeroFlag(){
    return isZero;
}
flag ALU::getPosFlag(){
    return isPos;
}
flag ALU::getNegFlag(){
    return isNeg;
}
