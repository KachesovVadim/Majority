// Copyright Epic Games, Inc. All Rights Reserved.

#include "MajorityCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "TreasureSearchingSkill_comp.h"
#include "FightingSkill_component.h"
#include "MetalDetector.h" 

//////////////////////////////////////////////////////////////////////////
// AMajorityCharacter

AMajorityCharacter::AMajorityCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	TreasureSearchingSkill_component = CreateDefaultSubobject<UTreasureSearchingSkill_comp>(TEXT("Treasure searching component"));
	FightingSkill_component = CreateDefaultSubobject<UFightingSkill_component>(TEXT("Component, related to everything about fighting"));
}

void AMajorityCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	InitMetalDetector();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMajorityCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMajorityCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMajorityCharacter::Look);
	}
}

void AMajorityCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMajorityCharacter::Look(const FInputActionValue& Value)
{
	return;
}

void AMajorityCharacter::InitMetalDetector()
{
	if (MetalDetectorClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		MetalDetectorActor = GetWorld()->SpawnActor<AMetalDetector>(
			MetalDetectorClass,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			SpawnParams
		);

		if (MetalDetectorActor)
		{
			FAttachmentTransformRules AttachmentRules(
				EAttachmentRule::SnapToTarget,
				EAttachmentRule::SnapToTarget,
				EAttachmentRule::KeepWorld,
				false
			);

			MetalDetectorActor->AttachToComponent(GetMesh(), AttachmentRules, TEXT("MetalDetector_socket"));
		}
	}
}

// --- ЛОГИКА ПЕРЕХВАТА УРОНА ---
void AMajorityCharacter::ApplyDamage(float DamageAmount, EDamageSource DamageSource, AActor* DamageCauser)
{
	// Проверяем блок и уворот через компонент боя
	if (FightingSkill_component)
	{
		// Если персонаж уворачивается, он неуязвим (урон 0)
		if (FightingSkill_component->IsDodging())
		{
			return;
		}

		// Если персонаж держит блок, урон не проходит, но срабатывает счетчик ударов по блоку
		if (FightingSkill_component->IsBlocking())
		{
			FightingSkill_component->OnBlockedHit();
			return;
		}
	}

	// Если блок/уворот не сработали (или их нет), вызываем базовую логику урона из ParentCharacter
	Super::ApplyDamage(DamageAmount, DamageSource, DamageCauser);
}