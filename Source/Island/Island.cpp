// Copyright Epic Games, Inc. All Rights Reserved.

#include "Island.h"
#include "Modules/ModuleManager.h"
#include "Misc/DefaultValueHelper.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"

#include "IslandGameMode.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, Island, "Island" );
 

static class FGMCmdExec : private FSelfRegisteringExec
{
public:
	/** Console commands, see embeded usage statement **/
	virtual bool Exec(UWorld* world, const TCHAR* Cmd, FOutputDevice& Ar) override
	{
		const bool bGmCommand = FParse::Command(&Cmd, TEXT("gm"));
		if (!bGmCommand) return false;

		FString AddArgs;
		const TCHAR* TempCmd = Cmd;

		FString ArgNoWhitespaces = FDefaultValueHelper::RemoveWhitespaces(TempCmd);
		const bool bIsEmpty = ArgNoWhitespaces.IsEmpty();
		if (bIsEmpty) return true;

		APawn* pawn = UGameplayStatics::GetPlayerPawn(world, 0);
		
		return true;
	}
}
GMExec;


static class FJsCmdExec : private FSelfRegisteringExec
{
public:
	/** Console commands, see embeded usage statement **/
	virtual bool Exec(UWorld* world, const TCHAR* Cmd, FOutputDevice& Ar) override
	{
		const bool bGmCommand = FParse::Command(&Cmd, TEXT("js"));
		if (!bGmCommand) return false;

		FString AddArgs;
		const TCHAR* TempCmd = Cmd;

		FString ArgNoWhitespaces = FDefaultValueHelper::RemoveWhitespaces(TempCmd);
		const bool bIsEmpty = ArgNoWhitespaces.IsEmpty();
		if (bIsEmpty) return true;

		APawn* pawn = UGameplayStatics::GetPlayerPawn(world, 0);

		AIslandGameMode::ExecuteJs(world, *ArgNoWhitespaces);

		return true;
	}
}
JsExec;

static class FLoadJsCmdExec : private FSelfRegisteringExec
{
public:
	/** Console commands, see embeded usage statement **/
	virtual bool Exec(UWorld* world, const TCHAR* Cmd, FOutputDevice& Ar) override
	{
		const bool bGmCommand = FParse::Command(&Cmd, TEXT("loadjs"));
		if (!bGmCommand) return false;

		FString AddArgs;
		const TCHAR* TempCmd = Cmd;

		FString ArgNoWhitespaces = FDefaultValueHelper::RemoveWhitespaces(TempCmd);
		const bool bIsEmpty = ArgNoWhitespaces.IsEmpty();
		if (bIsEmpty) return true;

		APawn* pawn = UGameplayStatics::GetPlayerPawn(world, 0);

		FString Content;
		if (FFileHelper::LoadFileToString(Content, *ArgNoWhitespaces))
		{
			AIslandGameMode::ExecuteJs(world, *Content);
		}

		return true;
	}
}
LoadJsExec;