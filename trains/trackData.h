#ifndef TRACK_DATA_H
#define TRACK_DATA_H

#include "trains/trackNode.h"

// The track initialization functions expect an array of this size.
#define TRACK_MAX 144

void init_tracka(TrackNode *track);
void init_trackb(TrackNode *track);

#endif
