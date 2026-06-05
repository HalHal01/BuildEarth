// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WorldWindSettings.generated.h"

/**
 * 
 */
UCLASS(Config = WorldWind, defaultconfig)
// config : 此属性将被设为可配置。当前值可被存入与类相关的 .ini 文件中，创建后将被加载。无法在默认属性中给定一个值。暗示为 BlueprintReadOnly。
class WORLDWIND_API UWorldWindSettings : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Config, Category = Http)
	FString EleTileBaseUrl = FString("http://10.67.8.90:8000/media/_alllayers");

	UPROPERTY(EditAnywhere, Config, Category = Http)
	FString ImgTileBaseUrl = FString("http://10.67.8.90:8000/media/_alllayers");

	UPROPERTY(EditAnywhere, Config, Category = OnlineTile)
	FString Token = FString(TEXT("fd011184766f373818e54ac526e22eca"));

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif


	float VerticalExaggeration = 0.1f;

	bool CameraTwistLock = true;
	float CameraRotationSpeed = 1.0;

	float Opacity = 1.f;

	bool CameraBankLock = false;
	bool CameraZoomAnalogFactor = false;
	
};
