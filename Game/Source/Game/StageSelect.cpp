//----------------------------------------------------------
// ファイル名		：StageSelect.cpp
// 概要				：ステージ画面で各ステージの切り替え
// 作成者			：19CU0222 鍾家同
// 更新内容			：2020/10/27
//					：
//----------------------------------------------------------

#include "StageSelect.h"
#include "Components/WidgetComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AStageSelect::AStageSelect() :
	canEnter(true),
	anyPressed(true),
	turnRight(true),
	scalingTime(1.0f),
	scalingRate(0.75f),
	finishSmall(false),
	rotatingTime(100.0f),
	rotatingRate(0.0f),
	rotatingRateMax(0.9f),
	finishRotating(false),
	startRotating(false),
	nextRotation(0.0f, 0.0f, 0.0f),
	stageNum(1),
	pressCount(0)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_MainSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MainSceneComponent"));
	RootComponent = m_MainSceneComponent;

	m_SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	m_SceneComponent->SetupAttachment(RootComponent);

	m_Stage01 = CreateDefaultSubobject<UWidgetComponent>(TEXT("stage03"));
	if (m_Stage01 != NULL) {
		m_Stage01->SetupAttachment(m_SceneComponent);
	}
	m_Stage01->SetDrawSize(FVector2D(1280.0f, 720.0f));
	m_Stage01->SetRelativeLocationAndRotation(FVector(-640.0f, 0.0f, 0.0f), FRotator(0.0f, -180.0f, 0.0f));

	m_Stage02 = CreateDefaultSubobject<UWidgetComponent>(TEXT("stage04"));
	if (m_Stage02 != NULL) {
		m_Stage02->SetupAttachment(m_SceneComponent);
	}
	m_Stage02->SetDrawSize(FVector2D(1280.0f, 720.0f));
	m_Stage02->SetRelativeLocationAndRotation(FVector(0.0f, 640.0f, 0.0f), FRotator(0.0f, 90.0f, 0.0f));

	m_Stage03 = CreateDefaultSubobject<UWidgetComponent>(TEXT("stage01"));
	if (m_Stage03 != NULL) {
		m_Stage03->SetupAttachment(m_SceneComponent);
	}
	m_Stage03->SetDrawSize(FVector2D(1280.0f, 720.0f));
	m_Stage03->SetRelativeLocation(FVector(640.0f, 0.0f, 0.0f));

	m_Stage04 = CreateDefaultSubobject<UWidgetComponent>(TEXT("stage02"));
	if (m_Stage04 != NULL) {
		m_Stage04->SetupAttachment(m_SceneComponent);
		m_Stage04->SetDrawSize(FVector2D(1280.0f, 720.0f));
		m_Stage04->SetRelativeLocationAndRotation(FVector(0.0f, -640.0f, 0.0f), FRotator(0.0f, -90.0f, 0.0f));

	}

	m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("m_pSpringArm"));
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

		m_pSpringArm->TargetArmLength = 1280.0f;
	}

	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));
	if (m_pCamera != NULL) {
		// カメラをスプリングアームにアタッチさせる
		m_pCamera->SetupAttachment(m_pSpringArm, USpringArmComponent::SocketName);
	}
}

// Called when the game starts or when spawned
void AStageSelect::BeginPlay()
{
	Super::BeginPlay();
	
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
}

// Called to bind functionality to input
void AStageSelect::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("SelectRight", IE_Pressed, this, &AStageSelect::SelectRight);
	InputComponent->BindAction("SelectLeft", IE_Pressed, this, &AStageSelect::SelectLeft);
	InputComponent->BindAction("Enter", IE_Pressed, this, &AStageSelect::SelectConfirm);
}

void AStageSelect::SelectRight()
{
	if (anyPressed) {
		UE_LOG(LogTemp, Warning, TEXT("Right!!"));
		turnRight = true;
		anyPressed = false;
		stageNum += 1;
		if (stageNum >= 5) stageNum = 1;
		pressCount += 1;
	}
	/*if (canEnter) {
		stageNum += 1;
		if (stageNum >= 5) stageNum = 1;
		pressCount += 1;
	}*/
}

void AStageSelect::SelectLeft()
{
	if (anyPressed) {
		UE_LOG(LogTemp, Warning, TEXT("Right!!"));
		turnRight = false;
		anyPressed = false;
		stageNum -= 1;
		if (stageNum <= 0) stageNum = 4;
		pressCount += 1;
	}
	/*if (canEnter) {
		stageNum -= 1;
		if (stageNum <= 0) stageNum = 4;
		pressCount += 1;
	}*/
	
}

void AStageSelect::SelectConfirm()
{
	UGameplayStatics::OpenLevel(this, "Game");

}

void AStageSelect::StageChanging(float _deltaTime)
{
	FRotator presRotation = m_SceneComponent->GetRelativeRotation();
	FVector presScale = m_SceneComponent->GetRelativeScale3D();

	UE_LOG(LogTemp, Warning, TEXT("TurnRight:%s"), turnRight ? TEXT("TRUE") : TEXT("FALSE"));
	UE_LOG(LogTemp, Warning, TEXT("preScale:%s"), *FString::SanitizeFloat(presScale.SizeSquared()));
	UE_LOG(LogTemp, Warning, TEXT("finishSmall:%s"), finishSmall ? TEXT("TRUE") : TEXT("FALSE"));
	UE_LOG(LogTemp, Warning, TEXT("finishRotating:%s"), finishRotating ? TEXT("TRUE") : TEXT("FALSE"));
	UE_LOG(LogTemp, Warning, TEXT("presRotation:%s"), *presRotation.ToString());
	UE_LOG(LogTemp, Warning, TEXT("rotatingRate:%s"), *FString::SanitizeFloat(rotatingRate));

	if (turnRight) {
		if (!finishSmall && presScale.SizeSquared() >= scalingRate) {
			m_SceneComponent->SetRelativeScale3D(
				FVector(presScale.X -= scalingTime * _deltaTime,
				presScale.Y -= scalingTime * _deltaTime,
				presScale.Z -= scalingTime * _deltaTime));
		}
		else if (!finishSmall) {
			finishSmall = true;
		}

		if (finishSmall && !finishRotating && rotatingRate <= rotatingRateMax * FMath::Abs(pressCount)) {
			startRotating = true;
			rotatingRate += _deltaTime;
			m_SceneComponent->SetRelativeRotation(
				FRotator(presRotation.Pitch, presRotation.Yaw += rotatingTime * _deltaTime, presRotation.Roll));
		}
		else if (finishSmall && rotatingRate > rotatingRateMax) {
			finishRotating = true;
			startRotating = false;
			rotatingRate = 0.0f;
			pressCount = 0.0f;
			canEnter = false;
		}

		if (finishRotating) {
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
		}
	}
	if (!turnRight) {
		if (!finishSmall && presScale.SizeSquared() >= scalingRate) {
			m_SceneComponent->SetRelativeScale3D(
				FVector(presScale.X -= scalingTime * _deltaTime,
				presScale.Y -= scalingTime * _deltaTime,
				presScale.Z -= scalingTime * _deltaTime));
		}
		else if (!finishSmall) {
			finishSmall = true;
		}

		if (finishSmall && !finishRotating && rotatingRate <= rotatingRateMax * (float) pressCount) {
			startRotating = true;
			rotatingRate += _deltaTime;
			m_SceneComponent->SetRelativeRotation(
				FRotator(presRotation.Pitch, presRotation.Yaw -= rotatingTime * _deltaTime, presRotation.Roll));
		}
		else if (finishSmall && rotatingRate > rotatingRateMax) {
			finishRotating = true;
			startRotating = false;
			rotatingRate = 0.0f;
			pressCount = 0.0f;
			canEnter = false;
		}

		if (finishRotating) {
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
		}
	}
}

// 現在選択されたステージ
void AStageSelect::CurrentStage()
{

}
