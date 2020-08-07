//----------------------------------------------------------
// ファイル名		：PlayerChara.cpp
// 概要				：プレイヤーキャラを制御するCharacterオブジェクト
// 作成者			：19CU0220 曹　飛
// 更新内容			：2020/08/07 作成
//----------------------------------------------------------

// インクルード
#include "PlayerChara.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"

//	defineマクロ
#define JUMP_HEIGHT (m_jumpTime * m_jumpTime * (-m_gravity) / 2) + (m_jumpTime * m_jumpPower)

// コンストラクタ
APlayerChara::APlayerChara()
	: m_pSpringArm(NULL)
	, m_pCamera(NULL)
	, m_charaMoveInput(FVector2D::ZeroVector)
	, m_moveSpeed(50.f)
	, m_gravity(1200.f)
	, m_jumpPower(900.f)
	, m_jumpTime(0.f)
	, m_nowJumpHeight(0.f)
	, m_prevJumpHeight(0.f)
	, m_bJumping(false)
	, m_bCanControl(true)
{
	// 毎フレーム、このクラスのTick()を呼ぶかどうかを決めるフラグ。必要に応じて、パフォーマンス向上のために切ることもできる。
	PrimaryActorTick.bCanEverTick = true;

	//	デフォルトプレイヤーとして設定
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	//	スプリングアームのオブジェクトを生成
	m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("m_pSpringArm"));
	if (m_pSpringArm != NULL)
	{
		m_pSpringArm->SetupAttachment(RootComponent);

		//	アームの長さを設定
		//	カメラの子リジョンテストを行うかを設定
		m_pSpringArm->bDoCollisionTest = false;
		//	カメラ追従ラグを使うかを設定
		m_pSpringArm->bEnableCameraLag = true;
		//	カメラ追従ラグの速度を設定
		m_pSpringArm->CameraLagSpeed = 20.f;
	}

	//	カメラのオブジェクトを生成
	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));
	if (m_pCamera != NULL)
	{
		//	カメラをスプリングアームにアタッチさせる
		m_pCamera->SetupAttachment(m_pSpringArm, USpringArmComponent::SocketName);
	}
}

// ゲームスタート時、または生成時に呼ばれる処理
void APlayerChara::BeginPlay()
{
	Super::BeginPlay();

	//	PlayerCharaが持っているメッシュコンポーネントの相対位置を変更
	USkeletalMeshComponent* pMeshComp = GetMesh();
	if (pMeshComp != NULL)
	{
		//	Z座標を下げる
		pMeshComp->SetRelativeLocation(FVector(0.f, 0.f, -85.f));
	}

	UCharacterMovementComponent* pCharMoveComp = GetCharacterMovement();
	if (pCharMoveComp != NULL)
	{
		//	ジャンプ時にも水平方向への移動が聞くように（0～1の間に設定することで移動する具合を調整）
		pCharMoveComp->AirControl = 0.8f;
	}
}

// 毎フレームの更新処理
void APlayerChara::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//	カメラ更新処理
	UpdateCamera(DeltaTime);

	//	移動処理
	UpdateMove(DeltaTime);

	//	ジャンプ処理
	UpdateJump(DeltaTime);
}

// 各入力関係メソッドとのバインド処理
void APlayerChara::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//	移動
	InputComponent->BindAxis("MoveRight", this, &APlayerChara::Chara_MoveRight);

	//	ジャンル
	InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerChara::JumpStart);
}

//	カメラ更新処理
void APlayerChara::UpdateCamera(float _deltaTime)
{
	//	ルートオブジェクトを中心に、スプリングアームについているカメラを回転させる
	USpringArmComponent* pSpringArm = m_pSpringArm;
	if (pSpringArm != NULL)
	{
		//	現在のFPSを測定
		float fps = 1.0f / _deltaTime;

		//	処理落ちしても、一定速度でカメラが回るように補正
		float rotateCorrection = 60.f / fps;

		//	カメラの新しい角度を求める
		//	現在の角度を取得
		FRotator NewRotation = pSpringArm->GetRelativeRotation();

		//	Yawは入力した分回す
		NewRotation.Yaw += m_cameraRotateInput.X * rotateCorrection;

		//	Pitchに関しては、上下の制限角度の範囲内で切る
		NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + (m_cameraRotateInput.Y * rotateCorrection), m_cameraPitchLimit.X, m_cameraPitchLimit.Y);

		//	新しい角度を反映
		pSpringArm->SetRelativeRotation(NewRotation);
	}
}

//	移動処理
void APlayerChara::UpdateMove(float _deltaTime)
{
	//	前に向くずっと移動する
	FVector NewLocation = GetActorLocation();
	NewLocation.X += 20.f;
	SetActorLocation(NewLocation);

	//	移動入力がある場合
	if (!m_charaMoveInput.IsZero())
	{
		//	コントロール可能の場合のみ
		if (m_bCanControl == false) { return; }

		//	入力に合わせて、Actorを左右前後に移動
		USpringArmComponent* pSpringArm = m_pSpringArm;
		if (pSpringArm != NULL)
		{
			//	キャラクターの移動
			{
				//	SpringArmが向く方向に、入力による移動量をPawnMovementComponentに渡す
				NewLocation.Y += (m_charaMoveInput.Y * m_moveSpeed);
				SetActorLocation(NewLocation);
			}
		}
	}
}

//	ジャンプ処理
void APlayerChara::UpdateJump(float _deltaTime)
{
	//	ジャンプ中フラグを確認してから
	if (m_bJumping)
	{
		//	ジャンプ量を計算
		m_nowJumpHeight = JUMP_HEIGHT;

		//	ジャンプ時間を増加
		m_jumpTime += _deltaTime;

		//	Actorの現在の座標を取得
		FVector nowPos = GetActorLocation();

		//	着地時（=ジャンプ量がマイナスに転じた時）、ジャンプ前状態に戻す
		if (m_nowJumpHeight < 0.0f)
		{
			m_bJumping = false;
			m_jumpTime = 0.0f;

			SetActorLocation(FVector(nowPos.X, nowPos.Y, m_posBeforeJump.Z));
		}
		//	それ以外は、ジャンプしているため座標を反映
		else
		{
			//	現在の座標にジャンプ量を足す
			SetActorLocation(FVector(nowPos.X, nowPos.Y, m_posBeforeJump.Z + m_nowJumpHeight), true);
		}

		//	ジャンプ量を保持
		m_prevJumpHeight = m_nowJumpHeight;

		////	デバッグ確認用
		//nowPos = GetActorLocation();
		//UE_LOG(LogTemp, Warning, TEXT("Jump > PosZ %f / Height %f"), nowPos.Z, m_nowJumpHeight);
	}
}

//	【入力バインド】キャラ移動:左右
void APlayerChara::Chara_MoveRight(float _axisValue)
{
	//	コントロール可能の場合のみ
	if (m_bCanControl == false) { return; }

	m_charaMoveInput.Y = FMath::Clamp(_axisValue, -1.0f, 1.0f) * 0.5f;
}

void APlayerChara::JumpStart()
{
	//	コントロール可能の場合のみ
	if (m_bCanControl == false) { return; }

	//	ジャンプ中ではない場合
	if (m_bJumping == false)
	{
		//	ジャンプ中状態にする
		m_bJumping = true;

		//	ジャンル前のActor座標を保持
		m_posBeforeJump = GetActorLocation();
	}
}

