#include "serial_decoder.h"
#include <cstring>

const SerialResponsePairing pairings[] =
{
    {"Successfully actuated mechanical brake\r\n", RESPONSE_MECHBRAKE_SUCCESS},
    {"Failed to actuate mechanical brake\r\n", RESPONSE_MECHBRAKE_FAILURE},
    {"Timeout while actuating mechanical brake\r\n", RESPONSE_MECHBRAKE_TIMEOUT},
    {"SMA retracted\r\n", RESPONSE_SMA_RETRACTED},
    {"SMA extending\r\n", RESPONSE_SMA_EXTENDING},
    {"SMA extended\r\n", RESPONSE_SMA_EXTENDED},
    {"BLDC motor speed stabilized\r\n", RESPONSE_BLDC_STABLE},
    {"BLDC motor timeout\r\n", RESPONSE_BLDC_TIMEOUT},
    {"BLDC motor acceleration complete\r\n", RESPONSE_BLDC_ACCEL_COMPLETE},
    {"BLDC motor coasting to stop\r\n", RESPONSE_BLDC_COASTING},
    {"BLDC motor stopped\r\n", RESPONSE_BLDC_STOPPED},
    {"Motion primitive unrecognized\r\n", RESPONSE_COMMAND_UNRECOGNIZED},
    {0, RESPONSE_NONE_YET}
};

SerialResponse pushNewChar(SerialDecoderBuffer* b, int newByte)
{
    SerialResponse retval = RESPONSE_NONE_YET;
    int hasresult = 0;
    //while((Serial.available() > 0) && (hasresult == 0))
    //while(hasresult == 0)
    {
        //int newByte = Serial.read();
        if(newByte >= 0)
        {
            // add the new character
            b->c[b->size++] = (char)newByte;

            // if we have reached an \r\n, search the list.
            if((b->size >= 2) && (b->c[b->size - 1] == '\n') &&
               (b->c[b->size - 2] == '\r'))
            {
                // search the list.  If we find something, set the hasresult
                // flag and store the corresponding enum in our return value.
                int found_valid_string = 0;
                for(int i = 0; pairings[i].c; i++)
                {
                    if(strncmp(pairings[i].c, b->c, b->size) == 0)
                    {
                        found_valid_string = 1;
                        retval = pairings[i].sr;
                        b->size = 0;
                        break;
                    }
                }

                // if there were no hits in the list, we still need to return
                // because a \r\n was detected.  We just got an invalid response
                if(!found_valid_string)
                {
                    retval = RESPONSE_UNKNOWN;
                    b->size = 0;
                }
                hasresult = 1;
            }
        }
    }

    return retval;
}
