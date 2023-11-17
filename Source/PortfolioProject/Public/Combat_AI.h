// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatBase.h"
#include "Components/ActorComponent.h"
#include "Combat_AI.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTFOLIOPROJECT_API UCombat_AI : public UActorComponent
{
	GENERATED_BODY()


	
public:	
	// Sets default values for this component's properties
	UCombat_AI();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere)
	float AttackRange;
	UPROPERTY(EditAnywhere)
	float MovementSpeed;
	UPROPERTY(EditAnywhere)
	float TimeInState[];
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	enum AI_State
	{
		Passive,
		Agressiv,
		Defensive,
	};
	AI_State CurrentState;
	int AmountOfTimesLeftInState;

	
	void ActPassiveState();
	void ActAgressiveState(float DeltaTime);
	void ActDefensiveState(float DeltaTime);
	void ChangeState();
	void MoveInDirection(FVector direction, float force);
	void ExecuteAfterStateSwitch();
	FTimerHandle TimerHandleChangeState;
	float TimeTillStateChange;
	
	UCombatBase Combatbase;

	AActor* Player;
	
	
	
};
