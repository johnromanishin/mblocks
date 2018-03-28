#ifndef _ESP_TO_CUBE_MAPPING_H
#define _ESP_TO_CUBE_MAPPING_H

typedef struct EspToCubeMapping
{
  int esp;
  int cube;
} EspToCubeMapping;

int getEspIDFromCube(int);
int getCubeIDFromEsp(int);
#endif
