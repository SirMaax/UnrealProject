// Fill out your copyright notice in the Description page of Project Settings.


#include "Test.h"

// Sets default values for this component's properties
UTest::UTest()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTest::BeginPlay()
{
	Super::BeginPlay();

	TArray<UActorComponent*> list;
	list = GetOwner()->GetComponentsByTag(UMeshComponent::StaticClass(),FName("Weapon"));
    	if(!list.IsEmpty())
    	{
    		WeaponMesh = Cast<UStaticMeshComponent>(list[0]);
    		WeaponMesh->OnComponentBeginOverlap.AddDynamic(this,&UTest::OnOverlapBegin);
    		WeaponMesh->SetGenerateOverlapEvents(true);
	}
}


// Called every frame
void UTest::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}



void UTest::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("collided with ur testmom"));

}

