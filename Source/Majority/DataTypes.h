// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "DataTypes.generated.h"

class MAJORITY_API DataTypes
{
public:
	DataTypes();
	~DataTypes();
};

UENUM(BlueprintType)
enum class ETreasureRank : uint8
{
    Common,
    Uncommon,
    Rare,
    Epic,
    Legendary
};

// Конфиг лимитов для каждого качества
USTRUCT(BlueprintType)
struct FTreasureRankLimits
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Config")
    ETreasureRank Rank = ETreasureRank::Common;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Config", meta = (ClampMin = "0"))
    int32 MinCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Config", meta = (ClampMin = "0"))
    int32 MaxCount = 10;
};
