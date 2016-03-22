#ifndef TRACK_GRAPH_H
#define TRACK_GRAPH_H

#include "common/common.h"
#include "trains/trackData.h"

#define GRAPH_DEGREE            2
#define GRAPH_PATH_LENGTH       TRACK_MAX

typedef struct
{
    ListU32Node     pathBuffer[GRAPH_PATH_LENGTH];
    ListU32         path;
} GraphPath;

int pathFind(TrackNode* graph, U8 start, U8 end, GraphPath* path);

#endif
