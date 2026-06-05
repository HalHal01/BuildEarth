// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GeographicCameraComponent.h"
#include "WorldWindSettings.h"

// Sets default values
AMyPawn::AMyPawn()
{
	/*// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);*/
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	/*// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;*/

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)




	// Create a camera boom (pulls in towards the player if there is a collision)
	GeographicCamera = CreateDefaultSubobject<UGeographicCameraComponent>(TEXT("GeographicCamera"));
	//GeographicCamera->SetupAttachment(RootComponent);

	/*QuadTileSetComponent = CreateDefaultSubobject<UQuadTileSet>(TEXT("QuadTileSet"));
	QuadTileSetComponent->SetupAttachment(RootComponent);*/
	
}



// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}


	// 自定义相机部分

	// TODO01:是否为正规写法待验证
	Cast<APlayerController>(Controller)->PlayerCameraManager->ViewRollMin = -179.999f;
	Cast<APlayerController>(Controller)->PlayerCameraManager->ViewRollMax = 179.999f;
	
	SetActorLocation(GeographicCamera->CameraPosition());
	Controller->SetControlRotation(GeographicCamera->CameraRotation());
	
	
}



/*// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}*/



// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);




	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		/*//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);*/

		/*//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AMyPawn::GetHeading);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMyPawn::MouseRel);*/

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPawn::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyPawn::Look);

		// 鼠标左键
		EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Started, this, &AMyPawn::OnLeftMouseButtonDown);
		EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Completed, this, &AMyPawn::OnLeftMouseButtonUp);
		EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Ongoing, this, &AMyPawn::OnLeftMouseButtonHolding);
		
		// 鼠标中键
		EnhancedInputComponent->BindAction(MouseMiddleAction, ETriggerEvent::Triggered, this, &AMyPawn::Middle);
		
		// 鼠标右键
		EnhancedInputComponent->BindAction(RightClickAction, ETriggerEvent::Started, this, &AMyPawn::OnRightMouseButtonDown);
		EnhancedInputComponent->BindAction(RightClickAction, ETriggerEvent::Completed, this, &AMyPawn::OnRightMouseButtonUp);
		EnhancedInputComponent->BindAction(RightClickAction, ETriggerEvent::Ongoing, this, &AMyPawn::OnRightMouseButtonHolding);

		//Looking
		EnhancedInputComponent->BindAction(MouseMoveAction, ETriggerEvent::Triggered, this, &AMyPawn::OnMouseMove);

		

	}

}



void AMyPawn::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		if (!DrawArgs.IsLeftMouseButtonDown)
		{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

			
		
		/*// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);*/



		/*// get forward vector
		const FVector ForwardDirection = ddd.GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = ddd.GetUnitAxis(EAxis::Y);*/

		// add movement 
		//AddMovementInput(ForwardDirection, MovementVector.Y);
		//AddMovementInput(RightDirection, MovementVector.X);

		double PanRight = -1 * FMath::RadiansToDegrees(MovementVector.X * (GeographicCamera->m_Altitude) / (300 * GeographicCamera->m_WorldRadius));
		double PanUp = FMath::RadiansToDegrees(MovementVector.Y * (GeographicCamera->m_Altitude) / (300 * GeographicCamera->m_WorldRadius));

		GeographicCamera->RotationYawPitchRoll(PanRight, PanUp, 0);

		SetActorLocation(GeographicCamera->CameraPosition());
		Controller->SetControlRotation(GeographicCamera->CameraRotation());



		// 要计算移动的方向
		}
	}
}

void AMyPawn::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		if (DrawArgs.IsLeftMouseButtonDown && !DrawArgs.isMouseDragging)
		{
			
		
		/*// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);*/

		//GeographicCamera->Look(LookAxisVector.X, LookAxisVector.Y);

		SetActorLocation(GeographicCamera->CameraPosition());

		Controller->SetControlRotation(GeographicCamera->CameraRotation());
		}
		
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, LookAxisVector.ToString());
}

void AMyPawn::OnMouseMove(const FInputActionValue& Value)
{

	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, MovementVector.ToString());

	if (Controller != nullptr)
	{
		if (DrawArgs.IsLeftMouseButtonDown && !DrawArgs.isMouseDragging)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("有效！"));

			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		//AddMovementInput(ForwardDirection, MovementVector.Y);
		//AddMovementInput(RightDirection, MovementVector.X);

		
		auto controller = static_cast<APlayerController*>(Controller);
		float MousePositionX, MousePositionY;
		controller->GetMousePosition(MousePositionX, MousePositionY);
	}
		// 要计算移动的方向
	}
	

}

void AMyPawn::Middle(const FInputActionValue& Value)
{

	float LookAxisVector = Value.Get<float>();


	GeographicCamera->ZoomStepped(-LookAxisVector);

	SetActorLocation(GeographicCamera->CameraPosition());
	Controller->SetControlRotation(GeographicCamera->CameraRotation());

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::SanitizeFloat(LookAxisVector));
}

// 鼠标左键按下
void AMyPawn::OnLeftMouseButtonDown()
{
	// 只做了记录位置的操作
	float MousePositionX, MousePositionY;
	if (Cast<APlayerController>(Controller)->GetMousePosition(MousePositionX, MousePositionY))
	{
		DrawArgs.LastMousePosition = FIntPoint(static_cast<int32>(MousePositionX), static_cast<int32>(MousePositionY));
		DrawArgs.MouseDownStartPosition = FIntPoint(static_cast<int32>(MousePositionX), static_cast<int32>(MousePositionY));
	}

	DrawArgs.IsLeftMouseButtonDown = true;
	UE_LOG(LogClass, Log, TEXT("鼠标左键按下了，鼠标位置：X=%f, Y=%f"), MousePositionX, MousePositionY);
}

// 鼠标左键释放
void AMyPawn::OnLeftMouseButtonUp()
{

	float MousePositionX, MousePositionY;
	if (Cast<APlayerController>(Controller)->GetMousePosition(MousePositionX, MousePositionY))
	{
		
		DrawArgs.LastMousePosition = FIntPoint(static_cast<int32>(MousePositionX), static_cast<int32>(MousePositionY));
		
	}

	DrawArgs.IsLeftMouseButtonDown = false;
	DrawArgs.isMouseDragging = false;
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("鼠标左键抬起了！"));

}

// 鼠标左键 holding
void AMyPawn::OnLeftMouseButtonHolding()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Holding!"));



	UWorldWindSettings* WorldSettings = GetMutableDefault<UWorldWindSettings>();
	
	//////////////////////////////////////////////////
	// WorldWind 复制
	
	/*// Default to default cursor
	DrawArgs.MouseCursor = CursorType.Arrow;*/
	
	float MousePositionX, MousePositionY;
	Cast<APlayerController>(Controller)->GetMousePosition(MousePositionX, MousePositionY);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::SanitizeFloat(MousePositionX) + FString::SanitizeFloat(MousePositionY));
	

	
	// 这段不知道啥意思
	bool handled = false;
	if (!DrawArgs.isMouseDragging)
	{
		/*handled = m_RootWidget.OnMouseMove(e);

		if (!handled)
		{
			handled = m_NewRootWidget.OnMouseMove(e);
		}*/
	}

	DrawArgs.isMouseDragging = true;
	if(!handled)
	{
		// 鼠标位置

		// 百分比
		int DeltaX = static_cast<int32>(MousePositionX) - DrawArgs.LastMousePosition.X;
		int32 DeltaY = static_cast<int32>(MousePositionY) - DrawArgs.LastMousePosition.Y;
		float deltaXNormalized = static_cast<float>(DeltaX)/DrawArgs.screenWidth;
		float deltaYNormalized = static_cast<float>(DeltaY)/DrawArgs.screenHeight;

		if(!DrawArgs.isMouseDragging)
		{
			/*if(this._menuBar.OnMouseMove(e))
			{
				base.OnMouseMove(e);*/
			return;
			/*}*/
		}

		
		if(DrawArgs.MouseDownStartPosition == FIntPoint::NoneValue)
		{
			return;
		}
					


		if (WorldSettings->CameraTwistLock)
		{
			double PrevLat, PrevLon, CurLat, CurLon;
			if (GeographicCamera->PickingRayIntersection(DrawArgs.LastMousePosition.X, DrawArgs.LastMousePosition.Y, PrevLat, PrevLon) &&
				GeographicCamera->PickingRayIntersection(static_cast<int32>(MousePositionX), static_cast<int32>(MousePositionY), CurLat, CurLon))
			{
				double DeltaLat = PrevLat - CurLat;
				double DeltaLon = PrevLon - CurLon;
				GeographicCamera->Pan(DeltaLat, DeltaLon);

				SetActorLocation(GeographicCamera->CameraPosition());
				Controller->SetControlRotation(GeographicCamera->CameraRotation());
			}
			else
			{
				// Old style pan
				double DeltaLat = FMath::RadiansToDegrees((double)DeltaY * (GeographicCamera->m_Altitude) / (800 * GeographicCamera->m_WorldRadius));
				double DeltaLon = FMath::RadiansToDegrees((double)-DeltaX * (GeographicCamera->m_Altitude) / (800 * GeographicCamera->m_WorldRadius));
				GeographicCamera->Pan(DeltaLat, DeltaLon);
			}
		}
		else
		{
			double factor = (GeographicCamera->m_Altitude) / (1500 * GeographicCamera->m_WorldRadius);
			GeographicCamera->RotationYawPitchRoll(
				FMath::RadiansToDegrees(DrawArgs.LastMousePosition.X - static_cast<int32>(MousePositionX)) * factor,
				FMath::RadiansToDegrees(static_cast<int32>(MousePositionY) - DrawArgs.LastMousePosition.Y) * factor,
				0);
		}
	}

	double cLat, cLon;
	GeographicCamera->PickingRayIntersection(MousePositionX, MousePositionY, cLat, cLon);

	DrawArgs.LastMousePosition.X = MousePositionX;
	DrawArgs.LastMousePosition.Y = MousePositionY;
}

// 鼠标右键按下
void AMyPawn::OnRightMouseButtonDown()
{
	// 只做了记录位置的操作
	float MousePositionX, MousePositionY;
	if (Cast<APlayerController>(Controller)->GetMousePosition(MousePositionX, MousePositionY))
	{
		DrawArgs.LastMousePosition = FIntPoint(static_cast<int32>(MousePositionX), static_cast<int32>(MousePositionY));
		DrawArgs.MouseDownStartPosition = FIntPoint(static_cast<int32>(MousePositionX), static_cast<int32>(MousePositionY));
	}

	DrawArgs.IsRightMouseButtonDown = true;
	UE_LOG(LogClass, Log, TEXT("鼠标右键按下了，鼠标位置：X=%f, Y=%f"), MousePositionX, MousePositionY);
}

// 鼠标右键释放
void AMyPawn::OnRightMouseButtonUp()
{
	float MousePositionX, MousePositionY;
	if (Cast<APlayerController>(Controller)->GetMousePosition(MousePositionX, MousePositionY))
	{
		
		DrawArgs.LastMousePosition = FIntPoint(static_cast<int32>(MousePositionX), static_cast<int32>(MousePositionY));
		
	}

	DrawArgs.IsRightMouseButtonDown = false;
	DrawArgs.isMouseDragging = false;
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("鼠标右键抬起了！"));
}

// 鼠标右键 holding
void AMyPawn::OnRightMouseButtonHolding()
{
	UWorldWindSettings* WorldSettings = GetMutableDefault<UWorldWindSettings>();
	
	//////////////////////////////////////////////////
	// WorldWind 复制
	
	/*// Default to default cursor
	DrawArgs.MouseCursor = CursorType.Arrow;*/  
	
	float MousePositionX, MousePositionY;
	Cast<APlayerController>(Controller)->GetMousePosition(MousePositionX, MousePositionY);  // TODO01：要加if判断嘛？
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::SanitizeFloat(MousePositionX) + FString::SanitizeFloat(MousePositionY));
	

	
	// 这段不知道啥意思
	bool handled = false;
	if (!DrawArgs.isMouseDragging)
	{
		/*handled = m_RootWidget.OnMouseMove(e);

		if (!handled)
		{
			handled = m_NewRootWidget.OnMouseMove(e);
		}*/
	}

	DrawArgs.isMouseDragging = true;
	if(!handled)
	{
		// 鼠标位置

		float ScreenWidth = GeographicCamera->m_ViewPortSize.X;
		float ScreenHeight = GeographicCamera->m_ViewPortSize.Y;
		// 百分比
		int DeltaX = static_cast<int32>(MousePositionX) - DrawArgs.LastMousePosition.X;
		int32 DeltaY = static_cast<int32>(MousePositionY) - DrawArgs.LastMousePosition.Y;
		float DeltaXNormalized = static_cast<float>(DeltaX)/ScreenWidth;
		float DeltaYNormalized = static_cast<float>(DeltaY)/ScreenHeight;

		// 似乎是不需要了？？？
		if(!DrawArgs.isMouseDragging)
		{
			/*if(this._menuBar.OnMouseMove(e))
			{
				base.OnMouseMove(e);*/
			return;
			/*}*/
		}

		
		if(DrawArgs.MouseDownStartPosition == FIntPoint::NoneValue)
		{
			return;
		}
		
		
		{
			//Right mouse button

			// Heading

			

			// tilt
			GeographicCamera->addHeading(FMath::RadiansToDegrees(-DeltaXNormalized * WorldSettings->CameraRotationSpeed),
				FMath::RadiansToDegrees(DeltaYNormalized * WorldSettings->CameraRotationSpeed));


			SetActorLocation(GeographicCamera->CameraPosition());
			Controller->SetControlRotation(GeographicCamera->CameraRotation());
		}
	}

	// double cLat, cLon;
	//GeographicCamera->PickingRayIntersection(MousePositionX, MousePositionY, cLat, cLon);

	DrawArgs.LastMousePosition.X = MousePositionX;
	DrawArgs.LastMousePosition.Y = MousePositionY;
}

void AMyPawn::OnMiddleMouseButtonDown()
{

}

void AMyPawn::OnMiddleMouseButtonUp()
{
}

void AMyPawn::OnMiddleMouseButtonHolding()
{
}

FVector AMyPawn::GetUpVector()
{
	return FVector::ZeroVector;
}



void OnMouseDown(/*MouseEventArgs e*/)
{
	/*this.Focus();  //fixes mousewheel not working problem

	DrawArgs.LastMousePosition.X = e.X;
	DrawArgs.LastMousePosition.Y = e.Y;

	mouseDownStartPosition.X = e.X;
	mouseDownStartPosition.Y = e.Y;


	try
	{
		bool handled = false;
		handled = m_RootWidget.OnMouseDown(e);

		if (!handled)
		{
			handled = m_NewRootWidget.OnMouseDown(e);
		}

		if(!handled)
		{
			if(!this._menuBar.OnMouseDown(e))
			{

			}
		}
	}
	finally
	{
		if(e.Button == MouseButtons.Left)
			DrawArgs.IsLeftMouseButtonDown = true;

		if(e.Button == MouseButtons.Right)
			DrawArgs.IsRightMouseButtonDown = true;
		// Call the base class method so that registered delegates receive the event.
		base.OnMouseDown(e);
	}*/
}


void AMyPawn::AddHeading(float Val)
{
	if (Val != 0.f && Controller && Controller->IsLocalPlayerController())
	{
		APlayerController* const PC = CastChecked<APlayerController>(Controller);
		PC->AddYawInput(Val);
	}
}

void AMyPawn::AddTile(float Val)
{
	if (Val != 0.f && Controller && Controller->IsLocalPlayerController())
	{
		APlayerController* const PC = CastChecked<APlayerController>(Controller);
		PC->AddRollInput(Val);
	}
}


void AMyPawn::GetViewPro(FMatrix& view)
{
	APlayerController* Player = Cast<APlayerController>(Controller);
	
	ULocalPlayer* const LP = Player ? Player->GetLocalPlayer() : nullptr;
	if (LP && LP->ViewportClient)
	{
		// get the projection data
		FSceneViewProjectionData ProjectionData;

		if (LP->GetProjectionData(LP->ViewportClient->Viewport, /*out*/ ProjectionData))
		{
			view = ProjectionData.ComputeViewProjectionMatrix();
		}
	}
}