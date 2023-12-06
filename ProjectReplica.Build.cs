// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectReplica : ModuleRules
{
	public ProjectReplica(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"HeadMountedDisplay", 
			"UMG", 
			"NavigationSystem", 
			"AIModule", 
			"GameplayTasks", 
			"Niagara", 
			"CinematicCamera",
			"TemplateSequence",
			"MovieScene",
			"ApexDestruction"
		});
	}
}
