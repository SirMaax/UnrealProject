// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat_AI.h"

#include "ImathMath.h"

// Sets default values for this component's properties
UCombat_AI::UCombat_AI()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called every frame
void UCombat_AI::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(CurrentState==Passive)ActPassiveState();
	else if(CurrentState==Agressiv)ActAgressiveState();
	else if(CurrentState==Defensive)ActDefensiveState();

	if(AmountOfTimesLeftInState==0)ChangeState();
}

// Called when the game starts
void UCombat_AI::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UCombat_AI::ActPassiveState()
{
}

void UCombat_AI::ActAgressiveState()
{
}

void UCombat_AI::ActDefensiveState()
{
}

void UCombat_AI::ChangeState()
{
	int chance = FMath::RandRange(0,100);
	float time;
	switch (CurrentState)
	{
	case Passive:
		if(chance <= 10)CurrentState= Passive;
		else if (chance <=55)CurrentState = Agressiv;
		else CurrentState = Defensive;
		// AmountOfTimesLeftInState += 1;
		time = FMath::RandRange(TimeTillStateChange/2,TimeTillStateChange);
		GetWorld()->GetTimerManager().SetTimer(TimerHandleChangeState,
			this,&UCombat_AI::ChangeState,time,false);
		break;
	case Agressiv:
		if(chance <= 20)CurrentState= Agressiv;
		else if (chance <=70)CurrentState = Defensive;
		else CurrentState = Passive;
		AmountOfTimesLeftInState += FMath::RandRange(0,4);
		break;
	case Defensive:
		if(chance <= 15)CurrentState= Defensive;
		else if (chance <=75)CurrentState = Agressiv;
		else CurrentState = Passive;
		AmountOfTimesLeftInState += FMath::RandRange(1,2);
		time = FMath::RandRange(TimeTillStateChange/2,TimeTillStateChange*2);
		GetWorld()->GetTimerManager().SetTimer(TimerHandleChangeState,
			this,&UCombat_AI::ChangeState,time,false);
		break;
	default: ;
	}
}


