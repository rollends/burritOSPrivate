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

static void buildPath(U8 start, U8 end, U8* mapCameFrom, GraphPath* path)
{
    if(start != end)
    {
        buildPath(start, mapCameFrom[end], mapCameFrom, path);
    }
    queueU8Push(&(path->qPath), end);
}

int pathFind(TrackNode* graph, U8 start, U8 end, GraphPath* path)
{
    HeapNodeU16 openList[0x7F];
    U8 mapCameFrom[TRACK_MAX];
    U16 costToNode[TRACK_MAX];

    memset(costToNode, 0xFF, sizeof(U16)*TRACK_MAX);
    memset(mapCameFrom, 0xFF, sizeof(U16)*TRACK_MAX);

    HeapU16 qOpen;
    HeapNodeU16 current = HEAP_NODE(start, 0);

    heapU16Init(&qOpen, openList, TRACK_MAX / 3);
    heapU16Push(&qOpen, current);

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
                    heapU16Push(&qOpen, newNode);
                }
            }
            ++ni;
        }
    }

    if( start == end ) mapCameFrom[end] = start;

    if( mapCameFrom[end] == 0xFF ) return -1;

    queueU8Init(&(path->qPath), path->pathBuffer, GRAPH_PATH_LENGTH);
    buildPath(start, end, mapCameFrom, path);
    return 0;
}
