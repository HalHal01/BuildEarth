// Fill out your copyright notice in the Description page of Project Settings.
#include "Compass3D/Compass3D.h"
#include "Kismet/KismetMathLibrary.h"

void UCompass3DWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 获取源二维纹理中的颜色数据
	TextureCompressionSettings OldCompressionSettings = SourceTexture2D->CompressionSettings;
	TextureMipGenSettings OldMipGenSettings = SourceTexture2D->MipGenSettings;
	bool OldSRGB = SourceTexture2D->SRGB;

	SourceTexture2D->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	SourceTexture2D->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	SourceTexture2D->SRGB = false;
	SourceTexture2D->UpdateResource();

	FColor* MipData = static_cast<FColor*>(SourceTexture2D->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_ONLY));
	check(MipData != nullptr);
	int64 MipDataSize = SourceTexture2D->GetPlatformData()->Mips[0].BulkData.GetBulkDataSize();

	const int32 Width = SourceTexture2D->Source.GetSizeX();
	const int32 Height = SourceTexture2D->Source.GetSizeY();

	TArray<FColor> PixelColors01;
	SourcePixelColors.Init(FColor::Transparent, Width*Height);
	TargetPixelColors.Init(FColor::Transparent, Width*Height);
	FMemory::Memcpy(SourcePixelColors.GetData(), MipData, MipDataSize);
	FMemory::Memcpy(TargetPixelColors.GetData(), MipData, MipDataSize);

	UE_LOG(LogClass, Log, TEXT("%d"), MipDataSize);
	// 还原设置
	SourceTexture2D->GetPlatformData()->Mips[0].BulkData.Unlock();
	SourceTexture2D->CompressionSettings = OldCompressionSettings;
	SourceTexture2D->MipGenSettings = OldMipGenSettings;
	SourceTexture2D->SRGB = OldSRGB;
	SourceTexture2D->UpdateResource();

	// 创建新的纹理
	TargetTexture2D = UTexture2D::CreateTransient(Width, Height, EPixelFormat::PF_B8G8R8A8);
	TargetTexture2D->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	TargetTexture2D->SRGB = true;
	TargetTexture2D->MipGenSettings = TMGS_NoMipmaps;
	TargetTexture2D->UpdateResource();
  
}

/*void UCompass3DWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}*/

void UCompass3DWidget::UpdateTexture(double Heading, double Tilt)
{
	FVector CameraLocation = UKismetMathLibrary::Spherical2DToUnitCartesian(FVector2D(FMath::DegreesToRadians(Tilt), FMath::DegreesToRadians(Heading))) * FVector(-128, -128, 128);
	FRotator CameraRotator = FRotator(Tilt - 90, Heading, 0);

	// 构建透视矩阵，FOV为90度的，宽高比为1的视锥体
	// Avoid divide by zero in the projection matrix calculation by clamping FOV
	FMatrix ProjectionMatrix = FReversedZPerspectiveMatrix(
		FMath::Max(0.001f, 90) * (float)PI / 360.0f,
		1.0f,
		1.0f,
		GNearClippingPlane );

	FMatrix ViewRotationMatrix = FInverseRotationMatrix(CameraRotator) * FMatrix(
	FPlane(0, 0, 1, 0),
	FPlane(1, 0, 0, 0),
	FPlane(0, 1, 0, 0),
	FPlane(0, 0, 0, 1));

	FMatrix ViewMatrix = FTranslationMatrix(-CameraLocation) * ViewRotationMatrix;
	FMatrix ViewProjectionMatrix01 =  ViewMatrix * ProjectionMatrix;

	ReMap(ViewProjectionMatrix01);
}

bool UCompass3DWidget::Project(const FVector& WorldPosition, const FMatrix& ViewProjectionMatrix, const int32 Width, const int32 Height, FVector2D& ScreenPosition)
{
	FPlane Result = ViewProjectionMatrix.TransformFVector4(FVector4(WorldPosition, 1.f));
	if (Result.W > 0.0f)
	{
		// the result of this will be x and y coords in -1..1 projection space
		const float RHW = 1.0f / Result.W;
		FPlane PosInScreenSpace = FPlane(Result.X * RHW, Result.Y * RHW, Result.Z * RHW, Result.W);

		// Move from projection space to normalized 0..1 UI space
		const float NormalizedX = (PosInScreenSpace.X / 2.f) + 0.5f;
		const float NormalizedY = 1.f - (PosInScreenSpace.Y / 2.f) - 0.5f;

		ScreenPosition = FVector2D(NormalizedX * (float)Width, NormalizedY * (float)Height);
		return true;
	}

	return false;
}

// TODO01：似乎只能在编辑器下使用？
// 算法需要优化
bool UCompass3DWidget::ReMap(FMatrix& ViewProjectionMatrix)
{
	const int32 Width = SourceTexture2D->Source.GetSizeX();
	const int32 Height = SourceTexture2D->Source.GetSizeY();

	// 全透明
	FMemory::Memzero(TargetPixelColors.GetData(), TargetPixelColors.Num() * TargetPixelColors.GetTypeSize());
	for (int32 y = 0; y < Height; y++)
	{
		for (int32 x = 0; x < Width; x++)
		{
			FVector2D PixelCenter = FVector2d((float)(x - Width/2) + 0.5, (float)(y - Height/2) + 0.5);
			FVector2D ScreenPosition;
			if (Project(FVector(PixelCenter, 0.f), ViewProjectionMatrix, Width, Height, ScreenPosition))
			{
				FColor DestPtr = SourcePixelColors[y * Width + x];

				int32 row = FMath::RoundToInt32(ScreenPosition.Y - 0.5);
				int32 column = FMath::RoundToInt32(ScreenPosition.X - 0.5);
				//UE_LOG(LogClass, Log, TEXT("%d---->%d"), row, column);
				if (FMath::IsWithin(row, 0, Height) && FMath::IsWithin(column, 0, Width))
				{
					TargetPixelColors[row * Width + column] = DestPtr;
				}
			}
		}
	}

	void* RawTextureData = TargetTexture2D->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(RawTextureData, TargetPixelColors.GetData(), TargetPixelColors.Num() * TargetPixelColors.GetTypeSize());
	TargetTexture2D->GetPlatformData()->Mips[0].BulkData.Unlock();
	TargetTexture2D->UpdateResource();

	return true;
}