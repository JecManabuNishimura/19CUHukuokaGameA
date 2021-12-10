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

// �ړ��l���͏����i�R���g���[���[�j
void ANewPlayer::InputSensor()
{

}

// �ړ��l���͏����i�L�[�{�[�h�E�p�b�h�j
void ANewPlayer::InputKeyboard(float _axisValue)
{
	// �R���g���[���[���g�p���̂�
	if (!m_IsSensor)
	{
		// ���͒� or �u���[�L��
		static bool isStartInput = false;

		// ���ۂ̈ړ��l
		static float value = 0.0f;

		// ���͂���
		if (_axisValue != 0.0f)
		{
			// ���͂����Ԃ�
			isStartInput = true;

			// ������������̎擾
			float accelDir = (_axisValue > 0.0f) ? 1.0f : -1.0f;

			// ��������
			m_CurrentSideAcceleration += m_SideAcceleration * accelDir;
			m_CurrentSideAcceleration = FMath::Clamp(m_CurrentSideAcceleration, -1.0f, 1.0f);

			value = _axisValue * m_CurrentSideAcceleration * accelDir;
		}
		// ���͂Ȃ�
		else if (isStartInput)
		{
			float mag = 1.0f - m_BrakePower;
			value *= mag;
			m_CurrentSideAcceleration *= mag;

			// ��~
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

// �ړ�����
void ANewPlayer::UpdateMove()
{
	// �O�i������������
	m_CurrentForwardAcceleration += m_ForwardAcceleration;
	m_CurrentForwardAcceleration = FMath::Clamp(m_CurrentForwardAcceleration, 0.0f, 1.0f);

	// ���ۂɈړ��ʂ𔽉f������
	m_UpdateValue.X = m_ForwardMaxSpeed * m_CurrentForwardAcceleration;
	SetActorLocation(GetActorLocation() + m_UpdateValue);
}

// �������Z�b�g
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

	// �ړ��\���
	if (m_CanMove)
	{
		// �R���g���[���[�g�p�̗L��
		if (m_IsSensor)
		{
			InputSensor();
		}

		UpdateMove();
	}
}

//	�I����
void ANewPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// �R���g���[���[����ؒf
	USensorManager::DisconnectToSensor();
}

// Called to bind functionality to input
void ANewPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// �ړ�
	InputComponent->BindAxis("MoveRight", this, &ANewPlayer::InputKeyboard);

	// �f�o�b�O�p--�����x���Z�b�g
	InputComponent->BindAction("Debug_Stop", IE_Pressed, this, &ANewPlayer::ResetAcceleration);
}