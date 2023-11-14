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

	TArray<UActorComponent*> list;
	list = GetOwner()->GetComponentsByTag(UMeshComponent::StaticClass(),FName("HitBox"));
	if(!list.IsEmpty())
	{
		HitboxMesh = Cast<UMeshComponent>(list[0]);
		HitboxMesh->OnComponentBeginOverlap.AddDynamic(this,&UCombatBase::OnOverlapBegin);
	}
	list = GetOwner()->GetComponentsByTag(UMeshComponent::StaticClass(),FName("Weapon"));
	if(!list.IsEmpty())
	{
		WeaponMesh = Cast<UMeshComponent>(list[0]);
		WeaponMesh->OnComponentBeginOverlap.AddDynamic(this,&UCombatBase::OnOverlapBegin);
		WeaponMesh->SetActive(false);
	}
	AnimatorComponent  = Cast<UAnimatorComponent>(GetOwner()->GetComponentByClass(UAnimatorComponent::StaticClass()));
	health = Cast<UHealthComponent>(GetOwner()->GetComponentByClass(UHealthComponent::StaticClass()));
	
}

// Called every frame
void UCombatBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AnimatorComponent->bIsBlocking = (bIsBlocking || bBlockingTimeIsRunningOut) && !bIsAttacking;
	AnimatorComponent->bIsAttacking = bIsAttacking;
	// ...
	// AnimatorComponent->bIsBlocking = bIsBlocking || bBlockingTimeIsRunningOut;
	UE_LOG(LogTemp, Warning, TEXT("Status blocking:%c") ,bIsBlocking || bBlockingTimeIsRunningOut);

}

void UCombatBase::Attack(const FInputActionValue& Value)
{
	if(bIsAttacking)return;
	// if(!WeaponMesh)return
	UE_LOG(LogTemp, Warning, TEXT("Attack Called"));
	//Active Mesh of Weapon
	// WeaponMesh->SetActive(true);
	AnimatorComponent->bIsAttacking = true;
	//TODO the rest
}

/**
 * @brief Is called when the attack animation is over and the Weapon should not collide with objects anymore
 */
void UCombatBase::AttackOver()
{
	if(!WeaponMesh)return;
	//Alternatively this can be done with a timer. Where it is known how long each attack takes.
	WeaponMesh->SetActive(false);
	bIsAttacking = false;
	AnimatorComponent->bIsAttacking = false;
	UE_LOG(LogTemp, Warning, TEXT("Attack Over"));

}

void UCombatBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if(!OtherComp->ComponentHasTag("Weapon"))return;
	GettingAttacked();
	UE_LOG(LogTemp, Warning, TEXT("collided with ur mom"));

}

void UCombatBase::StartBlocking()
{
	if(!health)return;
	if(health->IsPostureBroken)return;
	if(GetWorld()->GetTimerManager().IsTimerActive(TimerCountdownTimeForPerfectBlock))
	{
		bBlockingTimeIsRunningOut = false;
		GetWorld()->GetTimerManager().ClearTimer(TimerCountdownTimeForPerfectBlock);		
	}
	bIsBlocking = true;
	UE_LOG(LogTemp, Warning, TEXT("blocking"));
	TimeStartedBlock = FDateTime::Now();
}

void UCombatBase::EndBlocking()
{
	//Check if block was full block
	float time = (FDateTime::Now() - TimeStartedBlock).GetTotalSeconds();
	// UE_LOG(LogTemp, Warning, TEXT("%f"), time);
	if(time >= SecondsTillFullBlock)
	{
		bIsBlocking = false;
		return;
	}
	// UE_LOG(LogTemp, Warning, TEXT("ShortBlock"));
	bIsBlocking = true;
	bBlockingTimeIsRunningOut = true;
	GetWorld()->GetTimerManager().SetTimer(TimerCountdownTimeForPerfectBlock,
			this,&UCombatBase::TimerMethod,TimeNotBlockingAfterNotPressingButtonAnymore, false); 
}

void UCombatBase::GettingAttacked()
{
	if(bBlockingTimeIsRunningOut)
	{
		//Perfect Parry
		//Particles
	}
	else if(bIsBlocking)
	{
		//PartTimeDmg
		health->UpdateBlock(1);
		
	}
	else
	{
		//Not blocking full dmg
		health->UpdateHealth(1);
		health->GettingHit();
		//Get pushed back
	}
}

void UCombatBase::TimerMethod()
{
	bBlockingTimeIsRunningOut = false;
	bIsBlocking = false;

}
