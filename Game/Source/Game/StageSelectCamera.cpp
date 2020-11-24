//----------------------------------------------------------
// ファイル名		：PlayerCharaInStage.cpp
// 概要				：StageSelectでプレイヤーの制御
// 作成者			：19CU0222 鍾家同
// 更新内容			：2020/11/23 作成　プレイヤーのカメラの動き
//----------------------------------------------------------


#include "StageSelectCamera.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AStageSelectCamera::AStageSelectCamera() :
	m_pSpringArm(NULL),
	m_pCamera(NULL),
	m_cameraPitchLimit(FVector2D(-50.0f, 40.0f)),
	m_cameraYawLimit(FVector2D(-150.0f, -20.0f)),
	length(5000.0f),
	isLookAtStageSelect(false),
	SelectedActor(NULL),
	canPlayAction(false),
	isPlayingAction(false),
	isHit(false)
{
	PrimaryActorTick.bCanEverTick = true;

	// スプリングアームのオブジェクトを生成
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
	}*/

	// カメラのオブジェクトを生成
	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));
	if (m_pCamera != NULL) {
		// カメラをスプリングアームにアタッチさせる
		m_pCamera->SetupAttachment(RootComponent, USpringArmComponent::SocketName);
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

	// カメラ更新処理
	CameraMovement(DeltaTime);

	if (!isPlayingAction) ObjectSelect();
	else ObjectEnlarge(DeltaTime);
}

// 【入力バインド】
void AStageSelectCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("CameraUp", this, &AStageSelectCamera::CameraVertical);
	InputComponent->BindAxis("CameraRight", this, &AStageSelectCamera::CameraHorizontal);
}

// カメラ更新処理
void AStageSelectCamera::CameraMovement(float _deltaTime)
{
	// 現在のFPSを測定
	float fps = 1.0f / _deltaTime;

	// 処理落ちしても、一定速度でカメラが回るように補正
	float rotateCorrection = 60.0f / fps;

	// カメラの新しい角度を求める
	// 現在の角度を取得
	FRotator NewRotation = GetActorRotation();

	// Yawに関しては、左右の制限角度の範囲内で切る
	NewRotation.Yaw = FMath::Clamp(NewRotation.Yaw + (m_cameraRotateInput.X * rotateCorrection), m_cameraYawLimit.X, m_cameraYawLimit.Y);

	// Pitchに関しては、上下の制限角度の範囲内で切る
	NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + (m_cameraRotateInput.Y * rotateCorrection), m_cameraPitchLimit.X, m_cameraPitchLimit.Y);

	// 新しい角度を反映
	SetActorRotation(NewRotation);
}

void AStageSelectCamera::ObjectSelect()
{
	FVector Start = GetActorLocation();
	FVector End = GetActorLocation() + GetActorForwardVector() * length;
	FHitResult OutHit;
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f);
	//bool isHit;
	//FCollisionQueryParams CollisionParams;
	//CollisionParams.AddIgnoredActor(this);
	if (!isPlayingAction)
		isHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_WorldStatic/*, CollisionParams*/);
	else isHit = false;
	if (isHit) {
		UE_LOG(LogTemp, Warning, TEXT("%s"), *OutHit.GetActor()->GetName());
		if (OutHit.GetActor()->ActorHasTag("StageSelect")) {
			SelectedActor = OutHit.GetActor();
			isLookAtStageSelect = true;
			//canPlayAction = true;
			isPlayingAction = true;
		}
		else isLookAtStageSelect = false;
	}
}

// 選択されたアクターのリアクション
void AStageSelectCamera::ObjectEnlarge(float _deltaTime)
{
	/*if (SelectedActor->ActorHasTag("StageSelect")) FinalLocation = FVector(-1080.0f, -260.0f, 1792.0f);

	if (SelectedActor != NULL) {
		FVector currentLocation = SelectedActor->GetActorLocation();
	}*/
}

// =======================【入力バインド】=========================
// 入力バインド】カメラ移動:左右(Pitch)
void AStageSelectCamera::CameraHorizontal(float _axisValue)
{
	m_cameraRotateInput.X = _axisValue;
}
// 入力バインド】カメラ移動:前後(Yaw)
void AStageSelectCamera::CameraVertical(float _axisValue)
{
	m_cameraRotateInput.Y = _axisValue;
}
// =================================================================