// Fill out your copyright notice in the Description page of Project Settings.

#include "NewPlayer.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "..\..\UE4Duino\Source\UE4Duino\Public\Serial.h"

// Sets default values
ANewPlayer::ANewPlayer()
	: m_PlayerController(nullptr)
	, m_IsTrick(false)
	, m_TrickNum(0)
	, m_CurrentGravity(0.0f)
	, m_CurrentForwardAcceleration(0.0f)
	, m_SideValue(0.0f)
	, m_CurrentSideAcceleration(0.0f)
	, m_CurrentTrickSpinValue(0.0f)
	, m_AirSpeedAttenuation(1.0f)
	, m_InputAxisValue(FVector2D::ZeroVector)
	, m_CurrentTrick(ETrickType::None)
	, m_MaxSpeed(2500.0f)
	, m_MaxJumpSpeed(4000.0f)
	, m_IsJump(false)
	, m_FloatingPawnMovementComponent(nullptr)
	, m_RootCollisionBox(nullptr)
	, m_BoardMesh(nullptr)
	, m_PlayerMesh(nullptr)
	, m_SpringArm(nullptr)
	, m_PlayerCamera(nullptr)
	, m_BoxCollision(nullptr)
	, m_SpringArmLength(400.0f)
	, m_ArmLengthAdjust(100.0f)
	, m_CanMove(true)
	, m_AirSpeedAttenuationValue(1.0f / 2400.0f)
	, m_FloatPower(15.0f)
	, m_JumpGravity(35.0f)
	, m_FallGravity(30.0f)
	, m_AddJumpGravity(25.0f)
	, m_AddFallGravity(20.0f)
	, m_HoverLerpSpeed(1.0f)
	, m_AngleLerpSpeed(1.2f)
	, m_SideMaxSpeed(2.5f)
	, m_SideAcceleration(0.15f)
	, m_MaxAngle(75.0f)
	, m_score(0)
	, m_ScoreFlag(false)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// ���[�g�ɂȂ�R���W�����̐ݒ�
	m_RootCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RootCollision"));
	if (m_RootCollisionBox)
	{
		RootComponent = m_RootCollisionBox;
		m_RootCollisionBox->SetSimulatePhysics(false);
		m_RootCollisionBox->SetEnableGravity(false);
		m_RootCollisionBox->SetUseCCD(true);
		m_RootCollisionBox->SetCollisionProfileName(TEXT("BlockAllDynamic"));
		m_RootCollisionBox->SetBoxExtent(FVector(130.0f, 20.0f, 5.0f));
		m_RootCollisionBox->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_RootCollisionBox->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}

	// �{�[�h�̃��b�V���̐ݒ�
	m_BoardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoardMesh"));
	if (m_BoardMesh)
	{
		m_BoardMesh->SetupAttachment(RootComponent);
		m_BoardMesh->OnComponentHit.AddDynamic(this, &ANewPlayer::OnCompHit);
		m_BoardMesh->SetSimulatePhysics(false);
		m_BoardMesh->SetEnableGravity(false);
		m_BoardMesh->SetUseCCD(true);
		m_BoardMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
		m_BoardMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_BoardMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}

	// �v���C���[���b�V���̐ݒ�
	m_PlayerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	if (m_PlayerMesh)
	{
		m_PlayerMesh->SetupAttachment(m_BoardMesh, TEXT("NormanSocket"));
		m_PlayerMesh->SetEnableGravity(false);
		m_PlayerMesh->SetCollisionProfileName("NoCollision");
		m_PlayerMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_PlayerMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}

	// �J�����̃X�v�����O�A�[���̐ݒ�
	m_SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	if (m_SpringArm)
	{
		m_SpringArm->SetupAttachment(RootComponent);
		m_SpringArm->TargetArmLength = m_SpringArmLength;
		m_SpringArm->bDoCollisionTest = false;
		m_SpringArm->bEnableCameraLag = true;
		m_SpringArm->CameraLagSpeed = 10.0f;
		m_SpringArm->bEnableCameraRotationLag = true;
		m_SpringArm->CameraRotationLagSpeed = 10.0f;
		m_SpringArm->bInheritPitch = false;
		m_SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_SpringArm->SetRelativeRotation(FRotator(-40.0f, 0.0f, 0.0f));
	}

	// �J�����̐ݒ�
	m_PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	if (m_PlayerCamera)
	{
		m_PlayerCamera->SetupAttachment(m_SpringArm, USpringArmComponent::SocketName);
		m_PlayerCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_PlayerCamera->SetRelativeRotation(FRotator(25.0f, 0.0f, 0.0f));
	}

	// �g���K�[�p�R���W�����̐ݒ�
	m_BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	if (m_BoxCollision)
	{
		m_BoxCollision->SetupAttachment(m_BoardMesh);
		m_BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ANewPlayer::OnOverlapBegin);
		m_BoxCollision->bMultiBodyOverlap = true;
		m_BoxCollision->SetBoxExtent(FVector(150.0f, 50.0f, 100.0f));
		m_BoxCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
		m_BoxCollision->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}

	// �ړ��R���|�[�l���g�̐ݒ�
	m_FloatingPawnMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovementComponent"));
	if (m_FloatingPawnMovementComponent)
	{
		m_FloatingPawnMovementComponent->MaxSpeed = m_MaxSpeed;
		m_FloatingPawnMovementComponent->Acceleration = 1000.0f;
		m_FloatingPawnMovementComponent->Deceleration = 8000.0f;
		m_FloatingPawnMovementComponent->TurningBoost = 75.0f;
	}

	m_HoverRay.RayStartOffset = FVector(0.0f, 0.0f, 5.0f);
	m_HoverRay.DrawRayColor = FColor::Red;
	m_HoverRay.RayLength = 200.0f;

	m_HoverAngleFrontRay.RayStartOffset = FVector(200.0f, 0.0f, 100.0f);
	m_HoverAngleFrontRay.DrawRayColor = FColor::Blue;
	m_HoverAngleFrontRay.RayLength = 300.0f;

	m_HoverAngleRearRay.RayStartOffset = FVector(-200.0f, 0.0f, 100.0f);
	m_HoverAngleRearRay.DrawRayColor = FColor::Blue;
	m_HoverAngleRearRay.RayLength = 300.0f;
}

// X���̓��͏���
void ANewPlayer::InputAxisX(const float _axisValue)
{
	m_InputAxisValue.X = _axisValue;

	// �p�x�����E�ړ��̌��E���ł��邩�A�g���b�N���
	if (((m_BoardMesh->GetRelativeRotation().Yaw <= m_MaxAngle) && (m_BoardMesh->GetRelativeRotation().Yaw >= -m_MaxAngle)) || (m_IsJump && m_IsTrick))
	{
		// ���͂��� & �W�����v��Ԃł͂Ȃ�
		if (_axisValue != 0.0f && !m_IsJump)
		{
			// ��������
			m_CurrentSideAcceleration += m_SideAcceleration;

			// �ő呬�x�ɃN�����v����
			m_CurrentSideAcceleration = FMath::Clamp(m_CurrentSideAcceleration, 0.0f, m_SideMaxSpeed);

			m_SideValue = FMath::Clamp(_axisValue, -1.0f, 1.0f) * m_CurrentSideAcceleration;
		}
		// ���͂Ȃ�
		else
		{
			m_CurrentSideAcceleration = 0.0f;
			m_SideValue = 0.0f;
		}
	}
	else
	{
		m_CurrentSideAcceleration = 0.0f;
		m_SideValue = 0.0f;

		FRotator newRot = m_BoardMesh->GetRelativeRotation();
		newRot.Yaw = (m_BoardMesh->GetRelativeRotation().Yaw > m_MaxAngle) ? m_MaxAngle : -m_MaxAngle;

		m_BoardMesh->SetRelativeRotation(newRot);
	}
}

// Y���̓��͏���
void ANewPlayer::InputAxisY(const float _axisValue)
{
	m_InputAxisValue.Y = _axisValue;
}

// �z�o�[����
void ANewPlayer::Hover(const float _deltaTime)
{
	// �z�o�[���C�̈ʒu�ݒ�
	// �J�n�n�_���{�[�h�̈ʒu��
	m_HoverRay.SetStart(m_BoardMesh);
	// �I���n�_�𐂒��ɍ~�낵���n�_��
	m_HoverRay.SetEnd(m_BoardMesh);

	// ���C�̕\��
	MyDrawDebugLine(m_HoverRay);

	// �z�o�[���C���������Ă���Ώ㏸
	FVector pos = GetActorLocation();
	MyLineTrace(m_HoverRay);

	// �O�ɐi�ޕ���
	static FVector forwardVector;

	// �W�����v���łȂ���΃{�[�h�̑O������ۑ�
	if (!m_IsJump)
	{
		forwardVector = m_BoardMesh->GetForwardVector();
	}

	if (m_IsJump)
	{
		// ���x�����Ȃ�
		m_AirSpeedAttenuation = 1.0f;

		// �W�����v���̏d�͂�^����
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("[NewPlayer] Gravity Stat : Jump"), true, true, FColor::Cyan, _deltaTime);
		pos.Z -= m_JumpGravity + m_CurrentGravity;
		m_CurrentGravity += m_AddJumpGravity;
	}
	else if (m_HoverRay.hitResult.bBlockingHit)
	{
		// ���x�����Ȃ�
		m_AirSpeedAttenuation = 1.0f;

		// �n�ʂ���m_FloatPower����������
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("[NewPlayer] Gravity Stat : Hover"), true, true, FColor::Cyan, _deltaTime);
		pos = m_HoverRay.hitResult.ImpactPoint;
		pos.Z += m_FloatPower;
		m_CurrentGravity = 0.0f;
	}
	else
	{
		// ���x����
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("[NewPlayer] SpeedAttenuation"), true, true, FColor::Cyan, _deltaTime);
		m_AirSpeedAttenuation = FMath::Clamp(m_AirSpeedAttenuation - m_AirSpeedAttenuationValue, 0.0f, 1.0f);

		// �ʏ헎�����̏d�͂�^����
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("[NewPlayer] Gravity Stat : Fall"), true, true, FColor::Cyan, _deltaTime);
		pos.Z -= m_FallGravity + m_CurrentGravity;
		m_CurrentGravity += m_AddFallGravity;
	}

	// �{�[�h�̑O�����x�N�g���ɉ����x��ǉ�
	m_FloatingPawnMovementComponent->AddInputVector(forwardVector * m_AirSpeedAttenuation);

	// ������������Actor�ɓK�p
	SetActorLocation(FMath::VInterpTo(GetActorLocation(), pos, _deltaTime, m_HoverLerpSpeed), true);

	// �n�ʂƐڐG���Ă���΃W�����v��Ԃ�߂�
	if (m_HoverRay.hitResult.bBlockingHit && m_HoverRay.hitResult.Actor->ActorHasTag("Ground"))
	{
		m_IsJump = false;
		m_CurrentTrick = ETrickType::None;
		m_FloatingPawnMovementComponent->MaxSpeed = m_MaxSpeed;
		m_CurrentGravity = 0.0f;
	}
}

// 2�̃��C����v���C���[�̊p�x��ύX
void ANewPlayer::SetRotationWithRay(const float _deltaTime)
{
	// �O�̃��C�̐ݒ�E�`��
	m_HoverAngleFrontRay.SetStart(m_BoardMesh);
	m_HoverAngleFrontRay.SetEnd(m_BoardMesh);
	MyDrawDebugLine(m_HoverAngleFrontRay);

	// ���̃��C�̐ݒ�E�`��
	m_HoverAngleRearRay.SetStart(m_BoardMesh);
	m_HoverAngleRearRay.SetEnd(m_BoardMesh);
	MyDrawDebugLine(m_HoverAngleRearRay);

	// ���C�̂��������@������p�x���Z�o
	FVector frontVector = FVector::ZeroVector;
	FVector rearVector = FVector::ZeroVector;

	// ���C�������������ǂ���
	bool isFrontHit = MyLineTrace(m_HoverAngleFrontRay);
	bool isRearHit = MyLineTrace(m_HoverAngleRearRay);

	// �ǂ������������Ă���ꍇ�̂ݖ@�������L������
	if (isFrontHit && isRearHit)
	{
		frontVector = m_HoverAngleFrontRay.hitResult.ImpactNormal;
		rearVector = m_HoverAngleRearRay.hitResult.ImpactNormal;
	}

	// Vector����Rotator���Z�o����
	FRotator frontRot = UKismetMathLibrary::MakeRotFromX(frontVector);
	FRotator rearRot = UKismetMathLibrary::MakeRotFromX(rearVector);

	// ��낾���������Ă���̂őO��������
	if (!isFrontHit && isRearHit)
	{
		frontRot.Pitch = -60.0f;
	}
	// �O�����������Ă���̂Ō���������
	else if (isFrontHit && !isRearHit)
	{
		rearRot.Pitch = -60.0f;
	}
	// �ǂ������������Ă��炸�W�����v�ł��Ȃ��̂ł܂������ɂ���
	else if (!isFrontHit && !isRearHit && !m_IsJump)
	{
		frontRot.Pitch = 0.0f;
		rearRot.Pitch = 0.0f;
	}

	// ���ʂ̎���Pitch��90����0�����擾����Ă��܂����ς�����45���ɂȂ��Ă��܂��̂�0���ɌŒ�
	LockAngle(frontRot.Pitch, 90.0f, 0.0f, 1.0f);
	LockAngle(rearRot.Pitch, 90.0f, 0.0f, 1.0f);

	// �p�x�̕��ς��Z�o
	FRotator averageRotator = FRotator((frontRot.Pitch + rearRot.Pitch) / 2.0f, (frontRot.Yaw + rearRot.Yaw) / 2.0f, (frontRot.Roll + rearRot.Roll) / 2.0f);

	// Yaw�iZ���j�̓v���C���[�����삵�ĕύX����̂ŕύX���Ȃ�
	averageRotator.Yaw = m_BoardMesh->GetComponentRotation().Yaw;

	// Roll�iX���j�͌X��������Ɠ|�ꂻ���ɂȂ�̂�0�ŌŒ�
	averageRotator.Roll = 0.0f;

	// �p�x��Lerp�łȂ߂炩�ɕύX
	FRotator newRot = FMath::RInterpTo(m_BoardMesh->GetComponentRotation(), averageRotator, _deltaTime, m_AngleLerpSpeed);
	m_BoardMesh->SetWorldRotation(newRot);
}

// �ړ�����
void ANewPlayer::UpdateMove(const float _deltaTime)
{
	// �z�o�[
	Hover(_deltaTime);

	// ���E�ړ��̗ʂɉ����ĉ�]
	m_BoardMesh->AddLocalRotation(FRotator(0.0f, m_SideValue, 0.0f));

	// 2�̃��C����v���C���[�̊p�x��ύX
	SetRotationWithRay(_deltaTime);

	// �X�v�����O�A�[���̋�������
	float addLength = m_FloatingPawnMovementComponent->Velocity.X / m_ArmLengthAdjust;
	m_SpringArm->TargetArmLength = m_SpringArmLength + addLength;

	// �J�����̊p�x����
	FRotator cameraRot = m_PlayerCamera->GetRelativeRotation();
	float addPitch = FMath::Lerp(0.0f, 20.0f, m_FloatingPawnMovementComponent->Velocity.Size() / m_FloatingPawnMovementComponent->MaxSpeed);
	cameraRot.Pitch = 15.0f + addPitch;
	m_PlayerCamera->SetRelativeRotation(cameraRot);
}

// �g���b�N
void ANewPlayer::Trick()
{
	// �g���b�N�\��ԁi�W�����v���g���b�N�{�^���������Ă���j
	if (m_IsJump && m_IsTrick)
	{
		// �܂��g���b�N�����߂Ă��Ȃ����
		if (m_CurrentTrick == ETrickType::None)
		{
			m_CurrentTrickSpinValue = 0.0f;

			for (int i = 0; i < m_TrickBind.Num(); ++i)
			{
				// �g���b�N�ꗗ�̎����`�F�b�N
				float compInputValue = 0.0f;
				switch (m_TrickBind[i].AxisDirection)
				{
				case EInputAxis::X:
					compInputValue = m_InputAxisValue.X;
					break;

				case EInputAxis::Y:
					compInputValue = m_InputAxisValue.Y;
					break;

				default:
					break;
				}

				// ��r
				bool result = false;
				switch (m_TrickBind[i].ValueComparisonType)
				{
				case EComp::Auto:
					if (m_TrickBind[i].InputAxis > 0.0f)
					{
						result = compInputValue >= m_TrickBind[i].InputAxis;
					}
					else if (m_TrickBind[i].InputAxis < 0.0f)
					{
						result = compInputValue <= m_TrickBind[i].InputAxis;
					}
					break;

				case EComp::OrMore:
					result = compInputValue >= m_TrickBind[i].InputAxis;
					break;

				case EComp::MoreThan:
					result = compInputValue > m_TrickBind[i].InputAxis;
					break;

				case EComp::LessThan:
					result = compInputValue < m_TrickBind[i].InputAxis;
					break;

				case EComp::OrLess:
					result = compInputValue <= m_TrickBind[i].InputAxis;
					break;

				default:
					break;
				}

				if (result)
				{
					m_TrickNum = i;
					m_CurrentTrick = m_TrickBind[m_TrickNum].Trick;
					break;
				}
			}
		}
		else
		{
			// �g���b�N�ꗗ�̎����`�F�b�N
			float inputValue = 0.0f;
			switch (m_TrickBind[m_TrickNum].AxisDirection)
			{
			case EInputAxis::X:
				inputValue = m_InputAxisValue.X;
				break;

			case EInputAxis::Y:
				inputValue = m_InputAxisValue.Y;
				break;

			default:
				break;
			}

			// �g���b�N�����߂�
			switch (m_CurrentTrick)
			{
				// �t�����g�T�C�h�X�s���i�������iZ���j�Ŏ��v���j
			case ETrickType::FrontSideSpin:
				// �X�s�����������A0�`�ō����x�ɃN�����v
				m_CurrentTrickSpinValue = FMath::Clamp(m_CurrentTrickSpinValue + m_TrickBind[m_TrickNum].TrickSpinAcceleration, 0.0f, m_TrickBind[m_TrickNum].TrickSpinMaxValue);

				// �X�s��
				m_BoardMesh->AddRelativeRotation(FRotator(0.0f, -m_CurrentTrickSpinValue * inputValue, 0.0f));
				break;

				// �o�b�N�T�C�h�X�s���i�������iZ���j�Ŕ����v���j
			case ETrickType::BackSideSpin:

				// �X�s�����������A0�`�ō����x�ɃN�����v
				m_CurrentTrickSpinValue = FMath::Clamp(m_CurrentTrickSpinValue + m_TrickBind[m_TrickNum].TrickSpinAcceleration, 0.0f, m_TrickBind[m_TrickNum].TrickSpinMaxValue);

				// �X�s��
				m_BoardMesh->AddRelativeRotation(FRotator(0.0f, m_CurrentTrickSpinValue * inputValue, 0.0f));
				break;
			}
		}
	}
}

// �g���b�N�{�^���̓��͂��󂯕t����
void ANewPlayer::SetTrick(const bool _status)
{
	m_IsTrick = _status;
}

// FLinetraceInfo��FDebugRayInfo�����Ƀf�o�b�O�p�̃��C����\��
void ANewPlayer::MyDrawDebugLine(const FLinetraceInfo& linetrace)
{
	if (linetrace.IsDrawRay)
	{
		DrawDebugLine(GetWorld(), linetrace.rayStart, linetrace.rayEnd, linetrace.DrawRayColor, false, linetrace.DrawRayTime);
	}
}

// FLinetraceInfo�����Ƀ��C���΂�
bool ANewPlayer::MyLineTrace(FLinetraceInfo& linetrace)
{
	return GetWorld()->LineTraceSingleByObjectType(linetrace.hitResult, linetrace.rayStart, linetrace.rayEnd, linetrace.objectQueueParam, linetrace.collisionQueueParam);
}

// FCollisionQueryParams����ignoreActor���������g�ȊO����
void ANewPlayer::ClearIgnoreActor(FCollisionQueryParams& collisionQueryParams)
{
	collisionQueryParams.ClearIgnoredActors();
	collisionQueryParams.AddIgnoredActor(this);
}

// �n�����p�x�̒l�ioriginRot�j���w��̊p�x�ilockAxis�j�ɂقړ������ꍇ�A�w�肵���l�itargetAxis�j�ɂ���
void ANewPlayer::LockAngle(float& originRot, const float lockAxis, const float targetAxis, const float tolerance)
{
	if (FMath::IsNearlyEqual(originRot, lockAxis, tolerance))
	{
		originRot = targetAxis;
	}
}

// �f�o�b�O���[�v
void ANewPlayer::DebugWarp()
{
	int keyNum = -1;

	const FKey debugKeys[] = { EKeys::One, EKeys::Two, EKeys::Three, EKeys::Four, EKeys::Five, EKeys::Six, EKeys::Seven, EKeys::Eight, EKeys::Nine, EKeys::Zero, };

	if (!m_PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("[NewPlayer] Could not get the player controller. Debug warp is not available."));
		return;
	}

	for (int i = 0; i < 10; ++i)
	{
		if (m_PlayerController->IsInputKeyDown(debugKeys[i]))
		{
			keyNum = i;
			break;
		}
	}

	if (keyNum != -1)
	{
		if (m_WarpPoints.Num() > keyNum && m_WarpPoints[keyNum])
		{
			SetActorLocation(m_WarpPoints[keyNum]->GetActorLocation());
			SetActorRotation(FRotator::ZeroRotator);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[NewPlayer] No warp points specified for the entered number %d"), keyNum);
		}
	}
}

// Called when the game starts or when spawned
void ANewPlayer::BeginPlay()
{
	Super::BeginPlay();

	// �v���C���[�̃R���g���[���[�̎擾
	m_PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	m_HoverRay.collisionQueueParam.AddIgnoredActor(this);
	m_HoverAngleFrontRay.collisionQueueParam.AddIgnoredActor(this);
	m_HoverAngleRearRay.collisionQueueParam.AddIgnoredActor(this);
}

// Called every frame
void ANewPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// �ړ��\���
	if (m_CanMove)
	{
		UpdateMove(DeltaTime);
		Trick();
	}

	DebugWarp();
}

//	�I����
void ANewPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// Called to bind functionality to input
void ANewPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// �ړ�
	InputComponent->BindAxis("Right", this, &ANewPlayer::InputAxisX);
	InputComponent->BindAxis("Forward", this, &ANewPlayer::InputAxisY);

	// �}�J�[�u�i�h���t�g�j��Ԃ�ݒ�
	DECLARE_DELEGATE_OneParam(FCustomInputDelegate, const bool);

	// �g���b�NON
	InputComponent->BindAction<FCustomInputDelegate>("Trick", IE_Pressed, this, &ANewPlayer::SetTrick, true);

	// �g���b�NOFF
	InputComponent->BindAction<FCustomInputDelegate>("Trick", IE_Released, this, &ANewPlayer::SetTrick, false);
}

// ���E�ړ��̎擾�p
float ANewPlayer::GetSideMoveValue()
{
	return 0.0f;
}

// �ڒn���Ă��邩
bool ANewPlayer::GetIsLanding()
{
	return m_HoverRay.hitResult.IsValidBlockingHit();
}

void ANewPlayer::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->ComponentHasTag("Jump") && m_IsJump == false)
	{
		m_IsJump = true;
		m_CurrentGravity = 0.0f;

		m_FloatingPawnMovementComponent->MaxSpeed = m_MaxJumpSpeed;
	}
}

void ANewPlayer::OnComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void ANewPlayer::OnCompHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
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

int ANewPlayer::GetScore()
{
	int tmp = m_ScoreFlag ? m_score : 0;

	// �X�R�A�t���O���Z�b�g
	if (tmp != 0)m_ScoreFlag = false;

	return  tmp;
}