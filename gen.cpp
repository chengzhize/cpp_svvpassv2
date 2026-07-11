# include "md5.cpp"
#include <cstring>
#include <iostream>

int main(int argc, char* argv[]){
    std::string pwd = argc >= 2 ? argv[1] : "12345678";
    std::string hash = MD5(MD5(pwd).toStr()+MD5("hugo").toStr()).toStr().substr(8,16);
    std::cout << "password: " << pwd << std::endl;
    std::cout << "hash: " << hash << std::endl;
    return 0;
}
