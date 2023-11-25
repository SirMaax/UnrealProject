// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <mediaobj.h>

#include "CoreMinimal.h"
#include "AnimatorComponent.h"
#include "Components/ActorComponent.h"
#include "HeatlhComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTFOLIOPROJECT_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	bool IsPostureBroken;
	UStaticMesh* mesh;

# pragma region General
	UPROPERTY(EditAnywhere)
	bool bIsMainCharacter;
# pragma endregion 

# pragma region Health
private:
	UPROPERTY(EditAnywhere)
	float Health;
	
public:
	UPROPERTY(EditAnywhere);
	float MaxHealth;
	void UpdateHealth(float amount);
	void GettingHit();
	UFUNCTION(BlueprintCallable)
	float GetHealthPercent();
private:
	void Die();
	void UpdateUI();
# pragma endregion

# pragma region Block
private:
	UPROPERTY(EditAnywhere)
	float Block;
	UPROPERTY(EditAnywhere)
	float BlockRegen;
	float MaxBlock;
	bool bPostureIsBroken;
	void RestorePosture();
public:
	UFUNCTION(BlueprintCallable)
	float GetBlockPercent();
	void UpdateBlock(float amount);
	
public:
	void BreakPosture();
	void GettingHitFinished();
# pragma endregion
	
	UAnimatorComponent * AnimatorComponent;
};
