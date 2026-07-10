# include "md5.cpp"
#include <cstring>

int main(){
    std::cout<<MD5(MD5("123456")+MD5("hugo"));
}