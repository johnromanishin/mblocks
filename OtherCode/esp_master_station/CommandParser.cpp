#include "CommandParser.h"
#include <Arduino.h>
#include <cstring>

#define MAX_COMMAND_LEN 1024
#define MAX_COMMAND_ARGS 32

int callCommand(char* cmd, const command_t* list, int len, void* user)
{
  char* argv[MAX_COMMAND_ARGS];
  int argc = 1;
  argv[0] = cmd;
  
  // Turn all spaces in the cmd string into null terminators, and mark their
  // locations.  For instance, a string like the following
  //
  //    "set-virtual-arrow 5 2"
  //
  // will turn into
  // 
  //    "set-virtual-arrow|5|2"
  //
  // Where | denotes a null character.  Furthermore, the array called argv will
  // contain pointers to each of the 3 null terminated strings in the above 
  // example.
  int idex = 0;
  while(cmd[idex] && (idex < MAX_COMMAND_LEN))
  {
    if(cmd[idex] == ' ')
    {
      cmd[idex] = '\0';
      argv[argc++] = &cmd[idex + 1];
    }
    idex++;
  }

  // Search through the list of functions and see which one needs to be called.
  int i;
  for(i = 0; i < len; i++)
  {
    if(strcmp(list[i].name, cmd) == 0)
    {
      list[i].handler(argc, argv, user);
      break;
    }
  }

  if(i == len)
    return -1;
  else
    return 0;
}

int checkSerialAndCall(serial_decoder_buffer_t* buf, const command_t* list,
                       int len, void* user)
{
  while(Serial.available() > 0)
  {
    int newByte = Serial.read();
    if(newByte >= 0)
    {
      buf->c[buf->size++] = (char)newByte;
      Serial.write(newByte);
      if((buf->size >= 2) && (buf->c[buf->size - 1] == '\n') &&
         (buf->c[buf->size - 2] == '\r'))
      {
        // Insert appropriate null terminators.  See callCommand() for more info.
        char* argv[MAX_COMMAND_ARGS];
        int argc = 1;
        argv[0] = buf->c;
        int idex = 0;
        while(idex < buf->size)
        {
          if(buf->c[idex] == ' ')
          {
            buf->c[idex] = '\0';
            argv[argc++] = &buf->c[idex + 1];
          }
          if(buf->c[idex] == '\r')
          {
            buf->c[idex] = '\0';
          }
          idex++;
        }
        
        // Search through the list of commands and see which one needs to be 
        // called.
        int i;
        for(i = 0; i < len; i++)
        {
          if(strcmp(list[i].name, buf->c) == 0)
          {
            Serial.printf("Command %s proccessed with args ", list[i].name);
            for(int j = 1; j < argc; j++)
              Serial.printf("%s ", argv[j]);
            Serial.println();
            list[i].handler(argc, argv, user);
            break;
          }
        }
        
        if(i == len)
          Serial.println("Unknown command.  Type \'help\' for a list of commands");

        buf->size = 0;
      }
      
    }
  }

  return 0;
}

