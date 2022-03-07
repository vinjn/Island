// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Island : ModuleRules
{
	public Island(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });

		PrivateDependencyModuleNames.AddRange(new string[] { "glTFRuntime" });

	}
}
