// Copyright Epic Games, Inc. All Rights Reserved.

#include "BuildEarthPickUpComponent.h"

UBuildEarthPickUpComponent::UBuildEarthPickUpComponent()
{
	// Setup the Sphere Collision
	SphereRadius = 32.f;
}

void UBuildEarthPickUpComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register our Overlap Event
	OnComponentBeginOverlap.AddDynamic(this, &UBuildEarthPickUpComponent::OnSphereBeginOverlap);
}

void UBuildEarthPickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Checking if it is a First Person Character overlapping
	ABuildEarthCharacter* Character = Cast<ABuildEarthCharacter>(OtherActor);
	if(Character != nullptr)
	{
		// Notify that the actor is being picked up
		OnPickUp.Broadcast(Character);

		// Unregister from the Overlap Event so it is no longer triggered
		OnComponentBeginOverlap.RemoveAll(this);
	}
}
