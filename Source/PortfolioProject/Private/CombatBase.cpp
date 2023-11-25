// Fill out your copyright notice in the Description page of Project Settings.


#include "PortfolioProject/Public/CombatBase.h"
#include "NiagaraFunctionLibrary.h"

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
		WeaponMesh = Cast<UStaticMeshComponent>(list[0]);
		WeaponMesh->SetGenerateOverlapEvents(true);
		WeaponMesh->SetActive(false);
		// UE_LOG(LogTemp, Warning, TEXT("Has WeaponMesh"));
	}
	AnimatorComponent  = Cast<UAnimatorComponent>(GetOwner()->GetComponentByClass(UAnimatorComponent::StaticClass()));
	health = Cast<UHealthComponent>(GetOwner()->GetComponentByClass(UHealthComponent::StaticClass()));


}

// Called every frame
void UCombatBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(!AnimatorComponent)return;
	AnimatorComponent->bIsBlocking = (bIsBlocking || bBlockingTimeIsRunningOut) && !bIsAttacking;
	AnimatorComponent->bIsAttacking = bIsAttacking;

}

void UCombatBase::Attack(const FInputActionValue& Value)
{
	if(bIsAttacking || !WeaponMesh)return;
	//Active Mesh of Weapon
	WeaponMesh->SetActive(true);
	AnimatorComponent->bIsAttacking = true;
	bIsAttacking = true;

	GetWorld()->GetTimerManager().SetTimer
		(TimerAttackDuration,this,&UCombatBase::AttackOver,AttackTimes[0],false);
}

void UCombatBase::Attack(int AttackIndex = 0)
{
	if(bIsAttacking || !WeaponMesh)return;
	if(health->IsPostureBroken)return;
	
	bIsAttacking = true;
	AnimatorComponent->bIsAttacking = true;
	GetWorld()->GetTimerManager().SetTimer
		(TimerAttackDuration,this,&UCombatBase::AttackOver,AttackTimes[AttackIndex],false);
}

/**
 * @brief Is called when the attack animation is over and the Weapon should not collide with objects anymore
 */
void UCombatBase::AttackOver()
{
	if(!WeaponMesh || !AnimatorComponent)return;
	bIsAttacking = false;
}


void UCombatBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if(!OtherComp->ComponentHasTag("Weapon") || OtherActor == GetOwner())return;
	if(bIFramesActive)return;
	UCombatBase * otherCombat = Cast<UCombatBase>(OtherActor->GetComponentByClass(UCombatBase::StaticClass()));
	if(!otherCombat->bIsAttacking)return;
	UHealthComponent * otherHealth = Cast<UHealthComponent>(OtherActor->GetComponentByClass(UHealthComponent::StaticClass()));
	GettingAttacked(otherCombat, otherHealth);
	

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
	TimeStartedBlock = FDateTime::Now();
}

void UCombatBase::EndBlocking()
{
	UE_LOG(LogTemp, Warning , TEXT("ReleasedBlockButton"));
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

void UCombatBase::GettingAttacked(UCombatBase * otherCombat, UHealthComponent * otherHealth)
{
	if(bBlockingTimeIsRunningOut)
	{
		UE_LOG(LogTemp, Warning, TEXT("Fullblock"));
		otherHealth->UpdateBlock(-10);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),
			particleSystem,PositionSpark+GetOwner()->GetActorLocation());
	} 
	else if(bIsBlocking)
	{
		//PartTimeDmg
		UE_LOG(LogTemp, Warning, TEXT("Blocked"));
		health->UpdateBlock(-5);
		health->UpdateHealth(-1);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),
			particleSystem,PositionSpark+GetOwner()->GetActorLocation());
	}
	else
	{
		//Not blocking full dmg
		UE_LOG(LogTemp, Warning, TEXT("Got fully hit"));
		health->UpdateHealth(-10);
		if(!bIsAttacking)health->GettingHit();
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),
			bloodParticle,PositionBlood+GetOwner()->GetActorLocation());
	}
	GetWorld()->GetTimerManager().SetTimer(
			InvincibleTimeCountdown,this, &UCombatBase::CanBeHitAgain, InvinibleTimeAfterHit,false);
	bIFramesActive = true;
}

void UCombatBase::TimerMethod()
{
	bBlockingTimeIsRunningOut = false;
	bIsBlocking = false;
}

void UCombatBase::CanBeHitAgain()
{
	bIFramesActive = false;
}

void UCombatBase::SetBlockStatus(bool blocking)
{
	bIsBlocking = blocking;
}

void UCombatBase::SetAttackStatus(bool attack)
{
	bIsAttacking = attack;
}

bool UCombatBase::GetIsAttacking()
{
	return bIsAttacking;
}

