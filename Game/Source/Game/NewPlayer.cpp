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
	: m_IsSensor(false)
	, m_IsSharpcurve(false)
	, m_CurrentGravity(0.0f)
	, m_CurrentForwardAcceleration(0.0f)
	, m_SideValue(0.0f)
	, m_CurrentSideAcceleration(0.0f)
	, m_CurrentSharpcurvePower(0.0f)
	, m_AirSpeedAttenuation(1.0f)
	, m_MaxSpeed(2500.0f)
	, m_MaxJumpSpeed(4000.0f)
	, m_UpdateValue(FVector::ZeroVector)
	, m_BaseForwardVector(FVector::ZeroVector)
	, m_IsJump(false)
	, m_FloatingPawnMovementComponent(nullptr)
	, m_BoardMesh(nullptr)
	, m_PlayerMesh(nullptr)
	, m_SpringArm(nullptr)
	, m_PlayerCamera(nullptr)
	, m_BoxCollision(nullptr)
	, m_SpringArmLength(400.0f)
	, m_ArmLengthAdjust(100.0f)
	, m_JumpPower(FVector(5000.0f, 5000.0f, 5000.0f))
	, m_CanMove(true)
	, m_AirSpeedAttenuationValue(1.0f / 600.0f)
	, m_FloatPower(35.0f)
	, m_JumpGravity(150.0f)
	, m_FallGravity(2500.0f)
	, m_AddJumpGravity(100.0f)
	, m_HoverLerpSpeed(0.2f)
	, m_AngleLerpSpeed(0.6f)
	, m_SideMaxSpeed(2.5f)
	, m_SideAcceleration(0.15f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// �{�[�h�̃��b�V���̐ݒ�
	m_BoardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoardMesh"));
	if (m_BoardMesh)
	{
		RootComponent = m_BoardMesh;
		m_BoardMesh->bHasPerInstanceHitProxies = true;
		m_BoardMesh->OnComponentHit.AddDynamic(this, &ANewPlayer::OnCompHit);
		m_BoardMesh->SetSimulatePhysics(false);
		m_BoardMesh->SetEnableGravity(false);
		m_BoardMesh->SetUseCCD(true);
		m_BoardMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
		m_BoardMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_BoardMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}

	// �v���C���[���b�V���̐ݒ�
	m_PlayerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	if (m_PlayerMesh)
	{
		m_PlayerMesh->SetupAttachment(RootComponent);
		m_PlayerMesh->SetEnableGravity(false);
		m_PlayerMesh->SetCollisionProfileName("NoCollision");
		m_PlayerMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_PlayerMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}

	// �J�����̃X�v�����O�A�[���̐ݒ�
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
		m_SpringArm->SetRelativeRotation(FRotator(320.0f, 0.0f, 0.0f));
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
		m_BoxCollision->SetupAttachment(RootComponent);
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

	m_GroundRay.RayStartOffset = FVector(0.0f, 0.0f, 5.0f);
	m_GroundRay.RayLength = 200.0f;

	m_HoverRay.DrawRayColor = FColor::Red;
	m_HoverRay.RayLength = 100.0f;

	m_HoverAngleFrontRay.RayStartOffset = FVector(200.0f, 0.0f, 100.0f);
	m_HoverAngleFrontRay.DrawRayColor = FColor::Blue;
	m_HoverAngleFrontRay.RayLength = 300.0f;

	m_HoverAngleRearRay.RayStartOffset = FVector(-200.0f, 0.0f, 100.0f);
	m_HoverAngleRearRay.DrawRayColor = FColor::Blue;
	m_HoverAngleRearRay.RayLength = 300.0f;
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
void ANewPlayer::UpdateMove(const float deltaTime)
{
	// ���E�ړ��̗ʂɉ����ĉ�]
	AddActorLocalRotation(FRotator(0.0f, m_SideValue, 0.0f));

	// �{�[�h�̊e�x�N�g���̎擾
	FVector meshForwardVec = m_BoardMesh->GetForwardVector();
	FVector meshRightVec = m_BoardMesh->GetRightVector();
	FVector meshUpVector = m_BoardMesh->GetUpVector();

	UE_LOG(LogTemp, VeryVerbose, TEXT("[NewPlayer] meshForwardVec = %s"), *meshForwardVec.ToString());
	UE_LOG(LogTemp, VeryVerbose, TEXT("[NewPlayer] meshRightVec = %s"), *meshRightVec.ToString());
	UE_LOG(LogTemp, VeryVerbose, TEXT("[NewPlayer] meshUpVector = %s"), *meshUpVector.ToString());

	UE_LOG(LogTemp, Warning, TEXT("[NewPlayer] Jump = %d"), m_IsJump);
	UE_LOG(LogTemp, Warning, TEXT("[NewPlayer] Land = %d"), GetIsLanding());

	// ���n���背�C�̕\��
	m_GroundRay.rayStart = m_BoardMesh->GetComponentLocation() + (meshForwardVec * m_GroundRay.RayStartOffset.X) + (meshRightVec * m_GroundRay.RayStartOffset.Y) + (meshUpVector * m_GroundRay.RayStartOffset.Z);
	m_GroundRay.rayEnd = m_GroundRay.rayStart - (meshUpVector * m_GroundRay.RayLength);

	MyDrawDebugLine(m_GroundRay);

	// ���n���背�C���Ȃɂ��̃I�u�W�F�N�g�ɂ������Ă��邩
	if (MyLineTrace(m_GroundRay) || m_IsJump)
	{
		// ���x�����Ȃ�
		m_AirSpeedAttenuation = 1.0f;
	}
	else
	{
		// ���x����
		m_AirSpeedAttenuation = FMath::Clamp(m_AirSpeedAttenuation - m_AirSpeedAttenuationValue, 0.0f, 1.0f);
	}

	// �{�[�h�̑O�����x�N�g���ɉ����x��ǉ�
	m_FloatingPawnMovementComponent->AddInputVector(meshForwardVec * m_AirSpeedAttenuation);

	UE_LOG(LogTemp, VeryVerbose, TEXT("[NewPlayer] Velocity = %s"), *m_FloatingPawnMovementComponent->Velocity.ToString());
	UE_LOG(LogTemp, VeryVerbose, TEXT("[NewPlayer] Velocity Size = %f"), m_FloatingPawnMovementComponent->Velocity.Size());

	// �W�����v��Ԃ�߂���
	if (m_GroundRay.hitResult.bBlockingHit && m_GroundRay.hitResult.Actor->ActorHasTag("Ground"))
	{
		m_IsJump = false;
		m_FloatingPawnMovementComponent->MaxSpeed = m_MaxSpeed;
	}

	// �z�o�[���C�̕\��
	m_HoverRay.rayStart = m_BoardMesh->GetComponentLocation();
	m_HoverRay.rayEnd = m_HoverRay.rayStart - (meshUpVector * m_HoverRay.RayLength);

	MyDrawDebugLine(m_HoverRay);

	// �������Ă���Ώ�����ɗ͂�^����
	FVector pos = GetActorLocation();
	if (MyLineTrace(m_HoverRay))
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("[NewPlayer] Gravity Stat : Hover"), true, true, FColor::Cyan, deltaTime);
		pos = m_HoverRay.hitResult.ImpactPoint;
		pos.Z += m_FloatPower;
	}
	else if (m_IsJump)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("[NewPlayer] Gravity Stat : Jump"), true, true, FColor::Cyan, deltaTime);
		pos.Z -= m_JumpGravity + m_CurrentGravity;
		m_CurrentGravity += m_AddJumpGravity;
	}
	else
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("[NewPlayer] Gravity Stat : Fall"), true, true, FColor::Cyan, deltaTime);
		pos.Z -= m_FallGravity;
	}

	SetActorLocation(FMath::VInterpTo(GetActorLocation(), pos, deltaTime, m_HoverLerpSpeed));

	// �p�x�Z�o
	m_HoverAngleFrontRay.rayStart = m_BoardMesh->GetComponentLocation() + (meshForwardVec * m_HoverAngleFrontRay.RayStartOffset.X) + (meshRightVec * m_HoverAngleFrontRay.RayStartOffset.Y) + (meshUpVector * m_HoverAngleFrontRay.RayStartOffset.Z);
	m_HoverAngleFrontRay.rayEnd = m_HoverAngleFrontRay.rayStart - (meshUpVector * m_HoverAngleFrontRay.RayLength);

	m_HoverAngleRearRay.rayStart = m_BoardMesh->GetComponentLocation() + (meshForwardVec * m_HoverAngleRearRay.RayStartOffset.X) + (meshRightVec * m_HoverAngleRearRay.RayStartOffset.Y) + (meshUpVector * m_HoverAngleRearRay.RayStartOffset.Z);
	m_HoverAngleRearRay.rayEnd = m_HoverAngleRearRay.rayStart - (meshUpVector * m_HoverAngleRearRay.RayLength);

	MyDrawDebugLine(m_HoverAngleFrontRay);
	MyDrawDebugLine(m_HoverAngleRearRay);

	FVector frontVector = FVector::ZeroVector;
	FVector rearVector = FVector::ZeroVector;

	bool isFrontHit = MyLineTrace(m_HoverAngleFrontRay);
	bool isRearHit = MyLineTrace(m_HoverAngleRearRay);

	if (isFrontHit && isRearHit)
	{
		frontVector = m_HoverAngleFrontRay.hitResult.ImpactNormal;
		rearVector = m_HoverAngleRearRay.hitResult.ImpactNormal;
	}

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

	FRotator averageRotator = FRotator((frontRot.Pitch + rearRot.Pitch) / 2.0f, (frontRot.Yaw + rearRot.Yaw) / 2.0f, (frontRot.Roll + rearRot.Roll) / 2.0f);
	averageRotator.Yaw = GetActorRotation().Yaw;
	averageRotator.Roll = 0.0f;

	FRotator newRot = FMath::RInterpTo(GetActorRotation(), averageRotator, deltaTime, m_AngleLerpSpeed);

	/*
	newRot.Yaw = 0.0f;
	newRot.Roll = -newRot.Roll / 1.5f;
	AddActorLocalRotation(newRot);
	*/

	SetActorRotation(newRot);

	UE_LOG(LogTemp, Warning, TEXT("[NewPlayer] newRot %s"), *newRot.ToString());

	if (m_IsJump)
	{
		UE_LOG(LogTemp, Warning, TEXT("[NewPlayer] Jumping!"))
	}

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

// ���̊p�x���Œ肷��
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

	const FKey debugKeys[] = { EKeys::One, EKeys::Two, EKeys::Three, EKeys::Four, EKeys::Five, EKeys::Six, EKeys::Seven, EKeys::Eight, EKeys::Nine, EKeys::Zero,};

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("[NewPlayer] Could not get the player controller. Debug warp is not available."));
		return;
	}

	for (int i = 0; i < 10; ++i)
	{
		if (PlayerController->IsInputKeyDown(debugKeys[i]))
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

	m_BaseForwardVector = m_BoardMesh->GetForwardVector();

	m_GroundRay.collisionQueueParam.AddIgnoredActor(this);
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
		// �R���g���[���[�g�p�̗L��
		if (m_IsSensor)
		{
			InputSensor();
		}

		UpdateMove(DeltaTime);
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

// ���E�ړ��̎擾�p
float ANewPlayer::GetSideMoveValue()
{
	return 0.0f;
}

// �ڒn���Ă��邩
bool ANewPlayer::GetIsLanding()
{
	return m_GroundRay.hitResult.IsValidBlockingHit();
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