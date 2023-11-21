// Fill out your copyright notice in the Description page of Project Settings.


#include "HeatlhComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Microsoft/AllowMicrosoftPlatformTypes.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AnimatorComponent  = Cast<UAnimatorComponent>(GetOwner()->GetComponentByClass(UAnimatorComponent::StaticClass()));
	MaxHealth = Health;
	MaxBlock = Block;
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// ...
}

void UHealthComponent::UpdateHealth(float amount)
{
	Health-=amount;
	if(Health <= 0 )Die();
}

void UHealthComponent::GettingHit()
{
	if(!AnimatorComponent)return;
	AnimatorComponent->GotHit();
}

float UHealthComponent::GetHealthPercent()
{
	return Health/MaxHealth;
}

void UHealthComponent::Die()
{
	GetOwner()->Destroy();
	if(bIsMainCharacter)UGameplayStatics::OpenLevel(this,FName(GetWorld()->GetName()),false);
}

void UHealthComponent::UpdateUI()
{
	//Update UI here :D
}

float UHealthComponent::GetBlockPercent()
{
	return Block/MaxBlock;
}

void UHealthComponent::UpdateBlock(float amount)
{
	Block-=amount;
	if(Block <= 0)BreakPosture();
}

void UHealthComponent::BreakPosture()
{
		
}



