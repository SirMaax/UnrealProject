// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat_AI.h"

#include "ImathMath.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UCombat_AI::UCombat_AI()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Player = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
}

// Called every frame
void UCombat_AI::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(CurrentState==Passive)ActPassiveState();
	else if(CurrentState==Agressiv)ActAgressiveState(DeltaTime);
	else if(CurrentState==Defensive)ActDefensiveState(DeltaTime);

	if(AmountOfTimesLeftInState<=0)ChangeState();
}

// Called when the game starts
void UCombat_AI::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UCombat_AI::ActPassiveState()
{
	//Maybe move a bit to the right
}

void UCombat_AI::ActAgressiveState(float DeltaTime)
{
	if(Combatbase.GetIsAttacking())return;
	FVector direction =(Player->GetActorLocation() - GetOwner()->GetActorLocation());
	float distanceToPlayer = direction.Length();
	//Enemy not in attackrange -> move closer
	if(distanceToPlayer >= AttackRange)MoveInDirection(Player->GetActorLocation(),MovementSpeed * DeltaTime);
	//If in range choose one possible Attack to perform
	else
	{
		AmountOfTimesLeftInState -= 1;
		if(AmountOfTimesLeftInState<=0)return;
		Combatbase.SetAttackStatus(true);
		
		//Choose random attack, maybe lower chance for the same attack as the last attack
		int randomAttack = 0;
		Combatbase.Attack(randomAttack);
		
	}
}

void UCombat_AI::ActDefensiveState(float deltaTime)
{
	FVector direction =(Player->GetActorLocation() - GetOwner()->GetActorLocation());
	float distance = direction.Length();
	
	//Move further away from player
	if(distance <= AttackRange)MoveInDirection(-direction,MovementSpeed/2 * deltaTime);
}

void UCombat_AI::ChangeState()
{
	//State changes after certain time or after specified time runs out
	if(GetWorld()->GetTimerManager().IsTimerActive(TimerHandleChangeState))
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandleChangeState);
	}
	int chance = FMath::RandRange(0,100);
	switch (CurrentState)
	{
	case Passive:
		if(chance <= 10)CurrentState= Passive;
		else if (chance <=90)CurrentState = Agressiv;
		else CurrentState = Defensive;
		break;
	case Agressiv:
		if(chance <= 20)CurrentState= Agressiv;
		else if (chance <=70)CurrentState = Defensive;
		else CurrentState = Passive;
		
		break;
	case Defensive:
		Combatbase.SetBlockStatus(false);
		if(chance <= 15)CurrentState= Defensive;
		else if (chance <=85)CurrentState = Agressiv;
		else CurrentState = Passive;
		break;
	default: ;
	}
	ExecuteAfterStateSwitch();
}

void UCombat_AI::ExecuteAfterStateSwitch()
{
	float time;
	switch (CurrentState) {
		case Passive:
			time = FMath::RandRange(TimeInState[Passive]/2,TimeInState[Passive]);
			GetWorld()->GetTimerManager().SetTimer(TimerHandleChangeState,
				this,&UCombat_AI::ChangeState,time,false);
			break;
		case Agressiv:
			//Combat AmountOfTimesLeft translates to attacks. Although 3 attacks means 2 attacks etc.
			AmountOfTimesLeftInState += FMath::RandRange(3,5);
			GetWorld()->GetTimerManager().SetTimer(TimerHandleChangeState,
				this,&UCombat_AI::ChangeState,TimeInState[Agressiv],false);
			break;
		case Defensive:
			Combatbase.SetBlockStatus(true);
			GetWorld()->GetTimerManager().SetTimer(TimerHandleChangeState,
				this,&UCombat_AI::ChangeState,TimeInState[Defensive],false);
			break;
		
	default: ;
	}
}

void UCombat_AI::MoveInDirection(FVector direction, float force)
{
	FVector currentPosition = GetOwner()->GetActorLocation();
	GetOwner()->SetActorLocation(direction * force + currentPosition);
}


