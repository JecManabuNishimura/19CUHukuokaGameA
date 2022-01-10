// Fill out your copyright notice in the Description page of Project Settings.


#include "NewPlayer.h"
#include "SensorManager.h"
#include "Camera/CameraComponent.h"
#include "..\..\UE4Duino\Source\UE4Duino\Public\Serial.h"

// Sets default values
ANewPlayer::ANewPlayer()
	: m_IsSensor(false)
	, m_IsSharpcurve(false)
	, m_CurrentForwardAcceleration(0.0f)
	, m_SideValue(0.0f)
	, m_CurrentSideAcceleration(0.0f)
	, m_CurrentSharpcurvePower(0.0f)
	, m_UpdateValue(FVector::ZeroVector)
	, m_ProjectileMovement(nullptr)
	, m_BoardCapsuleCollision(nullptr)
	, m_BoardMesh(nullptr)
	, m_PlayerMesh(nullptr)
	, m_SpringArm(nullptr)
	, m_PlayerCamera(nullptr)
	, m_BoxCollision(nullptr)
	, m_RootRotationY(90.0f)
	, m_CanMove(false)
	, m_ForwardMaxSpeed(5.0f)
	, m_ForwardAcceleration(0.0125f)
	, m_SideMaxSpeed(5.0f)
	, m_SideAcceleration(0.1f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	m_ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	m_BoardCapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BoardCapsuleCollision"));
	RootComponent = m_BoardCapsuleCollision;
	m_BoardCapsuleCollision->SetSimulatePhysics(true);
	m_BoardCapsuleCollision->SetEnableGravity(true);
	m_BoardCapsuleCollision->SetUseCCD(true);
	m_BoardCapsuleCollision->SetCollisionProfileName(TEXT("PhysicsActor"));
	m_BoardCapsuleCollision->SetCapsuleHalfHeight(90.0f);
	m_BoardCapsuleCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	m_BoardCapsuleCollision->SetRelativeRotation(FRotator(m_RootRotationY, 0.0f, 0.0f));

	m_BoardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoardMesh"));
	m_BoardMesh->SetupAttachment(m_BoardCapsuleCollision);
	m_BoardMesh->SetCollisionProfileName("NoCollision");
	m_BoardMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	m_BoardMesh->SetRelativeRotation(FRotator(-m_RootRotationY, 0.0f, 0.0f));

	m_PlayerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	m_PlayerMesh->SetupAttachment(m_BoardCapsuleCollision);
	m_PlayerMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	m_PlayerMesh->SetRelativeRotation(FRotator(-m_RootRotationY, 0.0f, 0.0f));

	m_SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));

	m_PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	m_PlayerCamera->SetupAttachment(m_BoardCapsuleCollision);
	m_PlayerCamera->SetRelativeLocation(FVector(300.0f, 0.0f, 250.0f));
	m_PlayerCamera->SetRelativeRotation(FRotator(-m_RootRotationY - 30.0f, 0.0f, 0.0f));

	m_BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	m_BoxCollision->SetupAttachment(m_BoardCapsuleCollision);
	m_BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ANewPlayer::OnOverlapBegin);
	m_BoxCollision->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	m_BoxCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	m_BoxCollision->SetRelativeRotation(FRotator(-m_RootRotationY, 0.0f, 0.0f));
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
		// ������������̎擾
		float accelDir = (_axisValue > 0.0f) ? 1.0f : -1.0f;

		// �h���t�g���ǂ���
		if (m_IsSharpcurve)
		{
			// ��������
		}
		else
		{
			// ��������
			m_CurrentSideAcceleration += m_SideAcceleration * accelDir;

			// �ő呬�x�ɃN�����v����
			m_CurrentSideAcceleration = FMath::Clamp(m_CurrentSideAcceleration, -m_SideMaxSpeed, m_SideMaxSpeed);
		}

		m_SideValue = FMath::Clamp(_axisValue, -1.0f, 1.0f) * m_CurrentSideAcceleration;
	}
}

// �ړ�����
void ANewPlayer::UpdateMove()
{
	// ���E�ړ��̗ʂɉ����ĉ�]
	AddActorLocalRotation(FRotator(0.0f, m_SideValue, 0.0f));
	m_SideValue = 0.0f;

	// �O�i������������
	m_CurrentForwardAcceleration += m_ForwardAcceleration;
	m_CurrentForwardAcceleration = FMath::Clamp(m_CurrentForwardAcceleration, 0.0f, 1.0f);

	// ���ۂɈړ��ʂ𔽉f������
	float speed = m_ForwardMaxSpeed * m_CurrentForwardAcceleration;

	// �O�����x�N�g���̎擾
	FVector forwardVec = GetActorForwardVector();

	FVector meshVec = m_BoardMesh->GetForwardVector();

	UE_LOG(LogTemp, Warning, TEXT("Forward Vec = %s"), *forwardVec.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Board Vec = %s"), *meshVec.ToString());

	// �O�i�����ʂɉ����č��E�ړ��̃x�N�g���𐧌�
	forwardVec.X *= m_CurrentForwardAcceleration;

	// Z���̃x�N�g�������]���Ă���̂Œl�����]�����đ��x����Z
	forwardVec.Z *= -speed;

	AddActorLocalOffset(forwardVec);
}

// �������Z�b�g
void ANewPlayer::ResetAcceleration()
{
	m_CurrentForwardAcceleration = 0.0f;
}

// �}�J�[�u�i�h���t�g�j��Ԃɂ���
void ANewPlayer::SetDrift(bool _status)
{
	m_IsSharpcurve = _status;
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

	// �}�J�[�u�i�h���t�g�j��Ԃ�ݒ�
	DECLARE_DELEGATE_OneParam(FCustomInputDelegate, const bool);

	// �h���t�gON
	InputComponent->BindAction<FCustomInputDelegate>("Drift", IE_Pressed, this, &ANewPlayer::SetDrift, true);

	// �h���t�gOFF
	InputComponent->BindAction<FCustomInputDelegate>("Drift", IE_Released, this, &ANewPlayer::SetDrift, false);

	// �f�o�b�O�p--�����x���Z�b�g
	InputComponent->BindAction("Debug_Stop", IE_Pressed, this, &ANewPlayer::ResetAcceleration);
}

void ANewPlayer::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}