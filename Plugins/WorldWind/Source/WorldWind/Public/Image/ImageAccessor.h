// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ImageAccessor.generated.h"




class ImageTileInfo
{
	FString m_imagePath;
	FString m_uri;

	/*/// <summary>
	/// Local full path to the image file (cache or data)
	/// </summary>
	FString ImagePath
	{
		get
		{
			return m_imagePath;
		}
		set
		{
			m_imagePath = value;
		}
	}

	/// <summary>
	/// Uri for downloading image from network
	/// </summary>
	public string Uri
	{
		get
		{
			return m_uri;
		}
	}


		
	public ImageTileInfo(string imagePath)
	{
		m_imagePath = imagePath;
	}

	public ImageTileInfo(string imagePath, string uri)
	{
		m_imagePath = imagePath;
		m_uri = uri;
	}

	/// <summary>
	/// Check if this image tile is available locally.
	/// </summary>
	public bool Exists()
	{
		if (File.Exists(m_imagePath))
			return true;
		return false;
	}*/
};

// Summary description for ImageAccessor.


/// <summary>
/// Summary description for ImageAccessor.
/// </summary>
UCLASS()
class WORLDWIND_API UImageAccessor : public UObject
{
	GENERATED_BODY()

	FString	m_DataDirectory;
	int32	m_TextureSizePixels;
	double m_LevelZeroTileSizeDegrees;
	int32 m_NumberLevels;
	// 不知道是啥
	/*Texture m_ServerLogo;
	Rectangle m_ServerLogoSize;*/
	FTimespan m_DataExpirationTime = FTimespan(ETimespan::MaxTicks);


	/// <summary>
	/// File extension of the source image file format
	/// </summary>
	FString m_ImageFileExtension;

	/// <summary>
	/// Cache subdirectory for this layer
	/// </summary>
	FString m_CacheDirectory;

	FString m_DuplicateTexturePath;



	// 不知道是做啥的？
	/*protected WMSLayerAccessor m_wmsLayerAccessor;
	protected ImageTileService m_imageTileService;*/



	// 全是构造函数
	/*/// <summary>
	/// Initializes a new instance of the <see cref= "T:WorldWind.ImageAccessor"/> class.
	/// </summary>
	/// <param name="permanentTextureDirectory"></param>
	/// <param name="textureSizePixels"></param>
	/// <param name="levelZeroTileSizeDegrees"></param>
	/// <param name="numberLevels"></param>
	/// <param name="imageFileExtension"></param>
	/// <param name="cacheDirectory"></param>
	public ImageAccessor(
		FString permanentTextureDirectory, 
		int32 textureSizePixels, 
		double levelZeroTileSizeDegrees,
		int32 numberLevels,
		FString imageFileExtension,
		FString cacheDirectory)
	{
		m_DataDirectory = permanentTextureDirectory;
		m_TextureSizePixels = textureSizePixels;
		m_LevelZeroTileSizeDegrees = levelZeroTileSizeDegrees;
		m_NumberLevels = numberLevels;
		ImageExtension = imageFileExtension;
		m_CacheDirectory = cacheDirectory;
	}
	
	/// <summary>
	/// Initializes a new instance of the <see cref= "T:WorldWind.ImageAccessor"/> class.
	/// </summary>
	/// <param name="permanentTextureDirectory"></param>
	/// <param name="textureSizePixels"></param>
	/// <param name="levelZeroTileSizeDegrees"></param>
	/// <param name="numberLevels"></param>
	/// <param name="imageFileExtension"></param>
	/// <param name="cacheDirectory"></param>
	/// <param name="duplicateTextureFilePath"></param>
	public ImageAccessor(
		string permanentTextureDirectory, 
		int textureSizePixels, 
		double levelZeroTileSizeDegrees,
		int numberLevels,
		string imageFileExtension,
		string cacheDirectory,
		string duplicateTextureFilePath
		)
	{
		m_DataDirectory = permanentTextureDirectory;
		m_TextureSizePixels = textureSizePixels;
		m_LevelZeroTileSizeDegrees = levelZeroTileSizeDegrees;
		m_NumberLevels = numberLevels;
		ImageExtension = imageFileExtension;
		m_CacheDirectory = cacheDirectory;
		m_DuplicateTexturePath = duplicateTextureFilePath;
	}
	
	/// <summary>
	/// Initializes a new instance of the <see cref= "T:WorldWind.ImageAccessor"/> class.
	/// </summary>
	/// <param name="permanentTextureDirectory"></param>
	/// <param name="textureSizePixels"></param>
	/// <param name="levelZeroTileSizeDegrees"></param>
	/// <param name="numberLevels"></param>
	/// <param name="imageFileExtension"></param>
	/// <param name="cacheDirectory"></param>
	/// <param name="wmsLayerAccessor"></param>
	ImageAccessor(
		string permanentTextureDirectory, 
		int textureSizePixels, 
		double levelZeroTileSizeDegrees,
		int numberLevels,
		string imageFileExtension,
		string cacheDirectory,
		WMSLayerAccessor wmsLayerAccessor
		)
	{
		m_DataDirectory = permanentTextureDirectory;
		m_TextureSizePixels = textureSizePixels;
		m_LevelZeroTileSizeDegrees = levelZeroTileSizeDegrees;
		m_NumberLevels = numberLevels;
		ImageExtension = imageFileExtension;
		m_CacheDirectory = cacheDirectory;
		m_wmsLayerAccessor = wmsLayerAccessor;
	}
	
	/// <summary>
	/// Initializes a new instance of the <see cref= "T:WorldWind.ImageAccessor"/> class.
	/// </summary>
	/// <param name="permanentTextureDirectory"></param>
	/// <param name="textureSizePixels"></param>
	/// <param name="levelZeroTileSizeDegrees"></param>
	/// <param name="numberLevels"></param>
	/// <param name="imageFileExtension"></param>
	/// <param name="cacheDirectory"></param>
	/// <param name="imageTileService"></param>
	public ImageAccessor(
		string permanentTextureDirectory, 
		int textureSizePixels, 
		double levelZeroTileSizeDegrees,
		int numberLevels,
		string imageFileExtension,
		string cacheDirectory,
		ImageTileService imageTileService
		)
	{
		m_DataDirectory = permanentTextureDirectory;
		m_TextureSizePixels = textureSizePixels;
		m_LevelZeroTileSizeDegrees = levelZeroTileSizeDegrees;
		m_NumberLevels = numberLevels;
		ImageExtension = imageFileExtension;
		m_CacheDirectory = cacheDirectory;
		m_imageTileService  = imageTileService;
	}*/

		/*ImageTileInfo GetImageTileInfo(int level, int row, int col)
		{
			return GetImageTileInfo(level, row, col, true);
		}*/

		/*ImageTileInfo GetImageTileInfo(int level, int row, int col, bool allowCache)
		{
			if(level >= m_NumberLevels)
				throw new ArgumentException("Level " + level.ToString() + " not available.");

			FString relativePath = FString::Printf(TEXT("{0}\{1:D4}\{1:D4}_{2:D4}.{3}", level, row, col, m_imageFileExtension));
			
			if(m_DataDirectory != nullptr)
			{
				// Search data directory first
				FString rawFullPath = FPaths::Combine( m_DataDirectory, relativePath );
				if(FPaths::FileExists(rawFullPath))
					return new ImageTileInfo(rawFullPath);
			}
	
			// Try cache with default file extension
			FString cacheFullPath = FPaths::Combine( m_CacheDirectory, relativePath );
			if(FPaths::FileExists(cacheFullPath))
				return new ImageTileInfo(cacheFullPath);

			// Try cache but accept any valid image file extension
			const FString ValidExtensions = ".bmp.dds.dib.hdr.jpg.jpeg.pfm.png.ppm.tga.gif.tif";
			
			if(allowCache)
			{
				string cacheSearchPath = Path.GetDirectoryName(cacheFullPath);
				if(Directory.Exists(cacheSearchPath))
				{
					foreach( string imageFile in Directory.GetFiles(
						cacheSearchPath, 
						Path.GetFileNameWithoutExtension(cacheFullPath) + ".*") )
					{
						string extension = Path.GetExtension(imageFile).ToLower();
						if(ValidExtensions.IndexOf(extension)<0)
							continue;

						if(m_imageTileService != null)
						{
							return new ImageTileInfo(
								imageFile, 
								m_imageTileService.GetImageTileServiceUri(level, row, col) );
						}
			
						if(m_wmsLayerAccessor != null)
						{
							double tileRange = m_LevelZeroTileSizeDegrees * FMath::Pow(0.5, level);
							double west = -180.0 + col * tileRange;
							double south = -90.0 + row * tileRange;

							FString fileUri = m_wmsLayerAccessor.GetWMSRequestUrl(
								west, south, west + tileRange, south + tileRange,
								m_TextureSizePixels, m_TextureSizePixels);

							return new ImageTileInfo(imageFile, fileUri);
						}

						return new ImageTileInfo(imageFile);
					}
				}
			}

			if(m_imageTileService != nullptr)
			{
				return new ImageTileInfo(
					cacheFullPath, 
					m_imageTileService.GetImageTileServiceUri(level, row, col) );
			}
			
			if(m_wmsLayerAccessor != null)
			{
				double tileRange = m_LevelZeroTileSizeDegrees * FMath::Pow(0.5, level);
				double west = -180.0 + col * tileRange;
				double south = -90.0 + row * tileRange;

				string fileUri = m_wmsLayerAccessor.GetWMSRequestUrl(
					west, south, west + tileRange, south + tileRange,
					m_TextureSizePixels, m_TextureSizePixels);

				return new ImageTileInfo(Path.Combine(m_CacheDirectory, relativePath), fileUri);
			}
			
			// No success, return our "duplicate" tile if any
			if(m_DuplicateTexturePath != null && File.Exists(m_DuplicateTexturePath))
			{
				return new ImageTileInfo(m_DuplicateTexturePath, 
					Path.Combine(m_CacheDirectory, m_DuplicateTexturePath));
			}

			return null;
			//throw new ApplicationException("No image access method available.");
		}*/
		

		/*void Dispose()
		{
			if(m_ServerLogo!=null)
			{
				m_ServerLogo.Dispose();
				m_ServerLogo = null;
			}

			GC.SuppressFinalize(this);
		}*/

		
};