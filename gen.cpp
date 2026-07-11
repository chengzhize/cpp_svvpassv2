# include "md5.cpp"
#include <cstring>
#include <iostream>

int main(){
    std::cout<<MD5(MD5("12345678").toStr()+MD5("hugo").toStr()).toStr().substr(8,16);
}
