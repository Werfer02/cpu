#include "register.hpp"

byte Register::read(){
    return value;
}
void Register::write(byte b){
    value = b;
}
void Register::clear(){
    value = 0;
}