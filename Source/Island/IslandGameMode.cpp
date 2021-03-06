// Copyright Epic Games, Inc. All Rights Reserved.

#include "IslandGameMode.h"
#include "IslandHUD.h"
#include "IslandCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/BlueprintPathsLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "glTFRuntimeFunctionLibrary.h"
#include "glTFRuntimeAssetActor.h"

#include "GameFramework/Actor.h"

#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
//#include "Components/CapsuleComponent.h"
#include "Runtime/Engine/Public/EngineGlobals.h"


char buf[10000];  // Runtime JS memory
struct js* js;

DECLARE_LOG_CATEGORY_EXTERN(LogJs, Log, All);
DEFINE_LOG_CATEGORY(LogJs);

extern "C" {
#include "elk/elk.h"

	UWorld* s_world;
	TArray<UObject*> s_dynamicObjects;
	TArray<AActor*> s_dynamicActors;

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
		return TCHAR_TO_ANSI(*fstring);
	}

	void SetWindowTitle(const char* charz)
	{
		FText Text = FText::FromString(ANSI_TO_TCHAR(charz));
		UKismetSystemLibrary::SetWindowTitle(Text);
	}

	void ExecuteConsoleCommand(const char* charz)
	{
		FString Command = ANSI_TO_TCHAR(charz);
		UKismetSystemLibrary::ExecuteConsoleCommand(s_world, Command);
	}

	AActor* SpawnActor()
	{
		FVector Location(0.0f, 0.0f, 0.0f);
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		auto Actor = s_world->SpawnActor<AActor>(Location, Rotation);
		Actor->PrimaryActorTick.bCanEverTick = true;
		s_dynamicActors.Add(Actor);

		return Actor;
	}

	void RenameObject(UObject* Object, const char* charz)
	{
		Object->Rename(ANSI_TO_TCHAR(charz));
	}

	USphereComponent* CreateSphereComponent(AActor* Actor)
	{
		auto Shape = NewObject<USphereComponent>(Actor, TEXT("sphere"));
		Shape->SetupAttachment(Actor->GetRootComponent());
		Shape->RegisterComponent();

		return Shape;
	}

	UBoxComponent* CreateBoxComponent(AActor* Actor)
	{
		auto Shape = NewObject<UBoxComponent>(Actor, TEXT("box"));
		Shape->SetupAttachment(Actor->GetRootComponent());
		Shape->RegisterComponent();

		return Shape;
	}

	void SetActorLocation(AActor* Actor, int x, int y, int z)
	{
		Actor->SetActorLocation(FVector(x, y, z));
	}

	void SetActorScale3D(AActor* Actor, int x, int y, int z)
	{
		Actor->SetActorScale3D(FVector(x, y, z));
	}

	void printi(int milli)
	{ 
		UE_LOG(LogJs, Log, TEXT("%d"), milli);
		//GEngine->AddOnScreenDebugMessage(-1, 12.f, FColor::Red, TEXT("%d"), milli);
	}

	void prints(const char* str)
	{
		UE_LOG(LogJs, Log, TEXT("%s"), ANSI_TO_TCHAR(str));
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, ANSI_TO_TCHAR(str));
	}

	UglTFRuntimeAsset* glTFLoadAssetFromFilename(const char* Filename)
	{
		bool bPathRelativeToContent = false;
		FglTFRuntimeConfig Config;
		Config.bAllowExternalFiles = true;
		UglTFRuntimeAsset* asset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(ANSI_TO_TCHAR(Filename), 
			bPathRelativeToContent, Config);

		return asset;
	}

	// https://dawnarc.com/2018/11/ue4pass-parameters-before-triggering-beginplay-spawnactordeferred/
	AglTFRuntimeAssetActor* gltfSpawnActor(UglTFRuntimeAsset* asset)
	{
		FVector Location(0.0f, 0.0f, 0.0f);
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		FTransform SpawnTransform = FTransform::Identity;

		FActorSpawnParameters SpawnInfo;
		SpawnInfo.bDeferConstruction = true;
		auto Actor = s_world->SpawnActor<AglTFRuntimeAssetActor>(SpawnInfo);
		s_dynamicActors.Add(Actor);

		Actor->Asset = asset;
		Actor->StaticMeshConfig.bBuildSimpleCollision = true;
		//Actor->SkeletalMeshConfig.MaterialsConfig.

		Actor->FinishSpawning(SpawnTransform);

		return Actor;
	}

	void SetSimulatePhysics(AActor* Actor, bool bSimulate)
	{
		for (UActorComponent* Component : Actor->GetComponents())
		{
			if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Component))
			{
				PrimComp->SetSimulatePhysics(bSimulate);
			}
		}
	}
}

FTimerHandle HandleJsUpdate;

void AIslandGameMode::ExecuteJs(UWorld* world, const TCHAR* Cmd)
{
	for (auto& Object : s_dynamicObjects)
	{
		if (!Object) continue;
		if (!Object->IsValidLowLevel()) continue;

		Object->ConditionalBeginDestroy();
	}
	s_dynamicObjects.Empty();

	for (auto& Actor : s_dynamicActors)
	{
		world->DestroyActor(Actor, false);
	}
	s_dynamicActors.Empty();

	s_world = world;
	// js
	js = js_create(buf, sizeof(buf));
	jsval_t global = js_glob(js), gpio = js_mkobj(js);    // Equivalent to:

	//js_set(js, global, "printi", js_import(js, (uintptr_t)printi, "vi"));

#define REGISTER_FUNCTION(name, signature) js_set(js, global, #name, js_import(js, (uintptr_t)::name, signature));
	REGISTER_FUNCTION(printi, "vi");
	REGISTER_FUNCTION(prints, "vs");
	REGISTER_FUNCTION(GetProjectDirectory, "s");
	REGISTER_FUNCTION(SpawnActor, "p");
	REGISTER_FUNCTION(RenameObject, "vps");
	REGISTER_FUNCTION(SetWindowTitle, "vs");
	REGISTER_FUNCTION(ExecuteConsoleCommand, "vs");
	REGISTER_FUNCTION(SetActorLocation, "vpiii");
	REGISTER_FUNCTION(SetActorScale3D, "vpiii");
	REGISTER_FUNCTION(SetSimulatePhysics, "vpb");

	REGISTER_FUNCTION(CreateSphereComponent, "pp");
	REGISTER_FUNCTION(CreateBoxComponent, "pp");
	REGISTER_FUNCTION(glTFLoadAssetFromFilename, "ps");
	REGISTER_FUNCTION(gltfSpawnActor, "pp");

#undef REGISTER_FUNCTION

	jsval_t ret = js_eval(js, TCHAR_TO_ANSI(Cmd), ~0);
	if (ret != T_UNDEF)
	{
		auto Error = ANSI_TO_TCHAR(js_str(js, ret));
		UE_LOG(LogJs, Warning, TEXT("%s"), Error);
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Error);
	}

	world->GetTimerManager().ClearTimer(HandleJsUpdate);
	float FrameRate = 60.0f;
	if (GEngine && GEngine->bUseFixedFrameRate)
		FrameRate = GEngine->FixedFrameRate;
	world->GetTimerManager().SetTimer(HandleJsUpdate, [] {
		jsval_t ret = js_eval(js, "update()", ~0);
	}, 1 / FrameRate, true);
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
