// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PortfolioProject : ModuleRules
{
	public PortfolioProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "Imath" });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
