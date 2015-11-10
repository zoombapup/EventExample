// Fill out your copyright notice in the Description page of Project Settings.

#include "EventExampleProject.h"
#include "AStar.h"
#include "SimpleGenerator.h"

AStar::AStar()
{
	pGen = NULL;
}

AStar::~AStar()
{
}

/* PSEUDOCODE
1	Create a node containing the goal state node_goal
2	Create a node containing the start state node_start
3	Put node_start on the open list
4	while the OPEN list is not empty
5	{
6	Get the node off the open list with the lowest f and call it node_current
7	if node_current is the same state as node_goal we have found the solution; break from the while loop
8	    Generate each state node_successor that can come after node_current
9	    for each node_successor of node_current
10	    {
11	        Set the cost of node_successor to be the cost of node_current plus the cost to get to node_successor from node_current
12	        find node_successor on the OPEN list
13	        if node_successor is on the OPEN list but the existing one is as good or better then discard this successor and continue
14	        if node_successor is on the CLOSED list but the existing one is as good or better then discard this successor and continue
15	        Remove occurrences of node_successor from OPEN and CLOSED
16	        Set the parent of node_successor to node_current
17	        Set h to be the estimated distance to node_goal (Using the heuristic function)
18	         Add node_successor to the OPEN list
19	    }
20	    Add node_current to the CLOSED list
21	}

*/

AStar::StepStatus AStar::Step()
{
	if (mOpenList.Num() == 0) return Err_NoPath;
	// Open List is sorted by F, so simply take the lowest F node (first on list) and expand it
	AStarNode *pNode = mOpenList[0];
	// remove the first node off the open list (lowest F)
	mOpenList.RemoveAt(0, 1, false);
	// add to closed list
	mClosedList.Add(pNode);

	// if we have reached our target (endx,endy) with the current node.. then work backwards along its parent chain until we get to the start
	if (pNode->x == endx && pNode->y == endy)
	{
		// found our path! create the path nodes list and return success!
		// step backwards through our nodes from pNode until we reach our start node.. adding each to our final path
		while (pNode != NULL && (pNode->x != startx || pNode->y != starty))
		{
			mFinalPath.Insert(pNode,0);
			// node walking potential disaster :)
			pNode = pNode->pParentNode;
			// PC: TODO find the ASSERT macro for UE4!!
			//assert(pNode != NULL);
		}
		return PathComplete;
	}

	TArray<AStarNode *> tempNodes;
	// haven't reached our end goal, so lets expand our nodes
	pNode->GetConnectedNodes(pGen, tempNodes);
	for(AStarNode * node : tempNodes)
	{

		// Make the "current" node the parent of this new node
		// Record the F, G, and H costs of the new node
		node->pParentNode = pNode;
		node->G = pNode->G + pNode->GetCost(node);
		node->H = node->GetHCost(endx, endy);
		node->F = node->G + node->H;

		// find node in closed list if we find a node and it has a lower F value, then discard the new node as unimportant
		//int32 closedindex = mClosedList.IndexOfByKey(node);
		int32 closedindex = mClosedList.IndexOfByPredicate([&](AStarNode * n) { return (*n == *node); });
		if (closedindex != INDEX_NONE)
		{
			if (mClosedList[closedindex]->F < node->F)
				continue;
		}


		// find node in open list if we find a node and it has a lower F value, then discard the new node as unimportant
		//int32 index = mOpenList.IndexOfByKey(node);
		int32 index = mOpenList.IndexOfByPredicate([&](AStarNode * n) { return (*n == *node); });

		if (index != INDEX_NONE)
		{
			//If it is on the open list already, check to see if this path to that square is better, using F cost as the measure.
			if (mOpenList[index]->F < node->F)
				continue;
		}

		// remove nodes from closed or open lists if they weren't "better" but still existed..
		if (closedindex != INDEX_NONE)
		{
			mClosedList.RemoveAt(closedindex, 1, false);
		}

		if (index != INDEX_NONE)
		{
			mOpenList.RemoveAt(index, 1, false);
		}

		// finally, add our node to the open list
		mOpenList.Add(node);
		
	}
	// puke memory BAD! :)
	tempNodes.Empty();

	mOpenList.Sort(AStar::AStarNodePredicate);

	
	
	return PathContinue;
}


/// takes the AASimpleGenerator pointer to access grid from, plus start and end X,Y pairs (start and end of the path)
void AStar::Setup(const ASimpleGenerator *pGenerator, uint32 sx, uint32 sy, uint32 ex, uint32 ey)
{
	pGen = pGenerator;
	if (pGen == NULL) return;

	if (sx >= (uint32)pGen->GridWidth || sy >= (uint32)pGen->GridHeight || ex >= (uint32)pGen->GridWidth || ey >= (uint32)pGen->GridHeight) return;
	startx = sx;
	starty = sy;
	endx = ex;
	endy = ey;

	AStarNode *pNode = new AStarNode();
	pNode->x = sx;
	pNode->y = sy;
	// start node has no G cost because it doesn't move :)
	pNode->F = pNode->H = pNode->GetHCost(ex, ey);

	
	// add our first node to the open list
	mOpenList.Add(pNode);

	mOpenList.Sort(AStar::AStarNodePredicate);

}

void AStar::LogArray(TArray<AStarNode *> & array)
{
	for (AStarNode * node : array)
	{
		UE_LOG(LogTemp, Warning, TEXT("Node X: %d Y: %d F: %f G: %f H: %f"), node->x, node->y, node->F, node->G, node->H);
	}
}

// A* Node method implementations.. 

// create a list of nodes to expand from this node (note: might include nodes already expanded)
void AStarNode::GetConnectedNodes(const ASimpleGenerator *pGen, TArray<AStarNode *> & Connections)
{
	AStarNode *pNode = NULL;
	// left of current
	if (x > 0)
	{
		// upper left
		if (y > 0)
		{
			pNode = new AStarNode();
			pNode->x = x - 1;
			pNode->y = y - 1;
			Connections.Add(pNode);
		}

		// middle left
		pNode = new AStarNode();
		pNode->x = x - 1;
		pNode->y = y;
		Connections.Add(pNode);

		// below left
		if (y < (uint32)pGen->GridHeight - 1)
		{
			pNode = new AStarNode();
			pNode->x = x - 1;
			pNode->y = y + 1;
			Connections.Add(pNode);
		}


	}

	// right
	if (x < (uint32)pGen->GridWidth - 1)
	{
		// upper right
		if (y > 0)
		{
			pNode = new AStarNode();
			pNode->x = x + 1;
			pNode->y = y - 1;
			Connections.Add(pNode);
		}

		// right
		pNode = new AStarNode();
		pNode->x = x + 1;
		pNode->y = y;
		Connections.Add(pNode);

		// below right
		if (y < (uint32)pGen->GridHeight - 1)
		{
			pNode = new AStarNode();
			pNode->x = x + 1;
			pNode->y = y + 1;
			Connections.Add(pNode);
		}

	}

	// above
	if (y > 0)
	{
		pNode = new AStarNode();
		pNode->x = x;
		pNode->y = y - 1;
		Connections.Add(pNode);
	}

	// below
	if (y < (uint32)pGen->GridHeight - 1)
	{
		pNode = new AStarNode();
		pNode->x = x;
		pNode->y = y + 1;
		Connections.Add(pNode);
	}


}

// cost to move to next node.. how many "steps" are we taking?
float AStarNode::GetCost(AStarNode * pNext)
{
	int32 myx = (int32)x;
	int32 myy = (int32)y;
	int32 nx = (int32)pNext->x;
	int32 ny = (int32)pNext->y;

	int32 absx = abs(nx - myx);
	int32 absy = abs(ny - myy);

	// are we taking a diagonal step? if so we need a higher cost then a cardinal direction
	if (absx > 0 && absy > 0) return 14;
	return 10;
}

AStarNode::AStarNode()
{
	x = 0; y = 0; F = 0.0f; G = 0.0f; H = 0.0f; pParentNode = NULL;
}

// heuristic cost of travel to final goal from our current position via manhatten distance
float AStarNode::GetHCost(uint32 targetx, uint32 targety)
{
	int32 myx = (int32)x;
	int32 myy = (int32)y;
	int32 nx = (int32)targetx;
	int32 ny = (int32)targety;

	// how many nodes do we step in the X?
	int32 absx = abs(nx - myx);
	// how many in the Y?
	int32 absy = abs(ny - myy);
	// sum them together!
	return 10.0f * (float)(absx + absy);

}
