// Fill out your copyright notice in the Description page of Project Settings.

#include "NewPlayer.h"
#include "SensorManager.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
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
	, m_BaseForwardVector(FVector::ZeroVector)
	, m_FloatingPawnMovementComponent(nullptr)
	/*
	, m_BoardCapsuleCollision(nullptr)
	, m_BoardBoxCollision(nullptr)
	*/
	, m_BoardMesh(nullptr)
	, m_PlayerMesh(nullptr)
	, m_SpringArm(nullptr)
	, m_PlayerCamera(nullptr)
	, m_BoxCollision(nullptr)
	, m_SpringArmLength(350.0f)
	, m_ArmLengthAdjust(100.0f)
	, m_CanMove(false)
	, m_SideMaxSpeed(1.5f)
	, m_SideAcceleration(0.15f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	/*
	m_BoardCapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BoardCapsuleCollision"));
	if (m_BoardCapsuleCollision)
	{
		RootComponent = m_BoardCapsuleCollision;
		m_BoardCapsuleCollision->OnComponentHit.AddDynamic(this, &ANewPlayer::OnHit);
		m_BoardCapsuleCollision->SetSimulatePhysics(true);
		m_BoardCapsuleCollision->SetEnableGravity(true);
		m_BoardCapsuleCollision->SetUseCCD(true);
		m_BoardCapsuleCollision->SetCollisionProfileName(TEXT("PhysicsActor"));
		m_BoardCapsuleCollision->SetCapsuleHalfHeight(40.0f);
		m_BoardCapsuleCollision->SetCenterOfMass(FVector(0.0f, 0.0f, -5.0f));
		m_BoardCapsuleCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_BoardCapsuleCollision->SetRelativeRotation(FRotator(m_RootRotationY, 0.0f, 0.0f));
	}

	m_BoardBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoardBoxCollision"));
	if (m_BoardBoxCollision)
	{
		m_BoardBoxCollision->SetupAttachment(RootComponent);
		m_BoardBoxCollision->SetUseCCD(true);
		m_BoardBoxCollision->SetBoxExtent(FVector(m_BoardCapsuleCollision->GetScaledCapsuleRadius(), m_BoardCapsuleCollision->GetScaledCapsuleRadius(), m_BoardCapsuleCollision->GetScaledCapsuleHalfHeight() / 2.0f));
		m_BoardBoxCollision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	}
	*/

	m_BoardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoardMesh"));
	if (m_BoardMesh)
	{
		RootComponent = m_BoardMesh;
		m_BoardMesh->SetSimulatePhysics(true);
		m_BoardMesh->SetEnableGravity(true);
		m_BoardMesh->SetUseCCD(true);
		m_BoardMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
		m_BoardMesh->SetCenterOfMass(FVector(0.0f, 0.0f, -5.0f));
		m_BoardMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_BoardMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}

	m_PlayerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	if (m_PlayerMesh)
	{
		m_PlayerMesh->SetupAttachment(RootComponent);
		m_PlayerMesh->SetEnableGravity(false);
		m_PlayerMesh->SetCollisionProfileName("NoCollision");
		m_PlayerMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_PlayerMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}

	m_SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	if (m_SpringArm)
	{
		m_SpringArm->SetupAttachment(m_PlayerMesh);
		m_SpringArm->TargetArmLength = m_SpringArmLength;
		m_SpringArm->bDoCollisionTest = false;
		m_SpringArm->bEnableCameraLag = true;
		m_SpringArm->CameraLagSpeed = 10.0f;
		m_SpringArm->bEnableCameraRotationLag = true;
		m_SpringArm->CameraRotationLagSpeed = 10.0f;
		m_SpringArm->bInheritPitch = false;
		m_SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_SpringArm->SetRelativeRotation(FRotator(315.0f, 0.0f, 0.0f));
	}

	m_PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	if (m_PlayerCamera)
	{
		m_PlayerCamera->SetupAttachment(m_SpringArm, USpringArmComponent::SocketName);
		m_PlayerCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_PlayerCamera->SetRelativeRotation(FRotator(15.0f, 0.0f, 0.0f));
	}

	m_BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	if (m_BoxCollision)
	{
		m_BoxCollision->SetupAttachment(RootComponent);
		m_BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ANewPlayer::OnOverlapBegin);
		m_BoxCollision->bMultiBodyOverlap = true;
		m_BoxCollision->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
		m_BoxCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_BoxCollision->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}

	m_FloatingPawnMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovementComponent"));
	if (m_FloatingPawnMovementComponent)
	{
		m_FloatingPawnMovementComponent->MaxSpeed = 2500.0f;
		m_FloatingPawnMovementComponent->Acceleration = 1000.0f;
		m_FloatingPawnMovementComponent->Deceleration = 8000.0f;
		m_FloatingPawnMovementComponent->TurningBoost = 75.0f;
	}
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
		if (_axisValue != 0.0f)
		{
			// �h���t�g���ǂ���
			if (m_IsSharpcurve)
			{
				// ��������
			}
			else
			{
				// ��������
				m_CurrentSideAcceleration += m_SideAcceleration;

				// �ő呬�x�ɃN�����v����
				m_CurrentSideAcceleration = FMath::Clamp(m_CurrentSideAcceleration, 0.0f, m_SideMaxSpeed);
			}
		}
		else
		{
			m_CurrentSideAcceleration = 0.0f;
		}

		m_SideValue = FMath::Clamp(_axisValue, -1.0f, 1.0f) * m_CurrentSideAcceleration;
	}
}

// �ړ�����
void ANewPlayer::UpdateMove()
{
	// ���E�ړ��̗ʂɉ����ĉ�]
	AddActorLocalRotation(FRotator(0.0f, m_SideValue, 0.0f), true);

	// �{�[�h�̑O�����x�N�g���̎擾
	FVector meshForwardVec = m_BoardMesh->GetForwardVector();

	// �{�[�h�̑O�����x�N�g���ɉ����x��ǉ�
	m_FloatingPawnMovementComponent->AddInputVector(meshForwardVec);

	UE_LOG(LogTemp, Verbose, TEXT("[NewPlayer] Velocity = %s"), *m_FloatingPawnMovementComponent->Velocity.ToString());
	UE_LOG(LogTemp, Verbose, TEXT("[NewPlayer] Velocity Size = %f"), m_FloatingPawnMovementComponent->Velocity.Size());

	// �X�v�����O�A�[���̋�������
	float addLength = m_FloatingPawnMovementComponent->Velocity.X / m_ArmLengthAdjust;
	m_SpringArm->TargetArmLength = m_SpringArmLength + addLength;

	// �J�����̊p�x����
	FRotator cameraRot = m_PlayerCamera->GetRelativeRotation();
	float addPitch = FMath::Lerp(0.0f, 20.0f, m_FloatingPawnMovementComponent->Velocity.Size() / m_FloatingPawnMovementComponent->MaxSpeed);
	cameraRot.Pitch = 15.0f + addPitch;
	m_PlayerCamera->SetRelativeRotation(cameraRot);
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
	m_BaseForwardVector = m_BoardMesh->GetForwardVector();
	FRotator rot = UKismetMathLibrary::MakeRotFromX(m_BoardMesh->GetForwardVector());
	UE_LOG(LogTemp, Warning, TEXT("Rotation = %s"), *rot.ToString());
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
	UE_LOG(LogTemp, Verbose, TEXT("[NewPlayer] (%s) Overlap Actor Name = %s"), *OverlappedComponent->GetName(), *OtherActor->GetName());

	for (int i = 0; i < OtherActor->Tags.Num(); ++i)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[NewPlayer] Overlap Actor Tag[%d] = %s"), i, *OtherActor->Tags[i].ToString());
	}


	for (int i = 0; i < OtherComp->ComponentTags.Num(); ++i)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[NewPlayer] Overlap Component Tag[%d] = %s"), i, *OtherComp->ComponentTags[i].ToString());
	}
}

void ANewPlayer::OnComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Verbose, TEXT("[NewPlayer] (%s) Overlap End Actor Name = %s"), *OverlappedComponent->GetName(), *OtherActor->GetName());

	for (int i = 0; i < OtherActor->Tags.Num(); ++i)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[NewPlayer] Overlap End Actor Tag[%d] = %s"), i, *OtherActor->Tags[i].ToString());
	}


	for (int i = 0; i < OtherComp->ComponentTags.Num(); ++i)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[NewPlayer] Overlap End Component Tag[%d] = %s"), i, *OtherComp->ComponentTags[i].ToString());
	}
}

void ANewPlayer::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Verbose, TEXT("[NewPlayer] (%s) Hit Actor Name = %s"), *HitComponent->GetName(), *OtherActor->GetName());

	for (int i = 0; i < OtherActor->Tags.Num(); ++i)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[NewPlayer] Hit Actor Tag[%d] = %s"), i, *OtherActor->Tags[i].ToString());
	}


	for (int i = 0; i < OtherComp->ComponentTags.Num(); ++i)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[NewPlayer] Hit Component Tag[%d] = %s"), i, *OtherComp->ComponentTags[i].ToString());
	}
}