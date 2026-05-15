// Fill out your copyright notice in the Description page of Project Settings.


#include "TreasureSearchingSkill_comp.h"
#include "MajorityCharacter.h"
#include "MetalDetector.h"
#include "Kismet/GameplayStatics.h"
#include "Treasure.h"

#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UTreasureSearchingSkill_comp::UTreasureSearchingSkill_comp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTreasureSearchingSkill_comp::BeginPlay()
{
	Super::BeginPlay();

	MyCharacter = Cast<AMajorityCharacter>(GetOwner());
	
}


// Called every frame
void UTreasureSearchingSkill_comp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTreasureSearchingSkill_comp::ActivateSkill()
{
	if (!MyCharacter->MetalDetectorActor) { return; }
	MyCharacter->CurrentPlayerStatus = EPlayerStatus::TreasureHunting;

    if (FindRandomTreasure() && MyCharacter->MetalDetectorActor)
    {
        MyCharacter->MetalDetectorActor->SetTargetTreasure(CurrentTreasure);
        MyCharacter->MetalDetectorActor->StartSearching();
    }
}

void UTreasureSearchingSkill_comp::DeactivateSkill()
{
	MyCharacter->CurrentPlayerStatus = EPlayerStatus::Idle;
}

ATreasure* UTreasureSearchingSkill_comp::FindRandomTreasure()
{
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATreasure::StaticClass(), FoundActors);

    if (FoundActors.Num() == 0)
    {
        CurrentTreasure = nullptr;
        UE_LOG(LogTemp, Warning, TEXT("[TreasureSearch] Treasures was not found!"));
        return nullptr;
    }

    int32 RandomIndex = FMath::RandRange(0, FoundActors.Num() - 1);
    CurrentTreasure = Cast<ATreasure>(FoundActors[RandomIndex]);
    return CurrentTreasure;
    DrawDebugSphere(
        GetWorld(),             // World context
        CurrentTreasure->GetActorLocation()+FVector(0,0,300),
        33.0f,                 // Radius
        12,                     // Segments (smoothness)
        FColor::Yellow,            // Color
        true,                  // Persistent (true = stays forever)
        5555.0f,                   // LifeTime in seconds
        0,                      // Depth priority
        2.0f                    // Thickness
    );

    return CurrentTreasure;    
}

bool UTreasureSearchingSkill_comp::IsPlayerStayingNearTreasure()
{
    if (MyCharacter)
    {
        if (MyCharacter->MetalDetectorActor) 
        {
            return MyCharacter->MetalDetectorActor->bIsFound;
        }
    }  
    return false;
}

void UTreasureSearchingSkill_comp::DigTreasure()
{
    if (!IsPlayerStayingNearTreasure()) { return; }
    if (!CurrentTreasure) { return; }

    MyCharacter->DevelopmentMechanic(CurrentTreasure->TreasureRank);    

    CurrentTreasure->Destroy();

    ActivateSkill();
}
