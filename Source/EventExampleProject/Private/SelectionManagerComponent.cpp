// Fill out your copyright notice in the Description page of Project Settings.

#include "EventExampleProject.h"
#include "SelectionManagerComponent.h"




USelectionManagerComponent::USelectionManagerComponent()
{
	Selectables.Reset();
}

void USelectionManagerComponent::RegisterSelectable(USelectableComponent * sel)
{
	Selectables.AddUnique(sel);
}
