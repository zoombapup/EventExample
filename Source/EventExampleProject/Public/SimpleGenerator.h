// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "AStar.h"
#include "SimpleGenerator.generated.h"


UENUM(BlueprintType)
namespace PCG
{
	enum PCGType
	{
		PCG_RandomWalk		UMETA(DisplayName = "Random Walk"),
		PCG_Rooms			UMETA(DisplayName = "Rooms"),
		PCG_Path			UMETA(DisplayName = "Path"),
		PCG_Max				UMETA(Hidden),
	};
}


/**
 * 
 */
UCLASS()
class EVENTEXAMPLEPROJECT_API ASimpleGenerator : public AActor
{
	GENERATED_BODY()

public:

	ASimpleGenerator(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
		int32 GridWidth;
	//uint32 GridWidth;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Grid)
		int32 GridHeight;

	// how big is a grid cell?
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Grid)
		float GridSpacing;

	// reset the generator
	UFUNCTION(BlueprintCallable, Category = Gen)
		void ResetGenerator();

	// steps the PCG algorithm
	UFUNCTION(BlueprintCallable, Category = Gen)
		void Step();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gen)
		TEnumAsByte<PCG::PCGType> Algorithm;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Actors)
		TArray< TSubclassOf<class AActor> > ActorArray;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Gen)
		int32 numsteps;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Gen)
		int32 minroomwidth;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Gen)
		int32 minroomheight;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Gen)
		int32 maxroomwidth;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Gen)
		int32 maxroomheight;


	// current CA position
	uint32 xpos;
	uint32 ypos;

	// target end point for path!
	uint32 targetxpos;
	uint32 targetypos;

	uint32 roomwidth;
	uint32 roomheight;
	uint32 roomxpos;
	uint32 roomypos;

	// which "step" are we on.. 
	uint32 currentstep;

	// the actual 2D grid, in this case stored as a 1D array
	TArray< TWeakObjectPtr<AActor> > LevelGrid;

	// returns NULL or the contents of the cell as an actor
	AActor * GetCell(uint32 x, uint32 y);

	// clears the grid of any cells (by looping over all cells and if there is a valid AActor *, calling destroy and nulling it out)
	void ClearGrid();

	// is the X,Y passed in bounds? 
	bool IsInBounds(uint32 x, uint32 y);

	// randomize the position of the CA (within bounds of the width/height but not at edge!)
	void RandomizePositions();

	bool CanPlaceRoom(uint32 xpos, uint32 ypos, uint32 width, uint32 height);
	void PlaceRoom(uint32 xpos, uint32 ypos, uint32 width, uint32 height);

	void RandomizeRoom(uint32 minwidth, uint32 maxwidth, uint32 minheight, uint32 maxheight);


	void GenerateRoom();

	void GenerateRandomWalkStep();

	void GeneratePathStep();

};
