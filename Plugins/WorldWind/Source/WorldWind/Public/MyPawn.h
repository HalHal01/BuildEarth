// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DrawArgs.h"
#include "GameFramework/DefaultPawn.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "QuadTileSet.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "MyPawn.generated.h"

UCLASS()
class WORLDWIND_API AMyPawn : public APawn
{
	GENERATED_BODY()
public:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UGeographicCameraComponent* GeographicCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;



	/** 鼠标左键 Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LeftClickAction;

	/** 鼠标左键 Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RightClickAction;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MouseMoveAction;


	/** 缩放 Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MouseMiddleAction;

public:
	// Sets default values for this pawn's properties
	AMyPawn();


protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

// 鼠标移动
	/** Called for looking input */
	void OnMouseMove(const FInputActionValue& Value);

	/** 缩放 */
	void Middle(const FInputActionValue& Value);
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


	
	FDrawArgs DrawArgs;
	

	void AddHeading(float Val);
	

	void AddTile(float Val);

	// 鼠标左键相关
	void OnLeftMouseButtonDown();
	void OnLeftMouseButtonUp();
	void OnLeftMouseButtonHolding();

	// 鼠标右键相关
	void OnRightMouseButtonDown();
	void OnRightMouseButtonUp();
	void OnRightMouseButtonHolding();

	// 鼠标中键相关
	void OnMiddleMouseButtonDown();
	void OnMiddleMouseButtonUp();
	void OnMiddleMouseButtonHolding();

	FVector GetUpVector();

	UFUNCTION(BlueprintCallable)
	void GetViewPro(FMatrix& view);
	
};
