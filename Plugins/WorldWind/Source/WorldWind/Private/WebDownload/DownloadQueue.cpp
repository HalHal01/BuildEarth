// Fill out your copyright notice in the Description page of Project Settings.


#include "WebDownload/DownloadQueue.h"


UDownloadQueue* UDownloadQueue::m_pDownloadQueue = nullptr;

UDownloadQueue* UDownloadQueue::GetSingleton()
{
	if (m_pDownloadQueue == nullptr)
	{
		m_pDownloadQueue = NewObject<UDownloadQueue>();
	}
	return m_pDownloadQueue;
}

void UDownloadQueue::ddd()
{

	UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		//World->GetTimerManager().SetTimer(CountdownTimerHandle, this, &UDownloadQueue::AdvanceTimer, 0.05f, true);
	}
	
}

void UDownloadQueue::AdvanceTimer()
{
	
	--CountdownTime;
	//UpdateTimerDisplay();
	UGeoSpatialDownloadRequest* DownloadTask = NewObject<UGeoSpatialDownloadRequest>();
	//URL = URL01;
	//m_requests.

	/*UWorldWindSettings* TempSettings = GetMutableDefault<UWorldWindSettings>();

	FString ImgUrl = FString::Printf(TEXT("http://t0.tianditu.gov.cn/img_c/wmts?SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=img&STYLE=default&TILEMATRIXSET=w&FORMAT=tiles&TILEMATRIX=%d&TILEROW=%d&TILECOL=%d&tk=%s"),
m_Level, m_Row, m_Col, *TempSettings->Token);*/
	DownloadTask->Start(url);
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("XiazAI"));
	
	if (CountdownTime < 1)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
		//倒数完成，停止运行定时器。
		World->GetTimerManager().ClearTimer(CountdownTimerHandle);
		//定时器结束时，执行要执行的特殊操作。
		//CountdownHasFinished();
	}
	
}
