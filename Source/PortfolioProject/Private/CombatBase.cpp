// Fill out your copyright notice in the Description page of Project Settings.


#include "PortfolioProject/Public/CombatBase.h"

struct FInputActionValue;
// Sets default values for this component's properties

UCombatBase::UCombatBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCombatBase::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCombatBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCombatBase::Attack(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Attack Called"));
}

void UCombatBase::Block(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Block Called"))

	//Pressing

	//Releasing
	
}

void UCombatBase::StartBlocking()
{
	bIsBlocking = true;
	TimeStartedBlock = FDateTime::Now();
	if(GetWorld()->GetTimerManager().IsTimerActive(TimerCountdownTimeForPerfectBlock))
	{
		//TODO check when pressing to often stuff.
		GetWorld()->GetTimerManager().ClearTimer(TimerCountdownTimeForPerfectBlock);		
	}
}

void UCombatBase::EndBlocking()
{
	//Check if block was full block
	(FDateTime::Now() - TimeStartedBlock).GetSeconds();
	float time = (FDateTime::Now() - TimeStartedBlock).GetTotalSeconds();
	UE_LOG(LogTemp, Warning, TEXT("%f"), time);
	if(time >= SecondsTillFullBlock)
	{
		
		
		
		bIsBlocking = false;
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("ShortBlock"));
	bIsBlocking = true;
	bBlockingTimeIsRunningOut = true;
	GetWorld()->GetTimerManager().SetTimer(TimerCountdownTimeForPerfectBlock,
			this,&UCombatBase::TimerMethod,TimeNotBlockingAfterNotPressingButtonAnymore, false); 

	
}

void UCombatBase::TimerMethod()
{
	bBlockingTimeIsRunningOut = false;
	bIsBlocking = false;
}

