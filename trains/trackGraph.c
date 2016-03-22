#include "common/common.h"
#include "kernel/kernel.h"
#include "trains/trackGraph.h"

int pathFind(TrackNode* graph, U8 start, U8 end, GraphPath* path)
{
    HeapNodeU16 openList[0x3F];
    U8 mapCameFrom[TRACK_MAX];
    U16 costToNode[TRACK_MAX];

    memset(costToNode, 0xFF, sizeof(U16)*TRACK_MAX);
    memset(mapCameFrom, 0xFF, sizeof(U8)*TRACK_MAX);

    HeapU16 qOpen;
    HeapNodeU16 current = HEAP_NODE(start, 0);

    heapU16Init(&qOpen, openList, 0x3F);
    assert( heapU16Push(&qOpen, current) >= 0 );

    while(heapU16Pop(&qOpen, &current) >= OK)
    {
        U8 cInd = current.value;
        U16 cCost = current.cost;
        
        TrackEdge* neighbours = graph[cInd].edge;
        U8 degree = (graph[cInd].type == eNodeBranch ? 2 : 1);
        U8 ni = 0;
        while(degree--)
        {
            if(neighbours[ni].dest != 0)
            {
                HeapNodeU16 newNode = 
                    HEAP_NODE(
                        neighbours[ni].dest - graph, 
                        cCost + neighbours[ni].dist
                    );

                if(costToNode[newNode.value] > newNode.cost)
                {
                    mapCameFrom[newNode.value] = cInd;
                    costToNode[newNode.value] = newNode.cost;
                    assert( heapU16Push(&qOpen, newNode) >= 0 );
                }
            }
            ++ni;
        }
    }

    if( start == end ) mapCameFrom[end] = start;

    if( mapCameFrom[end] == 0xFF ) return -1;

    listU32Init(&(path->path), path->pathBuffer, GRAPH_PATH_LENGTH);
    while(1)
    {
        assert( listU32PushFront(&(path->path), end) >= 0 );
      
      if( end == start ) { break; }

        end = mapCameFrom[end];
    }
    return 0;
}
