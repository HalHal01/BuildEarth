// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"


//#include "QuadTile.h"


class UQuadTile;
/**
 * Base class for calculating local image paths and remote download urls
 */
class WORLDWIND_API FImageStore
{
public:
	FImageStore();
	virtual ~FImageStore();

	// Data directory for this layer (permanently stored images)
	// 永久保存地址
	FString	m_DataDirectory = FString(TEXT("E:/BMDownload/成都市_卫图/瓦片_ArcGis/_alllayers"));

	// Coverage of outer level 0 bitmaps (decimal degrees)
	// Level 1 has half the coverage, level 2 half of level 1 (1/4) etc.
	double m_LevelZeroTileSizeDegrees = 22.5f;

	// Number of detail levels
	int32 m_LevelCount = 1;

	// File extension of the source image file format
	FString m_ImageFileExtension = FString(TEXT("png"));

	// Cache subdirectory for this layer
	FString m_CacheDirectory;

	/// Default texture to be used (always ocean?)
	/// Can be either file or url
	FString m_DuplicateTexturePath;

	/// Server Logo path for Downloadable layers
    FString m_Serverlogo;

public:

	// 用途未知？？？
	virtual bool IsDownloadableLayer()
	{
		return false;
	}

	virtual FString GetLocalPath(UQuadTile* qt);

	// Figure out how to download the image.
	// 未完待续: Allow subclasses to have control over how images are downloaded, 
	// not just the download url.
	virtual FString GetDownloadUrl(UQuadTile* qt);


	/// Deletes the cached copy of the tile.
	/// <param name="qt"></param>
	virtual void DeleteLocalCopy(UQuadTile* qt)
	{
		FString filename = GetLocalPath(qt);
		if(FPaths::FileExists(filename))
		{
			IFileManager::Get().Delete(*filename);
		}
	}

	/*/// <summary>
	/// Converts image file to DDS
	/// </summary>
	protected virtual void ConvertImage(Texture texture, string filePath)
	{
		if(filePath.ToLower().EndsWith(".dds"))
			// Image is already DDS
				return;

		// User has selected to convert downloaded images to DDS
		string convertedPath = Path.Combine(
			Path.GetDirectoryName(filePath),
			Path.GetFileNameWithoutExtension(filePath)+".dds");

		TextureLoader.Save(convertedPath, ImageFileFormat.Dds, texture );

		// Delete the old file
		try
		{
			File.Delete(filePath);
		}
		catch
		{
		}
	}*/

	UTexture2D* LoadFile(UQuadTile* qt);


	void QueueDownload(UQuadTile* qt, FString filePath);

};
