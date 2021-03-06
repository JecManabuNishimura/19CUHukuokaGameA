//----------------------------------------------------------
// ファイル名		：StageSelect.cpp
// 概要				：ステージ画面で各ステージの切り替え
// 作成者			：19CU0222 鍾家同
// 更新内容			：2020/10/27
//					：2020/12/12 更新　画面の移動をStageSelectCameraで制御
//					：2021/03/04 更新　ステージ選択範囲を4から8までに増加
//----------------------------------------------------------

#include "StageSelect.h"
#include "Components/WidgetComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AStageSelect::AStageSelect() :
	canEnter(true),
	anyPressed(true),
	turnRight(true),
	scalingTime(1.0f),
	scalingRate(0.75f),
	finishSmall(false),
	rotatingTime(88.5f),
	rotatingRate(0.0f),
	rotatingRateMax(1.0f),
	finishRotating(false),
	startRotating(false),
	nextRotation(0.0f, 0.0f, 0.0f),
	currentStage(1),
	pressCount(0),
	stageAmount(4),
	m_Stage01(NULL),
	m_Stage02(NULL),
	m_Stage03(NULL),
	m_Stage04(NULL),
	m_Stage05(NULL),
	m_Stage06(NULL),
	m_Stage07(NULL),
	m_Stage08(NULL),
	ButtonRightPressed(false),
	ButtonLeftPressed(false)
	//isEnterButtonPressed(false),
	//isCancelButtonPressed(false)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//m_MainSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MainSceneComponent"));
	//RootComponent = m_MainSceneComponent;

	m_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	if (m_BoxComponent != NULL) {
		//m_BoxComponent->SetupAttachment(m_SceneComponent);
		RootComponent = m_BoxComponent;
		//m_BoxComponent->SetRelativeScale3D(FVector(20.0f, 20.0f, 20.0f));
	}

	m_SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	m_SceneComponent->SetupAttachment(RootComponent);

	m_Stage01 = CreateDefaultSubobject<UWidgetComponent>(TEXT("stage01"));
	if (m_Stage01 != NULL) {
		m_Stage01->SetupAttachment(m_SceneComponent);
		m_Stage01->SetDrawSize(FVector2D(1920.0f, 1080.0f));
		m_Stage01->SetRelativeLocationAndRotation(FVector(0.0f, 870.0f, 0.0f), FRotator(0.0f, 90.0f, 0.0f));
	}

	m_Stage05 = CreateDefaultSubobject<UWidgetComponent>(TEXT("stage05"));
	if (m_Stage05 != NULL) {
		m_Stage05->SetupAttachment(m_SceneComponent);
		m_Stage05->SetDrawSize(FVector2D(1920.0f, 1080.0f));
		m_Stage05->SetRelativeLocationAndRotation(FVector(0.0f, 870.0f, 0.0f), FRotator(0.0f, 90.0f, 0.0f));
	}

	m_Stage09 = CreateDefaultSubobject<UWidgetComponent>(TEXT("stage09"));
	if (m_Stage09 != NULL) {
		m_Stage09->SetupAttachment(m_SceneComponent);
		m_Stage09->SetDrawSize(FVector2D(1920.0f, 1080.0f));
		m_Stage09->SetRelativeLocationAndRotation(FVector(0.0f, 870.0f, 0.0f), FRotator(0.0f, 90.0f, 0.0f));
	}

	m_Stage02 = CreateDefaultSubobject<UWidgetComponent>(TEXT("stage02"));
	if (m_Stage02 != NULL) {
		m_Stage02->SetupAttachment(m_SceneComponent);
		m_Stage02->SetDrawSize((FVector2D(1920.0f, 1080.0f)));
		m_Stage02->SetRelativeLocation(FVector(870.0f, 0.0f, 0.0f));
	}

	m_Stage06 = CreateDefaultSubobject<UWidgetComponent>(TEXT("stage06"));
	if (m_Stage06 != NULL) {
		m_Stage06->SetupAttachment(m_SceneComponent);
		m_Stage06->SetDrawSize((FVector2D(1920.0f, 1080.0f)));
		m_Stage06->SetRelativeLocation(FVector(870.0f, 0.0f, 0.0f));
	}

	m_Stage03 = CreateDefaultSubobject<UWidgetComponent>(TEXT("stage03"));
	if (m_Stage03 != NULL) {
		m_Stage03->SetDrawSize((FVector2D(1920.0f, 1080.0f)));
		m_Stage03->SetRelativeLocationAndRotation(FVector(0.0f, -870.0f, 0.0f), FRotator(0.0f, -90.0f, 0.0f));
		m_Stage03->SetupAttachment(m_SceneComponent);
	}

	m_Stage07 = CreateDefaultSubobject<UWidgetComponent>(TEXT("stage07"));
	if (m_Stage07 != NULL) {
		m_Stage07->SetDrawSize((FVector2D(1920.0f, 1080.0f)));
		m_Stage07->SetRelativeLocationAndRotation(FVector(0.0f, -870.0f, 0.0f), FRotator(0.0f, -90.0f, 0.0f));
		m_Stage07->SetupAttachment(m_SceneComponent);
	}

	m_Stage04 = CreateDefaultSubobject<UWidgetComponent>(TEXT("stage04"));
	if (m_Stage04 != NULL) {
		m_Stage04->SetupAttachment(m_SceneComponent);
		m_Stage04->SetDrawSize((FVector2D(1920.0f, 1080.0f)));
		m_Stage04->SetRelativeLocationAndRotation(FVector(-870.0f, 0.0f, 0.0f), FRotator(0.0f, -180.0f, 0.0f));
	}

	m_Stage08 = CreateDefaultSubobject<UWidgetComponent>(TEXT("stage08"));
	if (m_Stage08 != NULL) {
		m_Stage08->SetupAttachment(m_SceneComponent);
		m_Stage08->SetDrawSize((FVector2D(1920.0f, 1080.0f)));
		m_Stage08->SetRelativeLocationAndRotation(FVector(-870.0f, 0.0f, 0.0f), FRotator(0.0f, -180.0f, 0.0f));
	}

	/*m_Stage01 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stage01"));
	if (m_Stage01 != NULL) {
		m_Stage01->SetupAttachment(m_SceneComponent);
		m_Stage01->SetRelativeLocationAndRotation(FVector(0.0f, 30.0f, 0.0f), FRotator(0.0f, 0.0f, 90.0f));
		m_Stage01->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	}
	m_Stage02 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stage02"));
	if (m_Stage02 != NULL) {
		m_Stage02->SetupAttachment(m_SceneComponent);
		m_Stage02->SetRelativeLocationAndRotation(FVector(30.0f, 0.0f, 0.0f), FRotator(-90.0f, 0.0f, 0.0f));
		m_Stage02->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	}
	m_Stage03 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stage03"));
	if (m_Stage03 != NULL) {
		m_Stage03->SetupAttachment(m_SceneComponent);
		m_Stage03->SetRelativeLocationAndRotation(FVector(0.0f, -30.0f, 0.0f), FRotator(0.0f, 0.0f, -90.0f));
		m_Stage03->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	}
	m_Stage04 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stage04"));
	if (m_Stage04 != NULL) {
		m_Stage04->SetupAttachment(m_SceneComponent);
		m_Stage04->SetRelativeLocationAndRotation(FVector(-30.0f, 0.0f, 0.0f), FRotator(90.0f, 0.0f, 0.0f));
		m_Stage04->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	}*/

	/*m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("m_pSpringArm"));
	if (m_pSpringArm != NULL) {
		m_pSpringArm->SetupAttachment(RootComponent);

		// アームの長さを設定
		// カメラのコリジョンテストを行うかを設定
		m_pSpringArm->bDoCollisionTest = false;
		// カメラ追従ラグを使うかを設定
		m_pSpringArm->bEnableCameraLag = true;
		// カメラ追従ラグの速度を設定
		m_pSpringArm->CameraLagSpeed = 10.0f;
		// カメラ回転ラグを使うかを設定
		m_pSpringArm->bEnableCameraRotationLag = true;
		// カメラ回転ラグの速度を設定
		m_pSpringArm->CameraRotationLagSpeed = 10.0f;

		m_pSpringArm->TargetArmLength = 1280.0f * 2.0f;
	}

	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));
	if (m_pCamera != NULL) {
		// カメラをスプリングアームにアタッチさせる
		m_pCamera->SetupAttachment(m_pSpringArm, USpringArmComponent::SocketName);
	}*/
}

// Called when the game starts or when spawned
void AStageSelect::BeginPlay()
{
	Super::BeginPlay();
	CurrentStage();
}

// Called every frame
void AStageSelect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 左右ボタンが押されたら、ステージ切り替え動画を実行する
	if (!anyPressed)
		StageChanging(DeltaTime);

	// 現在選択されたステージ
	CurrentStage();

	SelectRightFromSSCamera();
	SelectLeftFromSSCamera();
}

// Called to bind functionality to input
void AStageSelect::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("SelectRight", IE_Pressed, this, &AStageSelect::SelectRight);
	InputComponent->BindAction("SelectLeft", IE_Pressed, this, &AStageSelect::SelectLeft);
	InputComponent->BindAction("Enter", IE_Pressed, this, &AStageSelect::SelectConfirm);
	//InputComponent->BindAction("Enter", IE_Pressed, this, &AStageSelect::ButtonEnter);
	//InputComponent->BindAction("Back", IE_Pressed, this, &AStageSelect::ButtonCancel);
}

void AStageSelect::StageChanging(float _deltaTime)
{
	FRotator presRotation = m_SceneComponent->GetRelativeRotation();
	FVector presScale = m_SceneComponent->GetRelativeScale3D();

	//UE_LOG(LogTemp, Warning, TEXT("TurnRight:%s"), turnRight ? TEXT("TRUE") : TEXT("FALSE"));
	//UE_LOG(LogTemp, Warning, TEXT("anyPressed:%s"), anyPressed ? TEXT("TRUE") : TEXT("FALSE"));
	//UE_LOG(LogTemp, Warning, TEXT("preScale:%s"), *FString::SanitizeFloat(presScale.SizeSquared()));
	//UE_LOG(LogTemp, Warning, TEXT("finishSmall:%s"), finishSmall ? TEXT("TRUE") : TEXT("FALSE"));
	//UE_LOG(LogTemp, Warning, TEXT("finishRotating:%s"), finishRotating ? TEXT("TRUE") : TEXT("FALSE"));
	//UE_LOG(LogTemp, Warning, TEXT("presRotation:%s"), *presRotation.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("rotatingRate:%s"), *FString::SanitizeFloat(rotatingRate));

	if (turnRight) {
		// 縮小開始
		/*if (!finishSmall && presScale.SizeSquared() >= scalingRate) {
			m_SceneComponent->SetRelativeScale3D(
				FVector(presScale.X -= scalingTime * _deltaTime,
				presScale.Y -= scalingTime * _deltaTime,
				presScale.Z -= scalingTime * _deltaTime));
		}
		else if (!finishSmall) {
			finishSmall = true;
		}*/

		// 回転開始
		if (/*finishSmall &&*/ !finishRotating && rotatingRate < rotatingRateMax * FMath::Abs(pressCount)) {
			canEnter = true;
			startRotating = true;
			rotatingRate += _deltaTime;
			m_SceneComponent->SetRelativeRotation(
				FRotator(presRotation.Pitch, presRotation.Yaw += rotatingTime * _deltaTime, presRotation.Roll));
		}
		else if (/*finishSmall &&*/ rotatingRate >= rotatingRateMax) {
			finishRotating = false;
			startRotating = false;
			rotatingRate = 0.0f;
			pressCount = 0.0f;
			//canEnter = false;
			anyPressed = true;
		}

		// 拡大開始
		/*if (finishRotating) {
			if (presScale.SizeSquared() <= 3.0f) {
				m_SceneComponent->SetRelativeScale3D(
					FVector(presScale.X += scalingTime * _deltaTime,
					presScale.Y += scalingTime * _deltaTime,
					presScale.Z += scalingTime * _deltaTime));
			}
			else {
				m_SceneComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
				canEnter = true;
				anyPressed = true;
				finishSmall = false;
				finishRotating = false;
			}
		}*/
	}
	if (!turnRight) {
		// 縮小開始
		/*if (!finishSmall && presScale.SizeSquared() >= scalingRate) {
			m_SceneComponent->SetRelativeScale3D(
				FVector(presScale.X -= scalingTime * _deltaTime,
				presScale.Y -= scalingTime * _deltaTime,
				presScale.Z -= scalingTime * _deltaTime));
		}
		else if (!finishSmall) {
			finishSmall = true;
		}*/

		// 回転開始
		if (/*finishSmall &&*/ !finishRotating && rotatingRate <= rotatingRateMax * (float) pressCount) {
			canEnter = true;
			startRotating = true;
			rotatingRate += _deltaTime;
			m_SceneComponent->SetRelativeRotation(
				FRotator(presRotation.Pitch, presRotation.Yaw -= rotatingTime * _deltaTime, presRotation.Roll));
		}
		else if (/*finishSmall &&*/ rotatingRate > rotatingRateMax) {
			finishRotating = false;
			startRotating = false;
			rotatingRate = 0.0f;
			pressCount = 0.0f;
			//canEnter = false;
			anyPressed = true;
		}

		// 拡大開始
		/*if (finishRotating) {
			if (presScale.SizeSquared() <= 3.0f) {
				m_SceneComponent->SetRelativeScale3D(
					FVector(presScale.X += scalingTime * _deltaTime,
					presScale.Y += scalingTime * _deltaTime,
					presScale.Z += scalingTime * _deltaTime));
			}
			else {
				m_SceneComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
				canEnter = true;
				anyPressed = true;
				finishSmall = false;
				finishRotating = false;
			}
		}*/
	}
}

// 現在選択されたステージ
void AStageSelect::CurrentStage()
{
	switch (currentStage)
	{
	case 1:
		m_Stage01->SetHiddenInGame(false);

		m_Stage08->SetHiddenInGame(false);
		m_Stage04->SetHiddenInGame(true);

		m_Stage02->SetHiddenInGame(false);
		m_Stage06->SetHiddenInGame(true);
		break;
	case 2:
		m_Stage02->SetHiddenInGame(false);

		m_Stage01->SetHiddenInGame(false);
		m_Stage05->SetHiddenInGame(true);

		m_Stage03->SetHiddenInGame(false);
		m_Stage07->SetHiddenInGame(true);
		break;
	case 3:
		m_Stage03->SetHiddenInGame(false);

		m_Stage02->SetHiddenInGame(false);
		m_Stage06->SetHiddenInGame(true);

		m_Stage04->SetHiddenInGame(false);
		m_Stage08->SetHiddenInGame(true);
		break;
	case 4:
		m_Stage04->SetHiddenInGame(false);

		m_Stage03->SetHiddenInGame(false);
		m_Stage07->SetHiddenInGame(true);

		m_Stage05->SetHiddenInGame(false);
		m_Stage01->SetHiddenInGame(true);
		break;
	case 5:
		m_Stage05->SetHiddenInGame(false);

		m_Stage04->SetHiddenInGame(false);
		m_Stage08->SetHiddenInGame(true);

		m_Stage06->SetHiddenInGame(false);
		m_Stage02->SetHiddenInGame(true);
		break;
	case 6:
		m_Stage06->SetHiddenInGame(false);

		m_Stage05->SetHiddenInGame(false);
		m_Stage01->SetHiddenInGame(true);

		m_Stage07->SetHiddenInGame(false);
		m_Stage03->SetHiddenInGame(true);
		break;
	case 7:
		m_Stage07->SetHiddenInGame(false);

		m_Stage06->SetHiddenInGame(false);
		m_Stage02->SetHiddenInGame(true);

		m_Stage08->SetHiddenInGame(false);
		m_Stage04->SetHiddenInGame(true);
		break;
	case 8:
		m_Stage08->SetHiddenInGame(false);

		m_Stage07->SetHiddenInGame(false);
		m_Stage03->SetHiddenInGame(true);

		m_Stage01->SetHiddenInGame(false);
		m_Stage05->SetHiddenInGame(true);
		break;
	default:
		break;
	}
}

void AStageSelect::SelectRight()
{
	if (anyPressed) {
		//UE_LOG(LogTemp, Warning, TEXT("Right!!"));
		turnRight = true;
		anyPressed = false;

		currentStage += 1;
		if (currentStage >= stageAmount + 1) currentStage = 1;
		pressCount += 1;
		
		CurrentStage();
	}
	/*if (canEnter) {
		currentStage += 1;
		if (currentStage >= stageAmount + 1) currentStage = 1;
		pressCount += 1;
	}*/
}

void AStageSelect::SelectLeft()
{
	if (anyPressed) {
		//UE_LOG(LogTemp, Warning, TEXT("Right!!"));
		turnRight = false;
		anyPressed = false;
		currentStage -= 1;
		if (currentStage <= 0) currentStage = stageAmount;
		pressCount += 1;

		CurrentStage();
	}
	/*if (canEnter) {
		currentStage -= 1;
		if (currentStage <= 0) currentStage = stageAmount;
		pressCount += 1;
	}*/


}

void AStageSelect::SelectRightFromSSCamera()
{
	if (ButtonRightPressed) {
		//UE_LOG(LogTemp, Warning, TEXT("Right!!"));
		turnRight = true;
		anyPressed = false;
		ButtonRightPressed = false;
		ButtonLeftPressed = false;
		currentStage += 1;
		if (currentStage >= stageAmount + 1) currentStage = 1;
		pressCount += 1;
	}
}

void AStageSelect::SelectLeftFromSSCamera()
{
	if (ButtonLeftPressed) {
		//UE_LOG(LogTemp, Warning, TEXT("Right!!"));
		turnRight = false;
		anyPressed = false;
		ButtonLeftPressed = false;
		ButtonRightPressed = false;
		currentStage -= 1;
		if (currentStage <= 0) currentStage = stageAmount;
		pressCount += 1;
	}
}

void AStageSelect::SelectConfirm()
{
	//UGameplayStatics::OpenLevel(this, "Game");
}

/*void AStageSelect::ButtonEnter()
{
	isEnterButtonPressed = true;
}

void AStageSelect::ButtonCancel()
{
	isCancelButtonPressed = true;
}*/
