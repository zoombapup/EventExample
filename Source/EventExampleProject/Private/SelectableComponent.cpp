// Fill out your copyright notice in the Description page of Project Settings.

#include "EventExampleProject.h"
#include "SelectableComponent.h"
#include "EventExampleProjectPlayerController.h"
#include "SelectionManagerComponent.h"


// Sets default values for this component's properties
USelectableComponent::USelectableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	//PrimaryComponentTick.bCanEverTick = true;

	// ...
	bHighlighted = false;
	bSelected = false;

	//mid = nullptr;

}


// Called when the game starts
void USelectableComponent::BeginPlay()
{
	Super::BeginPlay();
	// make sure our event isn't bound to anything..
	ClickedEvent.Clear();

	// note: Needs Mouse events enabled in player controller for this to work!
	GetOwner()->OnBeginCursorOver.AddDynamic(this, &USelectableComponent::OnBeginCursorOver);
	GetOwner()->OnEndCursorOver.AddDynamic(this, &USelectableComponent::OnEndCursorOver);
	GetOwner()->OnClicked.AddDynamic(this, &USelectableComponent::OnClicked);


	// get selection manager object from the player controller and register this component with it..
	APlayerController * pc = GetWorld()->GetFirstPlayerController();
	if (pc != nullptr)
	{
		AEventExampleProjectPlayerController * selpc = Cast<AEventExampleProjectPlayerController>(pc);
		if (selpc != nullptr)
		{
			USelectionManagerComponent * sm = selpc->GetSelectionManager();
			if (sm != nullptr)
				sm->RegisterSelectable(this);
		}
	}

	// ...
	/*
	UStaticMeshComponent * staticmesh = GetOwner()->FindComponentByClass<UStaticMeshComponent>();
	if (staticmesh != nullptr)
	{
		mid = staticmesh->CreateAndSetMaterialInstanceDynamic(0); //staticmesh->GetMaterial(0);
	}
	*/

	// static meshes
	TArray<UStaticMeshComponent *> staticmeshes;
	GetOwner()->GetComponents<UStaticMeshComponent>(staticmeshes);
	for (UStaticMeshComponent* mesh : staticmeshes)
	{
		mids.Add(mesh->CreateAndSetMaterialInstanceDynamic(0));
	}

	// skeletal meshes
	TArray<USkeletalMeshComponent *> skelmeshes;
	GetOwner()->GetComponents<USkeletalMeshComponent>(skelmeshes);
	for (USkeletalMeshComponent* mesh : skelmeshes)
	{
		mids.Add(mesh->CreateAndSetMaterialInstanceDynamic(0));
	}


	UpdateVisualState();
	
}

void USelectableComponent::UpdateVisualState()
{
	/*
	if (mid != nullptr)
	{
		mid->SetScalarParameterValue(TEXT("Highlight"), (float)bHighlighted);
	}
	*/
	for (UMaterialInstanceDynamic * mid : mids)
	{
		if (mid != nullptr)
		{
			mid->SetScalarParameterValue(TEXT("Highlight"), (float)bHighlighted);
		}
	}

	// static meshes
	TArray<UStaticMeshComponent*> staticmeshes;
	GetOwner()->GetComponents<UStaticMeshComponent>(staticmeshes);
	for (UStaticMeshComponent* mesh : staticmeshes)
	{
		mesh->SetRenderCustomDepth(bSelected);
	}

	// skeletal meshes
	TArray<USkeletalMeshComponent*> skelmeshes;
	GetOwner()->GetComponents<USkeletalMeshComponent>(skelmeshes);
	for (USkeletalMeshComponent* mesh : skelmeshes)
	{
		mesh->SetRenderCustomDepth(bSelected);
	}


}

void USelectableComponent::OnBeginCursorOver()
{
	// we are being cursored over, so highlight ourselves
	bHighlighted = true;
	bSelected = true;

	UpdateVisualState();
}

void USelectableComponent::OnEndCursorOver()
{
	// we are being cursored over, so highlight ourselves
	bHighlighted = false;
	bSelected = false;

	UpdateVisualState();
}

void USelectableComponent::OnClicked()
{
	if (ClickedEvent.IsBound())
		ClickedEvent.Broadcast(GetOwner());
}
/*
// Called every frame
void USelectableComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}
*/
