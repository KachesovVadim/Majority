// Fill out your copyright notice in the Description page of Project Settings.

#include "FightingSkill_component.h"
#include "MajorityCharacter.h"

// Sets default values for this component's properties
UFightingSkill_component::UFightingSkill_component()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFightingSkill_component::BeginPlay()
{
	Super::BeginPlay();

	MyCharacter = Cast<AMajorityCharacter>(GetOwner());
	
}


// Called every frame
void UFightingSkill_component::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFightingSkill_component::ActivateSkill()
{
	MyCharacter->CurrentPlayerStatus = EPlayerStatus::Fighting;

	
}

void UFightingSkill_component::DeactivateSkill()
{
	MyCharacter->CurrentPlayerStatus = EPlayerStatus::Idle;
}
