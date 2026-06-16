// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DataTypes.h" // Для использования EDamageSource
#include "ParentCharacter.generated.h"

UCLASS()
class MAJORITY_API AParentCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AParentCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// --- ЗДОРОВЬЕ ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// --- БАЗОВАЯ ЛОГИКА УРОНА ---
	// Виртуальная функция, чтобы наследники могли переопределить логику получения урона
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void ApplyDamage(float DamageAmount, EDamageSource DamageSource, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	float GetCurrentHealth() const { return CurrentHealth; }

protected:
	// Базовая функция смерти
	virtual void Die();
};