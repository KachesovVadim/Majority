#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FightingSkill_component.generated.h"

class UAnimMontage;
class UBoxComponent;

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	None,
	Attacking,
	Dodging,
	Blocking,
	Stunned
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MAJORITY_API UFightingSkill_component : public UActorComponent
{
	GENERATED_BODY()

public:
	UFightingSkill_component();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	class AMajorityCharacter* MyCharacter;

	APlayerController* PC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	ECombatState CurrentCombatState = ECombatState::None;

	int32 BlockHitsTaken = 0;
	const int32 MaxBlockHits = 3;
	bool bIsBlocking = false;
	bool bIsDodging = false;

	FTimerHandle StunTimerHandle;

	// --- ПЕРЕМЕННЫЕ ДЛЯ УВОРОТА ---
	FVector DodgeStartPosition;

	void BreakGuard();

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// --- АНИМАЦИИ ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animations")
	TArray<UAnimMontage*> AttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animations")
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animations")
	UAnimMontage* BlockStunMontage;

	// --- COLLISION BOX ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Collision")
	UBoxComponent* HitBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Stats")
	float AttackDamage = 10.0f;

	// --- НАСТРОЙКИ УВОРОТА ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Dodge")
	float DodgeDistance = 300.0f; // Расстояние уворота в юнитах

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Dodge")
	float DodgeSpeed = 1000.0f; // Скорость перемещения во время уворота

	// --- ФУНКЦИИ ---
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformDodge();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EnableHitBox();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void DisableHitBox();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EndAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EndDodge();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EndStun();

	bool IsBlocking() const { return bIsBlocking; }
	bool IsDodging() const { return bIsDodging; }
	bool IsAttacking() const { return CurrentCombatState == ECombatState::Attacking; }
	bool IsStunned() const { return CurrentCombatState == ECombatState::Stunned; }

	void OnBlockedHit();

	UFUNCTION(BlueprintCallable)
	void ActivateSkill();

	UFUNCTION(BlueprintCallable)
	void DeactivateSkill();
};