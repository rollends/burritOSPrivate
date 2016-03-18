#ifndef TRACK_GRAPH_H
#define TRACK_GRAPH_H

#include "common/common.h"
#include "trains/trackData.h"

#define GRAPH_DEGREE            2
#define GRAPH_PATH_LENGTH       TRACK_MAX 

/*
typedef struct GraphNode
{
    struct GraphNode   *neighbours[GRAPH_DEGREE];
    U32                 cost[GRAPH_DEGREE];
    U8                  degree;
    U8                  id;
    U8                  mark;
    U8                  index;
} GraphNode;

typedef struct
{
    GraphNode           nodes[TRACK_MAX];    
    U32                 cardinality;
} Graph;
*/

typedef struct
{
    U8                  pathBuffer[GRAPH_PATH_LENGTH];
    QueueU8             qPath;
} GraphPath;

int pathFind(TrackNode* graph, U32 start, U32 end, GraphPath* path);
//int isReachable(Graph* graph, U32 start, U32 end);
//GraphNode* findNode(Graph* graph, U32 id);

#endif
