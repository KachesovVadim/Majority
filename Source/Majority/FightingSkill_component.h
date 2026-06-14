// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FightingSkill_component.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAJORITY_API UFightingSkill_component : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFightingSkill_component();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	class AMajorityCharacter* MyCharacter;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void ActivateSkill();

	UFUNCTION(BlueprintCallable)
	void DeactivateSkill();

};
