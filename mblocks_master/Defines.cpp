#include "Defines.h"

/**
 * For the ith face, faceRotations[i][0] will tell which face the intrisic arrow points towards.
 * faceRotations[i][1] tells which face the arrow rotated 90 degrees counter-clockwise from the
 * intrinsic arrow points towards.  face[rotations[i][2] give the 180-degree face.
 * 
 * For cubes 
 */
const int faceRotations[][4] =
{
  {4, 1, 5, 3},
  {0, 4, 2, 5},
  {3, 5, 1, 4},
  {5, 2, 4, 0},
  {1, 0, 3, 2},
  {2, 3, 0, 1}
};


