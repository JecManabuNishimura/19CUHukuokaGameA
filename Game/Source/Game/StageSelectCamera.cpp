//----------------------------------------------------------
// ファイル名		：PlayerCharaInStage.cpp
// 概要				：StageSelectでプレイヤーの制御
// 作成者			：19CU0222 鍾家同
// 更新内容			：2020/11/23 作成　プレイヤーのカメラの動き
//----------------------------------------------------------


#include "StageSelectCamera.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AStageSelectCamera::AStageSelectCamera() :
	m_pSpringArm(NULL),
	m_pCamera(NULL)
{
	PrimaryActorTick.bCanEverTick = true;

	// スプリングアームのオブジェクトを生成
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
	}

	// カメラのオブジェクトを生成
	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));
	if (m_pCamera != NULL) {
		// カメラをスプリングアームにアタッチさせる
		m_pCamera->SetupAttachment(m_pSpringArm, USpringArmComponent::SocketName);
	}
}

// Called when the game starts or when spawned
void AStageSelectCamera::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AStageSelectCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AStageSelectCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("CameraUp", this, &AStageSelectCamera::CameraVertical);
	InputComponent->BindAxis("CameraRight", this, &AStageSelectCamera::CameraHorizontal);
}

// =======================【入力バインド】=========================
// 入力バインド】カメラ移動:左右
void AStageSelectCamera::CameraHorizontal(float _axisValue)
{
	cameraRoll = _axisValue;
}
// 入力バインド】カメラ移動:前後
void AStageSelectCamera::CameraVertical(float _axisValue)
{
	cameraPitch = _axisValue;
}
// =================================================================