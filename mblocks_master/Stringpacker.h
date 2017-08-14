/**
 * The easyMesh library does not appear to support the transmission of raw
 * binary information.  In order to get around this, this class allow you to
 * easily pack and unpack bytes, integers, floats, and doubles into a string.
 *
 * A naive way of doing this is encoding numbers as human-readable numbers in
 * base 10.  Meaning that the number '101' would be encoded as
 *    "101"
 * This is not ideal because the act of encoding and decoding this message will
 * require very slow division operations.
 *
 * Instead, we encode binary data in base-64.  Because 64 is a power of 2, we
 * can use bitshifting operations to encode and decode the data, which is much
 * faster.  Now, the number '101' will be encoded as
 *    "@E"
 * https://en.wikipedia.org/wiki/Base64 has more information about this topic.
 *
 * Example usage
 *
 * Transmitter side:
 *    StringPacker p;
 *    p.packU32(101);
 *    p.packU8('a');
 *
 *    mesh.sendBroadcast(p.str());
 *
 * Reciever side:
 *    StringUnpacker p;
 *    char c = p.unpackChar();
 *    int i = p.unpackInteger();
 *
 *    // c = 'a' and i = 101.
 */

#ifndef _PACKABLE_STRING_H
#define _PACKABLE_STRING_H

#include <string>

using namespace std;

class StringPacker
{
  private:
    char* buf;
    int charCount;
    int bitModulo;
    const int asciiOffset;

  public:
    StringPacker(char*);
    void packU8(char);
    void packU32(int);
    string str();
};

class StringUnpacker
{
  private:
    string s;
    int charCount;
    int bitModulo;
    const int asciiOffset;

  public:
    StringUnpacker(string);
    void setString(string);
    char unpackU8();
    int unpackU32();
};

#endif
