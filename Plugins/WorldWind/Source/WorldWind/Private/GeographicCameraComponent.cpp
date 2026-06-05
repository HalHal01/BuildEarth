// Fill out your copyright notice in the Description page of Project Settings.

#include "GeographicCameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values for this component's properties
UGeographicCameraComponent::UGeographicCameraComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UGeographicCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	GetWorld()->GetGameViewport()->Viewport->ViewportResizedEvent.AddUObject(this, &UGeographicCameraComponent::OnViewportResized);
	GetWorld()->GetGameViewport()->GetViewportSize(m_ViewPortSize);

	ComputeDistance(m_Altitude, m_Tilt);
	ComputeAbsoluteMatrices();
}

// Called every frame
void UGeographicCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::SanitizeFloat(m_Distance));
}

void UGeographicCameraComponent::ComputeAbsoluteMatrices()
{
	m_ProjectionMatrix = FReversedZPerspectiveMatrix(
		FMath::Max(0.001f, m_Fov) * (float)PI / 360.0f,
		m_ViewPortSize.X / m_ViewPortSize.Y,
		1.0f,
		GNearClippingPlane);

	m_CameraPosition = UMathEngine::SphericalToCartesian(m_Latitude, m_Longitude, (m_WorldRadius + m_Altitude));
	FMatrix RotationMatrix = FRotationMatrix::MakeFromX(-m_CameraPosition);

	// 旋转顺序的缘故，必须折成两个单独的旋转矩阵
	RotationMatrix = FRotationMatrix(FRotator(m_Tilt, 0, 0)) * FRotationMatrix(FRotator(0, 0, m_Heading)) * RotationMatrix;
	m_CameraRotation = RotationMatrix.Rotator();

	m_ViewMatrix = RotationMatrix.Inverse() * FMatrix(
	FPlane(0, 0, 1, 0),
	FPlane(1, 0, 0, 0),
	FPlane(0, 1, 0, 0),
	FPlane(0, 0, 0, 1));

	m_ViewMatrix = FTranslationMatrix(-m_CameraPosition) * m_ViewMatrix;

	// 另外一种计算方式
	/*const FVector ZAxis = -m_CameraPosition.GetSafeNormal();
	const FVector XAxis = FVector::CrossProduct(m_CameraUpVector, ZAxis).GetSafeNormal();
	const FVector YAxis = FVector::CrossProduct(ZAxis, XAxis);

	// We can build the m_ViewMatrix by hand
	m_ViewMatrix.M[0][0] = XAxis.X;
	m_ViewMatrix.M[1][0] = XAxis.Y;
	m_ViewMatrix.M[2][0] = XAxis.Z;

	m_ViewMatrix.M[0][1] = YAxis.X;
	m_ViewMatrix.M[1][1] = YAxis.Y;
	m_ViewMatrix.M[2][1] = YAxis.Z;

	m_ViewMatrix.M[0][2] = ZAxis.X;
	m_ViewMatrix.M[1][2] = ZAxis.Y;
	m_ViewMatrix.M[2][2] = ZAxis.Z;

	m_ViewMatrix.M[3][0] = -(XAxis.X*m_CameraPosition.X + XAxis.Y*m_CameraPosition.Y + XAxis.Z*m_CameraPosition.Z);
	m_ViewMatrix.M[3][1] = -(YAxis.X*m_CameraPosition.X + YAxis.Y*m_CameraPosition.Y + YAxis.Z*m_CameraPosition.Z);
	m_ViewMatrix.M[3][2] = -(ZAxis.X*m_CameraPosition.X + ZAxis.Y*m_CameraPosition.Y + ZAxis.Z*m_CameraPosition.Z);

	m_ViewMatrix.M[0][3] = 0.0;
	m_ViewMatrix.M[1][3] = 0.0;
	m_ViewMatrix.M[2][3] = 0.0;
	m_ViewMatrix.M[3][3] = 1.0;

	// TODO01:这一步还没有搞清楚对应的几何转换
	m_ViewMatrix *= FInverseRotationMatrix(FRotator(0, -m_Heading, m_Tilt));*/

	m_ViewProjectionMatrix = m_ViewMatrix * m_ProjectionMatrix;

	double factor = m_Altitude / m_WorldRadius;
	if (factor > 1)
	{
		m_ViewRange = 180;
	}
	else
	{
		m_ViewRange = FMath::RadiansToDegrees(FMath::Abs(FMath::Asin((m_Altitude) / m_WorldRadius))*2);
	}

	m_Orientation = FRotator(m_Latitude, 90 - m_Longitude, -m_Heading);
	m_ViewFrustum.Update(m_ViewProjectionMatrix);
}

void UGeographicCameraComponent::SetPosition(double Longitude, double Latitude, double Altitude, double Heading,
	double Tilt, double Bank)
{
	m_Longitude = FMath::Clamp(Longitude, -180, 180);
	m_Latitude = FMath::Clamp(Latitude, -90, 90);
	m_Altitude = FMath::Clamp(Altitude, -180, 180);
	m_Heading = FMath::Clamp(Heading, -180, 180);
	m_Tilt = FMath::Clamp(Tilt, 0, 180);
	m_Bank = FMath::Clamp(Bank, -180, 180);

	ComputeAbsoluteMatrices();
}

bool UGeographicCameraComponent::PickingRayIntersection(int32 ScreenX, int32 ScreenY, FVector& Intersection)
{
	FMatrix const InvViewProjMatrix = m_ViewProjectionMatrix.Inverse();
	// Get the eye position and direction of the mouse cursor in two stages (inverse transform projection, then inverse transform view).
	// This avoids the numerical instability that occurs when a view matrix with large translation is composed with a projection matrix

	// Get the pixel coordinates into 0..1 normalized coordinates within the constrained view rectangle
	const float NormalizedX = (float)(ScreenX) / m_ViewPortSize.X;
	const float NormalizedY = (float)(ScreenY) / m_ViewPortSize.Y;

	// Get the pixel coordinates into -1..1 projection space
	const float ScreenSpaceX = (NormalizedX - 0.5f) * 2.0f;
	const float ScreenSpaceY = ((1.0f - NormalizedY) - 0.5f) * 2.0f;

	// The start of the ray trace is defined to be at mousex,mousey,1 in projection space (z=1 is near, z=0 is far - this gives us better precision)
	// To get the direction of the ray trace we need to use any z between the near and the far plane, so let's use (mousex, mousey, 0.5)
	const FVector4 RayStartProjectionSpace = FVector4(ScreenSpaceX, ScreenSpaceY, 1.0f, 1.0f);
	const FVector4 RayEndProjectionSpace = FVector4(ScreenSpaceX, ScreenSpaceY, 0.5f, 1.0f);

	// Projection (changing the W coordinate) is not handled by the FMatrix transforms that work with vectors, so multiplications
	// by the projection matrix should use homogeneous coordinates (i.e. FPlane).
	const FVector4 HGRayStartWorldSpace = InvViewProjMatrix.TransformFVector4(RayStartProjectionSpace);
	const FVector4 HGRayEndWorldSpace = InvViewProjMatrix.TransformFVector4(RayEndProjectionSpace);
	FVector RayStartWorldSpace(HGRayStartWorldSpace.X, HGRayStartWorldSpace.Y, HGRayStartWorldSpace.Z);
	FVector RayEndWorldSpace(HGRayEndWorldSpace.X, HGRayEndWorldSpace.Y, HGRayEndWorldSpace.Z);
	// divide vectors by W to undo any projection and get the 3-space coordinate
	if (HGRayStartWorldSpace.W != 0.0f)
	{
		RayStartWorldSpace /= HGRayStartWorldSpace.W;
	}
	if (HGRayEndWorldSpace.W != 0.0f)
	{
		RayEndWorldSpace /= HGRayEndWorldSpace.W;
	}

	FTwoVectors Intersections;

	if (UMathEngine::RayIntersectSphere(RayStartWorldSpace, RayEndWorldSpace, FVector::ZeroVector, m_WorldRadius, Intersections))
	{
		Intersection = UMathEngine::CartesianToSpherical(Intersections.v1);
		/*FVector SphericalCoord = UMathEngine::CartesianToSpherical(Intersections.v1);

		Latitude = SphericalCoord.Z;
		Longitude = SphericalCoord.Y;*/
		return true;
	}

	return false;
}



bool UGeographicCameraComponent::Project(const FVector& WorldPosition, FVector2D& ScreenPosition)
{
	FPlane Result = m_ViewProjectionMatrix.TransformFVector4(FVector4(WorldPosition, 1.f));
	if ( Result.W > 0.0f )
	{
		// the result of this will be x and y coords in -1..1 projection space
		const float RHW = 1.0f / Result.W;
		FPlane PosInScreenSpace = FPlane(Result.X * RHW, Result.Y * RHW, Result.Z * RHW, Result.W);

		// Move from projection space to normalized 0..1 UI space
		const float NormalizedX = ( PosInScreenSpace.X / 2.f ) + 0.5f;
		const float NormalizedY = 1.f - ( PosInScreenSpace.Y / 2.f ) - 0.5f;

		FVector2D RayStartViewRectSpace(
			( NormalizedX * (float)m_ViewPortSize.X ),
			( NormalizedY * (float)m_ViewPortSize.Y )
			);

		ScreenPosition = RayStartViewRectSpace /*+ FVector2D(static_cast<float>(ViewRect.Min.X), static_cast<float>(ViewRect.Min.Y))*/;

		return true;
	}

	return false;
}



// Roll 暂时不管
// Pitch 向上
// Yaw 向左
void UGeographicCameraComponent::RotationYawPitchRoll(double Yaw, double Pitch, double Roll)
{
	m_Orientation = UKismetMathLibrary::ComposeRotators(FRotator(Pitch, Yaw, Roll), m_Orientation);

	m_Heading = -m_Orientation.Roll;
	m_Latitude = m_Orientation.Pitch;
	m_Longitude = 90 - m_Orientation.Yaw;
	// TODO01:需要做边界检查

	ComputeAbsoluteMatrices();
}

void UGeographicCameraComponent::addHeading(double DeltaHeading, double DeltaTilt)
{
	double TempHeading = m_Heading + DeltaHeading;
	if (TempHeading < -180)
	{
		m_Heading = 360 + TempHeading;
	}
		
	
	else if (TempHeading > 180)
	{
		m_Heading = TempHeading - 360;
	}
		
	else
	{
		m_Heading = TempHeading;
	}
		
	
	m_Tilt = FMath::Clamp(m_Tilt + DeltaTilt, 0, 85);
	ComputeAbsoluteMatrices();
}


// 默认缩放至鼠标处
void UGeographicCameraComponent::Zoom(double Percent)
{

	float MouseX = 0.f;
	float MouseY = 0.f;
	FVector Target;
	// 获取鼠标信息
	bool onEarth = PickingRayIntersection(MouseX, MouseY, Target);

	

	if (onEarth)
	{



		// 计算旋转四元数
		FQuat rotCenterToTarget = FQuat::FindBetweenVectors(m_Center, Target);




	
		FQuat rot = FQuat::Slerp(FQuat::Identity, rotCenterToTarget, Percent);
		m_Center = rot.RotateVector(m_Center);
	}


	if(Percent>0)
	{
		m_Distance = FMath::Clamp(m_Distance/(1.0 + Percent), 0, m_WorldRadius*10);
	}
	else
	{
		m_Distance = FMath::Clamp(m_Distance*(1.0 - Percent), 0, m_WorldRadius*10);
	}

	ComputeAltitude(m_Distance, m_Tilt);
}


/// <summary>
/// Digital zoom (keyboard/mouse wheel style)
/// </summary>
/// <param name="ticks">Positive value for zoom in, negative for zoom out.</param>
void UGeographicCameraComponent::ZoomStepped(float ticks)
{
	/*int currentTickCount = Environment.TickCount;

	double factor = World.Settings.cameraZoomStepFactor;
	if(factor<0)
		factor = 0;
	if (factor > 1)
		factor = 1;

	double minTime = 50;  // <= 50ms: fastest
	double maxTime = 250; // >=250ms: slowest
	double time = currentTickCount - lastStepZoomTickCount;
	if (time<minTime)
		time = minTime;
	double multiplier = 1-Math.Abs( (time-minTime)/maxTime ); // Range: 1 .. 2
	if(multiplier<0)
		multiplier=0;

	multiplier= multiplier * World.Settings.cameraZoomAcceleration;
	double mulfac = Math.Pow(1 - factor, multiplier+1 );
	mulfac = Math.Pow(mulfac, Math.Abs(ticks));*/

	/*if (ticks > 0)
		TargetDistance *= mulfac;
	else
		TargetDistance /= mulfac;*/
	m_Distance = m_Distance*(1 + ticks/10);
	ComputeAltitude(m_Distance, m_Tilt);

	//lastStepZoomTickCount = currentTickCount;
}

void UGeographicCameraComponent::Pan(double DeltaLatitude, double DeltaLongitude)
{
	double TargetLatitude, TargetLongitude;

	// TODO01:在极地和0经度上需要特殊处理
	TargetLongitude = FMath::Clamp(m_Longitude + DeltaLongitude, -180, 180);
	TargetLatitude = FMath::Clamp(m_Latitude + DeltaLatitude, -90, 90);

	m_Orientation = FRotator(TargetLatitude, 90 - TargetLongitude, -m_Heading);

	m_Latitude = TargetLatitude;
	m_Longitude = TargetLongitude;

	ComputeAbsoluteMatrices();
}

void UGeographicCameraComponent::ComputeDistance(double Altitude, double Tilt)
{
	double cos = FMath::Cos(FMath::DegreesToRadians(Tilt));
	double hyp = m_WorldRadius + Altitude;
		
	double y = FMath::Sqrt(FMath::Square(hyp * cos) - FMath::Square(hyp) + FMath::Square(m_WorldRadius));
	double res = hyp * cos - y;
	if (res < 0)
	{
		res = hyp * cos + y;
	}

	m_Distance = res;
}

void UGeographicCameraComponent::ComputeAltitude(double Distance, double Tilt)
{
	double costilt = FMath::Cos(FMath::DegreesToRadians(Tilt));
	
	double dfromeq = FMath::Sqrt(FMath::Square(Distance*costilt) - FMath::Square(Distance) + FMath::Square(m_WorldRadius));
	double y = Distance*costilt + dfromeq;
	double alt = y - m_WorldRadius;
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::SanitizeFloat(alt));

	// TODO01:高程拉伸没有考虑
	m_Altitude = FMath::Clamp(alt, 0, m_WorldRadius*10);

	ComputeAbsoluteMatrices();
}

void UGeographicCameraComponent::ComputeTilt(double Altitude, double Distance)
{
	double a = m_WorldRadius + Altitude;
	double b = Distance;
	double c = m_WorldRadius;
	m_Tilt = FMath::RadiansToDegrees(FMath::Acos((a*a + b*b - c*c) / (2*a*b)));

	ComputeAbsoluteMatrices();
}

void UGeographicCameraComponent::OnViewportResized(FViewport* ViewPort, uint32 a)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, ViewPort->GetSizeXY().ToString());
	m_ViewPortSize.Set(ViewPort->GetSizeXY().X,  ViewPort->GetSizeXY().Y);
	ComputeAbsoluteMatrices();
}