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

void UTreasureSearchingSkill_comp::BeginPlay()
{
	Super::BeginPlay();

	MyCharacter = Cast<AMajorityCharacter>(GetOwner());

	// Инициализация таблиц данных
	ItemsOnLevels_dt = LoadObject<UDataTable>(nullptr, TEXT("/Game/Blueprints/Data/ItemsOnLevels_dt.ItemsOnLevels_dt"));
	All_items_dt = LoadObject<UDataTable>(nullptr, TEXT("/Game/Blueprints/Data/All_items_dt.All_items_dt"));

	// Проверки на случай, если пути неверные
	if (!ItemsOnLevels_dt)
	{
		UE_LOG(LogTemp, Error, TEXT("UTreasureSearchingSkill_comp: Failed to load ItemsOnLevels_dt! Check path."));
	}
	if (!All_items_dt)
	{
		UE_LOG(LogTemp, Error, TEXT("UTreasureSearchingSkill_comp: Failed to load All_items_dt! Check path."));
	}
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
        MyCharacter->MetalDetectorActor->SetActorHiddenInGame(false);
    }
}

void UTreasureSearchingSkill_comp::DeactivateSkill()
{
	MyCharacter->CurrentPlayerStatus = EPlayerStatus::Idle;
    if (MyCharacter->MetalDetectorActor)
    {
        MyCharacter->MetalDetectorActor->StopSearching();
        MyCharacter->MetalDetectorActor->SetActorHiddenInGame(true);
	}
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

	// 1. ОПРЕДЕЛЯЕМ, КАКОЙ ЗВУК ВОСПРОИЗВЕСТИ
	USoundCue* SoundToPlay = nullptr;

	switch (CurrentTreasure->TreasureRank)
	{
	case ETreasureRank::Common:
		SoundToPlay = SoundCuePoor;
		break;

	case ETreasureRank::Uncommon:
	case ETreasureRank::Rare:
	case ETreasureRank::Epic:
		SoundToPlay = SoundCueGood;
		break;

	case ETreasureRank::Legendary:
		SoundToPlay = SoundCueGreat;
		break;

	default:
		SoundToPlay = SoundCuePoor; // Fallback на всякий случай
		break;
	}

	// 2. ВОСПРОИЗВОДИМ ЗВУК В ТОЧКЕ, ГДЕ НАХОДИТСЯ СОКРОВИЩЕ
	if (SoundToPlay)
	{
		// PlaySoundAtLocation воспроизводит звук в указанной точке мира
		UGameplayStatics::PlaySoundAtLocation(
			this,
			SoundToPlay,
			CurrentTreasure->GetActorLocation()
		);
	}

    if (!ItemsOnLevels_dt || !All_items_dt)
    {
        UE_LOG(LogTemp, Error, TEXT("DigTreasure: Data tables are not loaded!"));
        return;
    }

    // 1. Получаем название текущего уровня и ищем строку в таблице ItemsOnLevels_dt
    // Так как RowName - это название карты, мы просто приводим FString к FName
    FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld(), false);
    FItemsOnLevel_st* LevelData = ItemsOnLevels_dt->FindRow<FItemsOnLevel_st>(*CurrentLevelName, TEXT("DigTreasure_FindLevel"));

    if (!LevelData || LevelData->Items.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("DigTreasure: No items configured for level '%s'"), *CurrentLevelName);
        return;
    }

    // 3. Фильтруем предметы по редкости (TreasureRank)
    TArray<FName> ValidItems;
    for (const FName& ItemName : LevelData->Items)
    {
        // Ищем структуру предмета в таблице All_items_dt
        FItem_st* ItemRow = All_items_dt->FindRow<FItem_st>(ItemName, TEXT("DigTreasure_FilterRank"));

        if (ItemRow && ItemRow->Rank == CurrentTreasure->TreasureRank)
        {
            ValidItems.Add(ItemName);
        }
    }

    // 4. Выбираем случайный предмет
    FName RandomItemName = NAME_None;

    if (ValidItems.Num() > 0)
    {
        // Если нашли предметы нужной редкости, берем случайный
        int32 RandomIndex = FMath::RandRange(0, ValidItems.Num() - 1);
        RandomItemName = ValidItems[RandomIndex];
    }
    else
    {
        // Fallback: если предметов нужной редкости на уровне нет (ошибка в настройке),
        // берем случайный из всех доступных, чтобы не ломать прогресс
        UE_LOG(LogTemp, Warning, TEXT("DigTreasure: No items of rank %d for level '%s'. Using fallback."),
            (int32)CurrentTreasure->TreasureRank, *CurrentLevelName);

        int32 RandomIndex = FMath::RandRange(0, LevelData->Items.Num() - 1);
        RandomItemName = LevelData->Items[RandomIndex];
    }

    // 5. Добавляем предмет в инвентарь
    if (!RandomItemName.IsNone())
    {
        int32 AddedAmount = MyCharacter->Inventory_component->AddItem(RandomItemName, 1);
        if (AddedAmount > 0)
        {
            UE_LOG(LogTemp, Log, TEXT("DigTreasure: Successfully added '%s' to inventory."), *RandomItemName.ToString());
        }
    }

    // 6. Вызываем DevelopmentMechanic
    MyCharacter->DevelopmentMechanic(CurrentTreasure->TreasureRank);

	CurrentTreasure->Destroy();

	MyCharacter->GetWorldTimerManager().SetTimer(
		DelayBetweenTreasures,
		this,
		&UTreasureSearchingSkill_comp::ActivateSkill,
		2.0f,
		false
	);

	//ActivateSkill();
}