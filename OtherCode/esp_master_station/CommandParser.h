#ifndef _COMMAND_PARSER_H
#define _COMMAND_PARSER_H

typedef struct command
{
  const char* name;
  const char* arghelp;
  void (*handler)(int, char**, void*);
} command_t;

typedef struct serial_decoder_buffer
{
  char* c;
  int capacity;
  int size;
} serial_decoder_buffer_t;

int callCommand(char*, const command_t*, int len, void*);
int checkSerialAndCall(serial_decoder_buffer_t*, const command_t*, int, void*);

#endif
