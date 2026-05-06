// Fill out your copyright notice in the Description page of Project Settings.


#include "MetalDetector.h"

// Sets default values
AMetalDetector::AMetalDetector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	detector_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Metal detector mesh"));
	detector_mesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AMetalDetector::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMetalDetector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

