// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Math/Frustum.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GeographicCameraComponent.generated.h"


/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WORLDWIND_API UGeographicCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGeographicCameraComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 窗口尺寸改变时的回调函数
	void OnViewportResized(FViewport* ViewPort, uint32 a);



	UFUNCTION(BlueprintGetter)
	FVector CameraPosition() const
	{
		return m_CameraPosition;
	}
	
public:
	double m_WorldRadius = 1000000.0; // The planet's radius in meters.默认为1000公里

	UPROPERTY(BlueprintReadOnly)
	double m_Latitude = 30.43208599;  // 大约在家的位置

	UPROPERTY(BlueprintReadOnly)  // 大约在家的位置
	double m_Longitude = 104.07214522;

	UPROPERTY(BlueprintReadOnly)
	double m_Altitude = 20000.0; // Altitude above sea level

	UPROPERTY(BlueprintReadOnly)
	double m_Heading = 15; // 就是测绘里面的方位角，与北方向的夹角，顺时针方向

	UPROPERTY(BlueprintReadOnly)
	double m_Tilt = 15.0; // 视线方向与球心的夹角

	UPROPERTY(BlueprintReadOnly)
	double m_Bank = 0; // TODO01：当于Yaw，相关功能还没有做

	UPROPERTY(BlueprintReadOnly)
	double m_Distance; // Distance from eye to target

	UPROPERTY(BlueprintReadWrite, BlueprintGetter=CameraPosition, Category="Media")
	FVector m_CameraPosition; // Camera position (World XYZ coordinates)

	UPROPERTY(BlueprintReadOnly)
	FRotator m_CameraRotation; // Camera Rotation

	float m_Fov = 90.0f;  // 相机水平视场角
	FFrustum m_ViewFrustum;
	FVector2D m_ViewPortSize;
	double m_ViewRange;

	FMatrix m_ProjectionMatrix; // Projection matrix used in last render.
	FMatrix m_ViewMatrix; // View matrix used in last render.
	UPROPERTY(BlueprintReadOnly)
	FMatrix m_ViewProjectionMatrix;
	FRotator m_Orientation; // protected Quaternion _orientation;

	FVector m_CameraUpVector = FVector(0, 0, 1);

	// ------------------------------------------------------------
	// 视线与地球表面的交点
	FVector m_Center; // Camera position (World XYZ coordinates)

	bool m_LastPointOnEarth = false;

public:


	FRotator CameraRotation() const
	{
		return m_CameraRotation;
	}

	void ComputeAbsoluteMatrices();

	// Sets camera position.所有参数单位均为十进制度
	void SetPosition(double Longitude, double Latitude, double Altitude, double Heading, double Tilt, double Bank);

	void SetPosition(double Longitude, double Latitude, double Altitude, double Heading, double Tilt)
	{
		SetPosition(Longitude, Latitude, Altitude, Heading, Tilt, m_Bank);
	}

	void SetPosition(double Longitude, double Latitude, double Altitude)
	{
		SetPosition(Longitude, Latitude, Altitude, m_Heading, m_Tilt, m_Bank);
	}

	void SetPosition(double Longitude, double Latitude)
	{
		SetPosition(Longitude, Latitude, m_Altitude, m_Heading, m_Tilt, m_Bank);
	}

	// Calculates latitude and longitude for given screen coordinate.
	bool PickingRayIntersection(int32 ScreenX, int32 ScreenY, FVector& Intersection);
	
	// Calculates latitude and longitude for given screen coordinate.
	bool PickingRayIntersection(int32 ScreenX, int32 ScreenY, double& Intersection, double& Distance)
	{
		return false;
	}

	UFUNCTION(BlueprintCallable)
	bool Project(const FVector& WorldPosition, FVector2D& ScreenPosition);

	void RotationYawPitchRoll(double Yaw, double Pitch, double Roll);

	void addHeading(double DeltaHeading, double DeltaTilt);
	
	// Zoom camera in/out (distance) 
	void Zoom(double Percent);

	void ZoomStepped(float ticks);

	// Pan the camera using delta values
	void Pan(double DeltaLatitude, double DeltaLongitude);

	// 用一元二次方程来求解
	void ComputeDistance(double Altitude, double Tilt);

	// 
	void ComputeAltitude(double Distance, double Tilt);

	// Distance：视线与球的交点的长度
	void ComputeTilt(double Altitude, double Distance);
};