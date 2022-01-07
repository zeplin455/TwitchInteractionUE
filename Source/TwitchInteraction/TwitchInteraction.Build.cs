// Copyright (c) 2021 Zeplin455.

using UnrealBuildTool;
using System.IO;

public class TwitchInteraction : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    public TwitchInteraction(ReadOnlyTargetRules Target) : base(Target)
	{
	    PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
						Path.Combine(ModuleDirectory, "Public"),
				// ... add public include paths required here ...
			}
			);


		PrivateIncludePaths.AddRange(
			new string[] {
				Path.Combine(ModuleDirectory, "Private"),
				// ... add other private include paths required here ...
			}
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "CoreUObject",
                "Engine",
				"Sockets",
				"Networking",
				"Json",
				"JsonUtilities",
				"WebSockets",
                "HTTP",
                "HttpServer"

                // ... add other public dependencies that you statically link with here ...
			}
			);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore"
				// ... add private dependencies that you statically link with here ...	
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
            }
            );
			
			if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            bEnableExceptions = true;

            PrivateIncludePaths.AddRange(
                new string[] {
                    "TwitchInteraction/Private",
                    "TwitchInteraction/Private/Win64",
                    "TwitchInteraction/Private/Win64/include",
                    });

            PublicIncludePaths.AddRange(
                new string[] {
                    Path.Combine(ModuleDirectory, "Private"),
                    Path.Combine(ModuleDirectory, "Private/Win64"),
                    Path.Combine(ModuleDirectory, "Private/Win64/include"),
                });
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            bEnableExceptions = true;

            PrivateIncludePaths.AddRange(
                new string[] {
                    "TwitchInteraction/Private",
                    "TwitchInteraction/Private/Mac",
                    "TwitchInteraction/Private/Mac/include",
                    });

            PublicIncludePaths.AddRange(
                new string[] {
                    Path.Combine(ModuleDirectory, "Private"),
                    Path.Combine(ModuleDirectory, "Private/Mac"),
                    Path.Combine(ModuleDirectory, "Private/Mac/include"),
                });
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            bEnableExceptions = true;

            PrivateIncludePaths.AddRange(
                new string[] {
                    "TwitchInteraction/Private",
                    "TwitchInteraction/Private/Linux",
                    "TwitchInteraction/Private/Linux/include",
                    });

            PublicIncludePaths.AddRange(
                new string[] {
                    Path.Combine(ModuleDirectory, "Private"),
                    Path.Combine(ModuleDirectory, "Private/Linux"),
                    Path.Combine(ModuleDirectory, "Private/Linux/include"),
                });
        }

	}
}
