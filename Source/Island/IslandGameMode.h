// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/World.h"
#include "IslandGameMode.generated.h"

UCLASS(minimalapi)
class AIslandGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AIslandGameMode();

	static void ExecuteJs(UWorld* world, const TCHAR* Cmd);
};



