// Fill out your copyright notice in the Description page of Project Settings.


#include "QuadTile.h"

#include "GeographicCameraComponent.h"
#include "Math/BoundingBox.h"
#include "WebDownload/DownloadRequest.h"
#include "ImageUtils.h"
#include "QuadTileSet.h"

#include "Text3DComponent.h"
#include "WorldWindSettings.h"
#include "Engine/Texture2DDynamic.h"
#include "Kismet/KismetMathLibrary.h"
#include "Terrain/TerrainAccessorSubsystem.h"



/*// Sets default values for this component's properties
UQuadTile::UQuadTile()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}*/


// Called when the game starts
void UQuadTile::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UQuadTile::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// Initializes a new instance of the <see cref= "T:WorldWind.Renderable.QuadTile"/> class.
// 构造的时候还是用度，这样可读性更强
void UQuadTile::UpdateTileInfo(double South, double North, double West, double East, int Level)
{
	m_South = South;
	m_North = North;
	m_West = West;
	m_East = East;
	m_CenterLatitude = (m_North + m_South) / 2 ;
	m_CenterLongitude = (m_West + m_East) / 2;
	m_LatitudeSpan = FMath::Abs(m_North - m_South);
	m_LongitudeSpan = FMath::Abs(m_East - m_West);

	m_Level = Level;

	UQuadTileSet* QuadTileSet = GetTypedOuter<UQuadTileSet>();
	m_BoundingBox = FBoundingBox(m_South, m_North, m_West, m_East,
		QuadTileSet->m_LayerRadius, QuadTileSet->m_LayerRadius + 1000.f); // 原本是30000
	// m_LocalOrigin = m_BoundingBox.CalculateCenter();
	m_LocalOrigin = UMathEngine::SphericalToCartesian(m_CenterLatitude, m_CenterLongitude, QuadTileSet->m_LayerRadius);

	// TODO01: 在UE5中应该不需要这样处理？
	/*// To avoid gaps between neighbouring tiles truncate the origin to 
	// a number that doesn't get rounded. (nearest 10km)
	m_LocalOrigin.X = FMath::RoundToDouble(m_LocalOrigin.X / 10000) * 10000;
	m_LocalOrigin.Y = FMath::RoundToDouble(m_LocalOrigin.Y / 10000) * 10000;
	m_LocalOrigin.Z = FMath::RoundToDouble(m_LocalOrigin.Z / 10000) * 10000;*/

	m_Row = UMathEngine::GetRowFromLatitude(m_CenterLatitude, m_LatitudeSpan);
	m_Col = UMathEngine::GetColFromLongitude(m_CenterLongitude, m_LongitudeSpan);
}

void UQuadTile::ResetCache()
{
	try
	{
		m_isResetingCache = true;
		this->m_isInitialized = false;

		if (m_NorthEastChild != nullptr)
		{
			m_NorthEastChild->ResetCache();
		}

		if (m_NorthWestChild != nullptr)
		{
			m_NorthWestChild->ResetCache();
		}

		if (m_SouthEastChild != nullptr)
		{
			m_SouthEastChild->ResetCache();
		}

		if (m_SouthWestChild != nullptr)
		{
			m_SouthWestChild->ResetCache();
		}

		Dispose();

		// TODO01: 应该是去停掉正在下载的图片？
        /*for (int i = 0; i < QuadTileSet.ImageStores.Length; i++)
        {
            if((QuadTileSet.ImageStores[i] != null) && QuadTileSet.ImageStores[i].IsDownloadableLayer)
                QuadTileSet.ImageStores[i].DeleteLocalCopy(this);
        }*/

		m_isResetingCache = false;
	}
	catch(...)
	{
	}
}

// Returns the QuadTile for specified location if available.
// Tries to queue a download if not available.
// <returns>Initialized QuadTile if available locally, else null.</returns>
UQuadTile* UQuadTile::ComputeChild(double ChildSouth, double ChildNorth, double ChildWest, double ChildEast)
{
	UQuadTile* Child = NewObject<UQuadTile>(this);
	Child->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
	Child->RegisterComponent();

	Child->m_South = ChildSouth;
	Child->m_North = ChildNorth;
	Child->m_East = ChildEast;
	Child->m_West = ChildWest;
	Child->m_Level = m_Level + 1;
	/*Child->m_QuadTileSet = m_QuadTileSet;*/
	Child->UpdateTileInfo(ChildSouth, ChildNorth, ChildWest, ChildEast, m_Level + 1/*, m_QuadTileSet*/);

	return Child;
}

void UQuadTile::ComputeChildren(UGeographicCameraComponent* GeographicCamera)
{
	// TODO01: 
	/*if (Level + 1 >= QuadTileSet.ImageStores[0].LevelCount)
		return;*/

	double CenterLat = (m_South + m_North) / 2;
	double CenterLon = (m_East + m_West) / 2;
	if (m_NorthWestChild == nullptr)
		m_NorthWestChild = ComputeChild(CenterLat, m_North, m_West, CenterLon);

	if (m_NorthEastChild == nullptr)
		m_NorthEastChild = ComputeChild(CenterLat, m_North, CenterLon, m_East);

	if (m_SouthWestChild == nullptr)
		m_SouthWestChild = ComputeChild(m_South, CenterLat, m_West, CenterLon);

	if (m_SouthEastChild == nullptr)
		m_SouthEastChild = ComputeChild(m_South, CenterLat, CenterLon, m_East);
}

void UQuadTile::Dispose()
{
	try
	{
		m_isInitialized = false;
		// TODO01: 
		/*for (int i = 0; i < textures.Length; i++)
		{
			if (textures[i] != null && !textures[i].Disposed)
			{
				textures[i].Dispose();
				textures[i] = null;
			}
		}*/
		m_Textures.Empty();
		if (m_NorthWestChild != nullptr)
		{
			m_NorthWestChild->Dispose();
			m_NorthWestChild = nullptr;
		}
		if (m_SouthWestChild != nullptr)
		{
			m_SouthWestChild->Dispose();
			m_SouthWestChild = nullptr;
		}
		if (m_NorthEastChild != nullptr)
		{
			m_NorthEastChild->Dispose();
			m_NorthEastChild = nullptr;
		}
		if (m_SouthEastChild != nullptr)
		{
			m_SouthEastChild->Dispose();
			m_SouthEastChild = nullptr;
		}
		// TODO01: 
		/*if(DownloadRequest != null)
		{
			QuadTileSet.RemoveFromDownloadQueue(DownloadRequest);
			DownloadRequest.Dispose();
			DownloadRequest = null;
		}*/

	}
	catch(...)
	{
	}

	// TODO01: 是否是这样写的，待验证
	DestroyComponent();
}

void UQuadTile::Initialize()
{
	if (m_isResetingCache)
		return;
	try
	{
		// TODO01: 
		/*if (DownloadRequest != null)
		{
			// Waiting for download
			return;
		}
		if (textures == null)
		{
			textures = new Texture[QuadTileSet.ImageStores.Length];

			// not strictly necessary
			for (int i = 0; i < textures.Length; i++)
				textures[i] = null;
		}*/

        // assume we're finished.
		m_WaitingForDownload = false;

		// TODO01: 似乎是纹理下载好了才去创建网格体
		/*// check for missing textures.
		for (int i = 0; i < textures.Length; i++)
		{
			Texture newTexture = QuadTileSet.ImageStores[i].LoadFile(this);
			if (newTexture == null)
			{
				// At least one texture missing, wait for download
				WaitingForDownload = true;
			}

			// not entirely sure if this is a good idea...
			if (textures[i] != null)
				textures[i].Dispose();

			textures[i] = newTexture;
		}*/
		if (m_WaitingForDownload)
		{
			return;
		}

		m_isDownloadingImage = false;
		CreateTileMesh();
	}
	//catch (Microsoft.DirectX.Direct3D.Direct3DXException)
	catch(...)
	{
		//Log.Write(ex);
		//Texture load failed.
	}

	m_isInitialized = true;

	UGeoSpatialDownloadRequest* DownloadTask = NewObject<UGeoSpatialDownloadRequest>(this);

	//UDownloadQueue::GetSingleton()->Add(DownloadTask);
	
	DownloadTask->OnSuccess.AddDynamic(this, &UQuadTile::UpdateTileTexture);  // 必须先绑定，否则不会执行
	DownloadTask->Start();


	if ((m_Row%4==0) && (m_Col%24==0))
	{
		UE_LOG(LogClass, Log, TEXT("层级：%d， 行：%d， 列：%d"), m_Level, m_Row, m_Col);
	}
	UQuadTileSet* QuadTileSet = GetTypedOuter<UQuadTileSet>();
	//QuadTileSet->AddToDownloadQueue(DownloadTask);


}

// Updates this layer (background)
void UQuadTile::Update(UGeographicCameraComponent* GeographicCamera)
{
	if (m_isResetingCache)
	{
		return;
	}

	try
	{
		UQuadTileSet* QuadTileSet = GetTypedOuter<UQuadTileSet>();
		double TileSize = m_North - m_South;
		if (!m_isInitialized)
		{
			// 相机的视场角的1/2小于（瓦片大小*瓦片绘制距离的乘积）
			// 镜头中心与瓦片的中心点的夹角要小于2.9*块大小*1.25
			// 块在显示区域范围内
			bool a1 = GeographicCamera->m_ViewRange * 0.5 < (QuadTileSet->TileDrawDistance * TileSize);
			double a2 = UMathEngine::SphericalDistance(m_CenterLatitude, m_CenterLongitude, GeographicCamera->m_Latitude, GeographicCamera->m_Longitude);
			bool a3 = GeographicCamera->m_ViewFrustum.Intersects(m_BoundingBox);

			if (m_Level == 5 && m_Row == 5 && m_Col == 25)
			{
				float c1 = a2;
				bool a4 = a1 && a3;
				UE_LOG(LogClass, Log, TEXT("%f"), c1);
			}

			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("%s:%f,%s"), a1, a2, a3));
			
			if (GeographicCamera->m_ViewRange * 0.5 < (QuadTileSet->TileDrawDistance * TileSize)
				&& UMathEngine::SphericalDistance(m_CenterLatitude, m_CenterLongitude, GeographicCamera->m_Latitude, GeographicCamera->m_Longitude) < (QuadTileSet->TileDrawSpread * TileSize * 1.25)
				&& GeographicCamera->m_ViewFrustum.Intersects(m_BoundingBox))
			{
				Initialize();
				//UE_LOG(LogClass, Log, TEXT("相交瓦片：%d层，%d行，%d列"))
			}
		}

		// TODO01: 
		/*if (m_isInitialized /*&& World.Settings.VerticalExaggeration != verticalExaggeration || m_CurrentOpacity != QuadTileSet.Opacity ||
			QuadTileSet.RenderStruts != renderStruts#1#)
		{
			CreateTileMesh();
		}*/

		if (m_isInitialized)
		{
			// 此处含义是如果此 Tile 已经初始化了，将镜头视野缩小一倍，再去判断是否成立，如果为 True，进行子 Tile 的流程
			if (GeographicCamera->m_ViewRange < (QuadTileSet->TileDrawDistance * TileSize)
				&& UMathEngine::SphericalDistance(m_CenterLatitude, m_CenterLongitude, GeographicCamera->m_Latitude, GeographicCamera->m_Longitude) < (QuadTileSet->TileDrawSpread * TileSize)
				&& GeographicCamera->m_ViewFrustum.Intersects(m_BoundingBox))
			{
				if (m_NorthEastChild == nullptr || m_NorthWestChild == nullptr || m_SouthEastChild == nullptr || m_SouthWestChild == nullptr)
				{
					ComputeChildren(GeographicCamera);
				}

				if (m_NorthEastChild != nullptr)
				{
					m_NorthEastChild->Update(GeographicCamera);
				}

				if (m_NorthWestChild != nullptr)
				{
					m_NorthWestChild->Update(GeographicCamera);
				}

				if (m_SouthEastChild != nullptr)
				{
					m_SouthEastChild->Update(GeographicCamera);
				}

				if (m_SouthWestChild != nullptr)
				{
					m_SouthWestChild->Update(GeographicCamera);
				}
			}
			else
			{
				if (m_NorthWestChild != nullptr)
				{
					m_NorthWestChild->Dispose();
					m_NorthWestChild = nullptr;
				}

				if (m_NorthEastChild != nullptr)
				{
					m_NorthEastChild->Dispose();
					m_NorthEastChild = nullptr;
				}

				if (m_SouthEastChild != nullptr)
				{
					m_SouthEastChild->Dispose();
					m_SouthEastChild = nullptr;
				}

				if (m_SouthWestChild != nullptr)
				{
					m_SouthWestChild->Dispose();
					m_SouthWestChild = nullptr;
				}
			}
		}

		if (m_isInitialized)
		{
			if (GeographicCamera->m_ViewRange / 2 > (QuadTileSet->TileDrawDistance * TileSize * 1.5f)
				|| UMathEngine::SphericalDistance(m_CenterLatitude, m_CenterLongitude, GeographicCamera->m_Latitude, GeographicCamera->m_Longitude) > (QuadTileSet->TileDrawSpread * TileSize * 1.5))
			{
				if (m_Level != 0 || (m_Level == 0 && !QuadTileSet->AlwaysRenderBaseTiles))
				{
					Dispose();
				}
			}
		}
	}
	catch(...)
	{
	}
}

// Builds flat or terrain mesh for current tile
void UQuadTile::CreateTileMesh()
{
	UWorldWindSettings* TempSettings = GetMutableDefault<UWorldWindSettings>();
	m_VerticalExaggeration = TempSettings->VerticalExaggeration;
	// TODO01: 应该没有用到
	/*m_CurrentOpacity = QuadTileSet.Opacity;*/

	UQuadTileSet* QuadTileSet = GetTypedOuter<UQuadTileSet>();
	m_RenderStruts = QuadTileSet->m_RenderStruts;

	if (QuadTileSet->m_isTerrainMapped && FMath::Abs(m_VerticalExaggeration) > 1e-3)
	{
		CreateElevatedMesh();
	}
	else
	{
		CreateFlatMesh();
	}
}

// Builds a flat mesh (no terrain)
// 一次创建4个子 Tile
void UQuadTile::CreateFlatMesh()
{
    double LayerRadius = GetTypedOuter<UQuadTileSet>()->m_LayerRadius;
    double ScaleFactor = 1.0 / (double)VertexCount;
    int32 ThisVertexCount = VertexCount / 2 + (VertexCount % 2); // 没有太大意义
    int32 ThisVertexCountPlus1 = ThisVertexCount + 1;

    int32 TotalVertexCount = ThisVertexCountPlus1 * ThisVertexCountPlus1;
    m_NorthWestVertices.Init(FPositionNormalTextured(), TotalVertexCount);
    m_SouthWestVertices.Init(FPositionNormalTextured(), TotalVertexCount);
    m_NorthEastVertices.Init(FPositionNormalTextured(), TotalVertexCount);
    m_SouthEastVertices.Init(FPositionNormalTextured(), TotalVertexCount);

    // Cache western sin/cos of longitude values
    TArray<double> SinLon;
	SinLon.Init(0, ThisVertexCountPlus1);
    TArray<double> CosLon;
	CosLon.Init(0, ThisVertexCountPlus1);

    int32 BaseIndex;
    double Angle;
	// 每个格子的 Size
    double DeltaAngle = FMath::DegreesToRadians(ScaleFactor * m_LongitudeSpan);

    for (int32 i = 0; i < ThisVertexCountPlus1; i++)
    {
        Angle = FMath::DegreesToRadians(m_West) + i * DeltaAngle;
        SinLon[i] = FMath::Sin(Angle);
        CosLon[i] = FMath::Cos(Angle);
    }

	BaseIndex = 0;
	DeltaAngle = FMath::DegreesToRadians(-ScaleFactor * m_LatitudeSpan);
	for (int32 i = 0; i < ThisVertexCountPlus1; i++)
	{
		Angle = FMath::DegreesToRadians(m_North) + i * DeltaAngle;
		double SinLat = FMath::Sin(Angle);
		double RadCosLat = FMath::Cos(Angle) * LayerRadius;

		for (int32 j = 0; j < ThisVertexCountPlus1; j++)
		{
			m_NorthWestVertices[BaseIndex].Position = FVector(RadCosLat * SinLon[j], RadCosLat * CosLon[j], LayerRadius * SinLat);
			m_NorthWestVertices[BaseIndex].UV = FVector2D(j * ScaleFactor, i * ScaleFactor);
			m_NorthWestVertices[BaseIndex].Normal = m_NorthWestVertices[BaseIndex].Position;
			m_NorthWestVertices[BaseIndex].Normal.Normalize();

			BaseIndex += 1;
		}
    }

    BaseIndex = 0;
    for (int32 i = 0; i < ThisVertexCountPlus1; i++)
    {
        Angle = FMath::DegreesToRadians((m_North + m_South) / 2) + i * DeltaAngle;
        double SinLat = FMath::Sin(Angle);
        double RadCosLat = FMath::Cos(Angle) * LayerRadius;

        for (int32 j = 0; j < ThisVertexCountPlus1; j++)
        {
            m_SouthWestVertices[BaseIndex].Position = FVector(RadCosLat * SinLon[j], RadCosLat * CosLon[j], LayerRadius * SinLat);
        	m_SouthWestVertices[BaseIndex].UV = FVector2D(j * ScaleFactor, (i + ThisVertexCount) * ScaleFactor);
            m_SouthWestVertices[BaseIndex].Normal = m_SouthWestVertices[BaseIndex].Position;
            m_SouthWestVertices[BaseIndex].Normal.Normalize();

            BaseIndex += 1;
        }
    }

    // Cache eastern sin/cos of longitude values
    DeltaAngle = FMath::DegreesToRadians(ScaleFactor * m_LongitudeSpan);
    for (int32 i = 0; i < ThisVertexCountPlus1; i++)
    {
        Angle = FMath::DegreesToRadians((m_West + m_East) / 2) + i * DeltaAngle;
        SinLon[i] = FMath::Sin(Angle);
        CosLon[i] = FMath::Cos(Angle);
    }

    BaseIndex = 0;
    DeltaAngle = FMath::DegreesToRadians(-ScaleFactor * m_LatitudeSpan);
    for (int32 i = 0; i < ThisVertexCountPlus1; i++)
    {
        Angle = FMath::DegreesToRadians(m_North) + i * DeltaAngle;
        double SinLat = FMath::Sin(Angle);
        double RadCosLat = FMath::Cos(Angle) * LayerRadius;

        for (int32 j = 0; j < ThisVertexCountPlus1; j++)
        {
            m_NorthEastVertices[BaseIndex].Position = FVector(RadCosLat * SinLon[j], RadCosLat * CosLon[j], LayerRadius * SinLat);
        	m_NorthEastVertices[BaseIndex].UV = FVector2D((j + ThisVertexCount) * ScaleFactor, i * ScaleFactor);
            m_NorthEastVertices[BaseIndex].Normal = m_NorthEastVertices[BaseIndex].Position;
            m_NorthEastVertices[BaseIndex].Normal.Normalize();

            BaseIndex += 1;
        }
    }

    BaseIndex = 0;
    for (int32 i = 0; i < ThisVertexCountPlus1; i++)
    {
        Angle = FMath::DegreesToRadians((m_North + m_South) / 2) + i * DeltaAngle;
        double SinLat = FMath::Sin(Angle);
        double RadCosLat = FMath::Cos(Angle) * LayerRadius;

        for (int32 j = 0; j < ThisVertexCountPlus1; j++)
        {
            m_SouthEastVertices[BaseIndex].Position = FVector(RadCosLat * SinLon[j], RadCosLat * CosLon[j], LayerRadius * SinLat);
        	m_SouthEastVertices[BaseIndex].UV = FVector2D((j + ThisVertexCount) * ScaleFactor, (i + ThisVertexCount) * ScaleFactor);
            m_SouthEastVertices[BaseIndex].Normal = m_SouthEastVertices[BaseIndex].Position;
            m_SouthEastVertices[BaseIndex].Normal.Normalize();

            BaseIndex += 1;
        }
    }
	// 三角形顶点（4个子瓦片的三角形顶点是一样的）
	m_VertexIndexes.Init(0, 2 * ThisVertexCount * ThisVertexCount * 3);

	for (int32 i = 0; i < ThisVertexCount; i++)
	{
		BaseIndex = (2 * 3 * i * ThisVertexCount);
		for (int32 j = 0; j < ThisVertexCount; j++)
		{
			m_VertexIndexes[BaseIndex] = i*ThisVertexCountPlus1 + j;
			m_VertexIndexes[BaseIndex + 1] = (i + 1)*ThisVertexCountPlus1 + j;
			m_VertexIndexes[BaseIndex + 2] = (i + 1)*ThisVertexCountPlus1 + j + 1;

			m_VertexIndexes[BaseIndex + 3] = i*ThisVertexCountPlus1 + j;
			m_VertexIndexes[BaseIndex + 4] = (i + 1)*ThisVertexCountPlus1 + j + 1;
			m_VertexIndexes[BaseIndex + 5] = i*ThisVertexCountPlus1 + j + 1;
			BaseIndex += 6;
		}
	}
}

// Build the elevated terrain mesh
// 加了一圈围裙
void UQuadTile::CreateElevatedMesh()
{
	UQuadTileSet* QuadTileSet = GetTypedOuter<UQuadTileSet>();
    m_isDownloadingTerrain = true;
	// Get height data with one extra sample around the tile
	double DegreePerSample = m_LatitudeSpan / m_VertexCountElevated;
	// 未完待续，高程的
	FTerrainTile TerrainTile;
	if (m_Level >= 12)
	{
		TerrainTile = GetWorld()->GetSubsystem<UTerrainAccessorSubsystem>()->GetElevationArray(m_Row, m_Col, m_Level);
		
	}

	// 这里也要拿四张图
	
	/*TerrainTile tile = QuadTileSet.World.TerrainAccessor.GetElevationArray(North + degreePerSample, South - degreePerSample, West - degreePerSample, East + degreePerSample, vertexCountElevated + 3);
	float[,] heightData = tile.ElevationData;*/

	// 临时测试用
	TArray<TArray<float>> HeightData = TerrainTile.ElevationData;

	if (HeightData.IsEmpty())
	{
		for (int32 i = 0; i < 11; i++)
		{
			TArray<float> temp;
			temp.Init(0.f, 11);
			HeightData.Add(temp);
		}
	}

    int32 VertexCountElevatedPlus3 = m_VertexCountElevated / 2 + 3;
    int32 TotalVertexCount = FMath::Square(VertexCountElevatedPlus3);

	m_NorthWestVertices.Init(FPositionNormalTextured(), TotalVertexCount);
	m_SouthWestVertices.Init(FPositionNormalTextured(), TotalVertexCount);
	m_NorthEastVertices.Init(FPositionNormalTextured(), TotalVertexCount);
	m_SouthEastVertices.Init(FPositionNormalTextured(), TotalVertexCount);

    double LayerRadius = QuadTileSet->m_LayerRadius;

    // Calculate mesh base radius (bottom vertices)
    // Find minimum elevation to account for possible bathymetry
	float MinimumElevation = UE_MAX_FLT;
	float MaximumElevation = -UE_MAX_FLT;

	for (auto& Item : HeightData)
	{
		int32 IndexOfMinValue, IndexOfMaxValue;
		float MinValue, MaxValue;
		UKismetMathLibrary::MinOfFloatArray(Item, IndexOfMinValue, MinValue);
		UKismetMathLibrary::MaxOfFloatArray(Item, IndexOfMaxValue, MaxValue);

		if (MinValue < MinimumElevation)
			MinimumElevation = MinValue;
		if (MaxValue > MaximumElevation)
			MaximumElevation = MaxValue;
	}

    MinimumElevation *= m_VerticalExaggeration;
    MaximumElevation *= m_VerticalExaggeration;

    if (MinimumElevation > MaximumElevation)
    {
        // Compensate for negative vertical exaggeration
        MinimumElevation = MaximumElevation;
        MaximumElevation = MinimumElevation;
    }

    double Overlap = 500 * m_VerticalExaggeration; // 500m high tiles

    // Radius of mesh bottom grid
    m_MeshBaseRadius = LayerRadius + MinimumElevation - Overlap;

    CreateElevatedMesh(EChildLocation::NorthWest, m_NorthWestVertices, m_MeshBaseRadius, HeightData);
    CreateElevatedMesh(EChildLocation::SouthWest, m_SouthWestVertices, m_MeshBaseRadius, HeightData);
    CreateElevatedMesh(EChildLocation::NorthEast, m_NorthEastVertices, m_MeshBaseRadius, HeightData);
    CreateElevatedMesh(EChildLocation::SouthEast, m_SouthEastVertices, m_MeshBaseRadius, HeightData);

    m_BoundingBox = FBoundingBox(m_South, m_North, m_West, m_East, LayerRadius, LayerRadius + 10000 * m_VerticalExaggeration);

    QuadTileSet->IsDownloadingElevation = false;

    // Build common set of indexes for the 4 child meshes	
    int32 VertexCountElevatedPlus2 = m_VertexCountElevated / 2 + 2;
    m_VertexIndexes.Init(0, 2 * VertexCountElevatedPlus2 * VertexCountElevatedPlus2 * 3);

    int32 BaseIndex = 0;
    for (int32 i = 0; i < VertexCountElevatedPlus2; i++)
    {
        for (int32 j = 0; j < VertexCountElevatedPlus2; j++)
        {
        	m_VertexIndexes[BaseIndex] = i*VertexCountElevatedPlus3 + j;
        	m_VertexIndexes[BaseIndex + 1] = (i + 1)*VertexCountElevatedPlus3 + j;
        	m_VertexIndexes[BaseIndex + 2] = (i + 1)*VertexCountElevatedPlus3 + j + 1;

        	m_VertexIndexes[BaseIndex + 3] = i*VertexCountElevatedPlus3 + j;
        	m_VertexIndexes[BaseIndex + 4] = (i + 1)*VertexCountElevatedPlus3 + j + 1;
        	m_VertexIndexes[BaseIndex + 5] = i*VertexCountElevatedPlus3 + j + 1;
        	BaseIndex += 6;
        }
    }
	// 计算法线的
    CalculateNormals(m_NorthWestVertices, m_VertexIndexes);
    CalculateNormals(m_SouthWestVertices, m_VertexIndexes);
    CalculateNormals(m_NorthEastVertices, m_VertexIndexes);
    CalculateNormals(m_SouthEastVertices, m_VertexIndexes);

    m_isDownloadingTerrain = false;
}


// Create child tile terrain mesh
// Build the mesh with one extra vertice all around for proper normals calculations later on.
// Use the struts vertices to that effect. Struts are properly folded after normals calculations.
void UQuadTile::CreateElevatedMesh(EChildLocation Corner, TArray<FPositionNormalTextured>& Vertices, double MeshBaseRadius, TArray<TArray<float>>& HeightData)
{
    // Figure out child lat/lon boundaries (radians)
    double ChildTileNorth = FMath::DegreesToRadians(m_North);
    double ChildTileWest = FMath::DegreesToRadians(m_West);

    // Texture coordinate offsets
    float TuOffset = 0.f;
    float TvOffset = 0.f;
    switch (Corner)
    {
        case EChildLocation::NorthWest:
            // defaults are all good
            break;
        case EChildLocation::NorthEast:
            ChildTileWest = FMath::DegreesToRadians((m_West + m_East) / 2);
            TuOffset = 0.5f;
            break;
        case EChildLocation::SouthWest:
            ChildTileNorth = FMath::DegreesToRadians((m_North + m_South) / 2);
            TvOffset = 0.5f;
            break;
        case EChildLocation::SouthEast:
            ChildTileNorth = FMath::DegreesToRadians((m_North + m_South) / 2);
            ChildTileWest = FMath::DegreesToRadians((m_West + m_East) / 2);
            TuOffset = 0.5f;
            TvOffset = 0.5f;
            break;
    }

    double LatitudeRadianSpan = FMath::DegreesToRadians(m_LatitudeSpan);
    double LongitudeRadianSpan = FMath::DegreesToRadians(m_LongitudeSpan);

	double LayerRadius = (double)GetTypedOuter<UQuadTileSet>()->m_LayerRadius;
    double ScaleFactor = 1.0 / m_VertexCountElevated;
    int32 TerrainLongitudeIndex = (int)(TuOffset * m_VertexCountElevated) + 1;
    int32 TerrainLatitudeIndex = (int)(TvOffset * m_VertexCountElevated) + 1;

    int32 VertexCountElevatedPlus1 = m_VertexCountElevated / 2 + 1;

    int32 VertexIndex = 0;
    for (int32 LatitudeIndex = -1; LatitudeIndex <= VertexCountElevatedPlus1; LatitudeIndex++)
    {
        double LatitudeFactor = LatitudeIndex * ScaleFactor;
        double Latitude = ChildTileNorth - LatitudeFactor * LatitudeRadianSpan;

        // Cache trigonometric values
        double CosLat = FMath::Cos(Latitude);
        double SinLat = FMath::Sin(Latitude);

        for (int32 LongitudeIndex = -1; LongitudeIndex <= VertexCountElevatedPlus1; LongitudeIndex++)
        {
            // Top of mesh for all (real terrain + struts)
        	double RealRadius = LayerRadius +
        		HeightData[TerrainLatitudeIndex + LatitudeIndex][TerrainLongitudeIndex + LongitudeIndex] * m_VerticalExaggeration;

            double LongitudeFactor = LongitudeIndex * ScaleFactor;

            // Texture coordinates
            Vertices[VertexIndex].UV = FVector2D(TuOffset + LongitudeFactor, TvOffset + LatitudeFactor);

            // Convert from spherical (radians) to cartesian
            double Longitude = ChildTileWest + LongitudeFactor * LongitudeRadianSpan;
            double RadCosLat = RealRadius * CosLat;
            Vertices[VertexIndex].Position = FVector(RadCosLat * FMath::Sin(Longitude), RadCosLat * FMath::Cos(Longitude), RealRadius * SinLat);

            VertexIndex++;
        }
    }
}


// 计算顶点的平均法线，生成围裙
void UQuadTile::CalculateNormals(TArray<FPositionNormalTextured>& Vertices, TArray<int32>& Indices)
{
	TArray<TArray<FVector>> NormalBuffer;
	NormalBuffer.SetNum(Vertices.Num());

    for (int32 i = 0; i < Indices.Num(); i += 3)
    {
        FVector P1 = Vertices[Indices[i + 0]].Position;
        FVector P2 = Vertices[Indices[i + 1]].Position;
        FVector P3 = Vertices[Indices[i + 2]].Position;

        FVector V1 = P2 - P1;
        FVector V2 = P3 - P1;
        FVector Normal = V1.Cross(V2);

        Normal.Normalize();

        // Store the face's normal for each of the vertices that make up the face.
        NormalBuffer[Indices[i + 0]].Add(Normal);
        NormalBuffer[Indices[i + 1]].Add(Normal);
        NormalBuffer[Indices[i + 2]].Add(Normal);
    }

    // Now loop through each vertex vector, and avarage out all the normals stored.
    for (int32 i = 0; i < Vertices.Num(); ++i)
    {
        for (int32 j = 0; j < NormalBuffer[i].Num(); ++j)
        {
            FVector CurNormal = NormalBuffer[i][j];

            if (Vertices[i].Normal == FVector::ZeroVector)
                Vertices[i].Normal = CurNormal;
            else
                Vertices[i].Normal += CurNormal;
        }

        Vertices[i].Normal *= (1.0f / NormalBuffer[i].Num());
    }

    // Adjust/Fold struts vertices using terrain border vertices positions
    int32 VertexDensity = FMath::Sqrt(static_cast<float>(Vertices.Num()));
    for (int32 i = 0; i < VertexDensity; i++)
    {
        if (i == 0 || i == VertexDensity - 1)
        {
            for (int32 j = 0; j < VertexDensity; j++)
            {
            	// i 等于 0 时，计算的是第一行，i 等于 VertexDensity - 1 时，计算的是最后一行
                int Offset = (i == 0) ? VertexDensity : -VertexDensity;
                if (j == 0)
                {
	                Offset++;
                }
                if (j == VertexDensity - 1)
                {
	                Offset--;
                }
                FVector P = Vertices[i * VertexDensity + j + Offset].Position;
                if (m_RenderStruts && m_CurrentOpacity == 255)
                {
	                P = ProjectOnMeshBase(P);
                }
                Vertices[i * VertexDensity + j].Position = P;
            }
        }
        else
        {
        	// 瓦片西侧，不包含最上和最下
            FVector P = Vertices[i * VertexDensity + 1].Position;
            if (m_RenderStruts && m_CurrentOpacity == 255)
            {
	            P = ProjectOnMeshBase(P);
            }
            Vertices[i * VertexDensity].Position = P;
        	// 瓦片东侧，不包含最上和最下
            P = Vertices[i * VertexDensity + VertexDensity - 2].Position;
            if (m_RenderStruts && m_CurrentOpacity == 255)
            {
	            P = ProjectOnMeshBase(P);
            }
            Vertices[i * VertexDensity + VertexDensity - 1].Position = P;
        }
    }
}


// Project an elevated mesh point to the mesh base
FVector UQuadTile::ProjectOnMeshBase(FVector P)
{
    // p = p + localOrigin;
    P.Normalize();
    // p = p * MeshBaseRadius - localOrigin;
	P = P * GetTypedOuter<UQuadTileSet>()->m_LayerRadius;
    return P;
}
// End edits by Patrick Murris : fixing mesh sides normals (2006-11-18)


bool UQuadTile::Render(UGeographicCameraComponent* GeographicCamera)
{
    try
    {
    	// 此Tile没有初始化或四个方向的网格没有数据就返回false
        if (!m_isInitialized ||
            m_NorthWestVertices.IsEmpty() ||
            m_NorthEastVertices.IsEmpty() ||
            m_SouthEastVertices.IsEmpty() ||
            m_SouthWestVertices.IsEmpty())
            return false;

        if (!GeographicCamera->m_ViewFrustum.Intersects(m_BoundingBox))
            return false;

        bool NorthWestChildRendered = false;
        bool NorthEastChildRendered = false;
        bool SouthWestChildRendered = false;
        bool SouthEastChildRendered = false;

        if (m_NorthWestChild != nullptr)
        {
        	if (m_NorthWestChild->Render(GeographicCamera))
        		NorthWestChildRendered = true;
        }

        if (m_SouthWestChild != nullptr)
        {
        	if (m_SouthWestChild->Render(GeographicCamera))
        		SouthWestChildRendered = true;
        }

        if (m_NorthEastChild != nullptr)
        {
        	if (m_NorthEastChild->Render(GeographicCamera))
        		NorthEastChildRendered = true;
        }

        if (m_SouthEastChild != nullptr)
        {
        	if (m_SouthEastChild->Render(GeographicCamera))
        		SouthEastChildRendered = true;
        }

    	//这段不是太相关
    	/*if (QuadTileSet.RenderFileNames &&
			(!northWestChildRendered || !northEastChildRendered || !southWestChildRendered || !southEastChildRendered))
    	{
    		Vector3 referenceCenter = new Vector3(
				(float)drawArgs.WorldCamera.ReferenceCenter.X,
				(float)drawArgs.WorldCamera.ReferenceCenter.Y,
				(float)drawArgs.WorldCamera.ReferenceCenter.Z);

    		RenderDownloadRectangle(drawArgs, System.Drawing.Color.FromArgb(255, 0, 0).ToArgb(), referenceCenter);

    		Vector3 cartesianPoint = MathEngine.SphericalToCartesian(
				CenterLatitude.Degrees,
				CenterLongitude.Degrees,
				drawArgs.WorldCamera.WorldRadius + drawArgs.WorldCamera.TerrainElevation);

    		if (ImageFilePath != null && drawArgs.WorldCamera.ViewFrustum.ContainsPoint(cartesianPoint))
    		{
    			Vector3 projectedPoint = drawArgs.WorldCamera.Project(cartesianPoint - referenceCenter);

    			System.Drawing.Rectangle rect = new System.Drawing.Rectangle(
					(int)projectedPoint.X - 100,
					(int)projectedPoint.Y,
					200,
					200);

    			drawArgs.defaultDrawingFont.DrawText(
					null,
					ImageFilePath,
					rect,
					DrawTextFormat.WordBreak,
					System.Drawing.Color.Red);
    		}
    	}*/
		// 4个子瓦片已经渲染，就不再渲染父瓦片了，避免重复渲染
        if (NorthWestChildRendered && NorthEastChildRendered && SouthWestChildRendered && SouthEastChildRendered)
        {
            return true;
        }

    	// 不知道是干嘛的
        /*Device device = DrawArgs.Device;

        for (int i = 0; i < textures.Length; i++)
        {
            if (textures[i] == null || textures[i].Disposed)
                return false;

            device.SetTexture(i, textures[i]);
        }

        drawArgs.numberTilesDrawn++;

    	DrawArgs.Device.Transform.World = Matrix.Translation(
    		(float)(localOrigin.X - drawArgs.WorldCamera.ReferenceCenter.X),
    		(float)(localOrigin.Y - drawArgs.WorldCamera.ReferenceCenter.Y),
    		(float)(localOrigin.Z - drawArgs.WorldCamera.ReferenceCenter.Z)
    		);*/

		// 只有这一层了，没有下一层
        if (!NorthWestChildRendered)
            Render(m_NorthWestVertices, m_NorthWestChild, EChildLocation::NorthWest);
        if (!SouthWestChildRendered)
            Render(m_SouthWestVertices, m_SouthWestChild, EChildLocation::SouthWest);
        if (!NorthEastChildRendered)
            Render(m_NorthEastVertices, m_NorthEastChild, EChildLocation::NorthEast);
        if (!SouthEastChildRendered)
            Render(m_SouthEastVertices, m_SouthEastChild, EChildLocation::SouthEast);

    	// 在这里加文字

#if WITH_EDITOR
    	FVector Txt3DLoc = m_NorthWestVertices.Last().Position;
    	FString txt3d = FString::Printf(TEXT("L:%02d\nR:%04d, C:%04d"), m_Level, m_Row, m_Col);
    	UText3DComponent* Txt3D = NewObject<UText3DComponent>(this);

    	Txt3D->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
    	Txt3D->RegisterComponent();

    	Txt3D->SetRelativeLocation(Txt3DLoc);
    	Txt3D->SetRelativeRotation(FRotationMatrix::MakeFromX(-Txt3DLoc).Rotator());
    	Txt3D->SetRelativeScale3D(FVector(FMath::Pow(static_cast<float>(2), static_cast<float>(12)-m_Level)));

    	Txt3D->SetHorizontalAlignment(EText3DHorizontalTextAlignment::Center);
    	Txt3D->SetVerticalAlignment(EText3DVerticalTextAlignment::Center);
    	Txt3D->SetText(FText::FromString(txt3d));
#endif

    	/*DrawArgs.Device.Transform.World = DrawArgs.Camera.WorldMatrix;*/

        return true;
    }
    /*catch (DirectXException)*/
    catch (...)
    {
    }
	return false;
}


/*/// <summary>
/// Render a rectangle around an image tile in the specified color
/// </summary>
void UQuadTile::RenderDownloadRectangle(DrawArgs drawArgs, int color, Vector3 referenceCenter)
{
	// Render terrain download rectangle
	Vector3 northWestV = MathEngine.SphericalToCartesian((float)North, (float)West, QuadTileSet.LayerRadius) - referenceCenter;
	Vector3 southWestV = MathEngine.SphericalToCartesian((float)South, (float)West, QuadTileSet.LayerRadius) - referenceCenter;
	Vector3 northEastV = MathEngine.SphericalToCartesian((float)North, (float)East, QuadTileSet.LayerRadius) - referenceCenter;
	Vector3 southEastV = MathEngine.SphericalToCartesian((float)South, (float)East, QuadTileSet.LayerRadius) - referenceCenter;

	downloadRectangle[0].X = northWestV.X;
	downloadRectangle[0].Y = northWestV.Y;
	downloadRectangle[0].Z = northWestV.Z;
	downloadRectangle[0].Color = color;

	downloadRectangle[1].X = southWestV.X;
	downloadRectangle[1].Y = southWestV.Y;
	downloadRectangle[1].Z = southWestV.Z;
	downloadRectangle[1].Color = color;

	downloadRectangle[2].X = southEastV.X;
	downloadRectangle[2].Y = southEastV.Y;
	downloadRectangle[2].Z = southEastV.Z;
	downloadRectangle[2].Color = color;

	downloadRectangle[3].X = northEastV.X;
	downloadRectangle[3].Y = northEastV.Y;
	downloadRectangle[3].Z = northEastV.Z;
	downloadRectangle[3].Color = color;

	downloadRectangle[4].X = downloadRectangle[0].X;
	downloadRectangle[4].Y = downloadRectangle[0].Y;
	downloadRectangle[4].Z = downloadRectangle[0].Z;
	downloadRectangle[4].Color = color;

	drawArgs.device.RenderState.ZBufferEnable = false;
	drawArgs.device.VertexFormat = CustomVertex.PositionColored.Format;
	drawArgs.device.TextureState[0].ColorOperation = TextureOperation.Disable;
	drawArgs.device.DrawUserPrimitives(PrimitiveType.LineStrip, 4, downloadRectangle);
	drawArgs.device.TextureState[0].ColorOperation = TextureOperation.SelectArg1;
	drawArgs.device.VertexFormat = CustomVertex.PositionNormalTextured.Format;
	drawArgs.device.RenderState.ZBufferEnable = true;
}*/


/// <summary>
/// Render one of the 4 quadrants with optional download indicator
/// </summary>
// TODO01：完全没有必要分成四块。。。
void UQuadTile::Render(/*Device device, */TArray<FPositionNormalTextured> Verts, UQuadTile* child, EChildLocation ChildLocation)
{
	bool isMultitexturing = false;

	// 太阳的
	/*if(!World.Settings.EnableSunShading)
	{
		if (World.Settings.ShowDownloadIndicator && child != null)
		{
			// Check/display download activity
			GeoSpatialDownloadRequest request = child.DownloadRequest;
			if (child.isDownloadingTerrain)
			{
				device.SetTexture(1, QuadTileSet.DownloadTerrainTexture);
				isMultitexturing = true;
			}
			//else if (request != null)
			else if(child.WaitingForDownload)
			{
				if (child.IsDownloadingImage)
					device.SetTexture(1, QuadTileSet.DownloadInProgressTexture);
				else
					device.SetTexture(1, QuadTileSet.DownloadQueuedTexture);
				isMultitexturing = true;
			}
		}
	}*/

	// 不知道是干嘛的
	/*if (isMultitexturing)
		device.SetTextureStageState(1, TextureStageStates.ColorOperation, (int)TextureOperation.BlendTextureAlpha);*/

	/*if(!verts.IsEmpty() && !vertexIndexes.IsEmpty())
	{
        if (QuadTileSet.Effect != null)
        {
            Effect effect = QuadTileSet.Effect;

            // FIXME: just use the first technique for now
            effect.Technique = effect.GetTechnique(0);
            effect.SetValue("WorldViewProj", Matrix.Multiply(device.Transform.World, Matrix.Multiply(device.Transform.View, device.Transform.Projection)));
            try
            {
                effect.SetValue("Tex0", textures[0]);
                effect.SetValue("Tex1", textures[1]);
                effect.SetValue("Brightness", QuadTileSet.GrayscaleBrightness);
                float opacity = (float)QuadTileSet.Opacity / 255.0f;
                effect.SetValue("Opacity", opacity);
            }
            catch (...)
            {
            }

            int numPasses = effect.Begin(0);
            for (int i = 0; i < numPasses; i++)
            {
                effect.BeginPass(i);
                device.DrawIndexedUserPrimitives(PrimitiveType.TriangleList, 0,
                    verts.Length, vertexIndexes.Length / 3, vertexIndexes, true, verts);

                effect.EndPass();
            }

            effect.End();
        }
        else if (!QuadTileSet.RenderGrayscale || (device.DeviceCaps.PixelShaderVersion.Major < 1))
        {
            if (World.Settings.EnableSunShading)
            {
                Point3d sunPosition = SunCalculator.GetGeocentricPosition(TimeKeeper.CurrentTimeUtc);
                Vector3 sunVector = new Vector3(
                    (float)sunPosition.X,
                    (float)sunPosition.Y,
                    (float)sunPosition.Z);

                device.RenderState.Lighting = true;
                Material material = new Material();
                material.Diffuse = System.Drawing.Color.White;
                material.Ambient = System.Drawing.Color.White;

                device.Material = material;
                device.RenderState.AmbientColor = World.Settings.ShadingAmbientColor.ToArgb();
                device.RenderState.NormalizeNormals = true;
                device.RenderState.AlphaBlendEnable = true;

                device.Lights[0].Enabled = true;
                device.Lights[0].Type = LightType.Directional;
                device.Lights[0].Diffuse = System.Drawing.Color.White;
                device.Lights[0].Direction = sunVector;

                device.TextureState[0].ColorOperation = TextureOperation.Modulate;
                device.TextureState[0].ColorArgument1 = TextureArgument.Diffuse;
                device.TextureState[0].ColorArgument2 = TextureArgument.TextureColor;
            }
            else
            {
                device.RenderState.Lighting = false;
                device.RenderState.Ambient = World.Settings.StandardAmbientColor;
            }

            device.RenderState.TextureFactor = System.Drawing.Color.FromArgb(m_CurrentOpacity, 0, 0, 0).ToArgb();
            device.TextureState[0].AlphaOperation = TextureOperation.BlendFactorAlpha;
            device.TextureState[0].AlphaArgument1 = TextureArgument.TextureColor;
            device.TextureState[0].AlphaArgument2 = TextureArgument.TFactor;

            device.DrawIndexedUserPrimitives(PrimitiveType.TriangleList, 0,
                verts.Length, vertexIndexes.Length / 3, vertexIndexes, true, verts);
        }
        else
        {
            if (grayscaleEffect == null)
            {
                device.DeviceReset += new EventHandler(device_DeviceReset);
                device_DeviceReset(device, null);
            }

            grayscaleEffect.Technique = "RenderGrayscaleBrightness";
            grayscaleEffect.SetValue("WorldViewProj", Matrix.Multiply(device.Transform.World, Matrix.Multiply(device.Transform.View, device.Transform.Projection)));
            grayscaleEffect.SetValue("Tex0", textures[0]);
            grayscaleEffect.SetValue("Brightness", QuadTileSet.GrayscaleBrightness);
            float opacity = (float)QuadTileSet.Opacity / 255.0f;
            grayscaleEffect.SetValue("Opacity", opacity);

            int numPasses = grayscaleEffect.Begin(0);
            for (int i = 0; i < numPasses; i++)
            {
                grayscaleEffect.BeginPass(i);
                device.DrawIndexedUserPrimitives(PrimitiveType.TriangleList, 0,
                    verts.Length, vertexIndexes.Length / 3, vertexIndexes, true, verts);

                grayscaleEffect.EndPass();
            }

            grayscaleEffect.End();
        }
	}*/
	
	/*if (Level != 4)
		return;*/

	FString TileName = FString::Printf(TEXT("L:%02d\nR:%04d, C:%04d"), m_Level, m_Row, m_Col);

	//FImageUtils::ImportFileAsTexture2D(UncheckedIconPath));
	
	/*
	FString ImgUrl;
	FString Key;

	UWorldWindSettings* TempSettings = GetMutableDefault<UWorldWindSettings>();
	UGeoSpatialDownloadRequest* DownloadTask = NewObject<UGeoSpatialDownloadRequest>();
	DownloadTask->m_QuadTile = this;
	UDownloadQueue::GetSingleton()->Add(DownloadTask);
	DownloadTask->m_Level = m_Level;
	DownloadTask->m_Row = m_Row;
	DownloadTask->m_Col = m_Col;

	ImgUrl = FString::Printf(TEXT("http://t0.tianditu.gov.cn/img_c/wmts?SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=img&STYLE=default&TILEMATRIXSET=c&FORMAT=tiles&TILEMATRIX=%d&TILEROW=%d&TILECOL=%d&tk=%s"),
	m_Level, m_Row, m_Col, *TempSettings->Token);

	//URL = URL01;
	DownloadTask->m_QuadTile = this;*/

	//GetOwner()

	// 测试用的
	TArray<FVector> Vertices;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;

	for (auto& Item : Verts)
	{
		Vertices.Add(Item.Position);
		UV0.Add(Item.UV);
		Normals.Add(Item.Normal);
	}

	TArray<FColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;

	const UQuadTileSet* cc = GetTypedOuter<UQuadTileSet>();

	UMaterialInstanceDynamic* VideoMID = UMaterialInstanceDynamic::Create(cc->TileDefaultMaterial, nullptr);
	
	
	if (m_Textures.Num() > 0)
	{
		VideoMID->SetTextureParameterValue(TEXT("Texture"), (m_Textures[0]));
	}
	FString txt3d;
	
	switch(ChildLocation)
	{
		// 西北
		case EChildLocation::NorthWest:
			CreateMeshSection(0, Vertices, m_VertexIndexes, Normals, UV0, VertexColors, Tangents, true);
			txt3d = FString::Printf(TEXT("L:%02d\nR:%04d, C:%04d"), m_Level + 1, m_Row*2, m_Col*2);
			SetMaterial(0, VideoMID);
			break;
		// 东北
		case EChildLocation::NorthEast:
			CreateMeshSection(1, Vertices, m_VertexIndexes, Normals, UV0, VertexColors, Tangents, true);
			txt3d = FString::Printf(TEXT("L:%02d\nR:%04d, C:%04d"), m_Level + 1, m_Row*2, m_Col*2 + 1);
			SetMaterial(1, VideoMID);
			break;
		// 西南
		case EChildLocation::SouthWest:
			CreateMeshSection(2, Vertices, m_VertexIndexes, Normals, UV0, VertexColors, Tangents, true);
			txt3d = FString::Printf(TEXT("L:%02d\nR:%04d, C:%04d"), m_Level + 1, m_Row*2 + 1, m_Col*2);
			SetMaterial(2, VideoMID);
			break;
		// 东南
		case EChildLocation::SouthEast:
			CreateMeshSection(3, Vertices, m_VertexIndexes, Normals, UV0, VertexColors, Tangents, true);
		txt3d = FString::Printf(TEXT("L:%02d\nR:%04d, C:%04d"), m_Level + 1, m_Row*2 + 1, m_Col*2 + 1);
			SetMaterial(3, VideoMID);
			break;
	}


}

// 现在是有问题的
void UQuadTile::UpdateTileTexture(UTexture2DDynamic* Texture, UQuadTile* hh)
{
	UE_LOG(LogClass, Log, TEXT("Quadtile的纹理更新了！"))
	// 
	/*const UQuadTileSet* cc = GetTypedOuter<UQuadTileSet>();*/

	m_Textures.Add(Texture);
	//CreateTileMesh();

}


/*void UQuadTile::device_DeviceReset(object sender, EventArgs e)
{
	Device device = (Device)sender;

	string outerrors = "";

	try
	{
		System.Reflection.Assembly assembly = System.Reflection.Assembly.GetExecutingAssembly();
		Stream stream = assembly.GetManifestResourceStream("WorldWind.Shaders.grayscale.fx");
                
		grayscaleEffect =
			Effect.FromStream(
			device,
			stream,
			null,
			null,
			ShaderFlags.None,
			null,
			out outerrors);

		if (outerrors != null && outerrors.Length > 0)
			Log.Write(Log.Levels.Error, outerrors);
	}
	catch (Exception ex)
	{
		Log.Write(ex);
	}
}*/




