// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DataTypes.generated.h"

class MAJORITY_API DataTypes
{
public:
	DataTypes();
	~DataTypes();
};

// ---------------------------------- ENUMERATORS ----------------------------------
UENUM(BlueprintType)
enum class ETreasureRank : uint8
{
    Common,
    Uncommon,
    Rare,
    Epic,
    Legendary
};

UENUM(BlueprintType)
enum class EPlayerStatus : uint8
{
    Idle            UMETA(DisplayName = "Character do nothing"),
    TreasureHunting UMETA(DisplayName = "Character is looking for a treasure"),
    Fighting        UMETA(DisplayName = "Character is fighting")
};


// ---------------------------------- STRUCTURES ----------------------------------
// Конфиг лимитов для каждого качества сокровища
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


USTRUCT(BlueprintType)
struct FItem_st : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Visual_name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Price;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ETreasureRank Rank;
};