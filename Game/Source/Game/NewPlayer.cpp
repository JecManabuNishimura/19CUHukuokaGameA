// Fill out your copyright notice in the Description page of Project Settings.


#include "NewPlayer.h"
#include "SensorManager.h"
#include "Camera/CameraComponent.h"
#include "..\..\UE4Duino\Source\UE4Duino\Public\Serial.h"

// Sets default values
ANewPlayer::ANewPlayer()
	: m_IsSensor(false)
	, m_CurrentForwardAcceleration(0.0f)
	, m_CurrentSideAcceleration(0.0f)
	, m_UpdateValue(FVector::ZeroVector)
	, m_CanMove(false)
	, m_ForwardMaxSpeed(15.0f)
	, m_ForwardAcceleration(0.0125f)
	, m_SideMaxSpeed(15.0f)
	, m_SideAcceleration(0.05f)
	, m_BrakePower(0.075f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
}

// 移動値入力処理（コントローラー）
void ANewPlayer::InputSensor()
{

}

// 移動値入力処理（キーボード・パッド）
void ANewPlayer::InputKeyboard(float _axisValue)
{
	// コントローラー未使用時のみ
	if (!m_IsSensor)
	{
		// 入力中 or ブレーキ中
		static bool isStartInput = false;

		// 実際の移動値
		static float value = 0.0f;

		// 入力あり
		if (_axisValue != 0.0f)
		{
			// 入力あり状態に
			isStartInput = true;

			// 加速する方向の取得
			float accelDir = (_axisValue > 0.0f) ? 1.0f : -1.0f;

			// 加速する
			m_CurrentSideAcceleration += m_SideAcceleration * accelDir;
			m_CurrentSideAcceleration = FMath::Clamp(m_CurrentSideAcceleration, -1.0f, 1.0f);

			value = _axisValue * m_CurrentSideAcceleration * accelDir;
		}
		// 入力なし
		else if (isStartInput)
		{
			float mag = 1.0f - m_BrakePower;
			value *= mag;
			m_CurrentSideAcceleration *= mag;

			// 停止
			if (FMath::IsNearlyEqual(value, 0.0f, (m_BrakePower / 5.0f)))
			{
				isStartInput = false;
				m_CurrentSideAcceleration = 0.0f;
				value = 0.0f;
			}
		}

		m_UpdateValue.Y = FMath::Clamp(value, -1.0f, 1.0f) * m_SideMaxSpeed;
	}
}

// 移動処理
void ANewPlayer::UpdateMove()
{
	// 前進を加速させる
	m_CurrentForwardAcceleration += m_ForwardAcceleration;
	m_CurrentForwardAcceleration = FMath::Clamp(m_CurrentForwardAcceleration, 0.0f, 1.0f);

	// 実際に移動量を反映させる
	m_UpdateValue.X = m_ForwardMaxSpeed * m_CurrentForwardAcceleration;
	SetActorLocation(GetActorLocation() + m_UpdateValue);
}

// 加速リセット
void ANewPlayer::ResetAcceleration()
{
	m_CurrentForwardAcceleration = 0.0f;
}

// Called when the game starts or when spawned
void ANewPlayer::BeginPlay()
{
	Super::BeginPlay();

	m_IsSensor = USensorManager::ConnectToSensor();
}

// Called every frame
void ANewPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 移動可能状態
	if (m_CanMove)
	{
		// コントローラー使用の有無
		if (m_IsSensor)
		{
			InputSensor();
		}

		UpdateMove();
	}
}

//	終了時
void ANewPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// コントローラーから切断
	USensorManager::DisconnectToSensor();
}

// Called to bind functionality to input
void ANewPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 移動
	InputComponent->BindAxis("MoveRight", this, &ANewPlayer::InputKeyboard);

	// デバッグ用--加速度リセット
	InputComponent->BindAction("Debug_Stop", IE_Pressed, this, &ANewPlayer::ResetAcceleration);
}