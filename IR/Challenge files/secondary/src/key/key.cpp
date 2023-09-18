#include <iostream>
#include "../obfuscation/obfuscation.h"
#include "../rc4/rc4.h"
#include "../z85/z85.hpp"

std::string get_key() {
    int payload_length = 30;
    int key_length = 13;

    unsigned char encoded_rc4_key [] = {0x33,0x3e,0x33,0x22,0x31,0x3e,0x39,0x33,0x3a,0x32,0x68,0x68,0x68};
    unsigned char * rc4_key = encryptDecryptUnsigned(encoded_rc4_key, 0x57, key_length);

    unsigned char tmp [] = {0xec,0x79,0x5d,0x46,0x22,0xfa,0x2f,0xec,0xd7,0xe5,0x4d,0x35,0xd1,0xd1,0xc6,0x79,0x50,0xcf,0x8e,0x37,0x5b,0x88,0x13,0x95,0x3b,0x8d,0xdd,0xa2,0x48,0x0f};


    struct rc4_state *s;
    s=(struct rc4_state *) malloc (sizeof(struct rc4_state));
    rc4_setup(s, rc4_key, key_length);
    rc4_crypt(s, tmp, payload_length);


    char * pre_z85 = reinterpret_cast<char *>(encryptDecryptUnsigned(tmp, 0x12, payload_length));


    std::string key = z85::decode(pre_z85, strlen(pre_z85));
    return key;
}