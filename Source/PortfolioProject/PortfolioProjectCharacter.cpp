// Copyright Epic Games, Inc. All Rights Reserved.

#include "PortfolioProjectCharacter.h"
#include "EngineUtils.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "Public/CombatBase.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// APortfolioProjectCharacter

APortfolioProjectCharacter::APortfolioProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	
}

void APortfolioProjectCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(!AnimatorComponent)return;
	AnimatorComponent->Speed = GetCharacterMovement()->Velocity.Length();
	if(CurrentFollowedActor)LockOn();
	// UE_LOG(LogTemp, Warning, TEXT("Speed %f"), GetCharacterMovement()->Velocity.Length());
}


void APortfolioProjectCharacter::BeginPlay()
{
	// Call the base class
	
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	baseWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	AnimatorComponent  = Cast<UAnimatorComponent>(GetComponentByClass(UAnimatorComponent::StaticClass()));
	TArray<AActor*> list;
	UGameplayStatics::GetAllActorsWithTag(GetWorld() ,FName("Enemy"),list);
	if(!list.IsEmpty())OnlyEnemy = list[0];
	camera = this->FindComponentByClass<UCameraComponent>();
}

//////////////////////////////////////////////////////////////////////////
// Input

void APortfolioProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APortfolioProjectCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APortfolioProjectCharacter::Look);

		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &APortfolioProjectCharacter::Sprint);


		combatBase = this->FindComponentByClass<UCombatBase>();
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, combatBase, &UCombatBase::Attack);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Started, combatBase, &UCombatBase::StartBlocking);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, combatBase, &UCombatBase::EndBlocking);

		EnhancedInputComponent->BindAction(LookOnAction, ETriggerEvent::Triggered, this, &APortfolioProjectCharacter::ToggleLockOn);
		// }
		
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void APortfolioProjectCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		FRotator Rotation;
		// find out which way is forward
		if(CurrentFollowedActor)Rotation = GetActorRotation();
		else Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APortfolioProjectCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	if(CurrentFollowedActor)return;
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APortfolioProjectCharacter::Sprint(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("SprintButtonPressed"));
	if(bIsSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = baseWalkSpeed;
		bIsSprinting = false; 
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = baseWalkSpeed * sprintSpeedMultiplier;
		bIsSprinting = true;
	}
}

void APortfolioProjectCharacter::ToggleLockOn(const FInputActionValue& Value)
{
	if(CurrentFollowedActor)
	{
		CurrentFollowedActor = NULL;
		USpringArmComponent* SpringArm = camera->GetOwner()->FindComponentByClass<USpringArmComponent>();
		SpringArm->bUsePawnControlRotation = true;
	}
	else
	{
		float dotProduct = FVector::DotProduct(camera->GetForwardVector(),OnlyEnemy->GetActorLocation() - GetActorLocation());
		//Check if actor is looking in the right direction
		if(dotProduct > 0.2)CurrentFollowedActor = OnlyEnemy;
	}
}

void APortfolioProjectCharacter::LockOn()
{
	//Change rotation of character to face other actor
	FVector direction = CurrentFollowedActor->GetActorLocation() - GetActorLocation();
	FRotator cameraRotation = direction.Rotation();
	direction.Z = 0;
	FRotator newRotation = direction.Rotation();
	direction.Normalize();
	SetActorRotation(newRotation);
	// FVector newCamDistance = direction * -500 + GetActorLocation();
	USpringArmComponent* SpringArm = camera->GetOwner()->FindComponentByClass<USpringArmComponent>();
	SpringArm->bUsePawnControlRotation = false;

	camera->SetWorldRotation(cameraRotation);

}


