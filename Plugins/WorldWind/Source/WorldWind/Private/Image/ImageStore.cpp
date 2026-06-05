// Fill out your copyright notice in the Description page of Project Settings.



#include "Image/ImageStore.h"
#include "WorldWindSettings.h"
#include "IImageWrapper.h"
#include "ImageUtils.h"
#include "QuadTile.h"

FImageStore::FImageStore()
{
}



FImageStore::~FImageStore()
{
}

// 修改完成
FString FImageStore::GetLocalPath(UQuadTile* qt)
{
	if(qt->m_Level >= m_LevelCount)
	{
		UE_LOG(LogClass, Log, TEXT("Level %d not available."), qt->m_Level);
	}

	FString RelativePath = FString::Printf(TEXT("/L%02d/R%08x/C%08x.%s"), 
		qt->m_Level, qt->m_Row, qt->m_Col, *m_ImageFileExtension);

	if(!m_DataDirectory.IsEmpty())
	{
		// Search data directory first
		FString rawFullPath = FPaths::Combine(m_DataDirectory, RelativePath );
		if(FPaths::FileExists(rawFullPath))
			return rawFullPath;
	}

	// If cache doesn't exist, fall back to duplicate texture path.
	if (m_CacheDirectory.IsEmpty())
		return m_DuplicateTexturePath;

	// Try cache with default file extension
	FString CacheFullPath = FPaths::Combine(m_CacheDirectory, RelativePath);
	if(FPaths::FileExists(CacheFullPath))
		return CacheFullPath;

	// 暂时只考虑PNG
	/*// Try cache but accept any valid image file extension
	//const string ValidExtensions = ".bmp.dds.dib.hdr.jpg.jpeg.pfm.png.ppm.tga.gif.tif";
	const TArray<FString> ValidExtensions = { TEXT("bmp"), TEXT("jpg"), TEXT("jpeg") };  // 只要这三个

	FString cacheSearchPath = FPaths::GetPath(CacheFullPath);
	if(FPaths::DirectoryExists(cacheSearchPath))
	{

		TArray<FString> FileNames;
		IFileManager::Get().FindFiles(FileNames, *cacheSearchPath);
		
		for(FString imageFile in Directory.GetFiles(
			cacheSearchPath, 
			Path.GetFileNameWithoutExtension(cacheFullPath) + ".*") )
		{
			string extension = Path.GetExtension(imageFile).ToLower();
			if(ValidExtensions.IndexOf(extension)<0)
				continue;

			return imageFile;
		}
	}*/

	return CacheFullPath;
}

FString FImageStore::GetDownloadUrl(UQuadTile* qt)
{

	// No local image, return our "duplicate" tile if any
	if(!m_DuplicateTexturePath.IsEmpty() && FPaths::FileExists(m_DuplicateTexturePath))
		return m_DuplicateTexturePath;

	UWorldWindSettings* TempSettings = GetMutableDefault<UWorldWindSettings>();

	FString ImgUrl = FString::Printf(TEXT("https://t0.tianditu.gov.cn/img_c/wmts?SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=img&STYLE=default&TILEMATRIXSET=c&FORMAT=tiles&TILEMATRIX=%d&TILEROW=%d&TILECOL=%d&tk=%s"),
		qt->m_Level, qt->m_Row, qt->m_Col, *TempSettings->Token);

	// No image available anywhere, give up
	return FString(TEXT(""));
	
}


UTexture2D* FImageStore::LoadFile(UQuadTile* qt)
{
    FString FilePath = GetLocalPath(qt);
	qt->ImageFilePath = FilePath;  // 应该没有什么用
	if(!FPaths::FileExists(FilePath))  // 本地文件不存在
	{
		FString badFlag = FilePath + TEXT(".txt");
		if(FPaths::FileExists(badFlag))
		{
			FFileStatData fi = IFileManager::Get().GetStatData(*badFlag);
			if(FDateTime::Now() - fi.ModificationTime < FTimespan::FromDays(1))
			{
				return nullptr;
			}
			// Timeout period elapsed, retry
			IFileManager::Get().Delete(*badFlag);
		}

		if(IsDownloadableLayer())
		{
            QueueDownload(qt, FilePath);
			return nullptr;
		}

		if(m_DuplicateTexturePath.IsEmpty())
			// No image available, neither local nor online.
			return nullptr;

        FilePath = m_DuplicateTexturePath;
	}


	
	// Use color key
	UTexture2D* Texture2D = nullptr;

	Texture2D = FImageUtils::ImportFileAsTexture2D(FilePath);


	// 这里不知道
	/*if(qt.QuadTileSet.HasTransparentRange)
	{
		texture = ImageHelper.LoadTexture( filePath, qt.QuadTileSet.ColorKey, 
			qt.QuadTileSet.ColorKeyMax);
	}
	else
	{
		texture = ImageHelper.LoadTexture( filePath, qt.QuadTileSet.ColorKey);
	}

	if(qt.QuadTileSet.CacheExpirationTime != TimeSpan.MaxValue)
	{
		FileInfo fi = new FileInfo(filePath);
		DateTime expiry = fi.LastWriteTimeUtc.Add(qt.QuadTileSet.CacheExpirationTime);
		if(DateTime.UtcNow > expiry)
			QueueDownload(qt, filePath);
	}

    // Only convert images that are downloadable (don't mess with things the user put here!)
	if(World.Settings.ConvertDownloadedImagesToDds && IsDownloadableLayer)
		ConvertImage(texture, filePath);*/

	return Texture2D;
}

void FImageStore::QueueDownload(UQuadTile* qt, FString filePath)
{

	FString url = GetDownloadUrl(qt);
	/*qt.QuadTileSet.AddToDownloadQueue(qt.QuadTileSet.Camera, 
		new GeoSpatialDownloadRequest(qt, this, filePath, url));*/
	
}
