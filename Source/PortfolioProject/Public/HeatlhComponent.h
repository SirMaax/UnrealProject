// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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

# pragma region Health
private:
	UPROPERTY(EditAnywhere)
	float Health;
	float MaxHealth;
public:
	void UpdateHealth(float amount);
	void GettingHit();
private:
	void Die();
	void UpdateUI();
# pragma endregion

# pragma region Block
private:
	UPROPERTY(EditAnywhere)
	float Block;
	float MaxBlock;

public:
	void UpdateBlock(float amount);
	
private:
	void BreakPosture();
# pragma endregion

	UAnimatorComponent * AnimatorComponent;
};
