// Copyright Epic Games, Inc. All Rights Reserved.

using System.Collections.Generic;
using UnrealBuildTool;

public class RustGamePlay : ModuleRules
{
	public RustGamePlay(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core", 
            "HeadMountedDisplay", 
            "EnhancedInput"
        });
        PrivateDependencyModuleNames.AddRange(new string[]
        {

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
        });
	}
}
