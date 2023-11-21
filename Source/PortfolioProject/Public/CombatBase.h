// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AnimatorComponent.h"
#include "EnhancedInputComponent.h"
#include "HeatlhComponent.h"
#include "Components/ActorComponent.h"
#include "CombatBase.generated.h"


struct FInputActionValue;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTFOLIOPROJECT_API UCombatBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatBase();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	UMeshComponent* HitboxMesh;
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

# pragma region Block
protected:
	bool bIsBlocking;
	bool bBlockingTimeIsRunningOut;
	bool bCanBlock;
private:
	FTimerHandle TimerCountdownTimeForPerfectBlock;
	FDateTime TimeStartedBlock;
	
	UPROPERTY(EditAnywhere, Category="Blocking")
	float SecondsTillFullBlock;
	UPROPERTY(EditAnywhere, Category="Blocking")
	float TimeNotBlockingAfterNotPressingButtonAnymore;
	// int HowOftenWasBlockUsedRecently;
	void TimerMethod();
	FTimerHandle TimerAttackDuration;
	UStaticMeshComponent* WeaponMesh;
	bool bIsAttacking;
	
public:
	void StartBlocking();
	void EndBlocking();
	void GettingAttacked();
	void SetBlockStatus(bool block);
	void SetAttackStatus(bool attack);
	bool GetIsAttacking();
# pragma endregion

# pragma region Attack
	void Attack(const FInputActionValue& Value);
	void Attack(int AttackIndex);
	void AttackOver();

	UPROPERTY(EditAnywhere)
	float AttackTimes[3];
# pragma endregion 


# pragma region Behavior
	UPROPERTY(EditAnywhere, Category="Behavior")
	float InvinibleTimeAfterHit;
	FTimerHandle InvincibleTimeCountdown;
	bool bIFramesActive;
	void CanBeHitAgain();
# pragma endregion 

# pragma region References
private:
	UHealthComponent * health;
	UAnimatorComponent * AnimatorComponent;
	// UPROPERTY(EditAnywhere, Category="Behavior")
	# pragma endregion
	
	
};
