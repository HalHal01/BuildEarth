// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "MathEngine.h"

/**
 * 
 */
struct WORLDWIND_API FBoundingBox
{
public:
	FBoundingBox();
	//~BoundingBox();

	TArray<FVector> Corners;

	/// <summary>
	/// Initializes a new instance of the <see cref= "T:WorldWind.BoundingSphere"/> class.
	/// </summary>
	/// <param name="V0"></param>
	/// <param name="V1"></param>
	/// <param name="V2"></param>
	/// <param name="V3"></param>
	/// <param name="V4"></param>
	/// <param name="V5"></param>
	/// <param name="V6"></param>
	/// <param name="V7"></param>
	FBoundingBox(FVector V0, FVector V1, FVector V2, FVector V3, FVector V4, FVector V5, FVector V6, FVector V7)
	{
		Corners.Init(FVector::ZeroVector, 8);
		Corners[0] = V0;
		Corners[1] = V1;
		Corners[2] = V2;
		Corners[3] = V3;
		Corners[4] = V4;
		Corners[5] = V5;
		Corners[6] = V6;
		Corners[7] = V7;
	}

		/// <summary>
		/// Initializes a new instance of the <see cref= "T:WorldWind.BoundingSphere"/> class
		/// from a set of lat/lon values (degrees)
		/// </summary>
		/// <param name="South"></param>
		/// <param name="North"></param>
		/// <param name="West"></param>
		/// <param name="East"></param>
		/// <param name="Radius1"></param>
		/// <param name="Radius2"></param>
		FBoundingBox(float South, float North, float West, float East, float Radius1, float Radius2)
		{
			double Scale = Radius2 / Radius1;
			Corners.Init(FVector::ZeroVector, 8);
			Corners[0] = UMathEngine::SphericalToCartesian(South, West, Radius1);
			Corners[1] = Corners[0] * Scale;
			Corners[2] = UMathEngine::SphericalToCartesian(South, East, Radius1);
			Corners[3] = Corners[2] * Scale;
			Corners[4] = UMathEngine::SphericalToCartesian(North, West, Radius1);
			Corners[5] = Corners[4] * Scale;
			Corners[6] = UMathEngine::SphericalToCartesian(North, East, Radius1);
			Corners[7] = Corners[6] * Scale;
		}

		FVector CalculateCenter()
		{
			FVector res = FVector::ZeroVector;
			for(auto& corner: Corners)
			{
				res += corner;
			}

			res *= (1.0f / Corners.Num());
			return res;
		}

		/// <summary>
		/// Calculate the screen area (pixels) covered by the bottom of the bounding box.
		/// </summary>
		float CalcRelativeScreenArea(class UGeographicCameraComponent* camera)
		{
			/*FVector2D a, b, c, d;
			camera->Project(Corners[0], a);
			camera->Project(Corners[2], b);
			camera->Project(Corners[6], c);
			camera->Project(Corners[4], d);

			FVector2D ab = b - a;
			FVector2D ac = c - a;
			FVector2D ad = d - a;

			float tri1SqArea = FVector2D::CrossProduct(ab, ac)/*.LengthSq()#1#; 
			float tri2SqArea = FVector2D::CrossProduct(ad, ac)/*.LengthSq()#1#; 
			// Real area = (sqrt(tri1SqArea)+sqrt(tri2SqArea))/2 but we're only interested in relative size
			return tri1SqArea + tri2SqArea; */
		return 0;
		}
	
};

inline FBoundingBox::FBoundingBox()
{
	Corners.Init(FVector::ZeroVector, 8);
}
