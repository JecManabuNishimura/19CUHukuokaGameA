//----------------------------------------------------------
// ファイル名		：PlayerChara.cpp
// 概要				：プレイヤーキャラを制御するCharacterオブジェクト
// 作成者			：19CU0220 曹　飛
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
	, m_moveSpeed(40.f)
	, m_gravity(600.f)
	, isRotate(false)
	, m_jumpPower(1200.f)
	, m_jumpTime(0.f)
	, m_nowJumpHeight(0.f)
	, m_prevJumpHeight(0.f)
	, m_bJumping(false)
	, m_speedUpTime(0.f)
	, m_bSpeedUp(false)
	, m_bCanControl(true)
	, sen_Rotator(FRotator::ZeroRotator)
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

	//	ルートオブジェクトを中心に、スプリングアームについているカメラを回転させる
	USpringArmComponent* pSpringArm = m_pSpringArm;
	if (pSpringArm != NULL)
	{
		pSpringArm->SetRelativeLocation(FVector(pSpringArm->GetRelativeLocation().X, pSpringArm->GetRelativeLocation().Y, 150.f));
	}
}

// 毎フレームの更新処理
void APlayerChara::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//	移動処理
	UpdateMove(DeltaTime);

	//	ジャンプ処理
	UpdateJump(DeltaTime);

	UpdateSpeedUp(DeltaTime);

}

// 各入力関係メソッドとのバインド処理
void APlayerChara::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//	移動
	InputComponent->BindAxis("MoveRight", this, &APlayerChara::Chara_MoveRight);
	InputComponent->BindAxis("RotateRight", this, &APlayerChara::Chara_RotateRight);
	
	//	ジャンル
	InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerChara::JumpStart);

	InputComponent->BindAction("SpeedUp", IE_Pressed, this, &APlayerChara::SpeedUpStart);
}

// ===
void APlayerChara::PassRotatorNum(FRotator rot1)
{
	sen_Rotator = rot1;
}


//	移動処理
void APlayerChara::UpdateMove(float _deltaTime)
{
	//	前に向くずっと移動する
	FVector NewLocation = GetActorLocation();
	NewLocation.X += 25.f;;

	FRotator NewRotation = sen_Rotator;
	//UE_LOG(LogLoad, Log, TEXT("NewRotation.Yaw:%f"), NewRotation.Yaw);

	NewRotation.Roll += NewRotation.Roll;

	NewLocation.Y += NewRotation.Roll * m_moveSpeed;

	//	新しい角度を反映
	SetActorRotation(NewRotation);

	SetActorLocation(NewLocation);
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
	}
}

void APlayerChara::UpdateSpeedUp(float _deltaTime)
{
	//	
	if (m_bSpeedUp)
	{
		//	
		m_speedUpTime += _deltaTime;

		//	
		if (m_speedUpTime >= 1.0f)
		{
			m_bSpeedUp = false;
			m_speedUpTime = 0.0f;
		}
		//	
		else
		{
			//	
			FVector NewLocation = GetActorLocation();
			NewLocation.X += 50.f;
			SetActorLocation(NewLocation);
		}
	}
}

//	【入力バインド】キャラ移動:左右
void APlayerChara::Chara_MoveRight(float _axisValue)
{
	//	コントロール可能の場合のみ
	if (m_bCanControl == false) { return; }

	m_charaMoveInput.Y = FMath::Clamp(_axisValue, -1.0f, 1.0f) * 0.5f;
}

void APlayerChara::Chara_RotateRight(float _axisValue)
{
	//	コントロール可能の場合のみ
	if (m_bCanControl == false) { return; }

	m_charaMoveInput.X = FMath::Clamp(_axisValue, -1.0f, 1.0f) * 0.5f;
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

void APlayerChara::SpeedUpStart()
{
	//	コントロール可能の場合のみ
	if (m_bCanControl == false) { return; }

	//	ジャンプ中ではない場合
	if (m_bSpeedUp == false)
	{
		//	ジャンプ中状態にする
		m_bSpeedUp = true;
	}
}

