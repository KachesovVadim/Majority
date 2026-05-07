// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MetalDetector.generated.h"

class UPointLightComponent;
class USoundBase;
class UAudioComponent;
class ATreasure;

UCLASS()
class MAJORITY_API AMetalDetector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMetalDetector();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* detector_mesh;

	UFUNCTION(BlueprintCallable, Category = "MetalDetector")
	void SetTargetTreasure(ATreasure* NewTreasure);

	UFUNCTION(BlueprintCallable, Category = "MetalDetector")
	void StartSearching();

	UFUNCTION(BlueprintCallable, Category = "MetalDetector")
	void StopSearching();

    UPROPERTY(VisibleAnywhere, Category = "Components")
    class UPointLightComponent* HintLight;

    UPROPERTY(EditAnywhere, Category = "CUSTOM")
    class USoundBase* BeepSound;

    UPROPERTY(EditAnywhere, Category = "CUSTOM")
    class USoundBase* FoundLoopSound; // Звук находки (должен зацикливаться)

    // Настройки дистанции и скорости мигания
    UPROPERTY(EditAnywhere, Category = "CUSTOM", meta = (ClampMin = "0.0"))
    float MinDistance = 100.0f;   // Дистанция самого быстрого мигания
    UPROPERTY(EditAnywhere, Category = "CUSTOM", meta = (ClampMin = "0.0"))
    float MaxDistance = 1000.0f;  // Дистанция самого медленного мигания
    UPROPERTY(EditAnywhere, Category = "CUSTOM", meta = (ClampMin = "0.0"))
    float MinBlinkInterval = 0.1f;// Интервал быстрого мигания (сек)
    UPROPERTY(EditAnywhere, Category = "CUSTOM", meta = (ClampMin = "0.0"))
    float MaxBlinkInterval = 1.0f;// Интервал медленного мигания (сек)
    UPROPERTY(EditAnywhere, Category = "CUSTOM")
    float LightIntensityWhenOn = 8000.0f; // Свечение при "вкл"
    UPROPERTY(EditAnywhere, Category = "CUSTOM", meta = (ClampMin = "0.0"))
    float FoundRadius = 150.0f; // Радиус "обнаружения"

    UPROPERTY()
    ATreasure* TargetTreasure = nullptr;

    UPROPERTY()
    UAudioComponent* FoundSoundComp = nullptr;

    float BlinkTimer = 0.0f;
    bool bIsLightOn = false;
    bool bIsSearching = false;
    bool bIsFound = false;

    void ToggleLight();
    float GetDistanceToTreasure() const;
    float CalculateBlinkInterval(float Distance) const;

    void EnterFoundState();
    void EnterSearchingState();
    void ResetFoundState();

};
