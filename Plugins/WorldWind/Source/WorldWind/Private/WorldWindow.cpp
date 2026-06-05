// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldWindow.h"

#include "DrawArgs.h"
#include "WorldWindSettings.h"

// Sets default values
AWorldWindow::AWorldWindow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	RootComponent = SceneComponent;*/

	QuadTileSetComponent = CreateDefaultSubobject<UQuadTileSet>(TEXT("QuadTileSet"));
	RootComponent = QuadTileSetComponent;
	
}

// Called when the game starts or when spawned
void AWorldWindow::BeginPlay()
{
	Super::BeginPlay();

	//Cast<UStaticMeshComponent>(PlacedActor->GetComponentByClass(UGeographicCameraComponent::StaticClass()))
	
}

// Called every frame
void AWorldWindow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
