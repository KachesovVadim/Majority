// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataTypes.h"
#include "Sound/SoundCue.h"
#include "TreasureSearchingSkill_comp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAJORITY_API UTreasureSearchingSkill_comp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTreasureSearchingSkill_comp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	class AMajorityCharacter* MyCharacter;

	// Delay after you digged the treasure before starting searching the new one
	FTimerHandle DelayBetweenTreasures;

	// --- ЗВУКОВЫЕ ЭФФЕКТЫ (Настраиваются в Blueprint) ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Treasure")
	USoundCue* SoundCuePoor;   // Для Common

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Treasure")
	USoundCue* SoundCueGood;   // Для Uncommon, Rare, Epic

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Treasure")
	USoundCue* SoundCueGreat;  // Для Legendary

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void ActivateSkill();
	
	UFUNCTION(BlueprintCallable)
	void DeactivateSkill();

	class ATreasure* FindRandomTreasure();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Treasure")
	class ATreasure* CurrentTreasure;

	UFUNCTION(BlueprintPure)
	bool IsPlayerStayingNearTreasure();

	UFUNCTION(BlueprintCallable)
	void DigTreasure();

private:
	// Ссылки на наши таблицы данных
	UPROPERTY()
	UDataTable* ItemsOnLevels_dt;
	UPROPERTY()
	UDataTable* All_items_dt;
};
