// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BoundingBox.h"

/**
 * 
 */
struct FFrustum
{
public:
	TArray<FPlane> m_Planes;

public:
	// Default constructor (no initialization). 
	FFrustum()
	{
		m_Planes.Init(FPlane(), 6);
	}
	
	void Update(FMatrix Mat)
	{
		m_Planes.Init(FPlane(), 6);
		//bottom (down) plane
		m_Planes[0] = FPlane(
			Mat.M[0][3] + Mat.M[0][1], Mat.M[1][3] + Mat.M[1][1], Mat.M[2][3] + Mat.M[2][1], Mat.M[3][3] + Mat.M[3][1]);
		//far plane
		m_Planes[1] = FPlane(
			Mat.M[0][3] - Mat.M[0][2], Mat.M[1][3] - Mat.M[1][2], Mat.M[2][3] - Mat.M[2][2], Mat.M[3][3] - Mat.M[3][2]);
		//right side plane
		m_Planes[2] = FPlane(
			Mat.M[0][3] - Mat.M[0][0], Mat.M[1][3] - Mat.M[1][0], Mat.M[2][3] - Mat.M[2][0], Mat.M[3][3] - Mat.M[3][0]);
		//left side plane
		m_Planes[3] = FPlane(
			Mat.M[0][3] + Mat.M[0][0], Mat.M[1][3] + Mat.M[1][0], Mat.M[2][3] + Mat.M[2][0], Mat.M[3][3] + Mat.M[3][0]);
		//near plane
		m_Planes[4] = FPlane(
			Mat.M[0][2], Mat.M[1][2], Mat.M[2][2], Mat.M[3][2]);
		//top (up) plane
		m_Planes[5] = FPlane(
			Mat.M[0][3] - Mat.M[0][1], Mat.M[1][3] - Mat.M[1][1], Mat.M[2][3] - Mat.M[2][1], Mat.M[3][3] - Mat.M[3][1]);

		for (FPlane Plane : m_Planes)
			Plane.Normalize();
	}

	// Test if a sphere intersects or is completely inside the frustum.
	// <returns>true when the sphere intersects.</returns>
	bool Intersects(FSphere c)
	{
		for (FPlane p : m_Planes)
		{
			float DistancePlaneToPoint = p.X * c.Center.X + p.Y * c.Center.Y + p.Z * c.Center.Z + p.W;
			if (DistancePlaneToPoint < -c.W)
			{
				// More than 1 radius outside the plane = outside
				return false;
			}
		}
		// else it's in view
		return true;
	}

	// Test if a point is inside the frustum.
	// <returns>true when the point is inside.</returns>
	// <param name="v">XYZ in world coordinates of the point to test.</param>
	bool ContainsPoint(FVector v)
	{
		for (FPlane p : m_Planes)
			if (FVector(p.X, p.Y, p.Z).Dot(v) + p.W < 0)
				return false;

		return true;
	}

	// Tests if the view frustum fully contains the bounding box.
	// <returns>true when the box is complete enclosed by the frustum.</returns>
	bool Contains(FBoundingBox BB)
	{
		//Code taken from Flip Code Article:
		// http://www.flipcode.com/articles/article_frustumculling.shtml
		int iTotalIn = 0;
		for (FPlane p : m_Planes)
		{
			int iInCount = 8;
			int iPtIn = 1;
			// 等待实现: Modify bounding box and only check 2 corners.
			for(int i = 0; i < 8; i++)
			{
				if (FVector(p.X, p.Y, p.Z).Dot(BB.Corners[i]) + p.W < 0)
				{
					iPtIn = 0;
					--iInCount;
				}
			}

			if(iInCount == 0)
			{
				return false;
			}

			iTotalIn += iPtIn;
		}

		if(iTotalIn == 6)
		{
			return true;
		}
		return false;
	}

	// Tests if the bounding box specified intersects with or is fully contained in the frustum.
	// <returns>true when the box intersects with the frustum.</returns>
	bool Intersects(FBoundingBox BB)
	{
		for (FPlane p :m_Planes)
		{
			FVector v = FVector(p.X, p.Y, p.Z);
			bool isInside = false;
			// 等待实现: Modify bounding box and only check 2 corners.
			for(int i = 0; i < 8; i++)
			{
				if(v.Dot(BB.Corners[i]) + p.W >= 0)
				{
					isInside = true;
					break;
				}
			}

			if(!isInside)
				return false;
		}

		return true;
	}

	FString ToString()
	{
		FString Res = FString::Printf(TEXT("Near:%s, Far:%s"), *m_Planes[4].ToString(), *m_Planes[1].ToString());
		return Res;
	}
};
