#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "DataTypes.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MAJORITY_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

    // ------------------------------------------------
    //  SETTINGS
    // ------------------------------------------------

    // Reference to the items DataTable (All_items_dt)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    UDataTable* ItemsDataTable;

    // Maximum number of unique items (0 = no limit)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 MaxUniqueItems = 0;

    // Fired whenever the inventory changes
    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnInventoryChanged OnInventoryChanged;

    // ------------------------------------------------
    //  CORE FUNCTIONS
    // ------------------------------------------------

    // Add an item. Returns the amount actually added.
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 AddItem(FName Name, int32 Amount = 1);

    // Remove an item. Returns the amount actually removed.
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 RemoveItem(FName Name, int32 Amount = 1);

    // Clear the whole inventory.
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void ClearInventory();

    // Add many items by name (each +1).
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddItemsByNames(const TArray<FName>& Names);

    // Get all items.
    // OutNames    - array of item names
    // OutItems    - array of struct rows (matches OutNames)
    // OutAmounts  - array of amounts (matches OutNames)
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void GetAllItems(
        TArray<FName>& OutNames,
        TArray<FItem_st>& OutItems,
        TArray<int32>& OutAmounts) const;

    // ------------------------------------------------
    //  UTILITIES
    // ------------------------------------------------

    UFUNCTION(BlueprintPure, Category = "Inventory")
    int32 GetItemCount(FName Name) const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool HasItem(FName Name) const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    int32 GetTotalItemCount() const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    int32 GetUniqueItemCount() const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    TMap<FName, int32> GetRawInventory() const;

    // Move ALL contents to another inventory and clear this one.
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void TransferAllTo(UInventoryComponent* TargetInventory);

    // Move a specific item to another inventory.
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 TransferItemTo(FName Name, int32 Amount, UInventoryComponent* TargetInventory);

protected:
    virtual void BeginPlay() override;

private:
    // Main storage: item name -> amount
    UPROPERTY(VisibleAnywhere, Category = "Inventory")
    TMap<FName, int32> Inventory;

    bool IsValidItem(FName Name) const;
    FItem_st* GetItemRow(FName Name) const;
    void BroadcastChange();
};