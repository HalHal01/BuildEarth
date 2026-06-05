// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "QuadTile.h"
#include "Misc.h"
//#include "QuadTile.h"
#include "Interfaces/IHttpRequest.h"
//#include "UObject/NoExportTypes.h"
#include "DownloadRequest.generated.h"


class UQuadTile;

//DECLARE_DELEGATE_TwoParams(FDownloadImageDelegate, UTexture2D*, EChildLocation);



class UTexture2DDynamic;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDownloadImageDelegate, UTexture2DDynamic*, Texture, UQuadTile*, hh);









/// <summary>
/// Base class for geo-spatial download requests
/// </summary>
UCLASS()
class WORLDWIND_API UGeoSpatialDownloadRequest : public UObject
{
	GENERATED_BODY()

public:
	//UGeoSpatialDownloadRequest();
	UFUNCTION(BlueprintCallable/*, meta=( BlueprintInternalUseOnly="true" )*/)
	static UGeoSpatialDownloadRequest* DownloadImage(FString URL);

	void LoadLocal(const FString& Filename);
	void Start(FString ImgURL);
	void Start();

private:
	void HandleImageRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	void dd(FHttpRequestPtr HttpRequest, int32 BytesSent, int32 BytesReceived);


	bool BufferAsDynamicTexture2D(const TArray<uint8>& Buffer, UTexture2DDynamic*& OutTexture);

public:
	UPROPERTY(BlueprintAssignable)
	FDownloadImageDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FDownloadImageDelegate OnFail;

	bool IsDownloading = true;

	FString m_ImageFileExtension = FString(TEXT("png"));
	// Data directory for this layer (permanently stored images)
	// 永久保存地址
	FString	m_DataDirectory = FString(TEXT("E://BMDownload//成都市_卫图//瓦片_ArcGis//_alllayers"));
	
	//FString URL;
public:
	//UPROPERTY(BlueprintAssignable)
	//FDownloadImageDelegate OnSuccess;

	UQuadTile* m_QuadTile = nullptr;

	// 需要有这个信息
	int32 m_Level, m_Row, m_Col;

	FString Key;  // 用来判断
	/// <summary>
	/// Starts processing this request
	/// </summary>
	
	/*
	//internal static DownloadQueue Queue;
	//object m_owner; 

	/// <summary>
	/// Initializes a new instance of the <see cref= "T:WorldWind.Net.DownloadRequest"/> class.
	/// </summary>
	/// <param name="owner"></param>
 
	/*DownloadRequest(object owner)
	{
		
	}#1#
public:
	/// <summary>
	/// A unique key identifying this request
	/// </summary>
	FString Key;

	/// <summary>
	/// The object that created this request
	/// </summary>
	UObject* Owner;


	/// <summary>
	/// Value (0-1) indicating how far the download has progressed.
	/// </summary>
	float Progress;

	/// <summary>
	/// Whether the request is currently being downloaded
	/// </summary>
	bool IsDownloading;

	/// <summary>
	/// Starts processing this request
	/// </summary>
	void Start(FString ImgURL);

	/// <summary>
	/// Calculates the score of this request.  Used to prioritize downloads.  
	/// Override in derived class to allow prioritization.
	/// </summary>
	/// <returns>Relative score or float.MinValue if request is no longer of interest.</returns>
	float CalculateScore()
	{
		return 0;
	}

	/// <summary>
	/// Derived classes should call this method to signal processing complete.
	/// </summary>
	///
	/// 
	/*virtual void OnComplete()
	{
		Queue.OnComplete(this);
	}#1#
	*/


	/// <summary>
	/// Calculates the score of this request.  Used to prioritize downloads.  
	/// Override in derived class to allow prioritization.
	/// </summary>
	/// <returns>Relative score or float.MinValue if request is no longer of interest.</returns>
	virtual float CalculateScore();



	void StartDownload()
	{}


	
};


