#include "OctreeClusterBPLibrary.h"
#include "octree.hpp"

#define EARTH_RADIUS_M 6371000.0f
#define EARTH_DIAMETER_M (EARTH_RADIUS_M * 2.0f)
#define EARTH_DIAMETER_CM (EARTH_DIAMETER_M * 100.0f)
#define GLOBAL_SEARCH_RADIUS (EARTH_DIAMETER_CM * 1.2f)
#define FALLBACK_FULL_SPREAD_RADIUS 1.0f
#define FALLBACK_FULL_CLUSTER_RADIUS EARTH_DIAMETER_CM

struct FOctreePoint
{
    float x;
    float y;
    float z;
    const FPOI* POI;
};

TArray<FPOICluster> UOctreeClusterBPLibrary::ClusterPOIsByOctree(TArray<FPOI> POIs, float ClusterRadius)
{
    TArray<FPOICluster> ResultClusters;
    TSet<const FPOI*> VisitedPoints;

    TMap<FString, TArray<const FPOI*>> POIsGroupedByType;
    for (const FPOI& POI : POIs)
    {
        POIsGroupedByType.FindOrAdd(POI.Type).Add(&POI);
    }

    for (const auto& TypeEntry : POIsGroupedByType)
    {
        const TArray<const FPOI*>& CurrentTypePOIs = TypeEntry.Value;
        std::vector<FOctreePoint> OctreePoints;

        for (const FPOI* POI : CurrentTypePOIs)
        {
            OctreePoints.push_back({
                static_cast<float>(POI->WorldLocation.X),
                static_cast<float>(POI->WorldLocation.Y),
                static_cast<float>(POI->WorldLocation.Z),
                POI
                });
        }

        unibn::Octree<FOctreePoint> Octree;
        Octree.initialize(OctreePoints);

        for (const FOctreePoint& PointData : OctreePoints)
        {
            const FPOI* POI = PointData.POI;
            if (VisitedPoints.Contains(POI))
                continue;

            std::vector<uint32_t> Indices;
            Octree.radiusNeighbors<unibn::L2Distance<FOctreePoint>>(
                PointData,
                ClusterRadius,
                Indices
            );

            TArray<const FPOI*> ValidPoints;
            for (uint32_t idx : Indices)
            {
                const FPOI* P = OctreePoints[idx].POI;
                if (!VisitedPoints.Contains(P))
                {
                    ValidPoints.Add(P);
                }
            }

            if (ValidPoints.Num() == 0)
                continue;

            FPOICluster Cluster;
            Cluster.WorldLocation = POI->WorldLocation;
            Cluster.Count = ValidPoints.Num();
            Cluster.Type = POI->Type;
            Cluster.DisplayText = POI->DisplayText;

            for (const FPOI* P : ValidPoints)
            {
                VisitedPoints.Add(P);
            }

            ResultClusters.Add(Cluster);
        }
    }

    return ResultClusters;
}

FPOIClusterRadiusConfig UOctreeClusterBPLibrary::CalculatePOIClusterRadiusLimits(TArray<FPOI> POIs)
{
    FPOIClusterRadiusConfig RadiusConfig;

    TMap<FString, TArray<const FPOI*>> POIsByType;
    for (const FPOI& P : POIs)
    {
        POIsByType.FindOrAdd(P.Type).Add(&P);
    }

    for (const auto& Entry : POIsByType)
    {
        const TArray<const FPOI*>& TypePoints = Entry.Value;
        if (TypePoints.Num() <= 1)
            continue;

        std::vector<FOctreePoint> OctreePoints;
        for (const FPOI* P : TypePoints)
        {
            OctreePoints.push_back({
                static_cast<float>(P->WorldLocation.X),
                static_cast<float>(P->WorldLocation.Y),
                static_cast<float>(P->WorldLocation.Z),
                P
                });
        }

        unibn::Octree<FOctreePoint> Octree;
        Octree.initialize(OctreePoints);

        for (const FOctreePoint& PointData : OctreePoints)
        {
            std::vector<uint32_t> Indices;
            Octree.radiusNeighbors<unibn::L2Distance<FOctreePoint>>(
                PointData,
                GLOBAL_SEARCH_RADIUS,
                Indices
            );

            for (uint32_t idx : Indices)
            {
                const FOctreePoint& Other = OctreePoints[idx];
                const float DX = Other.x - PointData.x;
                const float DY = Other.y - PointData.y;
                const float DZ = Other.z - PointData.z;
                const float Dist = FMath::Sqrt(DX * DX + DY * DY + DZ * DZ);

                if (Dist < 1.0f)
                    continue;

                if (Dist < RadiusConfig.MaxRadiusForFullSpread)
                    RadiusConfig.MaxRadiusForFullSpread = Dist;

                if (Dist > RadiusConfig.MinRadiusForFullCluster)
                    RadiusConfig.MinRadiusForFullCluster = Dist;
            }
        }
    }

    if (RadiusConfig.MaxRadiusForFullSpread == MAX_flt)
        RadiusConfig.MaxRadiusForFullSpread = FALLBACK_FULL_SPREAD_RADIUS;

    if (RadiusConfig.MinRadiusForFullCluster <= 0.0f)
        RadiusConfig.MinRadiusForFullCluster = FALLBACK_FULL_CLUSTER_RADIUS;

    return RadiusConfig;
}