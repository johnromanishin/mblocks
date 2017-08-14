#if 1
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <stdint.h>
#include "packablestring.h"
using namespace std;

void print_binary_8(uint8_t c)
{
    for(int i = 0; i < 8; i++)
    {
        printf("%c", (c & 0x80) ? '1':'0');
        c <<= 1;
    }
}

void print_binary_6(uint8_t c)
{
    for(int i = 0; i < 6; i++)
    {
        printf("%c", (c & 0x20) ? '1':'0');
        c <<= 1;
    }
}

typedef struct packit
{
    int whichone;
    union
    {
        uint32_t u32;
        uint8_t u8;
    } data;
} packit_t;

uint8_t topack[] = {1};

int main(int argc, char** argv)
{
    {
        char foo[1024];
        StringPacker p(foo);
        for(int i = 0; i < sizeof(topack); i++)
        {
            p.packU8(topack[i]);
            print_binary_8(topack[i]);
            printf(" ");
        }
        printf("\r\n");

        for(int i = 0; i < ((sizeof(topack) * 8) / 6); i++)
        {
            print_binary_6(foo[i]);
            printf(" ");
        }
        printf("\r\n");

        StringUnpacker u(p.str());
        for(int i = 0; i < sizeof(topack); i++)
        {
            print_binary_8(u.unpackU8());
            printf(" ");
        }
        printf("\r\n");

        //return 0;
    }

    int packamt = 1022;
    char foo[packamt + 1];
    packit groundtruth[packamt + 1];
    StringPacker p(foo);

    //generate a bunch of random things
    int amount_packed = 0;
    int i = 0;
    srand(28);
    while(amount_packed < (packamt / 2))
    {
        groundtruth[i].whichone = rand() % 2;
        if(groundtruth[i].whichone == 0)
        {
            groundtruth[i].data.u32 = rand();
            printf("encoding %08x\r\n", groundtruth[i].data.u32);
            p.packU32(groundtruth[i].data.u32);
            amount_packed += 4;
        }
        else
        {
            groundtruth[i].data.u8 = rand();
            printf("encoding %02x\r\n", groundtruth[i].data.u8);
            p.packU8(groundtruth[i].data.u8);
            amount_packed += 1;
        }
        i++;
    }

    printf("amount packed = %i\r\n", amount_packed);

    //try to decode them
    cout << p.str() << endl;
    printf("-------------------------------------------\r\n");
    StringUnpacker u(p.str());

    i = 0;

    while(amount_packed > 0)
    {
        if(groundtruth[i].whichone == 0)
        {
            uint32_t k = u.unpackU32();
            printf("unpacked %08x ", k);
            if(groundtruth[i].data.u32 != k)
                printf("failed.  Expected %08x", groundtruth[i].data.u32);
            amount_packed -= 4;
        }
        else
        {
            uint8_t k = u.unpackU8();
            printf("unpacked %02x ", k);
            if(groundtruth[i].data.u8 != k)
                printf("failed.  Expected %02x", groundtruth[i].data.u8);
            amount_packed -= 1;
        }

        printf("\r\n");
        i++;
    }
}

#endif
/**
 *
 calc_0321_drive[][6] =
{
    { 0, -1,  0,  1,  0,  0},
    { 1,  0, -1,  0,  0,  0},
    { 0,  1,  0, -1,  0,  0},
    {-1,  0,  1,  0,  0,  0},
    { 0,  0,  0,  0,  0,  0},
    { 0,  0,  0,  0,  0,  0}
};*/
