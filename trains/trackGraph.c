#include "common/common.h"
#include "trains/trackGraph.h"
/*
static U16 findNodeIndex(Graph* graph, U32 id)
{
    U16 i = 0;
    for(i = 0; i < graph->cardinality; ++i)
    {
        if(graph->nodes[i].id == id)
        {
            return i;
        }
    }
    return 0xFFFF;
}

GraphNode* findNode(Graph* graph, U32 id)
{
    U16 i = findNodeIndex(graph, id);
    if( i >= graph->cardinality )
    {
        return 0;
    }
    return graph->nodes + i;
}

int isReachable(Graph* graph, U32 start, U32 end)
{
    U16 openList[TRACK_MAX / 3];
    QueueU16 qOpen;
    queueU16Init(&qOpen, openList, TRACK_MAX / 3);

    U16 current = findNodeIndex(graph, start);
    U16 goal = findNodeIndex(graph, end);
   
    U8 i = 0;
    for(i = 0; i < graph->cardinality; ++i)
        graph->nodes[i].mark = 0;

    do
    {
        if(current == goal)
        {
            return 1;
        }
        
        GraphNode* c = graph->nodes + current;
        c->mark = 1;

        U8 n = 0;
        for(n = 0; n < c->degree; ++n)
            if(!c->neighbours[n]->mark)
                queueU16Push(&qOpen, c->neighbours[n]->index);
    } while(queueU16Pop(&qOpen, &current) >= OK);

    return 0;
}
*/

int pathFind(TrackNode* graph, U32 start, U32 end, GraphPath* path)
{
    HeapNodeU16 openList[TRACK_MAX / 3];
    U16 mapCameFrom[TRACK_MAX]; 
    U16 costToNode[TRACK_MAX];

    memset(costToNode, 0xFF, sizeof(U16)*TRACK_MAX);
    memset(mapCameFrom, 0xFF, sizeof(U16)*TRACK_MAX);

    HeapU16 qOpen;
    HeapNodeU16 current = HEAP_NODE(start, 0);

    heapU16Init(&qOpen, openList, TRACK_MAX / 3);
    heapU16Push(&qOpen, current);

    while(heapU16Pop(&qOpen, &current) >= OK)
    {
        U16 cInd = current.value;
        U16 cCost = current.cost;
        
        TrackEdge* neighbours = graph[cInd].edge;
        U8 degree = (graph[cInd].type == eNodeBranch ? 2 : 1);
        while(degree--)
        {
            HeapNodeU16 newNode = 
                HEAP_NODE(
                    neighbours[degree].dest - graph, 
                    cCost + neighbours[degree].dist
                );

            if(costToNode[newNode.value] > newNode.cost)
            {
                mapCameFrom[newNode.value] = cInd;
                costToNode[newNode.value] = newNode.cost;
                heapU16Push(&qOpen, newNode);
            }
        }
    }

    queueU8Init(&path->qPath, path->pathBuffer, GRAPH_PATH_LENGTH);
    
    U16 ind = end;
    U16 count = 0;
    do
    {
        count++;
        queueU8Push(&path->qPath, ind);

      if( ind == start ) break;

        ind = mapCameFrom[ind];
    } while( 1 );

    return 0;
}
