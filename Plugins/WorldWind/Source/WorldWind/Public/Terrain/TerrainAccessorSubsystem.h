// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TerrainTile.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TerrainAccessorSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class WORLDWIND_API UTerrainAccessorSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()


public:
	FTerrainTile GetElevationArray(int32 Row, int32 Col, int32 Level);
};

inline FTerrainTile UTerrainAccessorSubsystem::GetElevationArray(int32 Row, int32 Col, int32 Level)
{
	FTerrainTile  RST = FTerrainTile(Row, Col, Level);
	RST.Initialize();
	return RST;
}
