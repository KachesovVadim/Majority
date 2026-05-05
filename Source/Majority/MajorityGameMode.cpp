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
}
