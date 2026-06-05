// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"

#include "Math/BoundingBox.h"
#include "WebDownload/DownloadRequest.h"

#include "ProceduralMeshComponent.h"
#include "UObject/NoExportTypes.h"

#include "QuadTile.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )  // 未完待续
class WORLDWIND_API UQuadTile : public UProceduralMeshComponent
{
	GENERATED_BODY()

/*public:	
	// Sets default values for this component's properties
	UQuadTile();*/

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// Child tile location
	enum EChildLocation
	{
		NorthWest,
		SouthWest,
		NorthEast,
		SouthEast
	};
	
	struct FPositionNormalTextured
	{

		FPositionNormalTextured()
		{};
		//~BoundingBox();

		FVector Position;
		FVector Normal;
		FVector2D UV;
	};

public:
	//UQuadTileSet* m_QuadTileSet;

	double m_West, m_East, m_North, m_South;
	double m_CenterLatitude, m_CenterLongitude;
	double m_LatitudeSpan, m_LongitudeSpan;

	int32 m_Level, m_Row, m_Col;

	bool m_isInitialized;
	
	FBoundingBox m_BoundingBox;
	UGeoSpatialDownloadRequest* DownloadRequest;

	TArray<UTexture2DDynamic*> m_Textures;
	

	/// Number of points in child flat mesh grid (times 2)
	// 指的是大瓦片的行或者列的数量，顶点数量还要在此数值上加1
	const int32 VertexCount = 2;

	// Number of points in child terrain mesh grid (times 2)
	// 高程数组尺度为 11*11
	const int m_VertexCountElevated = 8;

	UQuadTile* m_NorthWestChild = nullptr;
	UQuadTile* m_SouthWestChild = nullptr;
	UQuadTile* m_NorthEastChild = nullptr;
	UQuadTile* m_SouthEastChild = nullptr;

	TArray<FPositionNormalTextured> m_NorthWestVertices;
	TArray<FPositionNormalTextured> m_SouthWestVertices;
	TArray<FPositionNormalTextured> m_NorthEastVertices;
	TArray<FPositionNormalTextured> m_SouthEastVertices;
	TArray<int32> m_VertexIndexes;

	// 子瓦片高程数据
	TArray<TArray<float>> NorthWestElevations;
	TArray<TArray<float>> SouthWestElevations;
	TArray<TArray<float>> NorthEastElevations;
	TArray<TArray<float>> SouthEastElevations;

	// 是否要渲染围裙
	bool m_RenderStruts = true;

    FVector m_LocalOrigin; // Add this offset to get world coordinates

	bool m_isResetingCache;

	/// <summary>
	/// The vertical exaggeration the tile mesh was computed for
	/// </summary>
    float m_VerticalExaggeration = 1.f;

    bool m_isDownloadingTerrain;
	bool m_WaitingForDownload = false;
	bool m_isDownloadingImage = false;

	// Initializes a new instance of the <see cref= "T:WorldWind.Renderable.QuadTile"/> class.
	void UpdateTileInfo(double South, double North, double West, double East, int Level);

	void ResetCache();
	/// <summary>
	/// Returns the QuadTile for specified location if available.
	/// Tries to queue a download if not available.
	/// </summary>
	/// <returns>Initialized QuadTile if available locally, else null.</returns>
	UQuadTile* ComputeChild(double ChildSouth, double ChildNorth, double ChildWest, double ChildEast);
	
	void ComputeChildren(UGeographicCameraComponent* drawArgs);

	virtual void Dispose();
	void Initialize();
	
	/// Updates this layer (background)
	virtual void Update(UGeographicCameraComponent* DrawArgs);

	
	/// <summary>
	/// Builds flat or terrain mesh for current tile
	/// </summary>
	void CreateTileMesh();
	
	// Edits by Patrick Murris : fixing mesh sides normals (2006-11-18)


	// Builds a flat mesh (no terrain)
	void CreateFlatMesh();
	double m_MeshBaseRadius = 0;
	
	// Build the elevated terrain mesh
	void CreateElevatedMesh();
	float m_CurrentOpacity = 255;
	
	/// <summary>
	/// Create child tile terrain mesh
	/// Build the mesh with one extra vertice all around for proper normals calculations later on.
	/// Use the struts vertices to that effect. Struts are properly folded after normals calculations.
	/// </summary>
	void CreateElevatedMesh(EChildLocation Corner, TArray<FPositionNormalTextured>& Vertices, double MeshBaseRadius, TArray<TArray<float>>& HeightData);
	
	void CalculateNormals(TArray<FPositionNormalTextured>& Vertices, TArray<int32>& Indices);
	
	// Project an elevated mesh point to the mesh base
	FVector ProjectOnMeshBase(FVector P);

	
	FString ImageFilePath;


	bool Render(UGeographicCameraComponent* DrawArgs);
	/*
	protected CustomVertex.PositionColored[] downloadRectangle = new CustomVertex.PositionColored[5];
	/// <summary>
	/// Render a rectangle around an image tile in the specified color
	/// </summary>
	void RenderDownloadRectangle(DrawArgs drawArgs, int color, Vector3 referenceCenter);
	static Effect grayscaleEffect = null;
	*/
	/// <summary>
	/// Render one of the 4 quadrants with optional download indicator
	/// </summary>
	/// 放了个位置参数，应该想个其他实现方式？？？
	void Render(/*Device device, */TArray<FPositionNormalTextured> Verts, UQuadTile* child, EChildLocation ChildLocation);
	/*
	void device_DeviceReset(object sender, EventArgs e);*/
	UFUNCTION()
	void UpdateTileTexture(UTexture2DDynamic* Texture, UQuadTile* hh);

};


