// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "SelectableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EVENTEXAMPLEPROJECT_API USelectableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USelectableComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	void UpdateVisualState();
	UFUNCTION()
	void OnBeginCursorOver();
	UFUNCTION()
	void OnEndCursorOver();
	UFUNCTION()
	void OnClicked();

	// Called every frame
	//virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	// material instance dynamics used for the selection material change
	TArray<UMaterialInstanceDynamic *> mids;


	UPROPERTY(BlueprintReadOnly, Category = "Selection")
	bool bHighlighted = false;
	UPROPERTY(BlueprintReadOnly, Category = "Selection")
	bool bSelected = false;

	DECLARE_EVENT_OneParam(USelectableComponent,FClickedEvent,AActor *)

	//UPROPERTY(BlueprintAssignable, Category = "Selection")
	FClickedEvent & OnClickedEvent()
	{
		return ClickedEvent;
	};

private:
	FClickedEvent ClickedEvent;
};
