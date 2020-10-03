// インクルード
#include "PlayerCharaNoSensor.h"
#include "Engine.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"

//	defineマクロ
#define JUMP_HEIGHT (m_jumpTime * m_jumpTime * (-m_gravity) / 2) + (m_jumpTime * m_jumpPower)

// コンストラクタ
APlayerCharaNoSensor::APlayerCharaNoSensor()
	: m_pSpringArm(NULL)
	, m_pCamera(NULL)
	, m_charaMoveInput(FVector2D::ZeroVector)
	, m_charaRotateInput(FVector2D::ZeroVector)
	, m_moveSpeed(50.f)
	, m_gravity(600.f)
	, m_jumpPower(1200.f)
	, m_jumpTime(0.f)
	, m_nowJumpHeight(0.f)
	, m_prevJumpHeight(0.f)
	, m_bJumping(false)
	, m_bGuarding(false)
	, m_bCanGuard(true)
	, m_bCanControl(true)
	, m_GuardValue(100.f)
	, m_GuardCostTime(70.f)
	, m_GuardRechargeTime(40.f)
	, m_bDashing(false)
	, m_bCanDash(true)
	, m_DashCostTime(70.f)
	, m_DashRechargeTime(40.f)
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
void APlayerCharaNoSensor::BeginPlay()
{
	Super::BeginPlay();

	//	PlayerCharaNoSensorが持っているメッシュコンポーネントの相対位置を変更
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

	// ViewPort上にGuardUIの表示
	if (PlayerGuardUIClass != nullptr) {
		PlayerGuardUI = CreateWidget(GetWorld(), PlayerGuardUIClass);
		PlayerGuardUI->AddToViewport();
	}
}

// 毎フレームの更新処理
void APlayerCharaNoSensor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//	カメラ更新処理
	UpdateCamera(DeltaTime);

	//	移動処理
	UpdateMove(DeltaTime);

	//	ジャンプ処理
	UpdateJump(DeltaTime);

	//	ガード処理
	UpdateGuard(DeltaTime);
}

// 各入力関係メソッドとのバインド処理
void APlayerCharaNoSensor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//	移動
	InputComponent->BindAxis("MoveRight", this, &APlayerCharaNoSensor::Chara_MoveRight);

	//	ジャンル
	InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharaNoSensor::JumpStart);

	//	ガード
	InputComponent->BindAxis("Guard", this, &APlayerCharaNoSensor::GuardStart);
}

//	カメラ更新処理
void APlayerCharaNoSensor::UpdateCamera(float _deltaTime)
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
void APlayerCharaNoSensor::UpdateMove(float _deltaTime)
{
	FVector NewLocation = GetActorLocation();
	//	前に向くずっと移動する
	if (!m_bGuarding)
	{
		//NewLocation.X += 20.f;		

		// Testing Value(from 19CU0222鍾家同)
		NewLocation.X += 5.f;
	}
	else
	{
		NewLocation.X += 12.f;
	}

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
void APlayerCharaNoSensor::UpdateJump(float _deltaTime)
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

//	ガード処理
void APlayerCharaNoSensor::UpdateGuard(float _deltaTime)
{
	if (m_bCanGuard) {
		if (!m_charaRotateInput.IsZero() && m_GuardValue > 0.0f)
		{
			m_bGuarding = true;

			FRotator nowRot = GetActorRotation();
			nowRot.Yaw -= 30.f;
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::SanitizeFloat(nowRot.Yaw));
			SetActorRotation(nowRot);

			m_GuardValue -= _deltaTime * m_GuardCostTime;
			if (m_GuardValue <= 0.0f) {
				m_GuardValue = 0.0f;
				m_bCanGuard = false;
			}
			UE_LOG(LogTemp, Warning, TEXT("guarding."));
		}
		else if (m_charaRotateInput.IsZero() && m_GuardValue > 0.0f)
		{
			m_bGuarding = false;
			if (m_GuardValue >= 100.0f) {
				m_GuardValue = 100.0f;
				m_bCanGuard = true;
			}
			else m_GuardValue += _deltaTime * m_GuardRechargeTime;
			UE_LOG(LogTemp, Warning, TEXT("can guard."));
		}
	}
	else {
		m_bCanGuard = false;
		m_bGuarding = false;
		if (m_GuardValue >= 100.0f) {
			m_GuardValue = 100.0f;
			m_bCanGuard = true;
		}
		else m_GuardValue += _deltaTime * m_GuardRechargeTime;
		UE_LOG(LogTemp, Warning, TEXT("guard is charging."));
	}
}

//	【入力バインド】キャラ移動:左右
void APlayerCharaNoSensor::Chara_MoveRight(float _axisValue)
{
	//	コントロール可能の場合のみ
	if (m_bCanControl == false) { return; }

	m_charaMoveInput.Y = FMath::Clamp(_axisValue, -1.0f, 1.0f) * 0.5f;
}

void APlayerCharaNoSensor::JumpStart()
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

void APlayerCharaNoSensor::GuardStart(float _axisValue)
{
	//	コントロール可能の場合のみ
	if (m_bCanControl == false) { return; }

	m_charaRotateInput.Y = _axisValue;
}

