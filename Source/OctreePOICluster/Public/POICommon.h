#pragma once
#include "CoreMinimal.h"
#include "POICommon.generated.h"

// Single POI point (with world position already converted in Blueprints)
USTRUCT(BlueprintType)
struct FPOI
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "POI")
    FVector WorldLocation;  // UE world coordinate from Cesium

    UPROPERTY(BlueprintReadWrite, Category = "POI")
    FString Type;          // POI type (only same types will cluster)

    UPROPERTY(BlueprintReadWrite, Category = "POI")
    FString DisplayText;   // Text to show for this point (NEWLY ADDED)
};

// Clustered result output
USTRUCT(BlueprintType)
struct FPOICluster
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Cluster")
    FVector WorldLocation;  // Center location of cluster

    UPROPERTY(BlueprintReadWrite, Category = "Cluster")
    int32 Count;            // Number of points in this cluster

    UPROPERTY(BlueprintReadWrite, Category = "Cluster")
    FString Type;           // Type of clustered points

    UPROPERTY(BlueprintReadWrite, Category = "Cluster")
    FString DisplayText;    // Display text for cluster (NEWLY ADDED)
};