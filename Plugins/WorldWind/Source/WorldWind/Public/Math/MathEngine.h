// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "Kismet/BlueprintFunctionLibrary.h"
#include "MathEngine.generated.h"

/**
 * 
 */
UCLASS()
class WORLDWIND_API UMathEngine : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// 传入经纬度坐标及半径，计算在球面上的笛卡尔坐标
	// 角度单位均为度
	UFUNCTION(BlueprintPure, Category="MathEngine")
	static FVector SphericalToCartesian(double Latitude, double Longitude, double Radius)
	{
		double RadLatitude = FMath::DegreesToRadians(Latitude);
		double RadLongitude = FMath::DegreesToRadians(Longitude);

		double RadCosLat = Radius * FMath::Cos(RadLatitude);
		return FVector(
			RadCosLat * FMath::Sin(RadLongitude),
			RadCosLat * FMath::Cos(RadLongitude),
			Radius * FMath::Sin(RadLatitude));
	}
	/// <summary>
	/// Computes the angular distance between two pairs of lat/longs.
	/// Fails for distances (on earth) smaller than approx. 2km. (returns 0)
	/// </summary>
	/// 全部用的是度来计算，返回的也是度
	UFUNCTION(BlueprintPure, Category="MathEngine")
	static double SphericalDistance(double LatA, double LonA, double LatB, double LonB)
	{
		double RadLatA = FMath::DegreesToRadians(LatA);
		double RadLatB = FMath::DegreesToRadians(LatB);
		double RadLonA = FMath::DegreesToRadians(LonA);
		double RadLonB = FMath::DegreesToRadians(LonB);

		return FMath::RadiansToDegrees(FMath::Acos(
			FMath::Cos(RadLatA) * FMath::Cos(RadLatB) * FMath::Cos(RadLonA - RadLonB) +
			FMath::Sin(RadLatA) * FMath::Sin(RadLatB)));
	}

	// 从笛卡尔坐标转换为 Planet 上的经纬度
	// 返回值是度
	UFUNCTION(BlueprintPure, Category="MathEngine")
	static FVector CartesianToSpherical(FVector CartesianCoord)
	{
		double rho = CartesianCoord.Length();
		double Longitude = FMath::RadiansToDegrees(FMath::Atan2(CartesianCoord.X, CartesianCoord.Y));
		double Latitude = FMath::RadiansToDegrees(FMath::Asin(CartesianCoord.Z / rho));

		return FVector(rho, Longitude, Latitude);
	}

	/*
	/// <summary>
	/// Compute the tile number (used in file names) for given latitude and tile size.
	/// </summary>
	/// <param name="latitude">Latitude (decimal degrees)</param>
	/// <param name="tileSize">Tile size  (decimal degrees)</param>
	/// <returns>The tile number</returns>
	static int GetRowFromLatitude(double latitude, double tileSize)
	{
		return (int)FMath.Round((FMath::Abs(-90.0 - latitude)%180)/tileSize, 1);
	}
	*/

	/// <summary>
	/// Compute the tile number (used in file names) for given latitude and tile size.
	/// </summary>
	/// <param name="Latitude">Latitude (decimal degrees)</param>
	/// <param name="TileSize">Tile size  (decimal degrees)</param>
	/// <returns>The tile number</returns>
	/// 需考虑异常情况
	static int32 GetRowFromLatitude(double Latitude, double TileSize)
	{
		return FMath::FloorToInt32(FMath::Abs(90.0 - Latitude)/TileSize);
	}


	/// <summary>
	/// Compute the tile number (used in file names) for given longitude and tile size.
	/// </summary>
	/// <param name="Longitude">Longitude (decimal degrees)</param>
	/// <param name="TileSize">Tile size  (decimal degrees)</param>
	/// <returns>The tile number</returns>
	/// 需考虑异常情况
	static int32 GetColFromLongitude(double Longitude, double TileSize)
	{
		return FMath::FloorToInt32((Longitude + 180)/TileSize);
	}

	/*/// <summary>
	/// Compute the tile number (used in file names) for given longitude and tile size.
	/// </summary>
	/// <param name="longitude">Longitude (decimal degrees)</param>
	/// <param name="tileSize">Tile size  (decimal degrees)</param>
	/// <returns>The tile number</returns>
	static int GetColFromLongitude(double longitude, double tileSize)
	{
		return (int)FMath.Round((FMath.Abs(-180.0 - longitude.Degrees)%360)/tileSize, 1);
	}*/

	// The following code is from SkyAtmosphere.usf
	// 射线与球的交点，应该返回是否有交点的，
	UFUNCTION(BlueprintPure, Category="MathEngine")
	static bool RayIntersectSphere(FVector RayStart, FVector RayEnd, FVector SphereOrigin, float SphereRadius, FTwoVectors& Intersections)
	{
		FVector RayDirection = RayEnd - RayStart;
		FVector LocalPosition = RayStart - SphereOrigin;
		double LocalPositionSqr = FVector::DotProduct(LocalPosition, LocalPosition);

		FVector QuadraticCoef;
		QuadraticCoef.X = FVector::DotProduct(RayDirection, RayDirection);
		QuadraticCoef.Y = 2.0f * FVector::DotProduct(RayDirection, LocalPosition);
		QuadraticCoef.Z = LocalPositionSqr - SphereRadius * SphereRadius;

		double Discriminant = QuadraticCoef.Y * QuadraticCoef.Y - 4.0f * QuadraticCoef.X * QuadraticCoef.Z;

		// Only continue if the ray intersects the sphere
		if (Discriminant >= 0)
		{
			double SqrtDiscriminant = FMath::Sqrt(Discriminant);
			double near = (-QuadraticCoef.Y - 1.0f * SqrtDiscriminant) / (2 * QuadraticCoef.X);
			double far = (-QuadraticCoef.Y + 1.0f * SqrtDiscriminant) / (2 * QuadraticCoef.X);

			Intersections.v1 = RayStart + near*(RayEnd - RayStart);
			Intersections.v2 = RayStart + far*(RayEnd - RayStart);
			return true;
		}

		return false;
	}

	
	// Returns true if the points forming a polygon have CCW winding
	// Returns true if the polygon isn't valid
	static bool IsPolygonWindingCCW(const TArray<FVector2D>& Points)
	{
		float Sum = 0.0f;
		const int PointCount = Points.Num();
		for (int PointIndex = 0; PointIndex < PointCount; ++PointIndex)
		{
			const FVector2D& A = Points[PointIndex];
			const FVector2D& B = Points[(PointIndex + 1) % PointCount];
			Sum += (B.X - A.X) * (B.Y + A.Y);
		}
		return (Sum < 0.0f);
	}

	// 多边形面积
	// 顺时针方向（虚幻引擎为逆时针），第一个点与最后一个点不等
	UFUNCTION(BlueprintPure, Category="MathEngine")
	static double PolygonArea(TArray<FVector2D> Vertices)
	{
		// 检查数据是否合规
		if (!UMathEngine::IsPolygonWindingCCW(Vertices))
		{
			FVector2D Temp;
			int32 ArraySize = Vertices.Num();
			for (int32 i = 0; i < ArraySize / 2; i++)
			{
				Temp = Vertices[i];
				Vertices[i] = Vertices[ArraySize - 1 - i];
				Vertices[ArraySize - 1 - i] = Temp;
			}
		}

		// If there are less than 3 verts
		if(Vertices.Num() < 3)
		{
			return 0;
		}
		
		FVector Side1, Side2;
		Side1 = FVector(Vertices[1] - Vertices[0], 0);
		double Area = 0;
		for(int32 i = 2; i < Vertices.Num(); i++)
		{
			Side2 = FVector(Vertices[i] - Vertices[0], 0);
			Area += (Side1 ^ Side2).Size() * 0.5;
			Side1 = Side2;
		}
		return Area;
	}

	UFUNCTION(BlueprintPure, Category="MathEngine")
	static FString DecDegreesToDMS(double DecDegree)
	{
		int32 Degree = FMath::TruncToInt32(DecDegree);
		int32 Minute = FMath::TruncToInt32(FMath::Frac(DecDegree) * 60);
		float Second = (FMath::Frac(DecDegree) - (float)Minute*60) * 3600;
		return FString::Printf(TEXT("%dE%02d%f"), Degree, Minute, Second);
	}
};
