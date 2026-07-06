using UnrealBuildTool;
using System.Collections.Generic;

public class HomewardTarget : TargetRules
{
	public HomewardTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("Homeward");
	}
}
