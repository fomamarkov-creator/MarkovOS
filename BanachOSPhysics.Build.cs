using UnrealBuildTool;
using System.IO;

public class BanachOSPhysics : ModuleRules
{
    public BanachOSPhysics(ReadOnlyTargetRules Target) 
        : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicDependencyModuleNames.AddRange(
            new string[] { "Core", "CoreUObject", "Engine", "InputCore" }
        );

        // Автоматически подключаем наш нативный бинарник V256
        string LibPath = Path.Combine(ModuleDirectory, "../../Binaries");
        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            PublicAdditionalLibraries.Add(
                Path.Combine(LibPath, "Android", "libbanachos_core.so")
            );
        }
    }
}
