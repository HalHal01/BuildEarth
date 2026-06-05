// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Compass3D.generated.h"

/**
 * 
 */
UCLASS()
class WORLDWIND_API UCompass3DWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* SourceTexture2D;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* TargetTexture2D;

	// 缓存阈值 - 只有当角度变化超过此值时才更新纹理
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Optimization")
	float UpdateThreshold = 0.5f;

	virtual void NativeConstruct() override;
	/*virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;*/

	UFUNCTION(BlueprintCallable)
	void UpdateTexture(double Heading, double Tilt);
	
	TArray<FColor> SourcePixelColors;
	TArray<FColor> TargetPixelColors;
	
private:
	// 缓存变量，用于避免重复计算
	double LastHeading = 0.0;
	double LastTilt = 0.0;
	FMatrix LastViewProjectionMatrix;
	bool bMatrixIsValid = false;
public:
	bool Project(const FVector& WorldPosition, const FMatrix& ViewProjectionMatrix, const int32 Width, const int32 Height, FVector2D& ScreenPosition);

public:
	UFUNCTION(BlueprintCallable)
	bool ReMap(UPARAM(Ref) FMatrix& ViewProjectionMatrix);
};
