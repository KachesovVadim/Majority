#include "FightingSkill_component.h"
#include "MajorityCharacter.h"
#include "ParentCharacter.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "DataTypes.h"

UFightingSkill_component::UFightingSkill_component()
{
	PrimaryComponentTick.bCanEverTick = true;

	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackHitBox"));
	HitBox->SetBoxExtent(FVector(50.f, 50.f, 50.f));
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitBox->SetCollisionObjectType(ECC_WorldDynamic);
	HitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void UFightingSkill_component::BeginPlay()
{
	Super::BeginPlay();
	MyCharacter = Cast<AMajorityCharacter>(GetOwner());
	PC = Cast<APlayerController>(MyCharacter->GetController());

	if (MyCharacter)
	{
		HitBox->AttachToComponent(MyCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("HitBoxSocket"));
		HitBox->SetRelativeLocation(FVector(100.f, 0.f, 50.f));
	}
}

void UFightingSkill_component::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!MyCharacter) return;

	// --- ËÎÃÈÊÀ ÓÂÎÐÎÒÀ - ÏÅÐÅÌÅÙÅÍÈÅ ÍÀ N ÞÍÈÒÎÂ ---
	if (CurrentCombatState == ECombatState::Dodging)
	{
		FVector CurrentPos = MyCharacter->GetActorLocation();
		float DistanceTraveled = FVector::Dist(CurrentPos, DodgeStartPosition);

		if (DistanceTraveled < DodgeDistance)
		{
			// Äâèãàåì ïåðñîíàæà âïåðåä
			FVector ForwardDir = MyCharacter->GetActorForwardVector();
			FVector NewPos = CurrentPos + ForwardDir * DodgeSpeed * DeltaTime;
			MyCharacter->SetActorLocation(NewPos);
		}
		else
		{
			// Åñëè ïðîøëè íóæíîå ðàññòîÿíèå, çàêàí÷èâàåì óâîðîò
			EndDodge();
		}
	}
	else
	{
		// --- ÀÂÒÎÌÀÒÈ×ÅÑÊÈÉ ÁËÎÊ ---
		if (MyCharacter->CurrentPlayerStatus == EPlayerStatus::Fighting)
		{
			if (CurrentCombatState == ECombatState::None || CurrentCombatState == ECombatState::Blocking)
			{
				if (MyCharacter->GetVelocity().SizeSquared() < 100.0f)
				{
					if (!bIsBlocking)
					{
						bIsBlocking = true;
						CurrentCombatState = ECombatState::Blocking;
					}
				}
				else
				{
					if (bIsBlocking)
					{
						bIsBlocking = false;
						CurrentCombatState = ECombatState::None;
					}
				}
			}
		}
		else
		{
			if (bIsBlocking)
			{
				bIsBlocking = false;
				CurrentCombatState = ECombatState::None;
			}
		}
	}
}

void UFightingSkill_component::ActivateSkill()
{
	if (MyCharacter)
	{
		MyCharacter->CurrentPlayerStatus = EPlayerStatus::Fighting;
	}
}

void UFightingSkill_component::DeactivateSkill()
{
	if (MyCharacter)
	{
		MyCharacter->CurrentPlayerStatus = EPlayerStatus::Idle;
		bIsBlocking = false;
		CurrentCombatState = ECombatState::None;
	}
}

void UFightingSkill_component::PerformAttack()
{
	if (CurrentCombatState != ECombatState::None && CurrentCombatState != ECombatState::Blocking) return;

	CurrentCombatState = ECombatState::Attacking;
	bIsBlocking = false;

	// --- ÁËÎÊÈÐÓÅÌ ÄÂÈÆÅÍÈÅ ÂÎ ÂÐÅÌß ÀÒÀÊÈ ---
	if (MyCharacter)
	{
		PC->SetIgnoreMoveInput(true);
	}

	if (MyCharacter && AttackMontages.Num() > 0)
	{
		int32 Index = FMath::RandRange(0, AttackMontages.Num() - 1);
		MyCharacter->PlayAnimMontage(AttackMontages[Index]);
	}
}

void UFightingSkill_component::PerformDodge()
{
	if (!MyCharacter) return;

	// Ðàçðåøàåì óâîðîò òîëüêî èç ñîñòîÿíèÿ ïîêîÿ èëè áëîêà
	if (CurrentCombatState != ECombatState::None && CurrentCombatState != ECombatState::Blocking) return;

	// 1. ÏÎËÓ×ÀÅÌ ÂÅÊÒÎÐ ÂÂÎÄÀ ÄÂÈÆÅÍÈß (óæå ñ ó÷åòîì ïîâîðîòà êàìåðû)
	FVector MoveInput = MyCharacter->GetLastMovementInputVector();

	// 2. ÏÐÎÂÅÐßÅÌ, ÍÀÆÀÒÀ ËÈ ÊËÀÂÈØÀ ÄÂÈÆÅÍÈß
	if (!MoveInput.IsNearlyZero())
	{
		// Íîðìàëèçóåì âåêòîð, ÷òîáû ïîëó÷èòü ÷èñòîå íàïðàâëåíèå (äëèíîé 1)
		FVector DesiredDirection = MoveInput.GetSafeNormal();

		// Ïðåîáðàçóåì âåêòîð íàïðàâëåíèÿ â ïîâîðîò (Rotator)
		FRotator TargetRotation = DesiredDirection.Rotation();

		// ÎÁßÇÀÒÅËÜÍÎ îáíóëÿåì Pitch (íàêëîí ââåðõ/âíèç) è Roll (êðåí), 
		// ÷òîáû ïåðñîíàæ ïîâîðà÷èâàëñÿ òîëüêî ïî ãîðèçîíòàëè (Yaw)
		TargetRotation.Pitch = 0.0f;
		TargetRotation.Roll = 0.0f;

		// ÌÃÍÎÂÅÍÍÎ ïîâîðà÷èâàåì ïåðñîíàæà â íóæíóþ ñòîðîíó
		MyCharacter->SetActorRotation(TargetRotation);
	}
	// Åñëè MoveInput ðàâåí íóëþ (èãðîê ñòîèò íà ìåñòå), ìû íè÷åãî íå äåëàåì ñ ïîâîðîòîì, 
	// è ïåðñîíàæ óâåðíåòñÿ â òó ñòîðîíó, êóäà îí ñìîòðèò ñåé÷àñ (ñòàíäàðòíîå ïîâåäåíèå).

	// 3. ÇÀÏÓÑÊÀÅÌ ÑÒÀÍÄÀÐÒÍÓÞ ËÎÃÈÊÓ ÓÂÎÐÎÒÀ
	CurrentCombatState = ECombatState::Dodging;
	bIsBlocking = false;
	bIsDodging = true;

	// Áëîêèðóåì ââîä äâèæåíèÿ, ÷òîáû èãðîê íå ìîã ìåíÿòü íàïðàâëåíèå âî âðåìÿ ïåðåêàòà
	PC->SetIgnoreMoveInput(true);

	// Ñîõðàíÿåì òî÷êó ñòàðòà äëÿ ðàñ÷åòà äèñòàíöèè ïåðåìåùåíèÿ
	DodgeStartPosition = MyCharacter->GetActorLocation();

	// Ïðîèãðûâàåì àíèìàöèþ
	if (DodgeMontage)
	{
		MyCharacter->PlayAnimMontage(DodgeMontage);
	}
}

void UFightingSkill_component::EnableHitBox()
{
	if (!HitBox) return;

	TArray<AActor*> OverlappingActors;
	HitBox->GetOverlappingActors(OverlappingActors, AParentCharacter::StaticClass());

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor != MyCharacter)
		{
			if (AParentCharacter* Target = Cast<AParentCharacter>(Actor))
			{
				Target->ApplyDamage(AttackDamage, EDamageSource::Player, MyCharacter);
			}
		}
	}
}

void UFightingSkill_component::DisableHitBox()
{
}

void UFightingSkill_component::EndAttack()
{
	CurrentCombatState = ECombatState::None;

	// --- ÐÀÇÁËÎÊÈÐÓÅÌ ÄÂÈÆÅÍÈÅ ---
	if (MyCharacter)
	{
		PC->SetIgnoreMoveInput(false);
	}
}

void UFightingSkill_component::EndDodge()
{
	bIsDodging = false;
	CurrentCombatState = ECombatState::None;

	// --- ÐÀÇÁËÎÊÈÐÓÅÌ ÓÏÐÀÂËÅÍÈÅ ---
	if (MyCharacter)
	{
		PC->SetIgnoreMoveInput(false);
	}
}

void UFightingSkill_component::EndStun()
{
	CurrentCombatState = ECombatState::None;

	// --- ÐÀÇÁËÎÊÈÐÓÅÌ ÄÂÈÆÅÍÈÅ ---
	if (MyCharacter)
	{
		PC->SetIgnoreMoveInput(false);
	}
}

void UFightingSkill_component::OnBlockedHit()
{
	BlockHitsTaken++;
	if (BlockHitsTaken >= MaxBlockHits)
	{
		BlockHitsTaken = 0;
		BreakGuard();
	}
}

void UFightingSkill_component::BreakGuard()
{
	CurrentCombatState = ECombatState::Stunned;
	bIsBlocking = false;

	// --- ÁËÎÊÈÐÓÅÌ ÄÂÈÆÅÍÈÅ ÂÎ ÂÐÅÌß ÎÃËÓØÅÍÈß ---
	if (MyCharacter)
	{
		PC->SetIgnoreMoveInput(true);
	}

	if (MyCharacter && BlockStunMontage)
	{
		MyCharacter->PlayAnimMontage(BlockStunMontage);
	}

	if (MyCharacter)
	{
		MyCharacter->GetWorldTimerManager().SetTimer(
			StunTimerHandle,
			this,
			&UFightingSkill_component::EndStun,
			2.0f,
			false
		);
	}
}