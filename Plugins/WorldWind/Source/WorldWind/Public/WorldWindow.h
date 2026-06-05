// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuadTileSet.h"
#include "GameFramework/Actor.h"
#include "WorldWindow.generated.h"

UCLASS()
class WORLDWIND_API AWorldWindow : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldWindow();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly/*, Category = "StreetMap"*/)
	USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly/*, Category = "StreetMap"*/)
	UQuadTileSet* QuadTileSetComponent;
};
