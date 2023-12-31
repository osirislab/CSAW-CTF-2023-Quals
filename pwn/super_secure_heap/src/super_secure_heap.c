#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define N 256
#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))


struct items_t {
    char * items[10];
    int item_sizes[10];
};


struct items_t keys;
struct items_t content;

void swap(unsigned char *a, unsigned char *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int KSA(char *key, unsigned char *S) {

    int len = strlen(key);
    int j = 0;

    for(int i = 0; i < N; i++)
        S[i] = i;

    for(int i = 0; i < N; i++) {
        j = (j + S[i] + key[i % len]) % N;

        swap(&S[i], &S[j]);
    }

    return 0;
}

int PRGA(unsigned char *S, char *plaintext, unsigned char *ciphertext) {

    int i = 0;
    int j = 0;

    for(size_t n = 0, len = strlen(plaintext); n < len; n++) {
        i = (i + 1) % N;
        j = (j + S[i]) % N;

        swap(&S[i], &S[j]);
        int rnd = S[(S[i] + S[j]) % N];

        ciphertext[n] = rnd ^ plaintext[n];

    }

    return 0;
}

int PRGA2(unsigned char *S, char *plaintext, unsigned char *ciphertext, int len) {

    int i = 0;
    int j = 0;

    for(size_t n = 0; n < len; n++) {
        i = (i + 1) % N;
        j = (j + S[i]) % N;

        swap(&S[i], &S[j]);
        int rnd = S[(S[i] + S[j]) % N];

        plaintext[n] = rnd ^ ciphertext[n];

    }

    return 0;
}

void secure_stuff(int index, int key, int size) {
    unsigned char sbox[N];
    KSA(keys.items[key], sbox);
    char plaintext[size];
    for (int i = 0; i < size; i++)
        plaintext[i] = content.items[index][i];
    PRGA(sbox, plaintext, content.items[index]);
}

int read_int() {
    char buf[10];
    fgets(buf, 10, stdin);
    int i = 0;
    sscanf(buf, "%d", &i);
    return i;
}


void add(struct items_t * obj){
    int i;
    for (i = 0; i < 10 && obj->items[i] != 0; i++);
    printf("Enter the size of your item:\n");
    int size = read_int();
    if (size < 4096) {
        (obj->item_sizes)[i] = size;
        (obj->items)[i] = malloc(size);
    }
}

void set(struct items_t * obj, int safe_store) {
    printf("Enter the item you want to modify:\n");
    int index = read_int();
    if (index < 10) {
        if (safe_store) {
            printf("Enter the key number you want to use to securely store the content with:\n");
            int key = read_int();
            if (key < 0 || key > 9 || keys.items[key] == 0){
                printf("Invalid key.\n");
                return;
            }
            printf("Enter the size of the content:\n");
            int size = read_int();
            if (size < obj->item_sizes[index]) {
                printf("Enter the content:\n");
                read(0, obj->items[index], size);
                secure_stuff(index, key, size);
            } else {
                printf("Invalid size.");
            }
        } else {
            printf("Enter the size of the content:\n");
            int size = read_int();
            if (size < obj->item_sizes[index]) {
                printf("Enter the content:\n");
                read(0, obj->items[index], obj->item_sizes[index]);
            } else {
                printf("Invalid size.");
            }
        }
    }
}

void delete(struct items_t * obj, int safe_delete) {
    printf("Enter the item you want to remove:\n");
    int index = read_int();
    if (-1 < index && index < 10) {
        free(obj->items[index]);
        if (safe_delete == 1){
            obj->item_sizes[index] = 0;
            obj->items[index] = 0;
        }
    }
}

void show(struct items_t * obj) {
    printf("Enter the item you want to show:\n");
    int index = read_int();
    if (-1 < index && index < 10) {
        printf("Here is your content: \n");
        printf("%s", obj->items[index]);
    }
}

int menu() {
    printf("Do you want to work with keys or content?\n");
    printf("1. Keys \n2. Content\n3. Exit\n");
    printf(">\n");
    int is_keys = read_int();
    is_keys = is_keys - 1;
    if (is_keys == 0 || is_keys == 1) {
        printf("\nSelect one of the following options: \n");
        printf("1. Add \n2. Delete\n3. Modify\n4. Show\n5. Exit\n");
        printf(">\n");
        int action = read_int();
        if (action == 1) {
            if (is_keys == 0)
                add(&keys);
            else
                add(&content);
        } else if (action == 2) {
            if (is_keys == 0)
                delete(&keys, 1);
            else
                delete(&content, 0);
        } else if (action == 3) {
            if (is_keys == 0)
                set(&keys, 0);
            else
                set(&content, 1);
        } else if (action == 4) {
            if (is_keys == 0)
                show(&keys);
            else
                show(&content);
        } else if (action == 5) {
            return 1;
        } else {
            return 1;
        }
        return 0;
    } else {
        return 1;
    }
}

void finalMessage() {
    unsigned char arr[] = {0x87,0x2f,0x2e,0x69,0x4f,0xf9,0x8c,0x22,0xb2,0xc2,0x82,0x0f,0xdb,0xbc,0x9f,0x3b,0xd4,0xd3,0x00,0x3a,0x7e,0x50,0xff,0x64,0xef,0x53,0x8c,0x8e,0x25,0x85,0x91,0x88,0x6a,0xa2,0x29,0xc8,0xac,0xf0,0xdd,0x09,0x62,0xa1,0xcc,0x70,0x24,0x39,0xdf,0xa6,0xc5,0x2b,0xe5,0x9b,0xd6,0xcc,0xe6,0xd7,0x14,0x8a,0x20,0x40,0x66,0x81,0xa3,0xf0,0x03,0xbe,0x64,0xd0,0xb7,0x61,0x22,0x88,0xc2,0xf4,0x23,0xf9,0x68,0xe8,0x1a,0x3d,0xf2,0x4c,0x8b,0xd7,0xe2,0x89,0x32,0x79,0x55,0x1e,0x9a,0x83,0x3d,0x24,0x5c,0x6e,0xa2,0xba,0x68,0x19,0x22,0xea,0xa7,0x67,0xda,0x93,0xc6,0x52,0xa4,0x46,0x44,0x3f,0xfc,0xe8,0xfa,0xc3,0xf0,0x32,0xc5,0x23,0xd5,0xc1,0x5c,0xf2,0x31,0x9e,0x2e,0xde,0x5f,0x7b,0x1c,0xb8,0x9d,0xfd,0xa4,0xd5,0xa5,0x39,0x93,0x51,0x48,0xcc,0x98,0x4e,0x40,0x93,0x16,0xb3,0x38,0x74,0xf2,0xac,0xea,0xb6,0x50,0x40,0x5c,0xb4,0x9d,0x1d,0xd4,0xc5,0x0a,0x4d,0x05,0x4f,0x75,0x50,0xaa,0x66,0x07,0x36,0x52,0xd9,0x9f,0x5d,0xc6,0x13,0xe5,0x82,0x30,0x28,0x68,0xe8,0x50,0xaa,0x66,0x1f,0xfc,0xca,0x3b,0x65,0x41,0x80,0xc1,0x57,0x80,0x0c,0xf4,0x28,0x8e,0x86,0xad,0x78,0x65,0x0a,0x7a,0x4e,0xb1,0x02,0x1f,0x40,0x98,0xe5,0x6f,0x28,0xc6,0x4f,0x31,0xab,0xec,0x4d,0xad,0xbe,0x3e,0xa7,0xaf,0x2d,0x0e,0x58,0x25,0x68,0x23,0x4c,0x75,0x46,0xee,0xeb,0x80,0xdd,0xde,0x55,0xd3,0x6b,0xc4,0xd9,0xbf,0xa0,0x76,0x2f,0x8a,0x9c,0x2c,0x0f,0xff,0xb4,0x88,0x31,0x95,0xe5,0x38,0x14,0x66,0x82,0xfa,0x10,0xf6,0xbb,0x79,0xd5,0x48,0x4a,0x01,0xa8,0x02,0x69,0xba,0x2c,0x70,0xcc,0xfa,0x0c,0xa0,0xda,0xc6,0xe2,0xa4,0x11,0x4d,0xda,0x06,0x61,0x08,0xb6,0x88,0xe2,0xfb,0xb2,0x5d,0x6c,0xaf,0x5d,0xba,0x27,0x43,0x08,0x4c,0x40,0xe0,0x0d,0xbf,0x9d,0xb2,0xfa,0x63,0x9f,0xea,0x55,0x71,0xf0,0xb6,0xa9,0x6c,0x28,0xeb,0x53,0x16,0xdf,0x82,0xa7,0x08,0xbc,0x3d,0x36,0x15,0x20,0x53,0xb6,0x1f,0xd1,0xb9,0x60,0x8f,0xad,0xee,0xe8,0x70,0xa2,0x34,0x24,0x2a,0xcf,0x1b,0xf4,0x97,0x36,0x85,0x30,0xb5,0xc0,0x2d,0x33,0x55,0x19,0xa1,0xb8,0x4c,0x65,0x86,0x38,0xb3,0xed,0x49,0x94,0x06,0xb2,0x13,0xec,0x95,0xc9,0x2a,0x0d,0x79,0x24,0x64,0xbf,0x7d,0x0e,0xa2,0x9d,0xfa,0xc3,0x14,0xe6,0x51,0xf4,0x61,0xef,0xaa,0x13,0xf0,0x2f,0x30,0xd9,0xf7,0xe9,0x65,0x82,0x96,0xe8,0x35,0x81,0x84,0x37,0x4e,0x3d,0xce,0x43,0x41,0xe7,0x21,0xa4,0xb9,0x47,0xb6,0x2b,0x68,0xd6,0xc6,0x81,0x70,0xe7,0x25,0xd3,0x63,0x06,0x2f,0x9c,0xe5,0x7e,0x95,0x6b,0xaf,0xef,0x77,0xa9,0x2e,0x1c,0x52,0x8f,0xf5,0x14,0x1b,0xe8,0x92,0xdc,0x94,0xc2,0xde,0x11,0x63,0x60,0x6b,0xb3,0xf3,0xc9,0xd4,0xb0,0x45,0x56,0x48,0xff,0x1e,0x01,0xe9,0xa4,0x86,0x7c,0x66,0x9c,0xac,0x59,0x83,0x75,0xa7,0x8a,0x86,0x24,0xa5,0xa3,0xda,0x04,0x4c,0x6c,0xf6,0x67,0x4b,0xf8,0xfe,0xab,0x91,0x5e,0x30,0x05,0x64,0x15,0x25,0x95,0xb8,0xa8,0x91,0x0d,0x10,0x7d,0x82,0x96,0xe4,0xff,0xbd,0xeb,0x7a,0x7e,0x9b,0xd6,0xdd,0xf4,0x0d,0x3c,0xf1,0xad,0xb6,0xb8,0xbb,0x45,0xe9,0x77,0xb4,0x66,0xba,0xa8,0x2f,0xbb,0xec,0xd4,0x5f,0xa8,0x1b,0x3f,0xba,0xd6,0x60,0xe4,0x7b,0x53,0xc5,0x13,0x0b,0xce,0x74,0x95,0x65,0x5d,0x43,0x5e,0x61,0xb3,0xca,0x03,0xeb,0xd2,0x40,0x91,0x9b,0x2d,0xe8,0x1c,0xcb,0x86,0x9d,0xfd,0xf8,0x69,0xc8,0x11,0xe2,0xa5,0x7f,0xcf,0xe2,0xf3,0x19,0xce,0xca,0x45,0xbc,0xb8,0x77,0xa6,0x3e,0x10,0x4c,0xc5,0x17,0xd9,0x8e,0xd5,0xbc,0x26,0x94,0x92,0x20,0x83,0x36,0x11,0x31,0x95,0xe9,0xc6,0x17,0x19,0xba,0xdd,0x66,0x3b,0x86,0x67,0xf0,0x1d,0xa8,0x2b,0x8b,0x03,0x44,0xa6,0x5a,0x68,0x06,0x68,0xe7,0x62,0x1f,0x64,0x8e,0x0a,0x80,0x1c,0x2e,0xc8,0x9d,0xe5,0xe8,0xde,0x57,0x4e,0x97,0xe8,0x79,0x07,0x11,0x37,0x14,0x17,0x23,0xd7,0x95,0xc9,0x58,0x00,0xae,0x15,0xfc,0x92,0xb9,0xd8,0x0a,0x37,0x72,0x3f,0xb3,0x40,0xd0,0x52,0x76,0xd7,0xb3,0xb1,0x18,0xb0,0x63,0x18,0x72,0xcc,0x04,0x2f,0x96,0xbf,0xf8,0xec,0x83,0xc5,0x8b,0xae,0x14,0x63,0xd8,0x8b,0xb2,0xf9,0x1d,0x11,0x39,0xc7,0x99,0x21,0xf0,0xc5,0x76,0xd6,0x5a,0x85,0x5b,0x2b,0x88,0x82,0x12,0xd3,0x76,0xbd,0x74,0x3d,0x5c,0xd5,0xa6,0x85,0xd8,0x1c,0xfb,0x1c,0xb7,0x0e,0xa2,0xe4,0x7f,0xb3,0xd8,0xb5,0x87,0x3b,0x44,0x62,0x58,0x0a,0x55,0x26,0xdc,0x2b,0x58,0x7d,0x8b,0x0f,0x28,0xa0,0xe6,0x38,0x97,0x2e,0xb9,0x60,0xa0,0xf9,0x49,0x6d,0x40,0x42,0xc9,0x22,0xbc,0x25,0xf9,0xed,0xc3,0x31,0xb7,0xf8,0x31,0x00,0xc8,0x75,0x21,0xe5,0x84,0xdd,0xc2,0xdb,0x2a,0xfa,0x32,0x42,0x5a,0x68,0x38,0x77,0xa6,0x96,0xa8,0x2e,0xe8,0x62,0x0b,0x95,0xb2,0x8b,0x66,0x5d,0xce,0xa2,0x69,0x76,0x54,0xbe,0xd7,0x20,0x91,0x4f,0x2d,0x6b,0xd4,0x7d,0x69,0x31,0xa8,0x8e,0x32,0xd9,0x1d,0x17,0xfc,0x8a,0xe7,0xbc,0x55,0x9a,0x21,0xd3,0x4a,0xc0,0xd4,0xbd,0xe8,0x1d,0xb7,0xd9,0x30,0x2d,0x18,0x77,0xf1,0xef,0x7d,0xd7,0xb7,0x3f,0x0c,0x8e,0x68,0x69,0x53,0x70,0x44,0x7a,0x06,0x4a,0x3a,0xff,0x39,0x9f,0x42,0x15,0x11,0x92,0xb5,0x8b,0x5e,0xf4,0x65,0x91,0x25,0x96,0xc6,0x60,0x7d,0x97,0x4d,0xc9,0x74,0x01,0x62,0x25,0xe8,0xa3,0x65,0xd1,0x37,0xa5,0x44,0x55,0xd6,0xd5,0x1e,0x48,0x60,0x8c,0x46,0xf9,0x3b,0x72,0x49,0x43,0x1d,0x6d,0x97,0x5c,0xf0,0x88,0x78,0x0c,0x8e,0x39,0x25,0x12,0x8c,0xdd,0x22,0xd4,0xd9,0x41,0x13,0x8f,0xad,0xcf,0x10,0xaa,0x2a,0x32,0xe0,0x19,0x98,0x9d,0x96,0x0c,0xf2,0x11,0xeb,0xbf,0x2e,0x53,0xc0,0xd0,0xd0,0xac,0x26,0xb7,0x46,0x94,0x07,0x08,0x16,0x38,0xfb,0xc1,0xf9,0xb2,0x1b,0x63,0x96,0x65,0x5d,0x2c,0x03,0x93,0x83,0xbd,0x99,0xe7,0x39,0xfa,0x11,0xca,0x23,0x83,0xb3,0xd7,0x55,0x78,0x6e,0xd0,0xba,0x9c,0xf1,0x4d,0xa2,0x9c,0x39,0xda,0x12,0xf3,0x4d,0xb4,0x0f,0xb7,0xb1,0x6e,0x03,0x95,0x1a,0x57,0xf8,0xb5,0x6b,0x3a,0xd8,0x57,0x51,0x0e,0xea,0x38,0x3b,0xfc,0x17,0x98,0xdc,0x78,0xef,0x5a,0xbf,0x96,0xd5,0x68,0x33,0x4c,0x33,0xa7,0x57,0xad,0x83,0xfe,0x2a,0x8e,0x07,0xe1,0xeb,0x0a,0x36,0x20,0xa7,0x16,0xfe,0x8f,0x37,0xd4,0x4b,0xc4,0xf2,0x6b,0x24,0xd0,0x74,0x56,0xf7,0xb7,0x65,0x39,0xe9,0x5e,0xfd,0x6d,0x50,0x8a,0xfe,0x46,0x8c,0xf9,0xcb,0xa4,0x5b,0x82,0x06,0x24,0xb4,0xda,0x5c,0x54,0x7c,0x71,0xb8,0x09,0x44,0x9f,0x49,0x8c,0xf3,0x19,0xbb,0x86,0x82,0x19,0xe9,0xd9,0x62,0xad,0xde,0x2e,0x60,0x9a,0x8f,0x95,0xab,0x35,0xbf,0xa0,0xb7,0x35,0x31,0x70,0x1e,0xb3,0xc2,0x07,0x6a,0xc7,0x12,0x5a,0x55,0x22,0x2a,0x86,0x11,0x05,0x4a,0xa0,0x5c,0x03,0xfc,0xf9,0x1e,0xa8,0xad,0x48,0x24,0xb1,0x46,0x0f,0x08,0x57,0xe8,0x24,0xc6,0x4b,0xe1,0x78,0xe9,0x7a,0xec,0x31,0x5b,0x59,0x1e,0x5a,0xb2,0x8b,0xca,0xe8,0xf2,0xce,0xdc,0xed,0x0c,0x19,0xf2,0x93,0x7d,0x8f,0x9b,0x07,0xb6,0x14,0x6e,0xf9,0xbb,0x57,0x9e,0x05,0x9d,0xb0,0xa7,0x3f,0x76,0x61,0x8c,0x50,0x58,0x76,0x1f,0xbd,0xe2,0xd7,0x98,0x6a,0x2e,0xda,0x08,0x84,0x53,0x0b,0x63,0x03,0xa8,0xdc,0x20,0xf6,0x68,0x72,0x65,0xa0,0xa8,0xa4,0x97,0x3f,0x79,0x18,0xd0,0x9d,0x0b,0x54,0x71,0x56,0x29,0xe9,0x33,0x18,0x62,0xeb,0xac,0xf6,0x29,0xd6,0x11,0xb8,0xf9,0x3f,0x4f,0x1e,0xf1,0xf7,0xe3,0x05,0x1d,0x15,0x87,0xd2,0xec,0x49,0x25,0x89,0xca,0x4e,0x23,0xce,0x83,0x3b,0x9a,0x6a,0xc2,0x19,0xf4,0x7f,0x7f,0x29,0x0b,0x37,0x7f,0x08,0xc2,0x65,0x46,0xe7,0x58,0xd5,0x1d,0xe1,0xab,0xaa,0x83,0x86,0x0e,0x07,0xce,0x31,0x3a,0x8c,0xfa,0x19,0x18,0x74,0xdd,0x27,0x9e,0xc4,0x69,0x3a,0x89,0xfb,0xbb,0x27,0x7f,0xcd,0xa6,0x62,0xba,0xe2,0x7a,0x0a,0x87,0x88,0x5d,0xb3,0xd2,0x2e,0xd5,0x77,0xed,0xd3,0x26,0x8b,0x24,0x22,0xd6,0xe6,0x22,0x4d,0x5d,0x7f,0x28,0x00,0x0e,0xf9,0xc2,0x35,0x04,0x4b,0xaa,0x88,0xc1,0x92,0xb4,0x5a,0xc9,0x0d,0x52,0xc4,0x4c,0xd5,0x09,0x56,0x30,0xcc,0x2e,0x5a,0x6d,0x1a,0x81,0xa8,0xe9,0x6e,0xf3,0x1b,0x03,0xff,0x82,0x84,0x65,0x3c,0x8e,0x57,0xd9,0xc8,0xd7,0xb6,0x7f,0x7b,0xbd,0xc1,0x85,0xd4,0x2b,0xcf,0xd2,0x24,0x13,0x66,0xba,0x8a,0x31,0xd5,0xd2,0x0c,0x2e,0xae,0x28,0x9b,0x38,0xc5,0x94,0x81,0xb9,0x87,0x44,0xd0,0x42,0x0e,0xd9,0xb5,0x17,0xb0,0x8e,0xc3,0xa7,0xe0,0x63,0x1b,0x84,0x1a,0xaf,0x92,0x73,0xa2,0x67,0x2e,0xcc,0x3c,0x2e,0x9b,0xab,0x35,0x9b,0xcd,0xd4,0x16,0xca,0xb1,0x60,0x4d,0x33,0x69,0xae,0x6a,0xcb,0x04,0xf5,0x97,0xd6,0x7d,0xe5,0x7a,0x6e,0xba,0xf9,0xf5,0xf0,0x66,0x7c,0x2a,0x77,0x44,0x85,0x51,0xa0,0xfe,0x11,0xad,0xbb,0x9e,0x24,0x60,0xee,0x73,0xa4,0xc5,0x63,0x16,0xf7,0x64,0x99,0x72,0x2d,0x14,0xb8,0x7c,0x86,0x48,0x79,0x35,0xeb,0x71,0x6d,0xf3,0x12,0x55,0xc9,0x9c,0xb8,0xaf,0xed,0x85,0xdb,0x7e,0x35,0xad,0x8c,0x3d,0x72,0xc9,0x9e,0x49,0xd8,0x22,0xd8,0x0e,0xeb,0x0f,0xc6,0xfb,0x85,0x6f,0x6e,0x8a,0xf6,0x70,0x70,0x93,0x6a,0x04,0x6c,0x73,0xf8,0x68,0x3c,0x4e,0x58,0xed,0x66,0x4f,0x1e,0x90,0x8e,0xe1,0x7a,0xe7,0x22,0xf0,0x1c,0x9e,0x67,0x9c,0x76,0x68,0xa2,0xa4,0xe2,0xe7,0xd8,0xd9,0x28,0x23,0xb8,0x45,0xdd,0xb8,0x3d,0xbe,0x53,0xa1,0x5c,0x39,0xb1,0xa2,0x10,0x1c,0xbd,0x8a,0x01,0x58,0x0b,0x50,0x97,0x73,0x0e,0xbb,0x22,0xc9,0xad,0x08,0x53,0x9e,0xbc,0x17,0xfe,0x90,0x80,0x63,0x80,0xa5,0x34,0xb6,0x9a,0xb8,0xd2,0xd8,0x8d,0x1e,0x45,0x03,0x18,0x2b,0xac,0x01,0xc6,0x00,0xb5,0x26,0xef,0xb2,0xa9,0x91,0x29,0x7c,0x53,0x94,0x30,0xe4,0xef,0xcf,0x23,0xfe,0xc5,0x63,0x17,0x29,0xc7,0xd1,0x6d,0x87,0x82,0x77,0x1e,0xdf,0x7e,0x9f,0xce,0x54,0x0f,0x56,0x64,0xd8,0x2a,0xf0,0x87,0xac,0xab,0xd2,0xcc,0xba,0x09,0xf3,0x5f,0xf4,0x26,0x80,0xbd,0xdb,0x4d,0x8f,0x19,0x27,0xd0,0x2d,0x25,0x93,0x48,0xf0,0xca,0xe6,0x59,0x90,0xdb,0x83,0xb1,0x14,0x3f,0xf3,0x7a,0xdf,0xcd,0x1a,0x3a,0x34,0x69,0xc4,0xdc,0xbe,0x57,0x79,0x5b,0x75,0xf8,0xb3,0x98,0x9a,0xaa,0xe7,0xb7,0x77,0x7f,0x2c,0x5c,0xcd,0xeb,0xc5,0x35,0xb5,0xe1,0xed,0x7b,0x6d,0xe8,0x44,0x1d,0x15,0x40,0xd1,0x72,0x29,0xab,0x02,0x1c,0x7f,0x2c,0xce,0x16,0xa5,0xf0,0x3b,0xd9,0x85,0xab,0x61,0x87,0x0f,0x83,0xfa,0x36,0x2f,0x85,0x4f,0x05,0x9a,0xb0,0x53,0xf1,0x93,0x07,0x53,0xfc,0x75,0x5f,0xc4,0xe1,0xbb,0xd8,0xa1,0x37,0x93,0x08,0x48,0xaf,0x08,0x13,0xd5,0x1d,0x36,0xba,0x25,0xcb,0xfe,0xd5,0xd1,0x2e,0xab,0xa8,0x8f,0x52,0xa3,0x68,0xae,0xa9,0x13,0x89,0x9e,0x28,0xed,0x37,0xd3,0x9f,0x47,0xea,0x5b,0x12,0x1c,0x46,0x51,0x7f,0x06,0x48,0x43,0x81,0x47,0x32,0x9d,0xbd,0x2d,0x35,0x11,0xe1,0xe2,0x97,0x73,0x42,0x22,0x42,0x50,0x87,0x7d,0xfa,0x60,0xb2,0x91,0x17,0xcb,0xc8,0xaf,0xa6,0x77,0x72,0x91,0x21,0x79,0x07,0x80,0xd3,0xa4,0xd0,0x97,0x33,0xf1,0xf1,0xc5,0x62,0x8e,0x27,0xec,0x73,0x1f,0xf1,0xe9,0x36,0x16,0xa8,0x85,0x32,0x4f,0xdd,0x12,0x01,0x36,0xff,0x5f,0x64,0x1c,0x44,0x26,0x5c,0x32,0x8d,0xf5,0x42,0xf6,0xda,0x65,0xbf,0xf5,0xa4,0xda,0xc4,0xe3,0x86,0xb4,0x6c,0x6e,0x66,0xef,0xe6,0xb6,0x45,0xe4,0xc1,0x79,0x21,0x14,0x04,0x1f,0x5c,0x51,0xf3,0x57,0x94,0x1c,0x5b,0xb1,0x02,0xb6,0x56,0x44,0xac,0xe1,0x78,0x43,0xa5,0xfe,0x52,0x65,0x5d,0xb5,0xad,0x48,0xca,0x8c,0x60,0x0f,0xf4,0xb4,0xb6,0xab,0x6b,0x12,0x8d,0xcd,0x4c,0x74,0xf3,0x05,0xe3,0x77,0x79,0xc7,0xe1,0x38,0x56,0x4f,0xec,0x28,0xd7,0xd4,0x57,0xad,0x8a,0xd2,0x20,0x67,0x48,0x7f,0x0d,0x27,0xce,0xd1,0x93,0xd0,0x2e,0x0a,0xa4,0xfd,0xd5,0xa9,0x37,0x76,0x7d,0x20,0xfa,0xc5,0x05,0xa6,0x05,0xaa,0x16,0xa9,0x17,0x53,0x64,0x1c,0xef,0x47,0x28,0x8f,0xd2,0x48,0xda,0x03,0x45,0x66,0xb6,0xae,0x33,0xf9,0x32,0x80,0x8c,0xe8,0x2b,0xd2,0xe3,0x9e,0x1e,0xb1,0x49,0xc9,0x33,0xda,0xe1,0x95,0xfd,0x73,0xa9,0x53,0x58,0x68,0xde,0x9b,0x71,0x82,0xdf,0xbb,0xfb,0x46,0x9e,0x60,0xde,0x14,0x42,0xc8,0x4d,0x50,0xa5,0x3e,0xbf,0x86,0x2d,0xa7,0x42,0x61,0xa0,0x03,0x4a,0x6d,0xbf,0xe1,0x11,0xf7,0x58,0xae,0x58,0xbc,0x31,0x21,0xf3,0x9a,0xa6,0x5d,0xc2,0xc4,0x76,0xb0,0x16,0xb5,0xe7,0x2c,0xba,0x43,0x01,0x43,0xae,0xdb,0xb9,0x95,0x15,0xe9,0x2e,0x91,0xa4,0x31,0xb5,0x01,0x54,0x6e,0xe2,0xee,0xc1,0x76,0x00,0xb5,0x34,0x63,0xd3,0xa5,0x74,0x78,0xf5,0x3b,0x6d,0x20,0x50,0x1b,0x52,0xc9,0xc4,0x6c,0xbd,0x2c,0x5e,0xc7,0xed,0x34,0x1a,0x42,0xef,0xaa,0x64,0x4c,0x1e,0x25,0x97,0x53,0x6b,0x9e,0xcd,0x0f,0x83,0xf0,0x30,0xb2,0x4d,0x47,0x6f,0xad,0x4a,0x78,0x0a,0x26,0x62,0x06,0x2f,0xbc,0x1b,0xdb,0x86,0x60,0xe3,0x1b,0xe3,0x20,0xb5,0xcb,0x95,0xcf,0x43,0x21,0xca,0x62,0x7c,0x0e,0x18,0xb5,0x87,0x88,0xe5,0x4c,0x4b,0x22,0x30,0x59,0x34,0xcd,0x16,0x06,0xf9,0x82,0xc4,0xce,0x1d,0x62,0x8b,0xd8,0x5e,0x8c,0x37,0x8b,0x7c,0x36,0x31,0x61,0x81,0x34,0xae,0x0d,0xf5,0x0b,0x3e,0x71,0xcc,0xbd,0x42,0xff,0x38,0x19,0x2b,0xa7,0xcf,0x43,0xab,0xc1,0xa3,0x95,0x22,0x0f,0x6c,0x98,0xdf,0x79,0x1f,0x77,0x82,0x1d,0x8f,0x60,0x86,0xef,0x24,0x25,0x1b,0x06,0x86,0xfb,0x2c,0x73,0xb2,0xd1,0x4a,0x9d,0xf1,0x15,0xcc,0xb4,0x6d,0xde,0x87,0x2a,0xa6,0x08,0x5c,0x6b,0x64,0xe4,0x1a,0x0a,0x16,0x1a,0x60,0x56,0xcd,0x6a,0x23,0xe4,0x00,0x04,0x62,0x25,0x88,0xc7,0x48,0xf1,0xfd,0x90,0x17,0x27,0xfd,0x6f,0x81,0x1f,0x2d,0x1a,0x63,0x5e,0xce,0xfa,0xdb,0x67,0xaa,0xba,0x23,0xe1,0x12,0xf7,0x35,0x42,0xa8,0x0c,0x5d,0xb8,0xe1,0x66,0x55,0x5f,0x44,0xc1,0xbd,0x4c,0x3e,0x39,0xd0,0x4f,0x25,0xa3,0x5b,0xd7,0x46,0x42,0x33,0x30,0xbc,0x81,0x61,0x6e,0x68,0x09,0xf7,0x37,0x92,0x4b,0x85,0xbb,0xb8,0xd1,0x4e,0x9f,0x6a,0x93,0xdf,0xfa,0xf7};
    unsigned char key[] = {0x50,0x02,0x40,0x53,0x46,0x12,0x41,0x72,0x6b,0x41,0x12,0x44,0x01,0x40,0x4b,0x12,0x5c,0x03,0x51,0x01};

    int s = 2367;
    int ks = 20;
    char new_key[ks];

    for(int i=0; i<ks; ++i)
        new_key[i] = key[i] ^ 0x32;

    unsigned char sbox[N];
    KSA(new_key, sbox);

    char plaintext[s];
    for (int i = 0; i < s; i++)
        plaintext[i] = arr[i];
    char * pt = plaintext;
    PRGA2(sbox, pt, arr, s);
    printf("\n\n%s\n\n", pt);
}

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);
    fflush(stdout);

    finalMessage();
    int exit = 0;
    while (exit == 0)
        exit = menu();

    printf("\nThank you for using Super Secure Heap (SSH). Exiting.\n");
}