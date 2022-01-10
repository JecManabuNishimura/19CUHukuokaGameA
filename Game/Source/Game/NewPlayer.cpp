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
		// 加速する方向の取得
		float accelDir = (_axisValue > 0.0f) ? 1.0f : -1.0f;

		// ドリフトかどうか
		if (m_IsSharpcurve)
		{
			// 加速する
		}
		else
		{
			// 加速する
			m_CurrentSideAcceleration += m_SideAcceleration * accelDir;

			// 最大速度にクランプする
			m_CurrentSideAcceleration = FMath::Clamp(m_CurrentSideAcceleration, -m_SideMaxSpeed, m_SideMaxSpeed);
		}

		m_SideValue = FMath::Clamp(_axisValue, -1.0f, 1.0f) * m_CurrentSideAcceleration;
	}
}

// 移動処理
void ANewPlayer::UpdateMove()
{
	// 左右移動の量に応じて回転
	AddActorLocalRotation(FRotator(0.0f, m_SideValue, 0.0f));
	m_SideValue = 0.0f;

	// 前進を加速させる
	m_CurrentForwardAcceleration += m_ForwardAcceleration;
	m_CurrentForwardAcceleration = FMath::Clamp(m_CurrentForwardAcceleration, 0.0f, 1.0f);

	// 実際に移動量を反映させる
	float speed = m_ForwardMaxSpeed * m_CurrentForwardAcceleration;

	// 前方向ベクトルの取得
	FVector forwardVec = GetActorForwardVector();

	FVector meshVec = m_BoardMesh->GetForwardVector();

	UE_LOG(LogTemp, Warning, TEXT("Forward Vec = %s"), *forwardVec.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Board Vec = %s"), *meshVec.ToString());

	// 前進加速量に応じて左右移動のベクトルを制御
	forwardVec.X *= m_CurrentForwardAcceleration;

	// Z軸のベクトルが反転しているので値も反転させて速度を乗算
	forwardVec.Z *= -speed;

	AddActorLocalOffset(forwardVec);
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
}