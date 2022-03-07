// Copyright Epic Games, Inc. All Rights Reserved.

#include "IslandGameMode.h"
#include "IslandHUD.h"
#include "IslandCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/BlueprintPathsLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

char buf[10000];  // Runtime JS memory
struct js* js;

DECLARE_LOG_CATEGORY_EXTERN(LogJs, Log, All);
DEFINE_LOG_CATEGORY(LogJs);

extern "C" {
#include "elk/elk.h"

	enum {
		// IMPORTANT: T_OBJ, T_PROP, T_STR must go first.  That is required by the
		// memory layout functions: memory entity types are encoded in the 2 bits,
		// thus type values must be 0,1,2,3
		T_OBJ, T_PROP, T_STR, T_UNDEF, T_NULL, T_NUM, T_BOOL, T_FUNC, T_CODEREF,
		T_ERR
	};

	const char* GetProjectDirectory()
	{
		FString fstring = UKismetSystemLibrary::GetProjectDirectory();
		char* charz = TCHAR_TO_ANSI(*fstring);
		return charz;
	}

	void printi(int milli)
	{ 
		UE_LOG(LogJs, Log, TEXT("%d"), milli);
	}

	void prints(const char* str)
	{
		UE_LOG(LogJs, Log, TEXT("%s"), ANSI_TO_TCHAR(str));
	}
}

void AIslandGameMode::ExecuteJs(UWorld* world, const TCHAR* Cmd)
{
	// js
	js = js_create(buf, sizeof(buf));
	jsval_t global = js_glob(js), gpio = js_mkobj(js);    // Equivalent to:
	js_set(js, global, "printi", js_import(js, (uintptr_t)printi, "vi"));
	js_set(js, global, "prints", js_import(js, (uintptr_t)prints, "vs"));
	js_set(js, global, "GetProjectDirectory", js_import(js, (uintptr_t)GetProjectDirectory, "s"));

#if 0
	js_set(js, global, "gpio", gpio);                     // let gpio = {};
	//js_set(js, gpio, "mode", js_import(js, (uintptr_t)myMode, "vii"));
	//js_set(js, gpio, "write", js_import(js, (uintptr_t)myWrite, "vii"));
	js_eval(js, "let pin = 13;"     // LED pin. Usually 13, but double-check
		"  delay(300);"
		"gpio.mode(pin, 1);"    // Set OUTPUT mode on a LED pin
		"while (true) {"
		"  delay(300);"
		"  gpio.write(pin, 1);"
		"  delay(300);"
		"  gpio.write(pin, 0);"
		"}"
		,
		~0);
#endif
	char* result = TCHAR_TO_ANSI(Cmd);
	jsval_t ret = js_eval(js, result, ~0);
	if (ret != T_UNDEF)
		UE_LOG(LogJs, Warning, TEXT("%s"), ANSI_TO_TCHAR(js_str(js, ret)));
}

AIslandGameMode::AIslandGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AIslandHUD::StaticClass();
}
