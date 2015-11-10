// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "SelectionManagerComponent.generated.h"


class USelectableComponent;

/**
 * 
 */
UCLASS()
class EVENTEXAMPLEPROJECT_API USelectionManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	USelectionManagerComponent();

	void RegisterSelectable(USelectableComponent * sel);

	TArray<USelectableComponent *> Selectables;
	
};
