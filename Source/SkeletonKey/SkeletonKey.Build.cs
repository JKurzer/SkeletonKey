// Copyright Epic Games, Inc. All Rights Reserved.
using System;
using System.IO;
using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class SkeletonKey : ModuleRules
{
	public SkeletonKey(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				Path.Combine(PluginDirectory,"Source/SkeletonKey"),
				
			}
		);
		//may also need to add an explicit runtime dependency.
		// Get the engine path. Ends with "Engine/"
		string engine_path = EngineDirectory;
		// Now get the base of UE's modules dir (could also be Developer, Editor, ThirdParty)
		string src_path = engine_path + "\\Source\\Runtime\\";
		//Don't do this.
		PublicIncludePaths.Add(src_path + "Engine\\");
		PublicIncludePaths.Add(src_path + "Engine\\Private\\");
		PublicIncludePaths.Add(src_path + "Engine\\Private\\LevelTick.cpp");

		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{	
				"Core",
                "CoreUObject",
                "Engine",
                "Slate",
                "ApplicationCore",
				// ... add other public dependencies that you statically link with here ...
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"ApplicationCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
