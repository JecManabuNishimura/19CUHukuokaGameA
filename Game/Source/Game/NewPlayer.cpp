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
	//, m_ProjectileMovementComponent(nullptr)
	, m_BoardCapsuleCollision(nullptr)
	, m_BoardMesh(nullptr)
	, m_PlayerMesh(nullptr)
	, m_SpringArm(nullptr)
	, m_PlayerCamera(nullptr)
	, m_BoxCollision(nullptr)
	, m_RootRotationY(90.0f)
	, m_SpringArmLength(350.0f)
	, m_ArmLengthAdjust(100.0f)
	, m_CanMove(false)
	, m_ForwardMaxSpeed(5.0f)
	, m_ForwardAcceleration(0.0125f)
	, m_SideMaxSpeed(1.0f)
	, m_SideAcceleration(0.15f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	m_BoardCapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BoardCapsuleCollision"));
	if (m_BoardCapsuleCollision)
	{
		RootComponent = m_BoardCapsuleCollision;
		m_BoardCapsuleCollision->OnComponentHit.AddDynamic(this, &ANewPlayer::OnHit);
		m_BoardCapsuleCollision->SetSimulatePhysics(true);
		m_BoardCapsuleCollision->SetEnableGravity(true);
		m_BoardCapsuleCollision->SetUseCCD(true);
		m_BoardCapsuleCollision->SetCollisionProfileName(TEXT("PhysicsActor"));
		m_BoardCapsuleCollision->SetCapsuleHalfHeight(90.0f);
		m_BoardCapsuleCollision->SetCenterOfMass(FVector(0.0f, 0.0f, -1.0f));
		m_BoardCapsuleCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_BoardCapsuleCollision->SetRelativeRotation(FRotator(m_RootRotationY, 0.0f, 0.0f));
	}

	m_BoardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoardMesh"));
	if (m_BoardMesh)
	{
		m_BoardMesh->SetupAttachment(RootComponent);
		m_BoardMesh->SetCollisionProfileName("NoCollision");
		m_BoardMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_BoardMesh->SetRelativeRotation(FRotator(-m_RootRotationY, 0.0f, 0.0f));
	}

	m_PlayerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	if (m_PlayerMesh)
	{
		m_PlayerMesh->SetupAttachment(RootComponent);
		m_PlayerMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_PlayerMesh->SetRelativeRotation(FRotator(-m_RootRotationY, 0.0f, 0.0f));
	}

	m_SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	if (m_SpringArm)
	{
		m_SpringArm->SetupAttachment(RootComponent);
		m_SpringArm->TargetArmLength = m_SpringArmLength;
		m_SpringArm->bDoCollisionTest = true;
		m_SpringArm->bEnableCameraLag = true;
		m_SpringArm->CameraLagSpeed = 10.0f;
		m_SpringArm->bEnableCameraRotationLag = true;
		m_SpringArm->CameraRotationLagSpeed = 10.0f;
		m_SpringArm->bInheritPitch = false;
		m_SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_SpringArm->SetRelativeRotation(FRotator(-m_RootRotationY + 315.0f, 0.0f, 0.0f));
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
		m_BoxCollision->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
		m_BoxCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_BoxCollision->SetRelativeRotation(FRotator(-m_RootRotationY, 0.0f, 0.0f));
	}

	/*
	m_ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	if (m_ProjectileMovementComponent)
	{
		m_ProjectileMovementComponent->SetUpdatedComponent(m_BoardCapsuleCollision);
		m_ProjectileMovementComponent->bRotationFollowsVelocity = true;
		m_ProjectileMovementComponent->bShouldBounce = true;
	}*/
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

		m_SideValue = -FMath::Clamp(_axisValue, -1.0f, 1.0f) * m_CurrentSideAcceleration;
	}
}

// 移動処理
void ANewPlayer::UpdateMove()
{
	// 左右移動の量に応じて回転
	AddActorLocalRotation(FRotator(0.0f, 0.0f, m_SideValue));

	// 前進を加速させる
	m_CurrentForwardAcceleration += m_ForwardAcceleration;
	m_CurrentForwardAcceleration = FMath::Clamp(m_CurrentForwardAcceleration, 0.0f, 1.0f);

	// 実際に移動量を反映させる
	float speed = m_ForwardMaxSpeed * m_CurrentForwardAcceleration;

	// 前方向ベクトルの取得
	FVector meshForwardVec = m_BoardMesh->GetForwardVector();
	FVector meshRightVec = m_BoardMesh->GetRightVector();

	// プレイヤーのX方向のベロシティと角度からY方向への移動量を算出
	// プレイヤーのボードの角度を算出
	FVector playerLocation = GetActorLocation();
	FVector vectorLocation = GetActorLocation() + (meshForwardVec * 10.0f);

	double rad = FMath::Atan2(vectorLocation.Y - playerLocation.Y, vectorLocation.X - playerLocation.X);
	double deg = FMath::RadiansToDegrees(rad);

	// ルートコンポーネントが回転している分を計算に考慮する
	deg += m_RootRotationY;
	rad = FMath::DegreesToRadians(deg);

	// 横方向への移動量を算出
	float side = GetVelocity().X / FMath::Tan(rad);
	UE_LOG(LogTemp, Verbose, TEXT("[NewPlayer] Side = %f ( %f [Velocity] / %f [Tan( %f Degree)])"), side, GetVelocity().X, FMath::Tan(rad), deg);

	// 斜辺の長さを算出
	float hyp = GetVelocity().X / FMath::Sin(rad);
	UE_LOG(LogTemp, Verbose, TEXT("[NewPlayer] hyp = %f ( %f [Velocity] / %f [Sin( %f Degree)])"), hyp, GetVelocity().X, FMath::Sin(rad), deg);

	if (side > 2000.0f)
	{
		side = 2000.0f;
	}

	// 角度（cos）による速度補正
	float adjust = FMath::Cos(rad);
	static float adjustPrev;
	adjust = FMath::Pow(adjust, 3);
	adjust = FMath::Abs(adjust);
	adjust = FMath::Clamp(adjust, 0.0f, 1.0f);

	FVector newVel = GetVelocity();
	newVel.Y = -side;

	UE_LOG(LogTemp, Warning, TEXT("adjust = %f"), adjust);
	
	// ベロシティを設定
	m_BoardCapsuleCollision->SetPhysicsLinearVelocity(newVel);

	adjustPrev = adjust;

	// スプリングアームの距離調整
	float addLength = GetVelocity().X / m_ArmLengthAdjust;
	m_SpringArm->TargetArmLength = m_SpringArmLength + addLength;
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

	// 急カーブ（ドリフト）状態を設定
	DECLARE_DELEGATE_OneParam(FCustomInputDelegate, const bool);

	// ドリフトON
	InputComponent->BindAction<FCustomInputDelegate>("Drift", IE_Pressed, this, &ANewPlayer::SetDrift, true);

	// ドリフトOFF
	InputComponent->BindAction<FCustomInputDelegate>("Drift", IE_Released, this, &ANewPlayer::SetDrift, false);

	// デバッグ用--加速度リセット
	InputComponent->BindAction("Debug_Stop", IE_Pressed, this, &ANewPlayer::ResetAcceleration);
}

void ANewPlayer::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("[NewPlayer] (%s) Actor Name = %s"), *OverlappedComponent->GetName(), *OtherActor->GetName());

	for (int i = 0; i < OtherActor->Tags.Num(); ++i)
	{
		UE_LOG(LogTemp, Warning, TEXT("[NewPlayer] Actor Tag[%d] = %s"), i, *OtherActor->Tags[i].ToString());
	}


	for (int i = 0; i < OtherComp->ComponentTags.Num(); ++i)
	{
		UE_LOG(LogTemp, Warning, TEXT("[NewPlayer] Tag[%d] = %s"), i, *OtherComp->ComponentTags[i].ToString());
	}
}

void ANewPlayer::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("[NewPlayer] (%s) Actor Name = %s"), *HitComponent->GetName(), *OtherActor->GetName());

	for (int i = 0; i < OtherActor->Tags.Num(); ++i)
	{
		UE_LOG(LogTemp, Warning, TEXT("[NewPlayer] Actor Tag[%d] = %s"), i, *OtherActor->Tags[i].ToString());
	}


	for (int i = 0; i < OtherComp->ComponentTags.Num(); ++i)
	{
		UE_LOG(LogTemp, Warning, TEXT("[NewPlayer] Tag[%d] = %s"), i, *OtherComp->ComponentTags[i].ToString());
	}
}