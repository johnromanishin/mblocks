#if 0
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include "packablestring.h"
using namespace std;

int main(int argc, char** argv)
{
    {
        char foo[1024];
        StringPacker p(foo);
        p.packU8(0x4);
        p.packU8(0x3);
        p.packU8(43);

        for(int i = 0; i < 10; i++)
            printf("%02x ", foo[i]);
        printf("\r\n");
        StringUnpacker u(p.str());

        int a = (int)u.unpackU8();
        int b = (int)u.unpackU8();
        int c = (int)u.unpackU8();

        cout << a << "  " << b << "  " << c << endl;

        return 0;
    }

    char foo[1024];
    int groundtruth[1024];
    StringPacker p(foo);


    //generate a bunch of random integers
    srand(100);
    for(int i = 0; i < 100; i++)
    {
        groundtruth[i] = rand();
        cout << "encoding " << groundtruth[i] << endl;
        p.packU32(groundtruth[i]);
    }

    //try to decode them
    cout << foo << endl;

    StringUnpacker u(p.str());

    for(int i = 0; i < 100; i++)
    {
        int k = u.unpackU32();
        cout << "decoding " << k;
        if(groundtruth[i] != k)
            cout << " failed";
        cout << endl;
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
