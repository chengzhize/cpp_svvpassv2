#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdlib>
#include <sstream>
 
using namespace std;
 
// MD5数据块大小
const int MD5_BLOCK_SIZE = 64;
const int MD5_DIGEST_SIZE = 16;
 
// MD5常量表
static const unsigned int T[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0x1fa27cf8, 0x3f6a0b68, 0x9c1e3a60, 0x1111f4e2, 0xd6d6f5d1, 0x75f7c8b8, 0xa3c4e6b2, 0x1c6ec54f,
    0x349bc0fc, 0x7f3c4201, 0x47fd7b9a, 0x9c0f50ab, 0x8c5c2a24, 0xe639a194, 0x7bc7103a, 0x08afce79,
    0x0bfa5a97, 0x5a91a5a3, 0x5368f8a4, 0x7c9057cf, 0xd3e8b0e3, 0x4c09dfad, 0x8d6b3be3, 0x2b0ccab6,
    0x383f1a2d, 0x6342347f, 0x1d1285b7, 0x49563c7f, 0x17e8b276, 0x835e8d07, 0xa5f96ba7, 0x43d98b6e,
    0x5b53ec60, 0x9fa6bba0, 0x9378300f, 0xc1c7df53, 0x6aab3b12, 0x19d080b9, 0x027bbdd9, 0x09ad4b3e
};
 
// 左旋转操作
inline unsigned int leftRotate(unsigned int x, int n) {
    return (x << n) | (x >> (32 - n));
}
 
// 填充数据
void padMessage(const unsigned char* message, unsigned long long messageLength, unsigned char* paddedMessage, unsigned long long& paddedMessageLength) {
    unsigned long long originalLength = messageLength * 8; // 字节长度转换为比特长度
    unsigned long long paddingLength = 0;
    
    // 填充字节长度以满足64位对齐
    paddingLength = (56 - messageLength % 64) % 64;
    paddedMessageLength = messageLength + paddingLength + 8;
 
    // 填充
    memcpy(paddedMessage, message, messageLength);
    paddedMessage[messageLength] = 0x80; // 添加一个1比特，后面填充0比特
    memset(paddedMessage + messageLength + 1, 0, paddingLength); // 填充0
    // 最后添加64位消息长度
    memcpy(paddedMessage + paddedMessageLength - 8, &originalLength, 8);
}
 
// 计算MD5
void computeMD5(const unsigned char* message, unsigned long long messageLength, unsigned char* hash) {
    // 初始化
    unsigned int A = 0x67452301;
    unsigned int B = 0xefcdab89;
    unsigned int C = 0x98badcfe;
    unsigned int D = 0x10325476;
    
    unsigned char paddedMessage[MD5_BLOCK_SIZE * 64];
    unsigned long long paddedMessageLength;
    
    // 填充消息并计算MD5
    padMessage(message, messageLength, paddedMessage, paddedMessageLength);
    
    unsigned int* M = reinterpret_cast<unsigned int*>(paddedMessage);
    unsigned int tempA, tempB, tempC, tempD;
    
    for (unsigned long long i = 0; i < paddedMessageLength / MD5_BLOCK_SIZE; i++) {
        tempA = A;
        tempB = B;
        tempC = C;
        tempD = D;
 
        // 主循环
        for (int j = 0; j < 64; j++) {
            unsigned int F, g;
            if (j < 16) {
                F = (B & C) | ((~B) & D);
                g = j;
            } else if (j < 32) {
                F = (D & B) | ((~D) & C);
                g = (5 * j + 1) % 16;
            } else if (j < 48) {
                F = B ^ C ^ D;
                g = (3 * j + 5) % 16;
            } else {
                F = C ^ (B | (~D));
                g = (7 * j) % 16;
            }
 
            unsigned int temp = D;
            D = C;
            C = B;
            B = B + leftRotate((A + F + T[j] + M[g]), (j < 16 ? 7 : (j < 32 ? 5 : (j < 48 ? 4 : 6)))); // 左旋转
            A = temp;
        }
 
        // 更新A, B, C, D
        A += tempA;
        B += tempB;
        C += tempC;
        D += tempD;
    }
 
    // 输出结果
    memcpy(hash, &A, 4);
    memcpy(hash + 4, &B, 4);
    memcpy(hash + 8, &C, 4);
    memcpy(hash + 12, &D, 4);
}
 
// 将MD5哈希值转换为16进制字符串
string toHexString(unsigned char* hash) {
    stringstream ss;
    for (int i = 0; i < MD5_DIGEST_SIZE; i++) {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}


string MD5(string input) {
    
    // cout << "Enter the text to hash: ";
   // getline(cin, input);
    
    unsigned char hash[MD5_DIGEST_SIZE];
    computeMD5(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);
    
    // 输出MD5哈希值
    //cout << "MD5 hash: " << toHexString(hash) << endl;
    
    return toHexString(hash);
}
