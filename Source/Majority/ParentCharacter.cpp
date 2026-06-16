// Fill out your copyright notice in the Description page of Project Settings.

#include "ParentCharacter.h"

// Sets default values
AParentCharacter::AParentCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	CurrentHealth = MaxHealth;
}

// Called when the game starts or when spawned
void AParentCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
}

// Called every frame
void AParentCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AParentCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AParentCharacter::ApplyDamage(float DamageAmount, EDamageSource DamageSource, AActor* DamageCauser)
{
	if (CurrentHealth <= 0.0f) return; // Уже мертв

	// Базовое вычитание здоровья
	CurrentHealth -= DamageAmount;
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);

	if (CurrentHealth <= 0.0f)
	{
		Die();
	}
}

void AParentCharacter::Die()
{
	// Базовая логика смерти. 
	// Можно переопределить в Blueprint или в наследниках (например, для врагов).
}