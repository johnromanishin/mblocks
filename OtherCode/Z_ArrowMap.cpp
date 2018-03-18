#include "Z_ArrowMap.h"
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





//Behavior crystalize(Cube* c, painlessMesh* m)
//{
//  if(DEBUG_BEHAVIOR) {Serial.println("crystalize");}
//  bool amICrystalized = false;
//  int ledCounter = 0;
//
//  while(true)
//  {
//    //update the sensors
//    c->updateSensors();
//
//    // ======= service commands that come from the json buffer =======
//    if(!jsonCircularBuffer.empty())
//    {
//      StaticJsonBuffer<256> jb;
//      JsonObject& root = jb.parseObject(jsonCircularBuffer.pop());
//
//      if((root["cubeID"] == getCubeIDFromEsp(ESP.getChipId())) ||
//         (root["cubeID"] == -1))
//      {
//        if(root["cmd"] == "blinkOnce")
//          c->blockingBlink(1, 1, 1, 1, 100);
//        else if(root["cmd"] == "clearArrows")
//          clearAllMappings(&c->arrowMap);
//        else if(root["cmd"] == "arrowUpdateAllFaces")
//        {
//          ArrowMapEntry mapEntry;
//          mapEntry.ID = root["arrowID"];
//          for(int i = 0; i < 6; i++)
//          {
//            mapEntry.arrows[i] = root["arrowDirections"][i];
//          }
//          addArrowMapping(&c->arrowMap, &mapEntry);
//        }
//        else if(root["cmd"] == "arrowUpdateOneFace")
//        {
//          addArrowMappingOneFace(&c->arrowMap, root["arrowID"], root["faceToUpdate"], root["newValue"]);
//        }
//        else if(root["cmd"] == "seedCrystal")
//        {
//          //if the cube is nominated to be the seed crystal, then it points its arrows in a pre-set direction
//          //the arrows on the 0, 1, 2, and 3 faces all point towards face 5.  Face 5 points out of the center
//          //of the cube and face 4 points into the center of the cube.
//          amICrystalized = true;
//          ledCounter = 0;
//          ArrowMapEntry mapEntry;
//          mapEntry.ID = getCubeIDFromEsp(ESP.getChipId());
//          mapEntry.arrows[0] = 0; //XXX TODO?
//          mapEntry.arrows[1] = 0; //XXX TODO ?
//          mapEntry.arrows[2] = 0; //XXX TODO ?
//          mapEntry.arrows[3] = 0; //XXX TODO ?
//          mapEntry.arrows[4] = 4; //face 4's arrow points into the center of the cube.
//          mapEntry.arrows[5] = 5; //face 5's arrow points out of the center of the cube.
//          addArrowMapping(&c->arrowMap, &mapEntry);
//        }
//      }
//    }
//
//    // ======= if we are crystalized, then just stupidly blink =======
//    if(amICrystalized)
//    {
//      //turn all of the corner lights blue
//      c->lightCube(0, 0, 1);
//
//      //blink faces to point in directions of arrows
//      //in the first step of the blink animation, we light faces pointing "into" the center of the cube.
//      ledCounter++;
//      if((ledCounter >= 0) && (ledCounter < 4))
//      {
//        for(int i = 0; i < 6; i++)
//        {
//          if(lookupMapping(&c->arrowMap, getCubeIDFromEsp(ESP.getChipId()), i) == 4)
//            c->faces[i].turnOnFaceLEDs(true, true, true, true);
//          else
//            c->faces[i].turnOnFaceLEDs(false, false, false, false);
//        }
//      }
//      else if((ledCounter >= 4) && (ledCounter < 8))
//      {
//        for(int i = 0; i < 6; i++)
//        {
//          int subsequentRot = lookupMapping(&c->arrowMap, getCubeIDFromEsp(ESP.getChipId()), i);
//          if((subsequentRot >= 0) && (subsequentRot <= 3))
//          {
//            subsequentRot += 2;
//            if(subsequentRot > 3)
//              subsequentRot -= 4;
//            int subsequentFace = faceRotations[i][subsequentRot];
//            c->setFaceLEDsAtEdge(i, subsequentFace);
//          }
//          else
//            c->faces[i].turnOnFaceLEDs(false, false, false, false);
//        }
//      }
//      else if((ledCounter >= 8) && (ledCounter < 12))
//      {
//        for(int i = 0; i < 6; i++)
//        {
//          int subsequentRot = lookupMapping(&c->arrowMap, getCubeIDFromEsp(ESP.getChipId()), i);
//          if((subsequentRot >= 0) && (subsequentRot <= 3))
//          {
//            if(subsequentRot > 3)
//              subsequentRot -= 4;
//            int subsequentFace = faceRotations[i][subsequentRot];
//            c->setFaceLEDsAtEdge(i, subsequentFace);
//          }
//          else
//            c->faces[i].turnOnFaceLEDs(false, false, false, false);
//        }
//      }
//      else if((ledCounter >= 12) && (ledCounter < 16))
//      {
//        for(int i = 0; i < 6; i++)
//        {
//          if(lookupMapping(&c->arrowMap, getCubeIDFromEsp(ESP.getChipId()), i) == 5)
//            c->faces[i].turnOnFaceLEDs(true, true, true, true);
//          else
//            c->faces[i].turnOnFaceLEDs(false, false, false, false);
//        }
//      }
//      else if(ledCounter == 16)
//        ledCounter = 0;
//    }
//
//    // ======= If we haven't crystalized yet,
//    else
//    {
//      //check for neighbors
//    }
//  }
//}

