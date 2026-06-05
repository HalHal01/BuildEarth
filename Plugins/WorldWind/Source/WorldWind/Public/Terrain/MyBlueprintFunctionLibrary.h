// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyBlueprintFunctionLibrary.generated.h"




/**
 * 
 */
UCLASS()
class WORLDWIND_API UMyBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	template <typename T>
	static double BilinearInter(FVector2D InterPt, TArray<TArray<T>>& RawData);

	UFUNCTION(BlueprintCallable, Category="Elvation")
	static bool GenerateEleDataGte12(int32 TileRow, int32 TileCol, FString FileSavePath);

	static TArray<TArray<float>> ExpandDataBlock(TArray<TArray<float>>& SourceDataArr);

	//UFUNCTION(BlueprintCallable)
	static bool LoadHgtFileToArray(TArray<TArray<uint16>>& Result, const TCHAR* Filename);
	
	UFUNCTION(BlueprintPure, Category="Elvation")
	static FMatrix CameraTest(double Latitude, double Longitude, double Heading, double Tilt);
	
	UFUNCTION(BlueprintPure, Category="Elvation")
	static FRotator Lookat(double Latitude, double Longitude, double Pitch, double Heading, double Roll);


	UFUNCTION(BlueprintPure, Category="Elvation")
	static double Area(const TArray<FVector2D> vets, bool& sss);


};
