// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataTypes.h"
#include "TreasureSpawner.generated.h"

UCLASS()
class MAJORITY_API ATreasureSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATreasureSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    // --- Настройки спавна (доступны в Details) ---
    UPROPERTY(EditAnywhere, Category = "Spawn Config", meta = (ClampMin = "1"))
    int32 TreasuresCount = 10;

    UPROPERTY(EditAnywhere, Category = "Spawn Config", meta = (ClampMin = "0"))
    float MinDistanceBetweenTreasures = 100.0f;

    UPROPERTY(EditAnywhere, Category = "Spawn Config", meta = (ClampMin = "0"))
    float SpawnRadius = 500.0f; // Диапазон +-500 от спавнера

    UPROPERTY(EditAnywhere, Category = "Spawn Config", meta = (ClampMin = "10"))
    float SpawnCheckRadius = 40.0f; // Радиус проверки коллизии перед спавном

    UPROPERTY(EditAnywhere, Category = "Spawn Config")
    TArray<FTreasureRankLimits> RankLimits;

    UPROPERTY(EditAnywhere, Category = "Spawn Config")
    TSubclassOf<class ATreasure> TreasureClass;

    // --- Внутренние методы ---
    TArray<ETreasureRank> AllocateRanks();
    FVector FindValidSpawnLocation(const TArray<FVector>& ExistingLocations, int32 MaxAttempts = 50);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SpawnTreasure();
};