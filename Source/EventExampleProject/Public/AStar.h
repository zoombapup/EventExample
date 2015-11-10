// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*
	Implements a non-optimized "simple" A* (A Star) search algorithm, although this one is currently setup for grid use
	you should note that A* works on ANY set of nodes as long as the nodes have connectivity information

	In general, A* is a search space algorith, very similar in form to a flood fill. The flood fill version of this
	code is actually what is known as Dijkstra's algorithm and can sometimes be useful for finding nearest items etc in grid.
	A* uses a "heuristic" to guide it towards the end point from the start, by altering the heuristic you get different
	behavior. 

	See the GetCost method in AStarNode for example of heuristic (cost for us to travel from one node to another)

	A* generally follows:

	Summary of the A* Method ( from: http://www.policyalmanac.org/games/aStarTutorial.htm)

	F = G + H

	where

	G = the movement cost to move from the starting point A to a given square on the grid, following the path generated to get there.
	H = the estimated movement cost to move from that given square on the grid to the final destination, point B. 
	
	This is often referred to as the heuristic, which can be a bit confusing. The reason why it is called that is because it is a guess. 
	We really don’t know the actual distance until we find the path, because all sorts of things can be in the way (walls, water, etc.). 
	You are given one way to calculate H in this tutorial, but there are many others that you can find in other articles on the web.

	1) Add the starting square (or node) to the open list.

	2) Repeat the following:

	a) Look for the lowest F cost square on the open list. We refer to this as the current square.

	b) Switch it to the closed list.

	c) For each of the 8 squares adjacent to this current square …

	If it is not walkable or if it is on the closed list, ignore it. Otherwise do the following.

	If it isn’t on the open list, add it to the open list. Make the current square the parent of this square. Record the F, G, and H costs of the square.

	If it is on the open list already, check to see if this path to that square is better, using G cost as the measure. 
	A lower G cost means that this is a better path. If so, change the parent of the square to the current square, and recalculate the G and F scores of the square. 
	If you are keeping your open list sorted by F score, you may need to resort the list to account for the change.

	d) Stop when you:

	Add the target square to the closed list, in which case the path has been found (see note below), or
	Fail to find the target square, and the open list is empty. In this case, there is no path.
	3) Save the path. Working backwards from the target square, go from each square to its parent square until you reach the starting square. That is your path.

*/

class ASimpleGenerator;

struct AStarNode
{

public:
	AStarNode();
	uint32 x;
	uint32 y;
	float F;
	float G;
	float H;
	AStarNode * pParentNode;
	virtual void GetConnectedNodes(const ASimpleGenerator *pGen, TArray<AStarNode *> & Connections);
	// compute cost to travel from THIS node to the one passed
	float GetCost(AStarNode * pNext);
	// heuristic cost
	float GetHCost(uint32 targetx, uint32 targety);
	
	inline bool operator == (const AStarNode & other)
	{
		return x == other.x && y == other.y;
	}

	bool Matches(const AStarNode & other)
	{
		return x == other.x && y == other.y;
	}
};




/**
 * 
 */
class EVENTEXAMPLEPROJECT_API AStar
{
public:
	AStar();
	~AStar();

	const ASimpleGenerator *pGen = nullptr;

	// start and end points of the current path
	uint32 startx, starty, endx, endy;

	enum StepStatus
	{
		Err_NoPath,
		PathContinue,
		PathComplete
	};

	TArray<AStarNode *> mOpenList;
	TArray<AStarNode *> mClosedList;

	TArray<AStarNode *> mFinalPath;

	void Setup(const ASimpleGenerator *pGen,uint32 startx, uint32 starty, uint32 endx, uint32 endy);
	// steps the AStar algorithm one step forward (expanding more nodes towards the end)
	StepStatus Step();


	inline static bool AStarNodePredicate(const AStarNode& node1, const AStarNode& node2)
	{
		return (node1.F < node2.F);
	}



	void LogArray(TArray<AStarNode *> & array);
};
