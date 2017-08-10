#include "packablestring.h"
//#include <iostream>
//using namespace std;

StringPacker::StringPacker(char* b)
    : charCount(0), bitModulo(0)
{
    this->buf = b;
    this->buf[0] = 0;
}

const int masks[] = {0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f};
void StringPacker::packU8(char c)
{
    int bitsleft = 8;
    while(bitsleft > 0)
    {
        // Cut off a chunk of the input byte that is small enough to fit inside
        // the sextet we are currently packing
        int remainChar = 6 - this->bitModulo;
        if(remainChar > bitsleft)
            remainChar = bitsleft;
        int temp = c & masks[remainChar - 1];
        temp <<= (this->bitModulo);

        // Put it in the sextet we are currently packing
        this->buf[this->charCount] |= temp;

        // Advance to the next sextet if we have filled the current one.
        // cout << "remainchar = " << remainChar << endl;
        c >>= remainChar;
        bitsleft -= remainChar;
        this->bitModulo += remainChar;
        if(this->bitModulo == 6)
        {
            this->bitModulo = 0;
            this->charCount++;
            this->buf[this->charCount] = 0;  // Clear out the next sextet
        }
    }
}

void StringPacker::packU32(int i)
{
    int bitsleft = 32;

    while(bitsleft > 0)
    {
        // bitmask part of
        int remainChar = 6 - this->bitModulo;
        if(remainChar > bitsleft)
            remainChar = bitsleft;
        int temp = i & masks[remainChar - 1];
        temp <<= (this->bitModulo);
        this->buf[this->charCount] |= temp;

        // update pointers
        i >>= remainChar;
        bitsleft -= remainChar;
        this->bitModulo += remainChar;
        // cout << "remainchar = " << remainChar << endl;
        if(this->bitModulo == 6)
        {
            this->bitModulo = 0;
            this->charCount++;
            this->buf[this->charCount] = 0;
        }
    }
}

string StringPacker::str()
{
    return string(this->buf);
}

StringUnpacker::StringUnpacker(string str="")
    : s(str), charCount(0), bitModulo(0){}

void StringUnpacker::setString(string s)
{
    this->s = s;
    this->charCount = (this->bitModulo = 0);
    for(int i = 0; i < this->s.length(); i++)
        this->s[i] -= 0;
}

char StringUnpacker::unpackU8()
{
    int bitsleft = 8;
    char retchar = 0;
    while(bitsleft > 0)
    {
        int remainChar = 6 - this->bitModulo;
        if(remainChar > bitsleft)
            remainChar = bitsleft;
        int maskedout = (this->s.c_str()[this->charCount] >> this->bitModulo) &
            masks[remainChar];
        maskedout <<= (8 - bitsleft);

        retchar |= maskedout;
        // cout << "remainchar = " << remainChar << endl;
        this->s[this->charCount] >>= remainChar;
        bitsleft -= remainChar;
        this->bitModulo += remainChar;
        if(this->bitModulo == 6)
        {
            this->bitModulo = 0;
            this->charCount++;
        }
    }

    return retchar;
}

int StringUnpacker::unpackU32()
{
    int bitsleft = 32;
    int ret = 0;
    while(bitsleft > 0)
    {
        int remainChar = 6 - this->bitModulo;
        if(remainChar > bitsleft)
            remainChar = bitsleft;
        int maskedout = (this->s.c_str()[this->charCount] >> this->bitModulo) &
            masks[remainChar];
        maskedout <<= (32 - bitsleft);

        ret |= maskedout;

        this->s[this->charCount] >>= remainChar;
        bitsleft -= remainChar;
        this->bitModulo += remainChar;
        if(this->bitModulo == 6)
        {
            this->bitModulo = 0;
            this->charCount++;
        }
    }

    return ret;
}
