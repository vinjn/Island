// Copyright Epic Games, Inc. All Rights Reserved.

#include "IslandGameMode.h"
#include "IslandHUD.h"
#include "IslandCharacter.h"
#include "UObject/ConstructorHelpers.h"

char buf[300];  // Runtime JS memory
struct js* js;

extern "C" {
#include "elk/elk.h"

	void myDelay(int milli)
	{ 
		UE_LOG(LogTemp, Warning, TEXT("myDelay %d"), milli);
	}

	void myWrite(int pin, int val)
	{
		UE_LOG(LogTemp, Warning, TEXT("myWrite %d %d"), pin, val);
	}

	void myMode(int pin, int mode)
	{ 
		UE_LOG(LogTemp, Warning, TEXT("myMode %d %d"), pin, mode);
	}
}

void AIslandGameMode::ExecuteJs(const char* Cmd)
{
	js_eval(js, Cmd, ~0);
}

AIslandGameMode::AIslandGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AIslandHUD::StaticClass();

	// js
	js = js_create(buf, sizeof(buf));
	jsval_t global = js_glob(js), gpio = js_mkobj(js);    // Equivalent to:
	js_set(js, global, "gpio", gpio);                     // let gpio = {};
	js_set(js, global, "delay", js_import(js, (uintptr_t)myDelay, "vi"));
	js_set(js, gpio, "mode", js_import(js, (uintptr_t)myMode, "vii"));
	js_set(js, gpio, "write", js_import(js, (uintptr_t)myWrite, "vii"));

	js_eval(js, "let pin = 13;"     // LED pin. Usually 13, but double-check
		"  delay(300);"
#if 0
		"gpio.mode(pin, 1);"    // Set OUTPUT mode on a LED pin
		"while (true) {"
		"  delay(300);"
		"  gpio.write(pin, 1);"
		"  delay(300);"
		"  gpio.write(pin, 0);"
		"}"
#endif
		,
		~0);

}
