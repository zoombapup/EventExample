// Fill out your copyright notice in the Description page of Project Settings.

#include "EventExampleProject.h"
#include "SimpleGenerator.h"


ASimpleGenerator::ASimpleGenerator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GridWidth = 10;
	GridHeight = 10;
	LevelGrid.Empty();
	RandomizePositions();
	//RandomizeRoom(3,6,3,6);
	minroomwidth = 2;
	minroomheight = 2;
	maxroomheight = 2;
	maxroomwidth = 2;
	//roomwidth = 4;

	//roomheight = 4;
	//roomxpos = 4;
	//roomypos = 3;
	PrimaryActorTick.bCanEverTick = true;
}



void ASimpleGenerator::GenerateRandomWalkStep()
{
	if (ActorArray.Num() == 0) return; // dont have any actors we can spawn.. so early out

	if (GridWidth > 0 && GridHeight > 0)
	{
		{
			uint8 dir = FMath::RandHelper(4);

			switch (dir)
			{
				// north
			case 0:
				if (IsInBounds(xpos, ypos + 1) == true)
					ypos++;
				break;
				// east
			case 1:
				if (IsInBounds(xpos + 1, ypos) == true)
					xpos++;
				break;
				// south
			case 2:
				if (IsInBounds(xpos, ypos - 1) == true)
					ypos--;
				break;
				// west
			case 3:
				if (IsInBounds(xpos - 1, ypos) == true)
					xpos--;
				break;

			}

			if (GetCell(xpos, ypos) == NULL)
			{
				FVector loc = GetTransform().GetLocation();
				loc.X += (float)xpos * GridSpacing;
				loc.Y += (float)ypos * GridSpacing;
				FActorSpawnParameters params;
				params.bNoCollisionFail = true; // this is now deprecated!! aarrgh!
				params.bNoFail = true;
				AActor * pActor = GetWorld()->SpawnActor(ActorArray[0], &loc, NULL, params);
				LevelGrid[GridWidth * ypos + xpos] = pActor;
			}
		}

	}

}


// returns NULL or the contents of the cell as an actor
AActor * ASimpleGenerator::GetCell(uint32 x, uint32 y)
{
	if (LevelGrid.IsValidIndex(GridWidth * y + x) == true)
		return Cast<AActor>(LevelGrid[GridWidth * y + x].Get());
	return NULL;
};

// clears the grid of any cells (by looping over all cells and if there is a valid AActor *, calling destroy and nulling it out)
void ASimpleGenerator::ClearGrid()
{
	for (uint32 i = 0; i < (uint32)LevelGrid.Num(); i++)
	{
		if (LevelGrid[i].IsValid())
		{
			AActor * pActor = LevelGrid[i].Get();
			if (pActor != NULL && pActor->IsValidLowLevel())
			{
				LevelGrid[i].Reset();
				if (GetWorld()->IsPlayInEditor())
				{
					GetWorld()->EditorDestroyActor(pActor, true);
				}
				else
				{
					GetWorld()->DestroyActor(pActor, false, true);
					//pActor->Destroy();
					//pActor->K2_DestroyActor(); // todo: confirm this usage? 

				}
				pActor = NULL;
			}
		}
	}
	// tell the garbage collector to delete stuff we just destroyed (we no longer have a pointer to them theoretically)
	GetWorld()->ForceGarbageCollection(true);
};

// is the X,Y passed in bounds? 
bool ASimpleGenerator::IsInBounds(uint32 x, uint32 y)
{
	if (x > (uint32)GridWidth) return false;
	if (y > (uint32)GridHeight) return false;
	return LevelGrid.IsValidIndex(y * (uint32)GridWidth + x);
};


// randomize the position of the CA (within bounds of the width/height but not at edge!) + target end points for path
void ASimpleGenerator::RandomizePositions()
{
	xpos = (uint32)FMath::RandHelper(GridWidth);
	ypos = (uint32)FMath::RandHelper(GridHeight);
	targetxpos = (uint32)FMath::RandHelper(GridWidth);
	targetypos = (uint32)FMath::RandHelper(GridHeight);
};


bool ASimpleGenerator::CanPlaceRoom(uint32 xpos, uint32 ypos, uint32 width, uint32 height)
{
	// early out if we are crossing out of bounds
	if (!IsInBounds(xpos,ypos)) return false;
	if (!IsInBounds(xpos+width,ypos+height)) return false;

	// do a nested for loop...
	for (uint32 ytest = 0; ytest < height; ytest++)
	{
		for (uint32 xtest = 0; xtest < width; xtest++)
		{
			uint32 x = xpos + xtest;
			uint32 y = ypos + ytest;
			if (GetCell(x, y) != NULL) return false;
		}
	}
	
	return true;
};

void ASimpleGenerator::PlaceRoom(uint32 xpos, uint32 ypos, uint32 width, uint32 height)
{
	// do a nested for loop...
	for (uint32 ytest = 0; ytest < height; ytest++)
	{
		for (uint32 xtest = 0; xtest < width; xtest++)
		{
			uint32 x = xpos + xtest;
			uint32 y = ypos + ytest;

			if (GetCell(x, y) == NULL)
			{
				FVector loc = GetTransform().GetLocation();
				loc.X += (float)x * GridSpacing;
				loc.Y += (float)y * GridSpacing;
				FActorSpawnParameters params;
				params.bNoCollisionFail = true;
				params.bNoFail = true;
				AActor * pActor = GetWorld()->SpawnActor(ActorArray[0], &loc, NULL, params);
				LevelGrid[GridWidth * y + x] = pActor;
			}
		}
	}

};

void ASimpleGenerator::RandomizeRoom(uint32 minwidth, uint32 maxwidth, uint32 minheight, uint32 maxheight)
{
	roomwidth = (uint32)FMath::RandRange(minwidth, maxwidth);
	roomheight = (uint32)FMath::RandRange(minheight, maxheight);

	roomxpos = (uint32)FMath::RandHelper(GridWidth);
	roomypos = (uint32)FMath::RandHelper(GridHeight);
};

void ASimpleGenerator::GenerateRoom()
{
	if (ActorArray.Num() == 0) return; // dont have any actors we can spawn.. so early out

	RandomizeRoom((uint32)minroomwidth, (uint32)maxroomwidth, (uint32)minroomheight, (uint32)maxroomheight);
	if (CanPlaceRoom(roomxpos, roomypos, roomwidth, roomheight))
	{
		PlaceRoom(roomxpos, roomypos, roomwidth, roomheight);
	}
};

void ASimpleGenerator::ResetGenerator()
{
	if (GridWidth > 0 && GridHeight > 0)
	{
		// clear any existing grid elements
		ClearGrid();
		// make sure our grid is the appropriate dimensions
		LevelGrid.SetNum(GridWidth * GridHeight, true);
		// clear grid again, so that all smart pointers are nulled
		ClearGrid();
	}

	currentstep = 0;
	
}

void ASimpleGenerator::Step()
{
	if (ActorArray.Num() == 0) return; // dont have any actors we can spawn.. so early out
	if (currentstep >= (uint32)numsteps) return; // early out if we reach numsteps

	switch (Algorithm)
	{
	case PCG::PCG_RandomWalk:
		// first step? do some initialization
		if (currentstep == 0)
		{
			// randomize our start position (affects xpos,ypos and is guaranteed to be within the bounds)
			RandomizePositions();
		}
		GenerateRandomWalkStep();
		break;
	case PCG::PCG_Path:
		GeneratePathStep();
		break;
	case PCG::PCG_Rooms:
		GenerateRoom();
		break;
	}
	// increment step
	currentstep++;
}

void ASimpleGenerator::GeneratePathStep()
{
	RandomizePositions();
	
	xpos = minroomwidth;
	ypos = minroomheight;
	targetxpos = maxroomwidth;
	targetypos = maxroomheight;

	AStar pather;
	pather.Setup(this, xpos, ypos, targetxpos, targetypos);
	AStar::StepStatus status = AStar::PathContinue;
	while (status == AStar::PathContinue)
	{
		status = pather.Step();
		UE_LOG(LogTemp, Warning, TEXT("OPEN"));
		pather.LogArray(pather.mOpenList);
		UE_LOG(LogTemp, Warning, TEXT("CLOSED"));
		pather.LogArray(pather.mClosedList);
	}
	if (status == AStar::PathComplete)
	{
		for (AStarNode * node : pather.mFinalPath)
		{
			int32 x = (int32)node->x;
			int32 y = (int32)node->y;

			if (GetCell(x, y) == NULL)
			{
				FVector loc = GetTransform().GetLocation();
				loc.X += (float)x * GridSpacing;
				loc.Y += (float)y * GridSpacing;
				FActorSpawnParameters params;
				params.bNoCollisionFail = true;
				params.bNoFail = true;
				AActor * pActor = GetWorld()->SpawnActor(ActorArray[0], &loc, NULL, params);
				LevelGrid[GridWidth * y + x] = pActor;

#if WITH_EDITOR
				pActor->SetFolderPath("/Generated");
#endif
			}
		}
	}

}

