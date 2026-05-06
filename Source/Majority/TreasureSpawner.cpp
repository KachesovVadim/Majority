// Fill out your copyright notice in the Description page of Project Settings.


#include "TreasureSpawner.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "Treasure.h" 
#include "Engine/Engine.h"

// Sets default values
ATreasureSpawner::ATreasureSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATreasureSpawner::BeginPlay()
{
	Super::BeginPlay();
    SpawnTreasure();
	
}

// Called every frame
void ATreasureSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATreasureSpawner::SpawnTreasure()
{
    if (!TreasureClass)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("ATreasureSpawner: TreasureClass is not set"));
        return;
    }
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("ATreasureSpawner: SPAWN STARTED"));

    // 1. Распределяем качества согласно лимитам
    TArray<ETreasureRank> RanksToSpawn = AllocateRanks();

    // 2. Храним уже использованные позиции для проверки дистанции
    TArray<FVector> SpawnedLocations;
    SpawnedLocations.Reserve(RanksToSpawn.Num());

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetInstigator();
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // 3. Спавним по одному
    for (ETreasureRank Rank : RanksToSpawn)
    {
        FVector SpawnLoc = FindValidSpawnLocation(SpawnedLocations);
        
        if (SpawnLoc == GetActorLocation()) { continue; }

        SpawnedLocations.Add(SpawnLoc);
        
        FRotator MyRotation = FRotator(0.f, 90.f, 0.f);

        // Create the transform
        FTransform NewTransform = FTransform(MyRotation, SpawnLoc, FVector(1.f, 1.f, 1.f));
        
        //ATreasure* NewTreasure = GetWorld()->SpawnActor<ATreasure>(TreasureClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);
        ATreasure* NewTreasure = GetWorld()->SpawnActorDeferred<ATreasure>(TreasureClass, NewTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
        if (NewTreasure)
        {
            // Передаём ранг заспавненному сокровищу. 
            // Замените на ваш метод/свойство: NewTreasure->CurrentRank = Rank;
            NewTreasure->TreasureRank = Rank;

            UE_LOG(LogTemp, Log, TEXT("[TreasureSpawner] Treasure spawned: %s | Rank: %d"), *SpawnLoc.ToString(), (int32)Rank);
        }
    }
}

TArray<ETreasureRank> ATreasureSpawner::AllocateRanks()
{
    TArray<ETreasureRank> Result;
    Result.Reserve(TreasuresCount);

    // 1. Применяем минимальные количества
    for (const FTreasureRankLimits& Limit : RankLimits)
    {
        for (int32 i = 0; i < Limit.MinCount; ++i)
        {
            Result.Add(Limit.Rank);
        }
    }

    // 2. Гарантируем хотя бы 1 Legendary, если он ещё не добавлен
    if (!Result.Contains(ETreasureRank::Legendary) && TreasuresCount > 0)
    {
        Result.Add(ETreasureRank::Legendary);
    }

    // Защита от некорректной настройки (Min > Total)
    if (Result.Num() > TreasuresCount)
    {
        UE_LOG(LogTemp, Warning, TEXT("Sum of MinCounts exceeds TreasuresCount. Trimming result."));
        Result.SetNum(TreasuresCount);
    }

    // 3. Заполняем остаток до TreasuresCount, соблюдая MaxCount
    int32 Remaining = TreasuresCount - Result.Num();
    TMap<ETreasureRank, int32> CurrentCounts;
    for (ETreasureRank R : Result)
    {
        CurrentCounts.FindOrAdd(R)++;
    }

    // Собираем ранги, которые ещё могут быть добавлены
    TArray<FTreasureRankLimits> AvailableRanks = RankLimits;
    for (int32 i = AvailableRanks.Num() - 1; i >= 0; --i)
    {
        if (CurrentCounts.FindRef(AvailableRanks[i].Rank) >= AvailableRanks[i].MaxCount)
        {
            AvailableRanks.RemoveAt(i);
        }
    }

    while (Remaining > 0 && AvailableRanks.Num() > 0)
    {
        int32 RandIdx = FMath::RandRange(0, AvailableRanks.Num() - 1);
        FTreasureRankLimits& Limit = AvailableRanks[RandIdx];

        Result.Add(Limit.Rank);
        Remaining--;
        CurrentCounts.FindOrAdd(Limit.Rank)++;

        if (CurrentCounts[Limit.Rank] >= Limit.MaxCount)
        {
            AvailableRanks.RemoveAt(RandIdx);
        }
    }

    // Перемешиваем массив, чтобы Legendary не всегда был в начале/конце
    for (int32 i = Result.Num() - 1; i > 0; --i)
    {
        Result.Swap(i, FMath::RandRange(0, i));
    }

    return Result;
}

FVector ATreasureSpawner::FindValidSpawnLocation(const TArray<FVector>& ExistingLocations, int32 MaxAttempts)
{
    FVector SpawnerLoc = GetActorLocation();
    float MinDistSq = FMath::Square(MinDistanceBetweenTreasures);

    // Параметры коллизии
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(TreasureSpawn), false, this);
    FCollisionShape CheckShape = FCollisionShape::MakeSphere(SpawnCheckRadius);
    FCollisionObjectQueryParams ObjQueryParams;
    ObjQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
    ObjQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

    for (int32 i = 0; i < MaxAttempts; ++i)
    {
        // Случайная точка в кубе +-SpawnRadius
        FVector Offset(SpawnRadius * FMath::RandRange(-1.0f, 1.0f),
            SpawnRadius * FMath::RandRange(-1.0f, 1.0f),
            SpawnerLoc.Z);
        FVector TestLocation = SpawnerLoc + Offset;

        // 1. Проверка дистанции до других сокровищ
        bool bTooClose = false;
        for (const FVector& Pos : ExistingLocations)
        {
            if ((TestLocation - Pos).SizeSquared() < MinDistSq)
            {
                bTooClose = true;
                break;
            }
        }
        if (bTooClose) continue;

        // 2. Проверка пересечений с WorldStatic & WorldDynamic
        TArray<FOverlapResult> OutOverlaps;
        bool bHasOverlap = GetWorld()->OverlapMultiByObjectType(
            OutOverlaps, TestLocation, FQuat::Identity,
            ObjQueryParams, CheckShape, QueryParams
        );

        if (!bHasOverlap)
        {
            return TestLocation; // Место свободно
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Cannot find free space for treasure after %d attempts. Coming back to the spawner center."), MaxAttempts);
    return SpawnerLoc; // Фоллбэк
}