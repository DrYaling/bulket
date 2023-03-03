// Copyright Epic Games, Inc. All Rights Reserved.

using System.Collections.Generic;
using UnrealBuildTool;

public class RustGamePlay : ModuleRules
{
	public RustGamePlay(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "HeadMountedDisplay",
            "EnhancedInput",
            "Projects",
            "InputCore",
            "EditorFramework",
            "UnrealEd",
            "ToolMenus",
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore",
            "BlueprintGraph",
            "GraphEditor",
            "KismetWidgets",
            "KismetCompiler",
            "PropertyEditor",
            "EditorWidgets",
            "ClassViewer",
            "EditorStyle",
            "AIModule"
        });
	}
}
