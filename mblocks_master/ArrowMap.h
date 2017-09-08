/**
 * Essentially a *really* primitive associative array.  Maps cube ID numbers and
 * faces to arrow orientation information.
 *
 * Each cube face has its magnets arranged in an inherently asymmetrical way.
 * Because of this, the magnetic sensors of the active cube can determine the
 * orientation of the cube they are attached to.  The "arrows" in these data
 * structures refer to a counterclockwise rotation of the orientation from
 * a pre-determined "default" position.
 *
 * The functions in this file are clearly a poorly-written subset of the C++
 * STL, but the C++ STL might end up taking up quite a bit of dynamic memory,
 * so we can't really trust it to run on an embedded system.
 */

#ifndef _ARROW_MAP
#define _ARROW_MAP

typedef struct ArrowMapEntry
{
    int ID;
    int arrows[6];
} ArrowMapEntry;

typedef struct ArrowMap
{
    int size;
    int capacity;
    ArrowMapEntry entries[32];
} ArrowMap;



void addArrowMapping(ArrowMap*, ArrowMapEntry*);
void addArrowMappingOneFace(ArrowMap*, int, int, int);
int lookupMapping(ArrowMap*, int, int);
int getNextFaceRotated(int, int);
void clearAllMappings(ArrowMap*);

#endif
