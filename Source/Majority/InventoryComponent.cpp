#include "InventoryComponent.h"
#include "Engine/DataTable.h"

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    bWantsInitializeComponent = false;
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    if (!ItemsDataTable)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[InventoryComponent] ItemsDataTable is not set on %s"),
            *GetOwner()->GetName());
    }
}

// ------------------------------------------------
//  ADD
// ------------------------------------------------

int32 UInventoryComponent::AddItem(FName Name, int32 Amount)
{
    if (Amount <= 0 || Name.IsNone())
        return 0;

    if (ItemsDataTable && !IsValidItem(Name))
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[InventoryComponent] Item '%s' not found in DataTable"),
            *Name.ToString());
        return 0;
    }

    // Check unique slot limit
    if (MaxUniqueItems > 0 && !Inventory.Contains(Name) && Inventory.Num() >= MaxUniqueItems)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[InventoryComponent] Inventory is full (unique slots)"));
        return 0;
    }

    // Check MaxStack from DataTable (optional extension - not in current struct)
    int32 ActualAmount = Amount;

    int32& CurrentAmount = Inventory.FindOrAdd(Name, 0);
    CurrentAmount += ActualAmount;

    BroadcastChange();

    UE_LOG(LogTemp, Verbose,
        TEXT("[InventoryComponent] + %d x '%s' (total: %d)"),
        ActualAmount, *Name.ToString(), CurrentAmount);

    return ActualAmount;
}

// ------------------------------------------------
//  REMOVE
// ------------------------------------------------

int32 UInventoryComponent::RemoveItem(FName Name, int32 Amount)
{
    if (Amount <= 0 || Name.IsNone())
        return 0;

    int32* CurrentAmount = Inventory.Find(Name);
    if (!CurrentAmount || *CurrentAmount <= 0)
        return 0;

    int32 ActualRemoved = FMath::Min(Amount, *CurrentAmount);
    *CurrentAmount -= ActualRemoved;

    if (*CurrentAmount <= 0)
    {
        Inventory.Remove(Name);
    }

    BroadcastChange();

    UE_LOG(LogTemp, Verbose,
        TEXT("[InventoryComponent] - %d x '%s'"),
        ActualRemoved, *Name.ToString());

    return ActualRemoved;
}

// ------------------------------------------------
//  CLEAR
// ------------------------------------------------

void UInventoryComponent::ClearInventory()
{
    if (Inventory.Num() == 0)
        return;

    Inventory.Empty();
    BroadcastChange();

    UE_LOG(LogTemp, Log, TEXT("[InventoryComponent] Inventory cleared"));
}

// ------------------------------------------------
//  ADD BY NAMES
// ------------------------------------------------

void UInventoryComponent::AddItemsByNames(const TArray<FName>& Names)
{
    if (Names.Num() == 0)
        return;

    bool bChanged = false;

    for (const FName& Name : Names)
    {
        if (Name.IsNone())
            continue;

        if (ItemsDataTable && !IsValidItem(Name))
        {
            UE_LOG(LogTemp, Warning,
                TEXT("[InventoryComponent] Item '%s' not found in DataTable, skipped"),
                *Name.ToString());
            continue;
        }

        int32& CurrentAmount = Inventory.FindOrAdd(Name, 0);
        CurrentAmount += 1;
        bChanged = true;
    }

    if (bChanged)
    {
        BroadcastChange();
    }
}

// ------------------------------------------------
//  GET ALL
// ------------------------------------------------

void UInventoryComponent::GetAllItems(
    TArray<FName>& OutNames,
    TArray<FItem_st>& OutItems,
    TArray<int32>& OutAmounts) const
{
    OutNames.Empty();
    OutItems.Empty();
    OutAmounts.Empty();

    if (Inventory.Num() == 0)
        return;

    const int32 Count = Inventory.Num();
    OutNames.Reserve(Count);
    OutItems.Reserve(Count);
    OutAmounts.Reserve(Count);

    for (const TPair<FName, int32>& Pair : Inventory)
    {
        OutNames.Add(Pair.Key);
        OutAmounts.Add(Pair.Value);

        FItem_st* Row = GetItemRow(Pair.Key);
        if (Row)
        {
            OutItems.Add(*Row);
        }
        else
        {
            FItem_st EmptyRow;
            EmptyRow.Name = Pair.Key;
            OutItems.Add(EmptyRow);
        }
    }
}

// ------------------------------------------------
//  UTILITIES
// ------------------------------------------------

int32 UInventoryComponent::GetItemCount(FName Name) const
{
    const int32* Amount = Inventory.Find(Name);
    return Amount ? *Amount : 0;
}

bool UInventoryComponent::HasItem(FName Name) const
{
    const int32* Amount = Inventory.Find(Name);
    return Amount && *Amount > 0;
}

int32 UInventoryComponent::GetTotalItemCount() const
{
    int32 Total = 0;
    for (const TPair<FName, int32>& Pair : Inventory)
    {
        Total += Pair.Value;
    }
    return Total;
}

int32 UInventoryComponent::GetUniqueItemCount() const
{
    return Inventory.Num();
}

TMap<FName, int32> UInventoryComponent::GetRawInventory() const
{
    return Inventory;
}

// ------------------------------------------------
//  TRANSFER
// ------------------------------------------------

void UInventoryComponent::TransferAllTo(UInventoryComponent* TargetInventory)
{
    if (!TargetInventory || Inventory.Num() == 0)
        return;

    for (const TPair<FName, int32>& Pair : Inventory)
    {
        TargetInventory->AddItem(Pair.Key, Pair.Value);
    }

    ClearInventory();

    UE_LOG(LogTemp, Log, TEXT("[InventoryComponent] All items transferred"));
}

int32 UInventoryComponent::TransferItemTo(
    FName Name,
    int32 Amount,
    UInventoryComponent* TargetInventory)
{
    if (!TargetInventory || Amount <= 0)
        return 0;

    int32 Available = GetItemCount(Name);
    int32 ToTransfer = FMath::Min(Amount, Available);

    if (ToTransfer <= 0)
        return 0;

    int32 ActuallyAdded = TargetInventory->AddItem(Name, ToTransfer);

    if (ActuallyAdded > 0)
    {
        RemoveItem(Name, ActuallyAdded);
    }

    return ActuallyAdded;
}

// ------------------------------------------------
//  INTERNAL
// ------------------------------------------------

bool UInventoryComponent::IsValidItem(FName Name) const
{
    if (!ItemsDataTable)
        return true;

    return ItemsDataTable->FindRow<FItem_st>(Name, TEXT("IsValidItem"), false) != nullptr;
}

FItem_st* UInventoryComponent::GetItemRow(FName Name) const
{
    if (!ItemsDataTable)
        return nullptr;

    return ItemsDataTable->FindRow<FItem_st>(Name, TEXT("GetItemRow"), false);
}

void UInventoryComponent::BroadcastChange()
{
    OnInventoryChanged.Broadcast();
}