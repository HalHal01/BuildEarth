// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

//#include "QuadTile.h"
//#include "ImageStore.h"

#include "Image/ImageStore.h"

#include "UObject/NoExportTypes.h"
#include "QuadTileSet.generated.h"

class UQuadTile;
class UGeographicCameraComponent;
class UGeoSpatialDownloadRequest;
//class FImageStore;
/**
 * 
 */
//UCLASS()

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )  // 未完待续
class WORLDWIND_API UQuadTileSet : public USceneComponent
{
	GENERATED_BODY()

public:	
    // Sets default values for this component's properties
    UQuadTileSet();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

public:	
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    
public:


    // 我加的
    UMaterial* TileDefaultMaterial;

    /// True when object is ready to be rendered.
    bool IsInitialized;
    

    bool m_RenderStruts = true;
    FString m_ServerLogoFilePath;
    //protected Image m_ServerLogoImage;

    //FHashTable m_topmostTiles = FHashTable();
    TMap<FString, UQuadTile*> TopmostTiles;
    double m_North = 90.0f;  // North bound for this QuadTileSet
    double m_South = -90.0f;  // South bound for this QuadTileSet
    double m_West = -180.0f;  // West bound for this QuadTileSet
    double m_East = 180.0f;  // East bound for this QuadTileSet
    bool RenderFileNames = false;
    /*
        UTexture2D m_iconTexture;
        protected Sprite sprite;
    
        protected Rectangle m_spriteSize;
        protected ProgressBar progressBar;

        protected Blend m_sourceBlend = Blend.BlendFactor;
        protected Blend m_destinationBlend = Blend.InvBlendFactor;

        // If this value equals CurrentFrameStartTicks the Z buffer needs to be cleared
        protected static long lastRenderTime;

        //public static int MaxConcurrentDownloads = 3;*/
    // 调试时半径为1千公里。。。
    double m_LayerRadius = 1000000;
    bool AlwaysRenderBaseTiles;
    float TileDrawSpread = 2.9f;
    float TileDrawDistance = 3.5f;
    bool IsDownloadingElevation;


    int m_numberRetries;
    TSet</*UQuadTile*, */UGeoSpatialDownloadRequest*> m_DownloadRequests/* = FHashTable()*/;
    int m_maxQueueSize = 400;
    bool m_isTerrainMapped = true;
    TArray<FImageStore> ImageStores;
    UGeographicCameraComponent* Camera = nullptr;
    TArray<UGeoSpatialDownloadRequest*> m_activeDownloads;
    TArray<FDateTime> m_downloadStarted;
    //= new DateTime[20];
    FTimespan m_connectionWaitTime = FTimespan::FromMinutes(2);
    FDateTime m_connectionWaitStart;
    bool m_isConnectionWaiting;
    bool m_enableColorKeying;


    int32 NumberRetries = 5;
    
    /*
    protected Effect m_effect = null;
    protected string m_effectPath = null;
    protected string m_effectTechnique = null;
    static protected EffectPool m_effectPool = new EffectPool();*/

    /// <summary>
    /// If images in cache are older than expration time a refresh
    /// from server will be attempted.
    /// </summary>
	FTimespan m_cacheExpirationTime = FTimespan::MaxValue();


    /*/// <summary>
    /// Texture showing download in progress
    /// </summary>
    public static UTexture2D* DownloadInProgressTexture;*/

    /// <summary>
    /// Texture showing queued download
    /// </summary>
    static UTexture2D* DownloadQueuedTexture;

    /// <summary>
    /// Texture showing terrain download in progress
    /// </summary>
    static UTexture2D* DownloadTerrainTexture;



    int ColorKey; // default: 100% transparent black = transparent

    /// <summary>
    /// If a color range is to be transparent this specifies the brightest transparent color.
    /// The darkest transparent color is set using ColorKey.
    /// </summary>
    int ColorKeyMax;


    bool RenderGrayscale = false;
    float GrayscaleBrightness = 0.0f;


        /// <summary>
        /// Path to a thumbnail image (e.g. for use as a download indicator).
        /// </summary>
    FString ServerLogoFilePath;

    void Initialize(UGeographicCameraComponent* GeographicCamera);

    bool PerformSelectionAction(UGeographicCameraComponent* GeographicCamera);

    UFUNCTION(BlueprintCallable)
    void Update(UGeographicCameraComponent* GeographicCamera);

    void RemoveInvisibleTiles(UGeographicCameraComponent* GeographicCamera);
    UFUNCTION(BlueprintCallable)
    void Render(UGeographicCameraComponent* GeographicCamera);
   
    void ServiceDownloadQueue();

    void AddToDownloadQueue(UGeoSpatialDownloadRequest* newRequest);


    UGeoSpatialDownloadRequest* GetClosestDownloadRequest();
};
