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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    UDataTable* ItemsDataTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 MaxUniqueItems = 0;

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnInventoryChanged OnInventoryChanged;

    // ------------------------------------------------
    //  CORE FUNCTIONS
    // ------------------------------------------------

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 AddItem(FName Name, int32 Amount = 1);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 RemoveItem(FName Name, int32 Amount = 1);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void ClearInventory();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddItemsByNames(const TArray<FName>& Names);

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

    // Get item data by its index in the sorted inventory.
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void GetItemByIndex(int32 Index, FName& OutName, bool& bIsValid, int32& OutAmount) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void TransferAllTo(UInventoryComponent* TargetInventory);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 TransferItemTo(FName Name, int32 Amount, UInventoryComponent* TargetInventory);

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Inventory")
    TMap<FName, int32> Inventory;

    bool IsValidItem(FName Name) const;
    FItem_st* GetItemRow(FName Name) const;
    void BroadcastChange();

    void SortInventory();
};