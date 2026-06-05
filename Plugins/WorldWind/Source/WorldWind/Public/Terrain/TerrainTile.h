// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"


/**
 * 
 */

class WORLDWIND_API FTerrainTile
{
	//GENERATED_BODY()

public:
	FString TerrainTileFilePath;
	double TileSizeDegrees;
	int32 SamplesPerTile = 8;
	double South;
	double North;
	double West;
	double East;
	int32 Row;
	int32 Col;
	int32 TargetLevel;
	//TerrainTileService m_owner;
	bool IsInitialized;
	bool IsValid;

	TArray<TArray<float>> ElevationData;
	//protected TerrainDownloadRequest request;

	FTerrainTile(){}

	FTerrainTile(int32 Row, int32 Col, int32 Level /*TerrainTileService owner */);
		/*{
			int32 Row;
			int32 Col;
			int32 TargetLevel;
		}*/
		/// <summary>
		/// This method initializes the terrain tile add switches to
		/// Initialize floating point/int 16 tiles
		/// </summary>
	void Initialize();

	// 二次线性插值
	float GetElevationAt(double Latitude, double Longitude);



		/*void Dispose()
		{
			if(request != null)
			{
				request.Dispose();
				request = null;
			}
			
			GC.SuppressFinalize(this);
		}*/
};


