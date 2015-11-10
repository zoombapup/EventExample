// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "EventExampleProject.h"
#include "EventExampleProjectPlayerController.h"
#include "EngineUtils.h"
#include "AI/Navigation/NavigationSystem.h"
#include "SelectionManagerComponent.h"
#include "AIController.h"
#include "SelectableComponent.h"


AEventExampleProjectPlayerController::AEventExampleProjectPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	bEnableMouseOverEvents = true;
	bEnableClickEvents = true;

	SelectionManager = CreateDefaultSubobject<USelectionManagerComponent>(TEXT("SelectionMgr"));

}

void AEventExampleProjectPlayerController::BeginPlay()
{
	if (GetWorld() != nullptr)
	{
		// iterate over all actors in the world
		for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// is this actor an AI controller? if so, we need to store a weak pointer to it
			AAIController * pController = Cast<AAIController>(*ActorItr);
			if (pController)
			{
				// this is an AIController, so store the weak pointer to it so we can use it later
				TWeakObjectPtr<AAIController> wptr = TWeakObjectPtr<AAIController>(pController);
				ControlledActors.Add(wptr);
			}
			else
			{
				RegisterSelectableActorCallback(*ActorItr);
			}

		}

		// bind an event on the world so that any new actors that are spawned we get a chance to see if they have USelectableComponent and bind those too!!
		FOnActorSpawned::FDelegate ActorSpawnedDelegate = FOnActorSpawned::FDelegate::CreateUObject(this,&AEventExampleProjectPlayerController::RegisterSelectableActorCallback);
		GetWorld()->AddOnActorSpawnedHandler(ActorSpawnedDelegate);
	}
}

void AEventExampleProjectPlayerController::RegisterSelectableActorCallback(AActor *pActor)
{
	if (!pActor) return;
	// not an AIController, so see if its got a selectable component! if it has, we need to bind an event.. 
	USelectableComponent * pSelectComp = Cast<USelectableComponent>(pActor->GetComponentByClass(USelectableComponent::StaticClass()));
	if (pSelectComp)
	{
		pSelectComp->OnClickedEvent().AddUObject(this, &AEventExampleProjectPlayerController::SelectableClicked);
	}

}


void AEventExampleProjectPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
}

void AEventExampleProjectPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &AEventExampleProjectPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &AEventExampleProjectPlayerController::OnSetDestinationReleased);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AEventExampleProjectPlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AEventExampleProjectPlayerController::MoveToTouchLocation);
}

void AEventExampleProjectPlayerController::MoveToMouseCursor()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(Hit.ImpactPoint);
	}
}

void AEventExampleProjectPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void AEventExampleProjectPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	
	if (ControlledActors.Num() > 0)
	{
		UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();

		for (auto ctr : ControlledActors)
		{
			if (!ctr.IsValid()) continue;
			AAIController * controlled = ctr.Get();
			APawn *pPawn = controlled->GetPawn();
			if (pPawn)
			{
				float const Distance = FVector::Dist(DestLocation, pPawn->GetActorLocation());
				// We need to issue move command only if far enough in order for walk animation to play correctly
				if (NavSys && (Distance > 120.0f))
				{
					NavSys->SimpleMoveToLocation(controlled, DestLocation);
				}
			}
		}
	}
}

void AEventExampleProjectPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	//bMoveToMouseCursor = true;
}

void AEventExampleProjectPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}

void AEventExampleProjectPlayerController::SelectableClicked(AActor *pClicked)
{
	if (pClicked)
	{
		SetNewMoveDestination(pClicked->GetActorLocation());
	}
	
}
