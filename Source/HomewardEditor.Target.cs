using UnrealBuildTool;
using System.Collections.Generic;

public class HomewardEditorTarget : TargetRules
{
	public HomewardEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("Homeward");
	}
}
