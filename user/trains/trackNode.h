#ifndef TRACK_NODE_H
#define TRACK_NODE_H

typedef enum {
    eNodeNone,
    eNodeSensor,
    eNodeBranch,
    eNodeMerge,
    eNodeEnter,
    eNodeExit,
} TrackNodeType;

#define DIR_AHEAD       0
#define DIR_STRAIGHT    0
#define DIR_CURVED      1

struct TrackNode;
typedef struct TrackNode TrackNode;
typedef struct TrackEdge TrackEdge;

struct TrackEdge {
    TrackEdge*  reverse;
    TrackNode*  src; 
    TrackNode*  dest;
    int         dist;       /* in millimetres */
    int         dx;
    int         dt;
};

struct TrackNode {
  const char *  name;
  TrackNodeType type;
  int           num;        /* sensor or switch number */
  TrackNode*    reverse;    /* same location, but opposite direction */
  TrackEdge     edge[2];
};

#endif
