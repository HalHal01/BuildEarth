// Fill out your copyright notice in the Description page of Project Settings.




#include "Terrain/TerrainTile.h"
#include "WorldWindSettings.h"
#include "Interfaces/IPluginManager.h"
#include "Terrain/MyBlueprintFunctionLibrary.h"


FTerrainTile::FTerrainTile(int32 TileRow, int32 TileCol, int32 TileLevel)
{
	Row = TileRow;
	Col = TileCol;
	TargetLevel = TileLevel;
	IsInitialized = false;
}


// 不管是否找到数据，都要填充高程数组
void FTerrainTile::Initialize()
{
	if(IsInitialized)
		return;
	/*// 下载的，跳过
	if(!FPaths::FileExists(TerrainTileFilePath))
	{
		// Download elevation
		if(request==null)
		{
			using( request = new TerrainDownloadRequest(this, m_owner, Row, Col, TargetLevel) )
			{
				request.SaveFilePath = TerrainTileFilePath;
				request.DownloadInForeground();
			}
		}
	}*/

	/*if(ElevationData==null)
		ElevationData = new float[SamplesPerTile, SamplesPerTile];*/
	
	//UWorldWindSettings* TempSettings = GetMutableDefault<UWorldWindSettings>();

	int32 Partitions = 8;

	// 去寻找12级的高程瓦片
	int32 Level12Row = Row / (1 <<(TargetLevel - 12));
	int32 Level12Col = Col / (1 <<(TargetLevel - 12));

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// 高程瓦片文件的路径
	TerrainTileFilePath = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("WorldWind"))->GetBaseDir(),
		TEXT("Terrain"),
		FString::Printf(TEXT("/L%02d/R%08x/C%08x.hgt"), 12, Level12Row, Level12Col));

	// 确保高程瓦片一定存在
	if(!FPaths::FileExists(TerrainTileFilePath))
	{
		if (!UMyBlueprintFunctionLibrary::GenerateEleDataGte12(Level12Row, Level12Col, TerrainTileFilePath))
		{
			
		}
	}

	TArray<TArray<float>> Level12ElevationData;
		
	if (IFileHandle* FileHandle = PlatformFile.OpenRead(*TerrainTileFilePath))
	{
		int32 ScanLineNum = FMath::RoundToInt(FMath::Sqrt(static_cast<float>(FileHandle->Size() / 4)));
		for (int32 i = 0; i < ScanLineNum; i++)
		{
			TArray<float> ScanLineData;
			ScanLineData.AddUninitialized(ScanLineNum);
			{
				FileHandle->Read((uint8*)ScanLineData.GetData(), ScanLineData.Num() * ScanLineData.GetTypeSize());
			}
			Level12ElevationData.Add(ScanLineData);
		}
		delete FileHandle;
	}

	// 将原始高程数据采样为适合该瓦片尺寸的二维数组
	float SamplingInter = (float)((Level12ElevationData.Num() - 1) / (1 <<(TargetLevel - 12))) / Partitions;

	// 该瓦片高程数据块在12级大瓦片中的位置，这里的256是人为设置的
	FIntPoint EveDataPos((Row % (1 <<(TargetLevel - 12))) * (256 / (1 <<(TargetLevel - 12))),
		(Col % (1 <<(TargetLevel - 12))) * (256 / (1 <<(TargetLevel - 12))));

	for (int32 i = 0; i <= Partitions; i++)
	{
		TArray<float> ScanlineData;
		ScanlineData.Add(0.f);
		for (int32 j = 0; j <= Partitions; j++)
		{
			double aaa = UMyBlueprintFunctionLibrary::BilinearInter(FVector2D((float)(EveDataPos.X + i*SamplingInter) / 256, (float)(EveDataPos.Y + j*SamplingInter) / 256), 
				Level12ElevationData);
			ScanlineData.Add(aaa);
		}
		ScanlineData.Add(0.f);
		ElevationData.Add(ScanlineData);
	}


	TArray<float> temp;
	temp.Init(0.f, Partitions + 3);
	ElevationData.Insert(temp, 0);
	ElevationData.Add(temp);

		

	




	

	IsInitialized = true;
	IsValid = true;
		
		/*// Load elevation file
		try
		{
			// TerrainDownloadRequest's FlagBadTile() creates empty files
			// as a way to flag "bad" terrain tiles.
			// Remove the empty 'flag' files after preset time.
			try
			{
				FileInfo tileInfo = new FileInfo(TerrainTileFilePath);
				if(tileInfo.Length == 0)
				{
					TimeSpan age = DateTime.Now.Subtract( tileInfo.LastWriteTime );
					if(age < m_owner.TerrainTileRetryInterval)
					{
						// This tile is still flagged bad
						IsInitialized = true;
					}
					else
					{
						// remove the empty 'flag' file
						File.Delete(TerrainTileFilePath);
					}
					return;
				}
			}
			catch (...)
			{
				// Ignore any errors in the above block, and continue.
				// For example, if someone had the empty 'flag' file
				// open, the delete would fail.
			}
			TArray<uint8> EleRawData;
			FFileHelper::LoadFileToArray(EleRawData, *TerrainTileFilePath);
			{
				BinaryReader reader = new BinaryReader(s);
				if(m_owner.DataType=="Int16")
				{
					/*
					byte[] tfBuffer = new byte[SamplesPerTile*SamplesPerTile*2];
					if (s.Read(tfBuffer,0,tfBuffer.Length) < tfBuffer.Length)
						throw new IOException(string.Format("End of file error while reading terrain file '{0}'.", TerrainTileFilePath) );

					int offset = 0;
					for(int y = 0; y < SamplesPerTile; y++)
						for(int x = 0; x < SamplesPerTile; x++)
							ElevationData[x,y] = tfBuffer[offset++] + (short)(tfBuffer[offset++]<<8);
					#1#
					for(int y = 0; y < SamplesPerTile; y++)
						for(int x = 0; x < SamplesPerTile; x++)
							ElevationData[x,y] = EleRawData;
				}
				if(m_owner.DataType=="Float32")
				{
					/*
					byte[] tfBuffer = new byte[SamplesPerTile*SamplesPerTile*4];
					if (s.Read(tfBuffer,0,tfBuffer.Length) < tfBuffer.Length)
							throw new IOException(string.Format("End of file error while reading terrain file '{0}'.", TerrainTileFilePath) );
					#1#
					for(int y = 0; y < SamplesPerTile; y++)
						for(int x = 0; x < SamplesPerTile; x++)
						{
							ElevationData[x,y] = reader.ReadSingle();
						}
				}
				IsInitialized = true;
				IsValid = true;
			}
			return;
		}*/

		/*// 各种异常
		catch(IOException)
		{
			// If there is an IO exception when reading the terrain tile,
			// then either something is wrong with the file, or with
			// access to the file, so try and remove it.
			try
			{
				File.Delete(TerrainTileFilePath);
			}
			catch(Exception ex)
			{
				throw new ApplicationException(String.Format("Error while trying to delete corrupt terrain tile {0}", TerrainTileFilePath), ex);
			}
		}
		catch(Exception ex)
		{
			// Some other type of error when reading the terrain tile.
			throw new ApplicationException(String.Format("Error while trying to read terrain tile {0}", TerrainTileFilePath), ex);
		}*/
	/*}
	
	{

		
		

		

		for (int32 i = 0; i <= SamplesPerTile; i++)
		{
			TArray<float> ScanlineData;
			
			ScanlineData.Init(0.0f, SamplesPerTile + 1);
			ElevationData.Add(ScanlineData);
		}
	}*/
}




// 二次线性插值
float FTerrainTile::GetElevationAt(double Latitude, double Longitude)
{
	double DeltaLat = North - Latitude;
	double DeltaLon = Longitude - West;

	double LatPixel = DeltaLat * (SamplesPerTile - 1) / TileSizeDegrees;
	double LonPixel = DeltaLon * (SamplesPerTile - 1) / TileSizeDegrees;

	int32 LatGridMin = FMath::Clamp(FMath::FloorToInt32(LatPixel), 0, SamplesPerTile - 1);
	int32 LatGridMax = FMath::Clamp(FMath::CeilToInt32(LatPixel), 0, SamplesPerTile - 1);
	int32 LonGridMin = FMath::Clamp(FMath::FloorToInt32(LonPixel), 0, SamplesPerTile - 1);
	int32 LonGridMax = FMath::Clamp(FMath::CeilToInt32(LonPixel), 0, SamplesPerTile - 1);

	float Delta = LatPixel - LatGridMin;
	float WestElevation = 
		ElevationData[LonGridMin][LatGridMin]*(1-Delta) + ElevationData[LonGridMin][LatGridMax]*Delta;

	float EastElevation = 
		ElevationData[LonGridMax][LatGridMin]*(1-Delta) + ElevationData[LonGridMax][LatGridMax]*Delta;

	Delta = LonPixel - LonGridMin;
	const float InterpolatedElevation = WestElevation*(1-Delta) + EastElevation*Delta;

	return InterpolatedElevation;
}