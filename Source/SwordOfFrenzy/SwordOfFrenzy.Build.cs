// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SwordOfFrenzy : ModuleRules
{
	public SwordOfFrenzy(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", 
		"EnhancedInput",        // ����ϵͳ
		"GameplayAbilities",    // GAS��ģ��
		"GameplayTags",
		"GameplayTasks",
		"UMG"                   // UIϵͳ
		});


        PrivateDependencyModuleNames.AddRange(new string[] { });

        PublicIncludePaths.AddRange(new string[] {
            "SwordOfFrenzy/Public"
        });

        PrivateIncludePaths.AddRange(new string[] {
            "SwordOfFrenzy/Private"
        });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
