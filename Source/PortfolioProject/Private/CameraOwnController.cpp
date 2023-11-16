// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraOwnController.h"

// Sets default values for this component's properties
UCameraOwnController::UCameraOwnController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCameraOwnController::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCameraOwnController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void LockAtPoint(FVector lookAt)
{
	//if Angle to object isnt to big look at it
}


