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

	// ボードのメッシュの設定
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

	// プレイヤーメッシュの設定
	m_PlayerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	if (m_PlayerMesh)
	{
		m_PlayerMesh->SetupAttachment(RootComponent);
		m_PlayerMesh->SetEnableGravity(false);
		m_PlayerMesh->SetCollisionProfileName("NoCollision");
		m_PlayerMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_PlayerMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}

	// カメラのスプリングアームの設定
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

	// カメラの設定
	m_PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	if (m_PlayerCamera)
	{
		m_PlayerCamera->SetupAttachment(m_SpringArm, USpringArmComponent::SocketName);
		m_PlayerCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_PlayerCamera->SetRelativeRotation(FRotator(25.0f, 0.0f, 0.0f));
	}

	// トリガー用コリジョンの設定
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

	// 移動コンポーネントの設定
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
		if (_axisValue != 0.0f)
		{
			// ドリフトかどうか
			if (m_IsSharpcurve)
			{
				// 加速する
			}
			else
			{
				// 加速する
				m_CurrentSideAcceleration += m_SideAcceleration;

				// 最大速度にクランプする
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

// 移動処理
void ANewPlayer::UpdateMove(const float deltaTime)
{
	// 左右移動の量に応じて回転
	AddActorLocalRotation(FRotator(0.0f, m_SideValue, 0.0f));

	// ボードの各ベクトルの取得
	FVector meshForwardVec = m_BoardMesh->GetForwardVector();
	FVector meshRightVec = m_BoardMesh->GetRightVector();
	FVector meshUpVector = m_BoardMesh->GetUpVector();

	UE_LOG(LogTemp, VeryVerbose, TEXT("[NewPlayer] meshForwardVec = %s"), *meshForwardVec.ToString());
	UE_LOG(LogTemp, VeryVerbose, TEXT("[NewPlayer] meshRightVec = %s"), *meshRightVec.ToString());
	UE_LOG(LogTemp, VeryVerbose, TEXT("[NewPlayer] meshUpVector = %s"), *meshUpVector.ToString());

	UE_LOG(LogTemp, Warning, TEXT("[NewPlayer] Jump = %d"), m_IsJump);
	UE_LOG(LogTemp, Warning, TEXT("[NewPlayer] Land = %d"), GetIsLanding());

	// 着地判定レイの表示
	m_GroundRay.rayStart = m_BoardMesh->GetComponentLocation() + (meshForwardVec * m_GroundRay.RayStartOffset.X) + (meshRightVec * m_GroundRay.RayStartOffset.Y) + (meshUpVector * m_GroundRay.RayStartOffset.Z);
	m_GroundRay.rayEnd = m_GroundRay.rayStart - (meshUpVector * m_GroundRay.RayLength);

	MyDrawDebugLine(m_GroundRay);

	// 着地判定レイがなにかのオブジェクトにあたっているか
	if (MyLineTrace(m_GroundRay) || m_IsJump)
	{
		// 速度減衰なし
		m_AirSpeedAttenuation = 1.0f;
	}
	else
	{
		// 速度減衰
		m_AirSpeedAttenuation = FMath::Clamp(m_AirSpeedAttenuation - m_AirSpeedAttenuationValue, 0.0f, 1.0f);
	}

	// ボードの前方向ベクトルに加速度を追加
	m_FloatingPawnMovementComponent->AddInputVector(meshForwardVec * m_AirSpeedAttenuation);

	UE_LOG(LogTemp, VeryVerbose, TEXT("[NewPlayer] Velocity = %s"), *m_FloatingPawnMovementComponent->Velocity.ToString());
	UE_LOG(LogTemp, VeryVerbose, TEXT("[NewPlayer] Velocity Size = %f"), m_FloatingPawnMovementComponent->Velocity.Size());

	// ジャンプ状態を戻すか
	if (m_GroundRay.hitResult.bBlockingHit && m_GroundRay.hitResult.Actor->ActorHasTag("Ground"))
	{
		m_IsJump = false;
		m_FloatingPawnMovementComponent->MaxSpeed = m_MaxSpeed;
	}

	// ホバーレイの表示
	m_HoverRay.rayStart = m_BoardMesh->GetComponentLocation();
	m_HoverRay.rayEnd = m_HoverRay.rayStart - (meshUpVector * m_HoverRay.RayLength);

	MyDrawDebugLine(m_HoverRay);

	// 当たっていれば上方向に力を与える
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

	// 角度算出
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

	// 後ろだけあたっているので前を下げる
	if (!isFrontHit && isRearHit)
	{
		frontRot.Pitch = -60.0f;
	}
	// 前だけあたっているので後ろを下げる
	else if (isFrontHit && !isRearHit)
	{
		rearRot.Pitch = -60.0f;
	}
	// どっちも当たっておらずジャンプでもないのでまっすぐにする
	else if (!isFrontHit && !isRearHit && !m_IsJump)
	{
		frontRot.Pitch = 0.0f;
		rearRot.Pitch = 0.0f;
	}


	// 平面の時にPitchが90°と0°が取得されてしまい平均を取ると45°になってしまうので0°に固定
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

	// スプリングアームの距離調整
	float addLength = m_FloatingPawnMovementComponent->Velocity.X / m_ArmLengthAdjust;
	m_SpringArm->TargetArmLength = m_SpringArmLength + addLength;

	// カメラの角度調整
	FRotator cameraRot = m_PlayerCamera->GetRelativeRotation();
	float addPitch = FMath::Lerp(0.0f, 20.0f, m_FloatingPawnMovementComponent->Velocity.Size() / m_FloatingPawnMovementComponent->MaxSpeed);
	cameraRot.Pitch = 15.0f + addPitch;
	m_PlayerCamera->SetRelativeRotation(cameraRot);
}

// 加速リセット
void ANewPlayer::ResetAcceleration()
{
	m_CurrentForwardAcceleration = 0.0f;
}

// 急カーブ（ドリフト）状態にする
void ANewPlayer::SetDrift(bool _status)
{
	m_IsSharpcurve = _status;
}

// FLinetraceInfoとFDebugRayInfoを元にデバッグ用のラインを表示
void ANewPlayer::MyDrawDebugLine(const FLinetraceInfo& linetrace)
{
	if (linetrace.IsDrawRay)
	{
		DrawDebugLine(GetWorld(), linetrace.rayStart, linetrace.rayEnd, linetrace.DrawRayColor, false, linetrace.DrawRayTime);
	}
}

// FLinetraceInfoを元にレイを飛ばす
bool ANewPlayer::MyLineTrace(FLinetraceInfo& linetrace)
{
	return GetWorld()->LineTraceSingleByObjectType(linetrace.hitResult, linetrace.rayStart, linetrace.rayEnd, linetrace.objectQueueParam, linetrace.collisionQueueParam);
}

// FCollisionQueryParamsをのignoreActorを自分自身以外解除
void ANewPlayer::ClearIgnoreActor(FCollisionQueryParams& collisionQueryParams)
{
	collisionQueryParams.ClearIgnoredActors();
	collisionQueryParams.AddIgnoredActor(this);
}

// 軸の角度を固定する
void ANewPlayer::LockAngle(float& originRot, const float lockAxis, const float targetAxis, const float tolerance)
{
	if (FMath::IsNearlyEqual(originRot, lockAxis, tolerance))
	{
		originRot = targetAxis;
	}
}

// デバッグワープ
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

	// 移動可能状態
	if (m_CanMove)
	{
		// コントローラー使用の有無
		if (m_IsSensor)
		{
			InputSensor();
		}

		UpdateMove(DeltaTime);
	}

	DebugWarp();
}

//	終了時
void ANewPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// Called to bind functionality to input
void ANewPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 移動
	InputComponent->BindAxis("MoveRight", this, &ANewPlayer::InputKeyboard);

	// 急カーブ（ドリフト）状態を設定
	DECLARE_DELEGATE_OneParam(FCustomInputDelegate, const bool);

	// ドリフトON
	InputComponent->BindAction<FCustomInputDelegate>("Drift", IE_Pressed, this, &ANewPlayer::SetDrift, true);

	// ドリフトOFF
	InputComponent->BindAction<FCustomInputDelegate>("Drift", IE_Released, this, &ANewPlayer::SetDrift, false);

	// デバッグ用--加速度リセット
	InputComponent->BindAction("Debug_Stop", IE_Pressed, this, &ANewPlayer::ResetAcceleration);
}

// 左右移動の取得用
float ANewPlayer::GetSideMoveValue()
{
	return 0.0f;
}

// 接地しているか
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