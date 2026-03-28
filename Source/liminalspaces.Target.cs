using UnrealBuildTool;

public class liminalspacesTarget : TargetRules
{
	public liminalspacesTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.AddRange(new string[] { "LiminalSpaces" });
	}
}
