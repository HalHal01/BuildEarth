// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "UObject/NoExportTypes.h"
#include "Windows/WindowsPlatformHttp.h"
#include "WebDownload.generated.h"




/**
 *
 */
class FWebdownloadThread : FRunnable
{
public:

	/**
	 * Creates and initializes a new instance.
	 *
	 */
	FWebdownloadThread();


	~FWebdownloadThread();

public:

	//~ FRunnable interface

	virtual void Exit() override;
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;


	

};





enum class EDownloadType
{
	Unspecified,
	Wms
};


DECLARE_DELEGATE_TwoParams(DownloadProgressHandler, int32 , int32);
DECLARE_DELEGATE_OneParam(DownloadCompleteHandler, UWebDownload*);

/**
 * 
 */
UCLASS()
class WORLDWIND_API UWebDownload : public UObject
{
	GENERATED_BODY()

public:

	 bool Log404Errors = false;
	 bool useWindowsDefaultProxy = true;
	 FString proxyUrl = "";
	 bool useDynamicProxy;
	 FString proxyUserName = "";
	 FString proxyPassword = "";

	/*// 这段是啥意思？？？
	static FString UserAgent = String.Format(
			CultureInfo.InvariantCulture,
			"World Wind v{0} ({1}, {2})",
			System.Windows.Forms.Application.ProductVersion,
			Environment.OSVersion.ToString(),
			CultureInfo.CurrentCulture.Name);*/


	FString Url;

	// 没有太多作用
	/*/// <summary>
	/// Memory downloads fills this stream
	/// </summary>

	public Stream ContentStream; // 这个对应哪个呢？*/

	IFileHandle* ContentStream;  // 用这个？？？

	FString SavedFilePath;
	bool IsComplete;

	/// <summary>
	/// Called when data is being received.  
	/// Note that totalBytes will be zero if the server does not respond with content-length.
	/// </summary>
	DownloadProgressHandler ProgressCallback;

	/// <summary>
	/// Called to update debug window.
	/// </summary>
	static DownloadCompleteHandler DebugCallback;

	/// <summary>
	/// Called when a download has ended with success or failure
	/// </summary>
	static DownloadCompleteHandler DownloadEnded;

	/// <summary>
	/// Called when download is completed.  Call Verify from event handler to throw any exception.
	/// </summary>
	DownloadCompleteHandler CompleteCallback;
		
	EDownloadType DownloadType = EDownloadType::Unspecified;
	FString ContentType;
	int32 BytesProcessed;
	int32 ContentLength;

	// variables to allow placefinder to use gzipped requests
	//  *default to uncompressed requests to avoid breaking other things
	bool Compressed = false;
	FString ContentEncoding;

	/// <summary>
	/// The download start time (or MinValue if not yet started)
	/// </summary>
	FDateTime DownloadStartTime = FDateTime::MinValue();



	/*HttpWebRequest request;
	HttpWebResponse response;
	*/
	

	// 报错信息
	/*protected Exception downloadException;*/

	//TSharedRef<IHttpRequest, ESPMode::ThreadSafe> request;  // 这样写？
	//TSharedRef<IHttpRequest, ESPMode::ThreadSafe> request;  // 这样写？
	TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> response;
	

	bool isMemoryDownload;
    /// <summary>
    /// used to signal thread abortion; if true, the download thread was aborted
    /// </summary>
    bool stopFlag = false;

	/*// 多线程
	protected Thread dlThread;*/

	/*/// <summary>
	/// Initializes a new instance of the <see cref="WebDownload"/> class.
	/// </summary>
	/// <param name="url">The URL to download from.</param>
	UWebDownload(FString url)
	{
		Url = url;
	}*/

		/*/// <summary>
		/// Initializes a new instance of the <see cref="T:WorldWind.Net.WebDownload"/> class.
		/// </summary>
		public WebDownload()
		{
		}*/

		/*/// <summary>
		/// Whether the download is currently being processed (active).
		/// </summary>
		public bool IsDownloadInProgress
		{
			get 
			{
				return dlThread != null && dlThread.IsAlive;
			}
		}*/

		/*/// <summary>
		/// Contains the exception that occurred during download, or null if successful.
		/// </summary>
		public Exception Exception
		{
			get 
			{
				return downloadException;
			}
		}*/

		// 多线程
		/// <summary>
		/// Asynchronous download of HTTP data to file. 
		/// </summary>
		void BackgroundDownloadFile()
		{
			/*CompleteCallback.
			if (CompleteCallback==null)
				throw new ArgumentException("No download complete callback specified.");


			FRunnable* Runnable = new MyRunnable();
			FRunnableThread* RunnableThread = FRunnableThread::Create(Runnable, TEXT("DOWNLOAD"));
			MyRunnableObject = new my_class();

			MyThread = FRunnableThread::Create(MyRunnableObject, TEXT("MyRunnableObject"));
			dlThread = new Thread(new ThreadStart(Download));
			dlThread.Name = "WebDownload.dlThread";
			dlThread.IsBackground = true;
			dlThread.CurrentUICulture = Thread.CurrentThread.CurrentUICulture;
			dlThread.Start();*/
		}
	
		/*/// <summary>
		/// Asynchronous download of HTTP data to file.
		/// </summary>
		void BackgroundDownloadFile(DownloadCompleteHandler completeCallback)
		{
			CompleteCallback += completeCallback;
			BackgroundDownloadFile();
		}*/
	
		/*/// <summary>
		/// Download image of specified type. (handles server errors for wms type)
		/// </summary>
		void BackgroundDownloadFile(EDownloadType dlType)
		{
			DownloadType = dlType;
			BackgroundDownloadFile();
		}*/

		/*/// <summary>
		/// Asynchronous download of HTTP data to in-memory buffer. 
		/// </summary>
		void BackgroundDownloadMemory()
		{
			if (CompleteCallback==null)
				throw new ArgumentException("No download complete callback specified.");

			isMemoryDownload = true;
			dlThread = new Thread(new ThreadStart(Download));
			dlThread.Name = "WebDownload.dlThread(2)";
			dlThread.IsBackground = true;
			dlThread.CurrentUICulture = Thread.CurrentThread.CurrentUICulture;
			dlThread.Start();
		}*/
	
		/*/// <summary>
		/// Asynchronous download of HTTP data to in-memory buffer. 
		/// </summary>
		void BackgroundDownloadMemory( DownloadCompleteHandler completeCallback )
		{
			CompleteCallback += completeCallback;
			BackgroundDownloadMemory();
		}*/
	
		/*/// <summary>
		/// Download image of specified type. (handles server errors for WMS type)
		/// </summary>
		/// <param name="dlType">Type of download.</param>
		void BackgroundDownloadMemory( EDownloadType dlType )
		{
			DownloadType = dlType;
			BackgroundDownloadMemory();
		}*/

		/*/// <summary>
		/// Synchronous download of HTTP data to in-memory buffer. 
		/// </summary>
		void DownloadMemory()
		{
			isMemoryDownload = true;
			Download();
		}*/

		/*/// <summary>
		/// Download image of specified type. (handles server errors for WMS type)
		/// </summary>
		void DownloadMemory( EDownloadType dlType )
		{
			DownloadType = dlType;
			DownloadMemory();
		}*/

		/*/// <summary>
		/// HTTP downloads to memory.
		/// </summary>
		/// <param name="progressCallback">The progress callback.</param>
		void DownloadMemory( DownloadProgressHandler progressCallback )
		{
			ProgressCallback += progressCallback;
			DownloadMemory();
		}*/

		/// <summary>
		/// Synchronous download of HTTP data to in-memory buffer. 
		/// </summary>
		void DownloadFile(FString destinationFile)
		{
			SavedFilePath = destinationFile;

			Download();
		}

		/*/// <summary>
		/// Download image of specified type to a file. (handles server errors for WMS type)
		/// </summary>
		void DownloadFile(FString destinationFile, DownloadType dlType )
		{
			DownloadType = dlType;
			DownloadFile(destinationFile);
		}*/

		/*/// <summary>
		/// Saves a http in-memory download to file.
		/// </summary>
		/// <param name="destinationFilePath">File to save the downloaded data to.</param>
		void SaveMemoryDownloadToFile(FString destinationFilePath )
		{
			if(ContentStream==null)
				throw new InvalidOperationException("No data available.");

			// Cache the capabilities on file system
			ContentStream.Seek(0,SeekOrigin.Begin);
			using(Stream fileStream = File.Create(destinationFilePath))
			{
				if(ContentStream is MemoryStream)
				{
					// Write the MemoryStream buffer directly (2GB limit)
					MemoryStream ms = (MemoryStream)ContentStream;
					fileStream.Write(ms.GetBuffer(), 0, (int)ms.Length);
				}
				else
				{
					// Block copy
					byte[] buffer = new byte[4096];
					while(true)
					{
						int numRead = ContentStream.Read(buffer, 0, buffer.Length);
						if(numRead<=0)
							break;
						fileStream.Write(buffer,0,numRead);
					}
				}
			}
			ContentStream.Seek(0,SeekOrigin.Begin);
		}*/

		/*/// <summary>
		/// Aborts the current download. 
		/// </summary>
		void Cancel()
		{
			CompleteCallback = null;
			ProgressCallback = null;
			if (dlThread!=null && dlThread != Thread.CurrentThread)
			{
                if (dlThread.IsAlive)
                {
                    Log.Write(Log.Levels.Verbose, "WebDownload.Cancel() : stopping download thread...");
                    stopFlag = true;
                    if (!dlThread.Join(500))
                    {
                        Log.Write(Log.Levels.Warning, "WebDownload.Cancel() : download thread refuses to die, forcing Abort()");
                        dlThread.Abort();
                    }
                }
				dlThread = null;
			}		
		}*/

		/// <summary>
		/// Notify event subscribers of download progress.
		/// </summary>
		/// <param name="bytesRead">Number of bytes read.</param>
		/// <param name="totalBytes">Total number of bytes for request or 0 if unknown.</param>
		void OnProgressCallback(int bytesRead, int totalBytes)
		{
			/*if (ProgressCallback != null)
			{
				ProgressCallback(bytesRead, totalBytes);
			}*/
		}

		/// <summary>
		/// Called with detailed information about the download.
		/// </summary>
		/// <param name="wd">The WebDownload.</param>
		 static void OnDebugCallback(UWebDownload* wd)
		{
			/*if (DebugCallback != null)
			{
				DebugCallback(wd);
			}*/
		}

		/*/// <summary>
		/// Called when downloading has ended.
		/// </summary>
		/// <param name="wd">The download.</param>
		static void OnDownloadEnded(WebDownload wd)
		{
			if (DownloadEnded != null)
			{
				DownloadEnded(wd);
			}
		}*/


	// Synchronous HTTP download
	void Download()
	{
        UE_LOG(LogClass, Log, TEXT("Starting download thread..."));

		// 断言？？？
        check(Url.StartsWith("http://")); 

		DownloadStartTime = FDateTime::Now();
		try
		{
			try
			{
				// If a registered progress-callback, inform it of our download progress so far.
				OnProgressCallback(0, 1);
				OnDebugCallback(this);

                // check to see if thread was aborted (multiple such checks within the thread function)
                if (stopFlag)
                {
                    IsComplete = true;
                    return;
                }

				// 不考虑这个方式
				/*// create content stream from memory or file
				if (isMemoryDownload && ContentStream == null)
				{
					ContentStream = new MemoryStream();
				}
				else*/
				{
					// Download to file
					FString targetDirectory = FPaths::GetPath(SavedFilePath);
					IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
					if(targetDirectory.Len() > 0)
					{
						PlatformFile.CreateDirectory(*targetDirectory);
					}
					/*ContentStream = new FileStream(SavedFilePath, FileMode.Create);*/
					ContentStream = PlatformFile.OpenWrite(*SavedFilePath);
				}

				// Create the request object.
				TSharedRef<IHttpRequest, ESPMode::ThreadSafe> request = FHttpModule::Get().CreateRequest();
				request->SetHeader(TEXT("User-Agent"), FPlatformHttp::GetDefaultUserAgent());  // 没必要

				if (Compressed)
				{
					request->SetHeader(TEXT("Accept-Encoding"), TEXT("gzip,deflate"));
				}

                if (stopFlag)
                {
                    IsComplete = true;
                    return;
                }

				// 代理的，不考虑
				/*request.Proxy = ProxyHelper.DetermineProxyForUrl(
					Url,
					useWindowsDefaultProxy,
					useDynamicProxy,
					proxyUrl,
					proxyUserName,
					proxyPassword);*/







				
                
                // TODO01: probably better done via BeginGetResponse() / EndGetResponse() because this may block for a while
                // causing warnings in thread abortion.

				
				//using (response = request.GetResponse() as HttpWebResponse)

				{
					// only if server responds 200 OK
					if (response->GetResponseCode() == 200)
					{
						ContentType = response->GetContentType();
						// UE 里面似乎没有这个
						/*ContentEncoding = response.ContentEncoding;*/


						// 不去 Header 中找文件大小
						// Find the data size from the headers.
						FString strContentLength = response->GetHeader(TEXT("Content-Length"));
						if (!strContentLength.IsEmpty())
						{
							/*ContentLength = int.Parse(strContentLength, CultureInfo.InvariantCulture);*/
						}


		


						FFileHelper::SaveArrayToFile(response->GetContent(), *SavedFilePath);
						
						/*byte[] readBuffer = new byte[1500];
						using (Stream responseStream = response.GetResponseStream())
						{
							while (true)
                            {
                                if (stopFlag)
                                {
                                    IsComplete = true;
                                    return;
                                }

								//  Pass do.readBuffer to BeginRead.
								int32 bytesRead = responseStream.Read(readBuffer, 0, readBuffer.Length);
								if (bytesRead <= 0)
									break;

								//TODO01: uncompress responseStream if necessary so that ContentStream is always uncompressed
								//  - at the moment, ContentStream is compressed if the requesting code sets
								//    download.Compressed == true, so ContentStream must be decompressed 
								//    by whatever code is requesting the gzipped download
								//  - this hack only works for requests made using the methods that download to memory,
								//    requests downloading to file will result in a gzipped file
								//  - requests that do not explicity set download.Compressed = true should be unaffected

								ContentStream.Write(readBuffer, 0, bytesRead);

								BytesProcessed += bytesRead;

								// If a registered progress-callback, inform it of our download progress so far.
								OnProgressCallback(BytesProcessed, ContentLength);
								OnDebugCallback(this);
							}
						}*/

					}
				}
				

				/*HandleErrors();*/
			}
			catch (...)
			{}
            /*catch (ThreadAbortException)
            {
                // re-throw to avoid it being caught by the catch-all below
                Log.Write(Log.Levels.Verbose, "Re-throwing ThreadAbortException.");
                throw;
            }
			catch (System.Configuration.ConfigurationException)
			{
				// is thrown by WebRequest.Create if App.config is not in the correct format
				// TODO01: don't know what to do with it
				throw;
			}*/
			/*catch (Exception caught)
			{
				try
				{
					// Remove broken file download
					if (ContentStream != null)
					{
						ContentStream.Close();
						ContentStream = null;
					}
					if (SavedFilePath != null && SavedFilePath.Length > 0)
					{
						File.Delete(SavedFilePath);
					}
				} 
				catch(Exception) 
				{
				}
				SaveException(caught);
			}*/

            /*if (stopFlag)
            {
                IsComplete = true;
                return;
            }

			if (ContentLength == 0)
			{
				ContentLength = BytesProcessed;
				// If a registered progress-callback, inform it of our completion
				OnProgressCallback(BytesProcessed, ContentLength);
			}

			if (ContentStream is MemoryStream)
			{
				ContentStream.Seek(0, SeekOrigin.Begin);
			}
			else if (ContentStream != null)
			{
				ContentStream.Close();
				ContentStream = null;
			}
		
			OnDebugCallback(this);
			
			if (CompleteCallback == null)
			{
				Verify();
			}
			else
			{
				CompleteCallback(this);
			}*/
		}
		catch (...)
		{
			
		}
		/*catch (ThreadAbortException)
		{
            Log.Write(Log.Levels.Verbose, "Download aborted.");
        }
		finally
		{
			IsComplete = true;
		}

		OnDownloadEnded(this);*/
	}



	void HandleImageRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
	{

		// 这个是做啥
		RemoveFromRoot();

		if ( bSucceeded && HttpResponse.IsValid() && HttpResponse->GetContentLength() > 0 )
		{
			
			HttpResponse->GetContent().GetData();
			ContentLength = HttpResponse->GetContentLength();

			
		}


	}




	

	/*/// <summary>
	/// Handle server errors that don't get trapped by the web request itself.
	/// </summary>
	void HandleErrors()
	{
		// HACK: Workaround for TerraServer failing to return 404 on not found
		if(ContentStream.Length == 15)
		{
			// a true 404 error is a System.Net.WebException, so use the same text here
			Exception ex = new FileNotFoundException("The remote server returned an error: (404) Not Found.", SavedFilePath );
			SaveException(ex);
		}

		// TODO01: WMS 1.1 content-type != xml
		// TODO01: Move WMS logic to WmsDownload
		if (DownloadType == DownloadType.Wms && (
			ContentType.StartsWith("text/xml") ||
			ContentType.StartsWith("application/vnd.ogc.se")))
		{
			// WMS request failure
			SetMapServerError();
		}
	}*/

	/*/// <summary>
	/// If exceptions occurred they will be thrown by calling this function.
	/// </summary>
	void Verify()
	{
		if(Exception!=null)
			throw Exception;
	}*/

	/*/// <summary>
	/// Log download error to log file
	/// </summary>
	/// <param name="exception"></param>
	void SaveException( Exception exception )
	{
		// Save the exception 
		downloadException = exception;

		if(Exception is ThreadAbortException)
			// Don't log canceled downloads
			return;

		if(Log404Errors)
		{
			Log.Write(Log.Levels.Error, "HTTP", "Error: " + Url );
			Log.Write(Log.Levels.Error+1, "HTTP", "     : " + exception.Message );
		}
	}*/

	/*/// <summary>
	/// Reads the xml response from the server and throws an error with the message.
	/// </summary>
	void SetMapServerError()
	{
		try
		{
			XmlDocument errorDoc = new XmlDocument();
			ContentStream.Seek(0,SeekOrigin.Begin);
			errorDoc.Load(ContentStream);
			string msg = "";
			foreach( XmlNode node in errorDoc.GetElementsByTagName("ServiceException"))
				msg += node.InnerText.Trim()+Environment.NewLine;
			SaveException( new WebException(msg.Trim()) );
		}
		catch(XmlException)
		{
			SaveException( new WebException("An error occurred while trying to download " + request.RequestUri.ToString()+".") );
		}
	}*/



	/*/// <summary>
	/// Performs application-defined tasks associated with freeing, releasing, or
	/// resetting unmanaged resources.
	/// </summary>
	void Dispose()
	{
		if (dlThread!=null && dlThread != Thread.CurrentThread)
		{
            if (dlThread.IsAlive)
            {
                Log.Write(Log.Levels.Verbose, "WebDownload.Dispose() : stopping download thread...");
                stopFlag = true;
                if (!dlThread.Join(500))
                {
                    Log.Write(Log.Levels.Warning, "WebDownload.Dispose() : download thread refuses to die, forcing Abort()");
                    dlThread.Abort();
                }
            }
			dlThread = null;
		}

		if(request!=null)
		{
			request.Abort();
			request = null;
		}

		if (ContentStream != null)
		{
			ContentStream.Close();
			ContentStream=null;
		}

		if(DownloadStartTime != DateTime.MinValue)
			OnDebugCallback(this);

		GC.SuppressFinalize(this);
	}*/


	
};
