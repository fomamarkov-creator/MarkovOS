#pragma once

#include "CoreMinimal.class"
#include "Modules/ModuleManager.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "banachos_mobile_extensions.hpp"
#include "BanachOSPhysics.generated.h"

// Экспортируем структуры в визуальную среду UE5 Blueprints
USTRUCT(BlueprintType)
struct FUnrealMolecularReport
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "BanachOS")
    float ExplosionRisk;

    UPROPERTY(BlueprintReadOnly, Category = "BanachOS")
    float ToxicityIndex;

    UPROPERTY(BlueprintReadOnly, Category = "BanachOS")
    FString EmergentState;
};

UCLASS()
class BANACHOSPHYSICS_API UBanachOSLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // Главная JIT-нода для игровых дизайнеров Unreal Engine 5
    UFUNCTION(BlueprintCallable, Category = "BanachOS|Physics")
    static FUnrealMolecularReport EvaluateChemicalNode(
        int AtomADensity, 
        int AtomBDensity, 
        int OddityFriction)
    {
        BanachOsSdkBridge sdk;
        SdkMolecularReport rep = sdk.compute_sdk_chemical_node(
            AtomADensity, AtomBDensity, OddityFriction);

        FUnrealMolecularReport UnrealReport;
        UnrealReport.ExplosionRisk = rep.explosion_risk_percentage;
        UnrealReport.ToxicityIndex = rep.toxicity_index;
        UnrealReport.EmergentState = FString(rep.emergent_state);

        return UnrealReport;
    }
};
