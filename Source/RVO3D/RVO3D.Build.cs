
using System.IO;
using UnrealBuildTool;

public class RVO3D: ModuleRules
{
	public RVO3D(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine"
            });

		PrivateDependencyModuleNames.AddRange(new string[] { });

        PrivateIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Private/ThirdParty/RVO2-3D"),
            }
        );
	}
}
