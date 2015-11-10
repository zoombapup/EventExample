// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/PlayerController.h"
#include "EventExampleProjectPlayerController.generated.h"


class USelectionManagerComponent;
class AAIController;

UCLASS()
class AEventExampleProjectPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AEventExampleProjectPlayerController();

	USelectionManagerComponent * GetSelectionManager()
	{
		return SelectionManager;
	}

	//UPROPERTY(BlueprintReadOnly,Category="Control")
	
	TArray<TWeakObjectPtr<AAIController>> ControlledActors;

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	// End PlayerController interface

	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();

	/** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);
	
	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation);

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

	void SelectableClicked(AActor *);
	void RegisterSelectableActorCallback(AActor *pActor);

protected:

	USelectionManagerComponent * SelectionManager = nullptr;
};


