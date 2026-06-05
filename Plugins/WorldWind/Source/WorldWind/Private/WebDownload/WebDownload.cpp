// Fill out your copyright notice in the Description page of Project Settings.


#include "WebDownload/WebDownload.h"

FWebdownloadThread::FWebdownloadThread()
{
}

FWebdownloadThread::~FWebdownloadThread()
{
}

bool FWebdownloadThread::Init()
{
	//bIsRunning = true;
	/*FString aa = FString(TEXT("Init"));
	GEngine->AddOnScreenDebugMessage(-1, 200.f, FColor(FMath::RandRange(0, 255), FMath::RandRange(0, 255), FMath::RandRange(0, 255)),
		aa);*/

	//UE_LOG(LogClass, Log, TEXT("Init"));
	return true;
}

uint32 FWebdownloadThread::Run()
{
	//UE_LOG(LogClass, Log, TEXT("Run"));
	FString aa = FString(TEXT("Run"));
	GEngine->AddOnScreenDebugMessage(-1, 200.f, FColor::Red, aa);
	/*while (bIsRunning)
	{
		++RunTime;
		FPlatformProcess::Sleep(1);
	}*/
	
	UWebDownload* DownloadTask = NewObject<UWebDownload>();

	//URL = URL01;


	//DownloadTask->OnSuccess.BindUObject(this, &UQuadTile::UpdateTileTexture);

	//DownloadTask->Start(FString(TEXT("http://10.67.8.90:8000/traffic/articles/20/01/300/")));

	
	return 0;
}

void FWebdownloadThread::Stop()
{
	UE_LOG(LogClass, Log, TEXT("Stop"));
	GEngine->AddOnScreenDebugMessage(-1, 200.f, FColor(FMath::RandRange(0, 255), FMath::RandRange(0, 255), FMath::RandRange(0, 255)),
FString(TEXT("Stop")));
	//bIsRunning = false;
	
}

void FWebdownloadThread::Exit()
{
	UE_LOG(LogClass, Log, TEXT("Exit"));
	
}