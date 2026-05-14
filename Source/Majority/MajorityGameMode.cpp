// Copyright Epic Games, Inc. All Rights Reserved.

#include "MajorityGameMode.h"
#include "MajorityCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMajorityGameMode::AMajorityGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Character/Player/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	/*static ConstructorHelpers::FClassFinder<AHUD> MainHUDBPClass(TEXT("/Game/Blueprints/UI/HUD_bp.HUD_bp_C"));
	if (MainHUDBPClass.Class != NULL)
	{
		HUDClass = MainHUDBPClass.Class;
	}*/
}
