// Fill out your copyright notice in the Description page of Project Settings.

#include "Terrain/MyBlueprintFunctionLibrary.h"

#include "Math/MathEngine.h"
#include "Interfaces/IPluginManager.h"


// 在SRTM1原始数据中双线性插值
// InterPt中 X 表示行的位置，Y 表示列的位置
template <typename T>
double UMyBlueprintFunctionLibrary::BilinearInter(FVector2D InterPt, TArray<TArray<T>>& RawData)
{
	double Rst = 0;
	if (InterPt.ComponentwiseAllGreaterOrEqual(FVector2D::ZeroVector) && InterPt.ComponentwiseAllLessOrEqual(FVector2D::UnitVector))
	{
		int32 ArrRowNum = RawData.Num();
	
		double LowerLeft, TopLeft, TopRight, LowerRight;
		FVector2D InterPtLoc = InterPt*(ArrRowNum - 1);
		FIntPoint TopLeftIndex(FMath::TruncToInt32(InterPtLoc.X), FMath::TruncToInt32(InterPtLoc.Y));

		// 待插值点刚好落在网格交点上面，直接返回该交点值
		if (FMath::Frac(InterPtLoc.X) < SMALL_NUMBER && FMath::Frac(InterPtLoc.Y) < SMALL_NUMBER)
		{
			Rst = (double)RawData[TopLeftIndex.X][TopLeftIndex.Y];
		}
		// 待插值点落在网格竖线上面，由上下两个点插值
		else if (FMath::Frac(InterPtLoc.X) >= SMALL_NUMBER && FMath::Frac(InterPtLoc.Y) < SMALL_NUMBER)
		{
			double Top = (double)RawData[TopLeftIndex.X][TopLeftIndex.Y];
			double Bottom = (double)RawData[TopLeftIndex.X + 1][TopLeftIndex.Y];
			Rst = FMath::Lerp(Top, Bottom, FMath::Frac(InterPtLoc.X));
		}
		// 待插值点落在网格横线上面，由左右两个点插值
		else if (FMath::Frac(InterPtLoc.X) < SMALL_NUMBER && FMath::Frac(InterPtLoc.Y) >= SMALL_NUMBER)
		{
			double Left = (double)RawData[TopLeftIndex.X][TopLeftIndex.Y];
			double Right = (double)RawData[TopLeftIndex.X][TopLeftIndex.Y + 1];
			Rst = FMath::Lerp(Left, Right, FMath::Frac(InterPtLoc.Y));
		}
		// 待插值点落在网格内，传统的二次线性插值
		else
		{
			TopLeft = (double)RawData[TopLeftIndex.X][TopLeftIndex.Y];
			TopRight = (double)RawData[TopLeftIndex.X][(TopLeftIndex.Y + 1)];
			LowerLeft = (double)RawData[(TopLeftIndex.X + 1)][TopLeftIndex.Y];
			LowerRight = (double)RawData[(TopLeftIndex.X + 1)][(TopLeftIndex.Y + 1)];

			double InterPtXFrac = FMath::Frac(InterPtLoc.X);
			double InterPtYFrac = FMath::Frac(InterPtLoc.Y);

			double TopInter = FMath::Lerp(TopLeft, TopRight, InterPtYFrac);
			double LowerInter = FMath::Lerp(LowerLeft, LowerRight, InterPtYFrac);
			Rst = FMath::Lerp(TopInter, LowerInter, InterPtXFrac);
		}
	}
	return Rst;
}


// 输出成9*9的
TArray<TArray<float>> UMyBlueprintFunctionLibrary::ExpandDataBlock(TArray<TArray<float>>& SourceDataArr)
{
	TArray<TArray<float>> Result;

	for (int32 i = 0; i <= 8; i++)
	{
		TArray<float> Temp;
		for (int32 j = 0; j <= 8; j++)
		{
			float Inter = (float)BilinearInter(FVector2D(double(i)/8, double(j)/8), SourceDataArr);
			Temp.Add(Inter/256);
		}
		Result.Add(Temp);
	}
	return Result;
}


// 传入12级的瓦片索引，生成12级-19级的高程瓦片
// X是瓦片的行（纬度），Y是瓦片的列（经度）
bool UMyBlueprintFunctionLibrary::GenerateEleDataGte12(int32 TileRow, int32 TileCol, FString FileSavePath)
{
	if ((TileRow < 0) || (TileRow >= 1<<11) || (TileCol < 0) && (TileCol >= 1<<12))
	{
		return false;
	}
	TArray<FVector2D> TileCorners;
	float TileDimen = 360 / FMath::Pow(2.f, 12.f);

	// 瓦片的角点经纬度坐标并非实际经纬度坐标
	// 按照左上角-右上角-右下角-左下角顺时针添加瓦片角点
	//
	TileCorners.Add(FVector2D(TileRow*TileDimen, TileCol*TileDimen));
	TileCorners.Add(FVector2D(TileRow*TileDimen, (TileCol + 1)*TileDimen));
	TileCorners.Add(FVector2D((TileRow + 1)*TileDimen, (TileCol + 1)*TileDimen));
	TileCorners.Add(FVector2D((TileRow + 1)*TileDimen, TileCol*TileDimen));

	// 读取该 Tile 可能用到的四个数据块进来，反正也只运行一次，内存也足够大
	// 比较粗糙的方法
	FString TopLeftFilePath =  FString::Printf(TEXT("E:/ElevationData/SRTM1/N%02dE%03d.hgt"), FMath::TruncToInt32(90 - TileCorners[0].X), FMath::TruncToInt32(TileCorners[0].Y - 180));
	FString TopRightFilePath =  FString::Printf(TEXT("E:/ElevationData/SRTM1/N%02dE%03d.hgt"), FMath::TruncToInt32(90 - TileCorners[1].X), FMath::TruncToInt32(TileCorners[1].Y - 180));
	FString LowerRightFilePath =  FString::Printf(TEXT("E:/ElevationData/SRTM1/N%02dE%03d.hgt"), FMath::TruncToInt32(90 - TileCorners[2].X), FMath::TruncToInt32(TileCorners[2].Y - 180));
	FString LowerLeftFilePath =  FString::Printf(TEXT("E:/ElevationData/SRTM1/N%02dE%03d.hgt"), FMath::TruncToInt32(90 - TileCorners[3].X), FMath::TruncToInt32(TileCorners[3].Y - 180));

	TArray<TArray<uint16>> LowerLeftRaw, TopLeftRaw, TopRightRaw, LowerRightRaw;
	// 必须四个文件都存在才行
	if (FPaths::FileExists(TopLeftFilePath) && FPaths::FileExists(TopRightFilePath) && FPaths::FileExists(LowerRightFilePath) && FPaths::FileExists(LowerLeftFilePath))
	{
		LoadHgtFileToArray(TopLeftRaw, *TopLeftFilePath);
		LoadHgtFileToArray(TopRightRaw, *TopRightFilePath);
		LoadHgtFileToArray(LowerRightRaw, *LowerRightFilePath);
		LoadHgtFileToArray(LowerLeftRaw, *LowerLeftFilePath);
	}
	else
	{
		return false;
	}

	// 左上角高程数据块的索引
	FVector2D TopLeftDataIndex(FMath::TruncToInt32(TileCorners[0].X), FMath::TruncToInt32(TileCorners[0].Y));

	// 生成12级瓦片的高程数据
	TArray<TArray<float>> Level12Data;
	// i控制行
	for (int32 i = 0; i <= 256; i++)
	{
		TArray<float> ScanLineData;
		// j控制列
		for (int32 j = 0; j <= 256; j++)
		{
			// 这里的坐标是横向0-360，纵向为0-180的，左上角为原点，类似于屏幕坐标系
			FVector2D InterPt = TileCorners[0] + FVector2D(i*TileDimen/256, j*TileDimen/256);
			FVector2D InterPtFrac = FVector2D(FMath::Frac(InterPt.X), FMath::Frac(InterPt.Y));

			// 采用右下角数据
			if (InterPt.X >= (TopLeftDataIndex.X + 1) && InterPt.Y >= (TopLeftDataIndex.Y + 1))
			{
				ScanLineData.Add(BilinearInter(InterPtFrac, LowerRightRaw));
			}
			// 采用左下角数据
			else if (InterPt.X >= (TopLeftDataIndex.X + 1) && InterPt.Y < (TopLeftDataIndex.Y + 1))
			{
				ScanLineData.Add(BilinearInter(InterPtFrac, LowerLeftRaw));
			}
			// 采用右上角数据
			else if (InterPt.X < (TopLeftDataIndex.X + 1) && InterPt.Y >= (TopLeftDataIndex.Y + 1))
			{
				ScanLineData.Add(BilinearInter(InterPtFrac, TopRightRaw));
			}
			else
			{
				ScanLineData.Add(BilinearInter(InterPtFrac, TopLeftRaw));
			}
		}
		Level12Data.Add(ScanLineData);
	}

	/*// 因为12级的分辨率和原始数据的分辨率较为接近，所以以12级数据为基准往下裁剪
	for (int32 ZoomLevel = 12; ZoomLevel < 13; ZoomLevel++)
	{
		int32 Partitions = (int32)(FMath::Pow(2, ZoomLevel - 12));
		int32 BlockSize = 256 / Partitions;
		// i控制行
		for (int32 i = 0; i < Partitions; i++)
		{
			// j控制列
			for (int32 j = 0; j < Partitions; j++)
			{
				TArray<uint8> DataBlock;
				for (int32 h = 0; h <= BlockSize; h++)
				{
					for (int32 k = 0; k <= BlockSize; k++)
					{
						DataBlock.Add(Level12Data[i*BlockSize + h][j*BlockSize + k]/256);
						DataBlock.Add(Level12Data[i*BlockSize + h][j*BlockSize + k]%256);
					}
				}

				FString EveTileName = FString::Printf(TEXT("E:/EveTile/L%02d/R%08x/C%08x.hgt"), ZoomLevel, TileIndex.X*Partitions + i, TileIndex.Y*Partitions + j);
				FFileHelper::SaveArrayToFile(ExpandDataBlock(DataBlock), *EveTileName);
			}
		}
	}*/

	// 将二维高程数组序列化至本地
	//FString EveTileName = FString::Printf(TEXT("E:/EveTile/L12/R%08x/C%08x.hgt"), TileIndex.X, TileIndex.Y);


	

	// 这里写的太啰嗦了
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!FPaths::DirectoryExists(*FPaths::GetPath(FileSavePath)))
	{
		if (!PlatformFile.CreateDirectoryTree(*FPaths::GetPath(FileSavePath)))
		{
			return false;
		}
	}

	IFileHandle* FileHandle = PlatformFile.OpenWrite(*FileSavePath);
	if (FileHandle)
	{
		for (auto& Item : Level12Data)
		{
			FileHandle->Write((const uint8*)Item.GetData(), Item.Num()*Item.GetTypeSize());
		}
		delete FileHandle;
	}
	else
	{
		return false;
	}

	return true;
}




bool UMyBlueprintFunctionLibrary::LoadHgtFileToArray(TArray<TArray<uint16>>& Result, const TCHAR* Filename)
{
	FArchive* Reader = IFileManager::Get().CreateFileReader(Filename);
	if(!Reader)
	{
		UE_LOG(LogStreaming,Warning,TEXT("Failed to read file '%s' error."), Filename);
		return false;
	}

	int64 TotalSize64 = Reader->TotalSize();
	if ( TotalSize64+2 > MAX_int32 )
	{
		UE_LOG(LogStreaming,Error,TEXT("File '%s' is too large for 32-bit reader (%lld), use TArray64."), Filename, TotalSize64);
		return false;
	}

	int32 TotalSize = FMath::TruncToInt32(FMath::Sqrt((float)(Reader->TotalSize()/2)));

	//Reader->ByteSwap()
	//Result.AddUninitialized( TotalSize );
	for (int32 i = 0; i < TotalSize; i++)
	{
		// Allocate slightly larger than file size to avoid re-allocation when caller null terminates file buffer
		TArray<uint16> Temp;
		Temp.AddUninitialized( TotalSize );

		Reader->Serialize(Temp.GetData(), Temp.Num() * Temp.GetTypeSize());
		for (auto& item : Temp)
		{
			item = BYTESWAP_ORDER16(item);
		}

		//Reader->byte
		Result.Add(Temp);
	}

	bool Success = Reader->Close();
	delete Reader;
	return Success;
}

FMatrix UMyBlueprintFunctionLibrary::CameraTest(double Latitude, double Longitude, double Heading, double Tilt)
{
	//FMatrix ViewRotationMatrix01 = FInverseRotationMatrix(Rotation);

	//FMatrix ViewRotationMatrix = ViewRotationMatrix01 * FMatrix(
	//	FPlane(0, 0, 1, 0),
	//	FPlane(1, 0, 0, 0),
	//	FPlane(0, 1, 0, 0),
	//	FPlane(0, 0, 0, 1));
	//
	///*UE_LOG(LogClass, Log, TEXT("%s"), *ViewRotationMatrix.ToString())*/

	FVector LookFrom = UMathEngine::SphericalToCartesian(Latitude, Longitude, 1200000);

	//FMatrix AbsoluteViewMatrix = FLookFromMatrix(
	//	MathEngine::SphericalToCartesian(Latitude, Longitude, 10000),
	//	FVector::Zero(),
	//	FVector::UpVector);
	UE_LOG(LogClass, Log, TEXT("视点位置：%s"), *LookFrom.ToString());
	
	FMatrix AbsoluteViewMatrix = FRotationMatrix::MakeFromX(-LookFrom);
	UE_LOG(LogClass, Log, TEXT("看向原点矩阵：%s"), *AbsoluteViewMatrix.ToString());
	
	// 旋转顺序的缘故，需要拆成两个旋转矩阵
	AbsoluteViewMatrix = FRotationMatrix(FRotator(Tilt, 0, 0)) * FRotationMatrix(FRotator(0, 0, Heading)) * AbsoluteViewMatrix;

	
	
	AbsoluteViewMatrix = AbsoluteViewMatrix.Inverse() * FMatrix(
	FPlane(0, 0, 1, 0),
	FPlane(1, 0, 0, 0),
	FPlane(0, 1, 0, 0),
	FPlane(0, 0, 0, 1));

	AbsoluteViewMatrix = FTranslationMatrix(-LookFrom) * AbsoluteViewMatrix;
	
	

	AbsoluteViewMatrix = FLookFromMatrix(LookFrom, -LookFrom, FVector(0, 0, 1));

	//AbsoluteViewMatrix = FRotationMatrix(FRotator(Tilt, 0, 0)) * FRotationMatrix(FRotator(0, 0, Heading)) * AbsoluteViewMatrix;

	
	UE_LOG(LogClass, Log, TEXT("最终欧拉角：%s"), *AbsoluteViewMatrix.ToString());

	return AbsoluteViewMatrix * FRotationMatrix(FRotator(0, Heading, 0)) * FRotationMatrix(FRotator(0, 0, -Tilt));
	

}



FRotator UMyBlueprintFunctionLibrary::Lookat(double Latitude, double Longitude, double Pitch, double Heading, double Roll)
{
	double lat = FMath::DegreesToRadians(90 - Latitude);
	double lon = FMath::DegreesToRadians(90 - Longitude);

	FVector Position = FVector2D(lat, lon).SphericalToUnitCartesian();

	auto RotationAtLocation = FRotationMatrix::MakeFromX(-Position);

	//RotationAtLocation.
	
	// The API euler angles need to be converted to the Unreal convention
	
	//const auto EngineRotation = RotationAtLocation * GeographicRotator.Quaternion();
	const auto EngineRotation = RotationAtLocation.ToQuat() * (FRotator(0, 0, Heading).Quaternion() * FRotator(Pitch, 0, 0).Quaternion());

	//return FMatrix(NewX, NewY, NewZ, FVector::ZeroVector).Rotator();
	return EngineRotation.Rotator();
}

double UMyBlueprintFunctionLibrary::Area(const TArray<FVector2D> vets, bool& sss)
{
	sss = UMathEngine::IsPolygonWindingCCW(vets);
	return UMathEngine::PolygonArea(vets);
}
