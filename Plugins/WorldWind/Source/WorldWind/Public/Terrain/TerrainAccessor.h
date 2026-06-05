// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TerrainTile.h"
#include "UObject/NoExportTypes.h"
#include "TerrainAccessor.generated.h"

/**
 * 
 */
UCLASS()
class WORLDWIND_API UTerrainAccessor : public UObject
{
	GENERATED_BODY()
	
		FString m_name;
		double m_north;
		double m_south;
		double m_east;
		double m_west;



		/*/// <summary>
		/// Gets the terrain elevation at a given Latitude, Longitude, 
		/// and resolution accuracy in the latitude/longitude geographic frame of reference.
		/// </summary>
		/// <param name="latitude">Latitude in decimal degrees.</param>
		/// <param name="longitude">Longitude in decimal degrees.</param>
		/// <param name="targetSamplesPerDegree"></param>
		/// <returns>Returns 0 if the tile is not available on disk.</returns>
		public abstract float GetElevationAt(double latitude, double longitude, double targetSamplesPerDegree);*/

		/*/// <summary>
		/// Get terrain elevation at specified location.  
		/// </summary>
		/// <param name="latitude">Latitude in decimal degrees.</param>
		/// <param name="longitude">Longitude in decimal degrees.</param>
		/// <returns>Returns 0 if the tile is not available on disk.</returns>
		virtual float GetElevationAt(double latitude, double longitude)
		{
			return GetElevationAt(latitude, longitude, 0);
		}*/

		/*/// <summary>
		/// Gets the elevation array for given geographic bounding box and resolution.
		/// </summary>
		/// <param name="north">North edge in decimal degrees.</param>
		/// <param name="south">South edge in decimal degrees.</param>
		/// <param name="west">West edge in decimal degrees.</param>
		/// <param name="east">East edge in decimal degrees.</param>
		/// <param name="samples"></param>
		virtual FTerrainTile GetElevationArray(double north, double south, double west, double east, int samples)
		{
			TerrainTile res = null;
			res = new TerrainTile(null);
			res.North = north;
			res.South = south;
			res.West = west;
			res.East = east;
			res.SamplesPerTile = samples;
			res.IsInitialized = true;
			res.IsValid = true;

			double latrange = FMath.Abs(north - south);
			double lonrange = FMath.Abs(east - west);

			float[,] data = new float[samples,samples];
			float scaleFactor = (float)1/(samples - 1);
			for(int x = 0; x < samples; x++)
			{
				for(int y = 0; y < samples; y++)
				{
					double curLat = north - scaleFactor * latrange * x;
					double curLon = west + scaleFactor * lonrange * y;

					data[x,y] = GetElevationAt(curLat, curLon, 0);
				}
			}
			res.ElevationData = data;

			return res;
		}*/

		/*public virtual void Dispose()
		{
		}*/
	
};
