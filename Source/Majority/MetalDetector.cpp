// Fill out your copyright notice in the Description page of Project Settings.


#include "MetalDetector.h"
#include "Components/PointLightComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Treasure.h"

// Sets default values
AMetalDetector::AMetalDetector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false; // Тик только во время поиска

	// Create default scene root
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(SceneRoot); 

	// create detector mesh 
	detector_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Metal detector mesh"));
	detector_mesh->SetupAttachment(SceneRoot);

	// create hint light
	HintLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("HintLight"));
	HintLight->SetupAttachment(SceneRoot);
	HintLight->SetVisibility(false);

}

// Called when the game starts or when spawned
void AMetalDetector::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMetalDetector::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsSearching || !IsValid(TargetTreasure))
    {
        StopSearching();
        return;
    }

    float Distance = GetDistanceToTreasure();

    // Проверка радиуса обнаружения
    if (Distance <= FoundRadius)
    {
        if (!bIsFound) EnterFoundState();
        return; // Пропускаем логику мигания
    }

    // Если игрок вышел из радиуса, возвращаем режим поиска
    if (bIsFound) EnterSearchingState();

    // Логика мигания
    BlinkTimer += DeltaTime;
    float CurrentInterval = CalculateBlinkInterval(Distance);

    if (BlinkTimer >= CurrentInterval)
    {
        BlinkTimer = 0.0f;
        ToggleLight();
    }
}

float AMetalDetector::GetDistanceToTreasure() const
{
    if (!IsValid(TargetTreasure)) return MaxDistance;

    FVector PlayerLocation = GetOwner()->GetActorLocation();
    return FVector::Dist2D(PlayerLocation, TargetTreasure->GetActorLocation());
}

float AMetalDetector::CalculateBlinkInterval(float Distance) const
{
    return FMath::GetMappedRangeValueClamped(
        FVector2D(MinDistance, MaxDistance),
        FVector2D(MinBlinkInterval, MaxBlinkInterval),
        Distance
    );
}

void AMetalDetector::ToggleLight()
{
    bIsLightOn = !bIsLightOn;
    HintLight->SetIntensity(bIsLightOn ? LightIntensityWhenOn : 0.0f);

    if (bIsLightOn && BeepSound)
    {
        UGameplayStatics::PlaySound2D(this, BeepSound);
    }
}

void AMetalDetector::EnterFoundState()
{
    bIsFound = true;
    HintLight->SetIntensity(LightIntensityWhenOn);
    HintLight->SetVisibility(true);
    bIsLightOn = true; // Свет горит постоянно

    // Запуск зацикленного звука находки
    if (FoundLoopSound)
    {
        if (!FoundSoundComp)
        {
            FoundSoundComp = UGameplayStatics::SpawnSoundAttached(
                FoundLoopSound, HintLight, NAME_None, FVector::ZeroVector,
                EAttachLocation::KeepRelativeOffset, true, 1.0f, 1.0f, 0.0f, nullptr, nullptr, false);

            if (FoundSoundComp)
            {
                //FoundSoundComp->bLooping=true;
                FoundSoundComp->Play();
            }
        }
        else if (!FoundSoundComp->IsPlaying())
        {
            FoundSoundComp->Play();
        }
    }
}

void AMetalDetector::EnterSearchingState()
{
    bIsFound = false;
    ResetFoundState();
}

void AMetalDetector::ResetFoundState()
{
    if (FoundSoundComp)
    {
        FoundSoundComp->Stop();
    }
    HintLight->SetIntensity(0.0f);
    bIsLightOn = false;
    BlinkTimer = 0.0f;
}

void AMetalDetector::SetTargetTreasure(ATreasure* NewTreasure)
{
    TargetTreasure = NewTreasure;
}

void AMetalDetector::StartSearching()
{
    if (!IsValid(TargetTreasure)) return;

    ResetFoundState();
    bIsSearching = true;
    bIsFound = false;
    SetActorTickEnabled(true);
    HintLight->SetVisibility(true);
}

void AMetalDetector::StopSearching()
{
    bIsSearching = false;
    ResetFoundState();
    SetActorTickEnabled(false);
    HintLight->SetVisibility(false);
}