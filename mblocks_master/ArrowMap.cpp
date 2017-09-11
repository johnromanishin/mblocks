#include "ArrowMap.h"
#include "Defines.h"

/**
 * This function will store a new arrow mapping in an ArrowMap associative array.
 *
 * This function in conjunction with an ArrowMap object can be used to keep track of which
 * "virtual arrows" point in which direction on cubes.
 */
void addArrowMapping(ArrowMap* map, ArrowMapEntry* newEntry)
{
  int i;
  for(i = 0; i < map->size; i++)
  {
    if(map->entries[i].ID == newEntry->ID)
    {
      for(int i = 0; i < 6; i++)
        map->entries[i].arrows[i] = newEntry->arrows[i];
    }
  }

  if((i == map->size) && (map->capacity != map->size))
  {
    map->entries[i].ID = newEntry->ID;
    for(int j = 0; j < 6; j++)
      map->entries[i].arrows[j] = newEntry->arrows[j];
  }
}

/**
 * Changes the arrow mapping for a single face
 * 
 * -1 signifies a face with no arrow.  Values >= 4 are reserved and might be application 
 * specific.  For instance, in the "crystalize" application, 4 could be designated to 
 * refer to the non-arrow faces of already crystalized cubes.
 */
void addArrowMappingOneFace(ArrowMap* map, int id, int face, int newEntry)
{
  int i;
  for(i = 0; i < map->size; i++)
  {
    if(map->entries[i].ID == id)
    {
      map->entries[i].arrows[face] = newEntry;
    }
  }

  if((i == map->size) && (map->capacity != map->size))
  {
    map->entries[i].ID = id;
    for(int j = 0; j < 6; j++)
      map->entries[i].arrows[j] = -1;
    map->entries[i].arrows[face] = newEntry;
  }
}

/**
 * Given a cube ID, an arrow map, and a face, this will tell what direction the "virtual 
 * arrow" points.
 */
 
int lookupMapping(ArrowMap* map, int id, int face)
{
    for(int i = 0; i < map->size; i++)
    {
        if(map->entries[i].ID == id)
            return map->entries[i].arrows[face];
    }

    return -1;
}

void clearAllMappings(ArrowMap* map)
{
  map->size = 0;
}

/**
 * When an active cube is trying to figure out what direction to turn in order
 * to follow a virtual arrow, it must figure out how far to rotate counter-
 * clockwise.  Using the above matrix, this function tells which face a cube 
 * should flip towards in order to follow a virtual arrow.
 * 
 * returns -1 in case of error.
 */
int getNextFaceRotated(int adjacentFace, int nextFace, int virtualArrowRotation)
{
  int idex;
  for(idex = 0; (faceRotations[adjacentFace][idex] != nextFace) && (idex < 4); idex++);

  // if idex reaches the end of a faceRotations[][] row without matching, an 
  // invald adjacentFace / nextFace combo was entered.
  if(idex == 4)
    return -1;

  if(virtualArrowRotation >= 4)
    return -1;

  idex += virtualArrowRotation;
  if(idex >= 4)
    idex -= 4;

  return faceRotations[adjacentFace][idex];
}

