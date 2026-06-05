// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "QuadTileSet.h"
#include "GeographicCameraComponent.h"
#include "WebDownload/DownloadRequest.h"
#include "Image/ImageStore.h"
#include "QuadTile.h"
#include "Kismet/GameplayStatics.h"
#include "WebDownload/DownloadQueue.h"


// Sets default values for this component's properties
UQuadTileSet::UQuadTileSet()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;

    // ...
}


// Called when the game starts
void UQuadTileSet::BeginPlay()
{
    Super::BeginPlay();

    TArray<UActorComponent*> Components;
    GetOwner()->GetComponents(UStaticMeshComponent::StaticClass(), Components);
    //GetOwner()->GetComponents(USpringArmComponent::StaticClass())

    UDownloadQueue* DownloadTask = NewObject<UDownloadQueue>(this);
    DownloadTask->ddd();

    APawn * aaa = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    
    UGeographicCameraComponent* sss  = Cast<UGeographicCameraComponent>(aaa->GetComponentByClass(UGeographicCameraComponent::StaticClass()));

    // ...
	
}


// Called every frame
void UQuadTileSet::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    

    /*for (auto& Elem :MoveComp)
    {
        Cast<UStaticMeshComponent>(Elem.Value)->AddRelativeLocation(FVector(20.f, 20.f, 0));
    }*/


    // ...
}


/*/// <summary>
/// Initializes a new instance of the <see cref= "T:WorldWind.Renderable.QuadTileSet"/> class.
/// </summary>
/// <param name="name"></param>
/// <param name="parentWorld"></param>
/// <param name="distanceAboveSurface"></param>
/// <param name="north"></param>
/// <param name="south"></param>
/// <param name="west"></param>
/// <param name="east"></param>
/// <param name="terrainAccessor"></param>
/// <param name="imageAccessor"></param>
UQuadTileSet::UQuadTileSet(
		FString name,
		World parentWorld,
		double distanceAboveSurface,
		double north,
		double south,
		double west,
		double east,
		bool terrainMapped,
								ImageStore[] imageStores)
	: base(name, parentWorld)
{
	float layerRadius = (float)(parentWorld.EquatorialRadius + distanceAboveSurface);
	m_north = north;
	m_south = south;
	m_west = west;
	m_east = east;

	// Layer center position
	Position = MathEngine.SphericalToCartesian(
			(north + south) * 0.5f,
			(west + east) * 0.5f,
			layerRadius);

	m_layerRadius = layerRadius;
	m_tileDrawDistance = 3.5f;
	m_tileDrawSpread = 2.9f;
	m_imageStores = imageStores;
	m_terrainMapped = terrainMapped;

	// Default terrain mapped imagery to terrain mapped priority
	if (terrainMapped)
		m_renderPriority = RenderPriority.TerrainMappedImages;
}*/


void UQuadTileSet::Initialize(UGeographicCameraComponent* GeographicCamera)
{
    
    TileDefaultMaterial = LoadObject<UMaterial>(nullptr, TEXT("/Script/Engine.Material'/WorldWind/Materials/M_TileDefaultMaterial.M_TileDefaultMaterial'"));
    check(TileDefaultMaterial);
    /*if (Material != nullptr)
    {
        PreviewMaterial = UMaterialInstanceDynamic::Create(Material, GetToolManager());
    }*/

    
    // 哪有这么多相机啊？？？
    //Camera = FDrawArgs::Camera;

    /*// Initialize download rectangles
    if (DownloadInProgressTexture == null)
        DownloadInProgressTexture = CreateDownloadRectangle(
                DrawArgs.Device, World.Settings.DownloadProgressColor, 0);
    if (DownloadQueuedTexture == null)
        DownloadQueuedTexture = CreateDownloadRectangle(
                DrawArgs.Device, World.Settings.DownloadQueuedColor, 0);
    if (DownloadTerrainTexture == null)
        DownloadTerrainTexture = CreateDownloadRectangle(
                DrawArgs.Device, World.Settings.DownloadTerrainRectangleColor, 0);*/

    try
    {

        for (auto Elem : TopmostTiles)
        {
            Elem.Value->Initialize();
     
        }
        // 要锁住？？？
        /*lock (m_topmostTiles.SyncRoot)
        {
            foreach (QuadTile qt in m_topmostTiles.Values)
                qt.Initialize();
        }*/
    }
    catch(...)
    {
    }
    IsInitialized = true;

    

    /*if (MetaData.ContainsKey("EffectPath"))
    {
        m_effectPath = MetaData["EffectPath"] as string;
    }
    else
    {
        m_effectPath = null;
    }
    m_effect = null;*/
}



bool UQuadTileSet::PerformSelectionAction(UGeographicCameraComponent* GeographicCamera)
{
    return false;
}

void UQuadTileSet::Update(UGeographicCameraComponent* GeographicCamera)
{
    if (!IsInitialized)
    {
        Initialize(GeographicCamera);
    }
        

    double Levle4TileSize = 22.5f;
    
    // 特效的，先不管
    /*if (m_effectPath != null && m_effect == null)
    {
        string errs = string.Empty;
        m_effect = Effect.FromFile(DrawArgs.Device, m_effectPath, null, "", ShaderFlags.None, m_effectPool, out errs);

        if (errs != null && errs != string.Empty)
        {
            Log.Write(Log.Levels.Warning, "Could not load effect " + m_effectPath + ": " + errs);
            Log.Write(Log.Levels.Warning, "Effect has been disabled.");
            m_effectPath = null;
            m_effect = null;
        }
    }*/

    //if (ImageStores[0].LevelZeroTileSizeDegrees < 180)
    if (true)
    {
        // Check for layer outside view
        double vrd = GeographicCamera->m_ViewRange;
        double LatitudeMax = GeographicCamera->m_Latitude + vrd;
        double LatitudeMin = GeographicCamera->m_Latitude - vrd;
        double LongitudeMax = GeographicCamera->m_Longitude + vrd;
        double LongitudeMin = GeographicCamera->m_Longitude - vrd;
        if (LatitudeMax < m_South || LatitudeMin > m_North || LongitudeMax < m_West || LongitudeMin > m_East)
        {
            return;
        }
    }

    // 这个暂时不知道。。。
    /*if (DrawArgs.Camera->ViewRange * 0.5f > TileDrawDistance * Levle4TileSize)
    {
        // 多线程
        //lock (m_topmostTiles.SyncRoot)
        {
            for (auto Elem : m_topmostTiles)
                Elem.Value->Dispose();
            m_topmostTiles.Empty();
            // 清除下载队列，暂时用不到
            // ClearDownloadRequests();
        }

        return;
    }*/

    RemoveInvisibleTiles(GeographicCamera);
    try
    {
        // 1级的时候，整幅地图被分为左右两片，TileSize 为 360/2^1
        // 4级 TileSize 为 360/2^4，即为22.5
        // 从4级开始进行四叉树计算，更高级的 Tile 不做，直接用一张高分辩的图
        // 4级 Tile 的行和列
        int32 MiddleRow = UMathEngine::GetRowFromLatitude(GeographicCamera->m_Latitude, Levle4TileSize);
        int32 MiddleCol = UMathEngine::GetColFromLongitude(GeographicCamera->m_Longitude, Levle4TileSize);
        // 4级中心 Tile 的边界
        double MiddleSouth = 90.0f - (MiddleRow + 1) * Levle4TileSize;
        double MiddleNorth = 90.0f - MiddleRow * Levle4TileSize;
        double MiddleWest = -180.0f + MiddleCol * Levle4TileSize;
        double MiddleEast = -180.0f + (MiddleCol + 1) * Levle4TileSize;

        double MiddleCenterLat = 0.5f * (MiddleNorth + MiddleSouth);
        double MiddleCenterLon = 0.5f * (MiddleWest + MiddleEast);

        int32 TileSpread = 4;
        
        // 中心周围9*9范围的不在缓存中但是确实当前窗口需要的瓦片影像数据
        for (int32 i = 0; i < TileSpread; i++)  // 这层循环初看起来没有必要，其实是为了先加载中心，再加载边缘瓦片
        {
            for (int32 j = -i; j <= i; j++)
            {
                for (int32 k = -i; k <= i; k++)
                {
                    int32 CurRow = UMathEngine::GetRowFromLatitude(MiddleCenterLat + j * Levle4TileSize, Levle4TileSize);
                    int32 CurCol = UMathEngine::GetColFromLongitude(MiddleCenterLon + k * Levle4TileSize, Levle4TileSize);

                    FString Key = FString::Printf(TEXT("Row=%d,Col=%d"), CurRow, CurCol);

                    //UQuadTile* qt = m_topmostTiles.Find(key);
                    if (TopmostTiles.Contains(Key))
                    {
                        TopmostTiles[Key]->Update(GeographicCamera);
                        continue;
                    }

                    // Check for tile outside layer boundaries
                    double CurTileWest = -180.0f + CurCol * Levle4TileSize;
                    if (CurTileWest > m_East)
                        continue;

                    double CurTileEast = -180.0f + (CurCol + 1) * Levle4TileSize;
                    if (CurTileEast < m_West)
                        continue;

                    double CurTileSouth = 90.0f - (CurRow + 1) * Levle4TileSize;
                    if (CurTileSouth > m_North)
                        continue;

                    double CurTileNorth = 90.0f - CurRow * Levle4TileSize;
                    if (CurTileNorth < m_South)
                        continue;

                    UQuadTile* QT = NewObject<UQuadTile>(this);

                    QT->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
                    QT->RegisterComponent();

                    // Object 没有带参的构造函数，需调用 UpdateTileInfo 函数
                    QT->UpdateTileInfo(CurTileSouth, CurTileNorth, CurTileWest, CurTileEast, 4/*, this*/);

                    if (GeographicCamera->m_ViewFrustum.Intersects(QT->m_BoundingBox))
                    {
                        /*lock (m_topmostTiles.SyncRoot)
                            m_topmostTiles.Add(key, qt);*/
                        UE_LOG(LogClass, Log, TEXT("QuadTileSet 计算 相交：%d,行,%d列"), CurRow, CurCol);
                        TopmostTiles.Add(Key, QT);
                        QT->Update(GeographicCamera);
                    }
                }
            }
        }
    }
    catch(...)
    {}
    /*catch (System.Threading.ThreadAbortException)
    {
    }
    catch (Exception caught)
    {
        Log.Write(caught);
    }*/
}


void UQuadTileSet::RemoveInvisibleTiles(UGeographicCameraComponent* GeographicCamera)
{
    TArray<FString> deletionList;
    // 要加锁
    /*lock (m_topmostTiles.SyncRoot)
    {*/
        for (auto Elem : TopmostTiles)
        {
            UQuadTile* qt = TopmostTiles[Elem.Key];
            if (!GeographicCamera->m_ViewFrustum.Intersects(qt->m_BoundingBox))
                deletionList.Add(Elem.Key);
        }

        for (FString deleteThis : deletionList)
        {
            UQuadTile* qt = TopmostTiles[deleteThis];
            if (qt != nullptr)
            {
                TopmostTiles.Remove(deleteThis);
                qt->Dispose();
            }
        }
    /*}*/
}

void UQuadTileSet::Render(UGeographicCameraComponent* GeographicCamera)
{
    try
    {
        /*// 不知道
        lock (m_topmostTiles.SyncRoot)
        {
            if (m_topmostTiles.Num() <= 0)
            {
                return;
            }

            Device device = DrawArgs.Device;

            // Temporary fix: Clear Z buffer between rendering
            // terrain mapped layers to avoid Z buffer fighting
            //if (lastRenderTime == DrawArgs.CurrentFrameStartTicks)
            device.Clear(ClearFlags.ZBuffer, 0, 1.0f, 0);
            device.RenderState.ZBufferEnable = true;
            lastRenderTime = DrawArgs.CurrentFrameStartTicks;*/

            //                              if (m_renderPriority < RenderPriority.TerrainMappedImages)
            //                                      // No Z buffering needed for "flat" layers
            //                                      device.RenderState.ZBufferEnable = false;


            /*      if (m_opacity < 255 && device.DeviceCaps.DestinationBlendCaps.SupportsBlendFactor)
                    {
                            // Blend
                            device.RenderState.AlphaBlendEnable = true;
                            device.RenderState.SourceBlend = m_sourceBlend;
                            device.RenderState.DestinationBlend = m_destinationBlend;
                            // Set Red, Green and Blue = opacity
                            device.RenderState.BlendFactorColor = (m_opacity << 16) | (m_opacity << 8) | m_opacity;
                    }
                    else if (EnableColorKeying && device.DeviceCaps.TextureCaps.SupportsAlpha)
                    {
                            device.RenderState.AlphaBlendEnable = true;
                            device.RenderState.SourceBlend = Blend.SourceAlpha;
                            device.RenderState.DestinationBlend = Blend.InvSourceAlpha;
                    }
*/
            /*// 太阳光照的，还没有到这一步
            if (!World.Settings.EnableSunShading)
            {
                // Set the render states for rendering of quad tiles.
                // Any quad tile rendering code that adjusts the state should restore it to below values afterwards.
                device.VertexFormat = CustomVertex.PositionNormalTextured.Format;
                device.SetTextureStageState(0, TextureStageStates.ColorOperation, (int)TextureOperation.SelectArg1);
                device.SetTextureStageState(0, TextureStageStates.ColorArgument1, (int)TextureArgument.TextureColor);
                device.SetTextureStageState(0, TextureStageStates.AlphaArgument1, (int)TextureArgument.TextureColor);
                device.SetTextureStageState(0, TextureStageStates.AlphaOperation, (int)TextureOperation.SelectArg1);

                // Be prepared for multi-texturing
                device.SetTextureStageState(1, TextureStageStates.ColorArgument2, (int)TextureArgument.Current);
                device.SetTextureStageState(1, TextureStageStates.ColorArgument1, (int)TextureArgument.TextureColor);
                device.SetTextureStageState(1, TextureStageStates.TextureCoordinateIndex, 0);
            }
            device.VertexFormat = CustomVertex.PositionNormalTextured.Format;*/

            for (auto Elem : TopmostTiles)
            {
                UQuadTile* qt = TopmostTiles[Elem.Key];
                qt->Render(GeographicCamera);
            }

            /*// 这片也不知道。。。
            // Restore device states
            device.SetTextureStageState(1, TextureStageStates.TextureCoordinateIndex, 1);

            if (m_renderPriority < RenderPriority.TerrainMappedImages)
                device.RenderState.ZBufferEnable = true;*/
            /*
                                                   if (m_opacity < 255 || EnableColorKeying)
                                                   {
                                                           // Restore alpha blend state
                                                           device.RenderState.SourceBlend = Blend.SourceAlpha;
                                                           device.RenderState.DestinationBlend = Blend.InvSourceAlpha;
                                                   }*/
        
    }
    catch (...)
    {
    }
    /*// 下载的？？？
    {
        if (IsConnectionWaiting)
        {
            if (DateTime.Now.Subtract(TimeSpan.FromSeconds(15)) < ConnectionWaitStart)
            {
                string s = "Problem connecting to server... Trying again in 2 minutes.\n";
                drawArgs.UpperLeftCornerText += s;
            }
        }

        int i = 0;
        foreach (GeoSpatialDownloadRequest request in m_activeDownloads)
        {
            if (request != null && !request.IsComplete && i < 10)
            {
                RenderDownloadProgress(drawArgs, request, i++);
                // Only render the first
                //break;
            }
        }
    }*/
}




void UQuadTileSet::AddToDownloadQueue(UGeoSpatialDownloadRequest* newRequest)
{
    UQuadTile* key = Cast<UQuadTile>(newRequest->GetOuter());
    key->m_WaitingForDownload = true;
    /*lock (m_downloadRequests.SyncRoot)*/
    {
        if (m_DownloadRequests.Contains(newRequest))
        {
            return;
        }

        m_DownloadRequests.Add(newRequest);

        if (m_DownloadRequests.Num() >= m_maxQueueSize)
        {
            //remove spatially farthest request
            UGeoSpatialDownloadRequest* farthestRequest = nullptr;
            double curDistance = 0;
            double farthestDistance = 0;
            for (auto& Elem :m_DownloadRequests)
            {
                UQuadTile* key01 = Cast<UQuadTile>(Elem->GetOuter());
                curDistance = UMathEngine::SphericalDistance(
                                key01->m_CenterLatitude,
                                key01->m_CenterLongitude,
                                Camera->m_Latitude,
                                Camera->m_Longitude);

                if (curDistance > farthestDistance)
                {
                    farthestRequest = Elem;
                    farthestDistance = curDistance;
                }
            }

            //farthestRequest->Dispose();
            /*farthestRequest.QuadTile.DownloadRequest = null;*/
            m_DownloadRequests.Remove(farthestRequest);
        }
        /*}*/

        /*ServiceDownloadQueue();*/
    }
}

/// <summary>
/// Removes a request from the download queue.
/// </summary>
void RemoveFromDownloadQueue(UGeoSpatialDownloadRequest* removeRequest)
{
    /*lock (m_downloadRequests.SyncRoot)
    {*/
        /*UQuadTile* key = removeRequest.QuadTile;
        GeoSpatialDownloadRequest request = (GeoSpatialDownloadRequest)m_downloadRequests[key];
        if (request != null)
        {
            m_downloadRequests.Remove(key);
            request.QuadTile.DownloadRequest = null;
        }*/
    /*}*/
}

/// <summary>
/// Starts downloads when there are threads available
/// </summary>
void UQuadTileSet::ServiceDownloadQueue()
{
    /*UE_LOG(LogClass, Log, TEXT("QTS", "ServiceDownloadQueue: " + m_DownloadRequests.Num() + " requests waiting");*/
    /*lock (m_downloadRequests.SyncRoot)
    {*/
        for (int i = 0; i </* World.Settings.MaxSimultaneousDownloads*/20; i++)
        {
            if (m_activeDownloads[i] == nullptr)
                continue;

            /*if (!m_activeDownloads[i].IsComplete)
                continue;

            m_activeDownloads[i].Cancel();
            m_activeDownloads[i].Dispose();
            m_activeDownloads[i] = null;*/
        }

        if (NumberRetries >= 5 || m_isConnectionWaiting)
        {
            // Anti hammer in effect
            if (!m_isConnectionWaiting)
            {
                m_connectionWaitStart = FDateTime::Now();
                m_isConnectionWaiting = true;
            }

            if (FDateTime::Now() - m_connectionWaitTime > m_connectionWaitStart)
            {
                NumberRetries = 0;
                m_isConnectionWaiting = false;
            }
            return;
        }

        // Queue new downloads
        for (int i = 0; i < /*World.Settings.MaxSimultaneousDownloads*/20; i++)
        {
            if (m_activeDownloads[i] != nullptr)
                continue;

            if (m_DownloadRequests.Num() <= 0)
                continue;

            m_activeDownloads[i] = GetClosestDownloadRequest();
            if (m_activeDownloads[i] != nullptr)
            {
                m_downloadStarted[i] = FDateTime::Now();
                m_activeDownloads[i]->StartDownload();
            }
        }
    /*}*/
}

/// <summary>
/// Finds the "best" tile from queue
/// </summary>
UGeoSpatialDownloadRequest* UQuadTileSet::GetClosestDownloadRequest()
{
    UGeoSpatialDownloadRequest* closestRequest = nullptr;
    float largestArea = -UE_MAX_FLT;

    for (UGeoSpatialDownloadRequest* curRequest : m_DownloadRequests)
    {
        if (curRequest->IsDownloading)  // 下载中，跳过
            continue;

        UQuadTile* qt = Cast<UQuadTile>(curRequest->GetOuter());
        if (!Camera->m_ViewFrustum.Intersects(qt->m_BoundingBox)) // 不在相机视角中，跳过
            continue;

        float screenArea = qt->m_BoundingBox.CalcRelativeScreenArea(Camera);
        if (screenArea > largestArea)
        {
            largestArea = screenArea;
            closestRequest = curRequest;
        }
    }

    return closestRequest;
}
    