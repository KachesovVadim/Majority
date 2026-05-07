// Fill out your copyright notice in the Description page of Project Settings.


#include "Treasure.h"

// Sets default values
ATreasure::ATreasure()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	debug_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Debug mesh"));
	SetRootComponent(debug_mesh);
	//debug_mesh->SetupAttachment(SceneRoot);

}

// Called when the game starts or when spawned
void ATreasure::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATreasure::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

