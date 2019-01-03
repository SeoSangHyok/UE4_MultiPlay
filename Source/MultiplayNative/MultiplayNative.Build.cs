// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MultiplayNative : ModuleRules
{
	public MultiplayNative(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "HeadMountedDisplay",
            "SlateCore",
            "Slate",
            "UMG",
            "OnlineSubsystem",
            "OnlineSubsystemUtils",
            "Sockets",
            "Networking",
            "Json",
            "JsonUtilities"
        });

        DynamicallyLoadedModuleNames.Add("OnlinesubsystemNull");
	}
}
