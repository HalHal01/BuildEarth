// Fill out your copyright notice in the Description page of Project Settings.


#include "WebDownload/DownloadRequest.h"
#include "IImageWrapper.h"
#include "HttpModule.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"
#include "QuadTile.h"
#include "WorldWindSettings.h"
#include "Engine/Texture2DDynamic.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IPluginManager.h"


/*UGeoSpatialDownloadRequest::UGeoSpatialDownloadRequest()
	
{
}*/



class FTestTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FTestTask>;
	//FUpdateLogTextDel UpdateLogText;
	FString ToolConsoleLog;
	FEvent* downloadCompleteEvent;
	FEvent* SaveCompleteEvent;
	TArray<uint8> httpData;

	//……
	void DoWork();
	
	void OnDownloadRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	void OnRequestDownloadProgress(FHttpRequestPtr HttpRequest, int32 BytesSend, int32 InBytesReceived);

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FTestTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};


void FTestTask::DoWork()
{
	FString URL = FString(TEXT("http://10.67.8.2/pics/_alllayers/L04/R00000002/C0000000c.png"));
	// Create HTTP request for downloading oculus platform tool
	downloadCompleteEvent = FGenericPlatformProcess::GetSynchEventFromPool(false);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> httpRequest = FHttpModule::Get().CreateRequest();

	httpRequest->OnProcessRequestComplete().BindRaw(this, &FTestTask::OnDownloadRequestComplete);
	//httpRequest->OnRequestProgress().BindRaw(this, &FTestTask::OnRequestDownloadProgress);
	httpRequest->SetURL(URL);
	httpRequest->SetVerb("GET");

	httpRequest->ProcessRequest();

	//UpdateLogText.Execute(SOculusPlatformToolWidget::LogText + LOCTEXT("DownloadProgress", "Downloading Platform Tool: {0}%\n").ToString());
	//ToolConsoleLog = SOculusPlatformToolWidget::LogText;
	//UpdateProgressLog(0);
	
	// Wait for download to complete
	downloadCompleteEvent->Wait();

	// Save HTTP data

	FString fullPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir()) + FGuid::NewGuid().ToString() + TEXT(".json");
	if (FFileHelper::SaveArrayToFile(httpData, *fullPath))
	{
		//UpdateLogText.Execute(SOculusPlatformToolWidget::LogText + LOCTEXT("DownloadSuccess", "Platform tool successfully downloaded.\n").ToString());
	}
	else
	{
		//UpdateLogText.Execute(SOculusPlatformToolWidget::LogText + LOCTEXT("DownloadError", "An error has occured with downloading the platform tool.\n").ToString());
	}

	if (SaveCompleteEvent != NULL)
	{
		SaveCompleteEvent->Trigger();
	}
}



void FTestTask::OnRequestDownloadProgress(FHttpRequestPtr HttpRequest, int32 BytesSend, int32 InBytesReceived)
{
	// Update progress on download in tool console log
	FHttpResponsePtr httpResponse = HttpRequest->GetResponse();
	if (httpResponse.IsValid())
	{
		int progress = ((float)InBytesReceived / (float)httpResponse->GetContentLength()) * 100;
		//UpdateProgressLog(progress);
	}
}

void FTestTask::OnDownloadRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	// Extract data from HTTP response and trigger download complete event
	if (bSucceeded && HttpResponse.IsValid())
	{
		httpData = HttpResponse->GetContent();
		downloadCompleteEvent->Trigger();
	}
}

UGeoSpatialDownloadRequest* UGeoSpatialDownloadRequest::DownloadImage(FString URL01)
{
	

	FAutoDeleteAsyncTask<FTestTask>* MyTask = new FAutoDeleteAsyncTask<FTestTask>(/*可以在这里传入FTestTask需要的参数*/);
	//FAsyncTask<FTestTask>()* MyTask = new FAsyncTask<FTestTask>(/*可以在这里传入FTestTask需要的参数*/)

	MyTask->StartBackgroundTask();
	
	/*AsyncTask(ENamedThreads::GameThread, [this, WeakThis, ImageSize, Pixels = MoveTemp(OutPixels)]() mutable {
	if (WeakThis.IsValid())
	{
		ExportImage(MoveTemp(Pixels), ImageSize);
	}
});*/


	UGeoSpatialDownloadRequest* DownloadTask = NewObject<UGeoSpatialDownloadRequest>();
	//URL = URL01;


	/*UWorldWindSettings* TempSettings = GetMutableDefault<UWorldWindSettings>();

	FString ImgUrl = FString::Printf(TEXT("http://t0.tianditu.gov.cn/img_c/wmts?SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=img&STYLE=default&TILEMATRIXSET=w&FORMAT=tiles&TILEMATRIX=%d&TILEROW=%d&TILECOL=%d&tk=%s"),
m_Level, m_Row, m_Col, *TempSettings->Token);*/



	// 临时注销给调试用
	/*DownloadTask->Start(URL01);*/

	return DownloadTask;
}


// 好像没有什么作用
bool UGeoSpatialDownloadRequest::BufferAsDynamicTexture2D(const TArray<uint8>& Buffer, UTexture2DDynamic*& OutTexture)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrappers[3] =
	{
		ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG),
		ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG),
		ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP),
	};

	for (auto ImageWrapper : ImageWrappers)
	{
		if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(Buffer.GetData(), Buffer.GetAllocatedSize()))
		{
			TArray64<uint8> RawData;
			const ERGBFormat InFormat = ERGBFormat::BGRA;
			if (ImageWrapper->GetRaw(InFormat, 8, RawData))
			{
				OutTexture = UTexture2DDynamic::Create(ImageWrapper->GetWidth(), ImageWrapper->GetHeight());
				
				if (OutTexture)
				{
					OutTexture->SRGB = true;
					OutTexture->UpdateResource();

					FTexture2DDynamicResource* TextureResource = static_cast<FTexture2DDynamicResource*>(OutTexture->GetResource());
					if (TextureResource)
					{
						ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture)(
							[TextureResource, RawData = MoveTemp(RawData)](FRHICommandListImmediate& RHICmdList)
							{
								TextureResource->WriteRawToTexture_RenderThread(RawData);
							});
					}
					return true;
				}
			}
		}
	}

	return false;
}





void UGeoSpatialDownloadRequest::LoadLocal(const FString& Filename)
{
	TArray<uint8> Buffer;
	if (FFileHelper::LoadFileToArray(Buffer, *Filename))
	{
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrappers[3] =
		{
			ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG),
			ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG),
			ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP),
		};

		for (auto ImageWrapper : ImageWrappers)
		{
			if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(Buffer.GetData(), Buffer.GetAllocatedSize()))
			{
				TArray64<uint8> RawData;
				const ERGBFormat InFormat = ERGBFormat::BGRA;
				if (ImageWrapper->GetRaw(InFormat, 8, RawData))
				{
					if (UTexture2DDynamic* Texture = UTexture2DDynamic::Create(ImageWrapper->GetWidth(), ImageWrapper->GetHeight()))
					{
						Texture->SRGB = true;
						Texture->UpdateResource();

						FTexture2DDynamicResource* TextureResource = static_cast<FTexture2DDynamicResource*>(Texture->GetResource());
						if (TextureResource)
						{
							ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture)(
								[TextureResource, RawData = MoveTemp(RawData)](FRHICommandListImmediate& RHICmdList)
								{
									TextureResource->WriteRawToTexture_RenderThread(RawData);
								});
						}

						OnSuccess.Broadcast(Texture, Cast<UQuadTile>(GetOuter()));
						UE_LOG(LogClass, Log, TEXT("本地瓦片加载完成！"));
						IsDownloading = false;
						return;
					}
				}
			}
		}
	}

	OnFail.Broadcast(nullptr, m_QuadTile);
	UE_LOG(LogClass, Log, TEXT("本地瓦片加载失败！"));
}


// 需要做两件事：1、从 HTTP 下载图像并转为 Texture2D; 2、将数据保存到本地
void UGeoSpatialDownloadRequest::HandleImageRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (bSucceeded && HttpResponse.IsValid() && HttpResponse->GetContentLength() > 0)
	{
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrappers[3] =
		{
			ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG),
			ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG),
			ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP),
		};

		for (auto ImageWrapper : ImageWrappers)
		{
			if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(HttpResponse->GetContent().GetData(), HttpResponse->GetContentLength()))
			{
				TArray64<uint8> RawData;
				const ERGBFormat InFormat = ERGBFormat::BGRA;
				if (ImageWrapper->GetRaw(InFormat, 8, RawData))
				{
					UQuadTile* Owner = Cast<UQuadTile>(GetOuter());
					// 影像瓦片文件的路径
					FString TerrainTileFilePath = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("WorldWind"))->GetBaseDir(),
						TEXT("Image"),
						FString::Printf(TEXT("/L%02d/R%08x/C%08x.png"), Owner->m_Level, Owner->m_Row, Owner->m_Col));

					// 把图片保存在本地
					FFileHelper::SaveArrayToFile(HttpResponse->GetContent(), *TerrainTileFilePath);

					if (UTexture2DDynamic* Texture = UTexture2DDynamic::Create(ImageWrapper->GetWidth(), ImageWrapper->GetHeight()))
					{
						Texture->SRGB = true;
						Texture->UpdateResource();

						FTexture2DDynamicResource* TextureResource = static_cast<FTexture2DDynamicResource*>(Texture->GetResource());
						if (TextureResource)
						{
							ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture)(
								[TextureResource, RawData = MoveTemp(RawData)](FRHICommandListImmediate& RHICmdList)
								{
									TextureResource->WriteRawToTexture_RenderThread(RawData);
								});
						}
						OnSuccess.Broadcast(Texture, Cast<UQuadTile>(GetOuter()));
						UE_LOG(LogClass, Log, TEXT("网络瓦片下载完成！"));
						IsDownloading = false;
						return;
					}
				}
			}
		}
	}

	OnFail.Broadcast(nullptr, m_QuadTile);
	UE_LOG(LogClass, Log, TEXT("瓦片下载失败！"));
}


void UGeoSpatialDownloadRequest::dd(FHttpRequestPtr HttpRequest, int32 BytesSent, int32 BytesReceived)
{
	//GEngine->AddOnScreenDebugMessage(-1, 200.f, FColor::Green, FString::FromInt(BytesReceived) + TEXT("    ") + FString::FromInt(HttpRequest->GetResponse()->GetContentLength()));
	/*// Update progress on download in tool console log
	FHttpResponsePtr httpResponse = HttpRequest->GetResponse();
	if (httpResponse.IsValid())
	{
		int progress = ((float)BytesReceived / httpResponse->GetContentLength()) * 100;
		
	}*/
}


void UGeoSpatialDownloadRequest::Start(FString ImgURL)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->SetURL(ImgURL);
	HttpRequest->SetVerb(TEXT("GET"));
	//HttpRequest->SetHeader(TEXT("Accept"), TEXT("image/png, image/x-png, image/jpeg; q=0.8, image/vnd.microsoft.icon, image/x-icon, image/bmp, image/*; q=0.5, image/webp; q=0.0"));

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGeoSpatialDownloadRequest::HandleImageRequest);
	//HttpRequest->OnRequestProgress().BindUObject(this, &UGeoSpatialDownloadRequest::dd);
	HttpRequest->ProcessRequest();
}


void UGeoSpatialDownloadRequest::Start()
{


	UWorldWindSettings* TempSettings = GetMutableDefault<UWorldWindSettings>();
	UQuadTile* Owner = Cast<UQuadTile>(GetOuter());
	
	FString RelativePath = FString::Printf(TEXT("/L%02d/R%08x/C%08x.%s"),
		Owner->m_Level, Owner->m_Row, Owner->m_Col, *m_ImageFileExtension);
	FString FullLocalPath = FPaths::Combine(m_DataDirectory, RelativePath);

	FString CacheFilePath = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("WorldWind"))->GetBaseDir(),
		TEXT("Image"),
		RelativePath);
	
	if (FPaths::FileExists(FullLocalPath))
	{
		UE_LOG(LogClass, Log, TEXT("加载本地文件，文件地址：%s"), *FullLocalPath)
		LoadLocal(FullLocalPath);

	}
	else if (FPaths::FileExists(CacheFilePath))
	{
		UE_LOG(LogClass, Log, TEXT("加载缓存文件，文件地址：%s"), *CacheFilePath)
		LoadLocal(CacheFilePath);
	}
	else
	{
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

		FString FullLocalPath01 = FString::Printf(TEXT("http://t0.tianditu.gov.cn/img_c/wmts?SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=img&STYLE=default&TILEMATRIXSET=c&FORMAT=tiles&TILEMATRIX=%d&TILEROW=%d&TILECOL=%d&tk=%s"),
		Owner->m_Level, Owner->m_Row, Owner->m_Col, *TempSettings->Token);
		
		UE_LOG(LogClass, Log, TEXT("从网络下载纹理，URL：%s"), *FullLocalPath01)
		
		HttpRequest->SetURL(FullLocalPath01);
		HttpRequest->SetVerb(TEXT("GET"));
		//HttpRequest->SetHeader(TEXT("Accept"), TEXT("image/png, image/x-png, image/jpeg; q=0.8, image/vnd.microsoft.icon, image/x-icon, image/bmp, image/*; q=0.5, image/webp; q=0.0"));

		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGeoSpatialDownloadRequest::HandleImageRequest);
		//HttpRequest->OnRequestProgress().BindUObject(this, &UGeoSpatialDownloadRequest::dd);
		HttpRequest->ProcessRequest();
	}
	

}

float UGeoSpatialDownloadRequest::CalculateScore()
{



	/*AsyncTask(ENamedThreads::GameThread, [this, WeakThis, ImageSize, Pixels = MoveTemp(OutPixels)]() mutable {
	if (WeakThis.IsValid())
	{
		ExportImage(MoveTemp(Pixels), ImageSize);
	}
});*/
	/*float screenArea = m_QuadTile->m_BoundingBox.CalcRelativeScreenArea(m_QuadTile.QuadTileArgs.Camera);
	return screenArea;*/
	return 0;
}

