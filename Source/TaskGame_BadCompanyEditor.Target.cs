// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class TaskGame_BadCompanyEditorTarget : TargetRules
{
	public TaskGame_BadCompanyEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		ExtraModuleNames.Add("TaskGame_BadCompany");
		ExtraModuleNames.Add("GitEditor");//’Ç‰Á
	}
}
