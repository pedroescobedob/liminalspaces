using UnrealBuildTool;

public class liminalspacesEditorTarget : TargetRules
{
	public liminalspacesEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.AddRange(new string[] { "LiminalSpaces" });
	}
}
