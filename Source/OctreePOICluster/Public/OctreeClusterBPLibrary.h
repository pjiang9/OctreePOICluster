#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "POICommon.h"
#include "OctreeClusterBPLibrary.generated.h"

USTRUCT(BlueprintType)
struct FPOIClusterRadiusConfig
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Cluster Radius")
    float MaxRadiusForFullSpread;

    UPROPERTY(BlueprintReadWrite, Category = "Cluster Radius")
    float MinRadiusForFullCluster;

    FPOIClusterRadiusConfig()
        : MaxRadiusForFullSpread(MAX_flt)
        , MinRadiusForFullCluster(0.0f)
    {
    }
};

UCLASS()
class UOctreeClusterBPLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Octree POI Cluster")
    static TArray<FPOICluster> ClusterPOIsByOctree(
        TArray<FPOI> POIs,
        float ClusterRadius
    );

    UFUNCTION(BlueprintCallable, Category = "Octree POI Cluster")
    static FPOIClusterRadiusConfig CalculatePOIClusterRadiusLimits(TArray<FPOI> POIs);
};