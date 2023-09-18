#include <string>

std::string encryptDecrypt(std::string toEncrypt, char key) {
    std::string output = toEncrypt;

    for (int i = 0; i < toEncrypt.size(); i++)
        output[i] = toEncrypt[i] ^ key;

    return output;
}

unsigned char * encryptDecryptUnsigned(unsigned char * toEncrypt, char key, int length) {
    unsigned char * output = toEncrypt;

    for (int i = 0; i < length; i++)
        output[i] = toEncrypt[i] ^ key;

    return output;
}