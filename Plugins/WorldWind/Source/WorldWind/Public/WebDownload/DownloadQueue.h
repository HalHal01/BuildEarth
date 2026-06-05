// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DownloadRequest.h"
#include "UObject/NoExportTypes.h"
#include "DownloadQueue.generated.h"

/**
 * Download queue with priorities
 */

UCLASS()
class WORLDWIND_API UDownloadQueue : public UObject
{
	GENERATED_BODY()

public:
	
	//! \brief 获取RealspaceView
	static UDownloadQueue* GetSingleton();

private:
	int32 CountdownTime = 400;
	//! \brief RealspaceView����
	static UDownloadQueue* m_pDownloadQueue;
	
public:
	FString url = FString(TEXT("http://t0.tianditu.gov.cn/img_c/wmts?SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=img&STYLE=default&TILEMATRIXSET=w&FORMAT=tiles&TILEMATRIX=16&TILEROW=10847&TILECOL=51738&tk=fd011184766f373818e54ac526e22eca"));
	FTimerHandle CountdownTimerHandle;

	void ddd();

	void AdvanceTimer();

	/*static */int32 MaxQueueLength = 200;
	
	/*static*/ int MaxConcurrentDownloads = 2;
		TArray<UGeoSpatialDownloadRequest*> m_requests;
		TArray<UGeoSpatialDownloadRequest*> m_activeDownloads;

		/*/// <summary>
		/// Initializes a new instance of the <see cref= "T:WorldWind.Net.DownloadRequest"/> class 
		/// with default data.
		/// </summary>
		public DownloadQueue()
		{
			DownloadRequest.Queue = this;
		}

		/// <summary>
		/// Request for download queue
		/// </summary>
		public ArrayList Requests
		{
			get
			{
				return m_requests;
			}
		}

	*/

	// Remove all requests with a certain owner.
	virtual void Clear(UQuadTile* owner)
	{
		{
			for(int i=m_requests.Num()-1; i>=0; i--)
			{
				UGeoSpatialDownloadRequest* request = m_requests[i];
				if(request->m_QuadTile == owner)
				{
					m_requests.RemoveAt(i);
				}
			}
		}
		ServiceDownloadQueue();
	}

	// Finds the next file to download	
	virtual UGeoSpatialDownloadRequest* GetNextDownloadRequest()
	{
		UGeoSpatialDownloadRequest* bestRequest = nullptr;
		float highestScore = -UE_MAX_FLT;

		for (int32 i = m_requests.Num() - 1; i >= 0; i--)
		{
			UGeoSpatialDownloadRequest* request = m_requests[i];
			if(request->IsDownloading)
			{
				continue;
			}

			float score = request->CalculateScore();
			if(score == -UE_MAX_FLT)
			{
				// Request is of no interest anymore, remove it
				m_requests.RemoveAt(i);

				continue;
			}

			if( score > highestScore )
			{
				highestScore = score;
				bestRequest = request;
			}
		}

	return bestRequest;
	}

	/// <summary>
	/// Add a download request to the queue.
	/// </summary>
	void Add(UGeoSpatialDownloadRequest* newRequest)
	{
		for (auto& request : m_requests)
		{
			if(request->Key.Equals(newRequest->Key, ESearchCase::IgnoreCase))  // 说明下载的是同一个东西
			{
				return;
			}
		}

		m_requests.Add(newRequest);

		if (m_requests.Num() > MaxQueueLength)  // 
		{
			// Remove lowest scoring queued request
			UGeoSpatialDownloadRequest* leastImportantRequest = nullptr;
			float lowestScore = -UE_MAX_FLT;

			for (int32 i = m_requests.Num()-1; i>=0; i--)
			{
				UGeoSpatialDownloadRequest* request = m_requests[i];
				if(request->IsDownloading)
				{
					continue;  // 跳过正在下载中的请求
				}

				float score = request->CalculateScore();
				if(score == -UE_MAX_FLT)
				{
					// Request is of no interest anymore, remove it
					m_requests.Remove(request);
					return;
				}

				if(score < lowestScore)  // 这段是什么意思？
				{
					lowestScore = score;
					leastImportantRequest = request;
				}
			}

			if(leastImportantRequest != nullptr)
			{
				m_requests.Remove(leastImportantRequest);
			}
		}
		ServiceDownloadQueue();
	}

	// Removes a request from the download queue.
	void Remove(UGeoSpatialDownloadRequest* request)
	{


		m_requests.Remove(request);

		ServiceDownloadQueue();
	}

	// Starts downloads when there are threads available
	void ServiceDownloadQueue()
	{
		// Remove finished downloads
		
		m_activeDownloads.RemoveAll([](const UGeoSpatialDownloadRequest* Request){
			return !Request->IsDownloading;
		});

		// Start new downloads
		while (m_activeDownloads.Num() < MaxConcurrentDownloads)
		{
			UGeoSpatialDownloadRequest* request = GetNextDownloadRequest();
			if (request == nullptr)
			{
				break;
			}

			m_activeDownloads.Add(request);
			request->Start();
		}
	}

	// Callback when download is complete.
	void OnComplete(UGeoSpatialDownloadRequest* request)
	{
		{
			// Remove the finished item from queue
			m_requests.Remove(request);
		}

		// Start next download
		ServiceDownloadQueue();
	}



		/*void Dispose()
		{
			lock(m_requests.SyncRoot)
			{
				foreach(DownloadRequest request in m_requests)
					request.Dispose();
				m_requests.Clear();
				m_activeDownloads.Clear();
			}

			GC.SuppressFinalize(this);
		}*/

	
	
	
};
