//----------------------------------------------------------
// �t�@�C����		�FStageSelect.cpp
// �T�v				�F�X�e�[�W��ʂŊe�X�e�[�W�̐؂�ւ�
// �쐬��			�F19CU0222 �߉Ɠ�
// �X�V���e			�F2020/10/27
//					�F
//----------------------------------------------------------

#include "StageSelect.h"
#include "Components/WidgetComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AStageSelect::AStageSelect() :
	canEnter(true),
	anyPressed(true),
	turnRight(true),
	scalingTime(1.0f),
	scalingRate(0.75f),
	finishSmall(false),
	rotatingTime(88.5f),
	rotatingRate(0.0f),
	rotatingRateMax(1.0f),
	finishRotating(false),
	startRotating(false),
	nextRotation(0.0f, 0.0f, 0.0f),
	currentStage(1),
	pressCount(0),
	stageAmount(4)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//m_MainSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MainSceneComponent"));
	//RootComponent = m_MainSceneComponent;

	m_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	if (m_BoxComponent != NULL) {
		//m_BoxComponent->SetupAttachment(m_SceneComponent);
		RootComponent = m_BoxComponent;
		//m_BoxComponent->SetRelativeScale3D(FVector(20.0f, 20.0f, 20.0f));
	}

	m_SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	m_SceneComponent->SetupAttachment(RootComponent);

	m_Stage01 = CreateDefaultSubobject<UWidgetComponent>(TEXT("stage01"));
	if (m_Stage01 != NULL) {
		m_Stage01->SetupAttachment(m_SceneComponent);
	}
	m_Stage01->SetDrawSize(FVector2D(1280.0f, 720.0f));
	m_Stage01->SetRelativeLocationAndRotation(FVector(-700.0f, 0.0f, 0.0f), FRotator(0.0f, -180.0f, 0.0f));

	m_Stage02 = CreateDefaultSubobject<UWidgetComponent>(TEXT("stage02"));
	if (m_Stage02 != NULL) {
		m_Stage02->SetupAttachment(m_SceneComponent);
	}
	m_Stage02->SetDrawSize(FVector2D(1280.0f, 720.0f));
	m_Stage02->SetRelativeLocationAndRotation(FVector(0.0f, 700.0f, 0.0f), FRotator(0.0f, 90.0f, 0.0f));

	m_Stage03 = CreateDefaultSubobject<UWidgetComponent>(TEXT("stage03"));
	if (m_Stage03 != NULL) {
		m_Stage03->SetupAttachment(m_SceneComponent);
	}
	m_Stage03->SetDrawSize(FVector2D(1280.0f, 720.0f));
	m_Stage03->SetRelativeLocation(FVector(700.0f, 0.0f, 0.0f));

	m_Stage04 = CreateDefaultSubobject<UWidgetComponent>(TEXT("stage04"));
	if (m_Stage04 != NULL) {
		m_Stage04->SetupAttachment(m_SceneComponent);
		m_Stage04->SetDrawSize(FVector2D(1280.0f, 720.0f));
		m_Stage04->SetRelativeLocationAndRotation(FVector(0.0f, -700.0f, 0.0f), FRotator(0.0f, -90.0f, 0.0f));

	}

	/*m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("m_pSpringArm"));
	if (m_pSpringArm != NULL) {
		m_pSpringArm->SetupAttachment(RootComponent);

		// �A�[���̒�����ݒ�
		// �J�����̃R���W�����e�X�g���s������ݒ�
		m_pSpringArm->bDoCollisionTest = false;
		// �J�����Ǐ]���O���g������ݒ�
		m_pSpringArm->bEnableCameraLag = true;
		// �J�����Ǐ]���O�̑��x��ݒ�
		m_pSpringArm->CameraLagSpeed = 10.0f;
		// �J������]���O���g������ݒ�
		m_pSpringArm->bEnableCameraRotationLag = true;
		// �J������]���O�̑��x��ݒ�
		m_pSpringArm->CameraRotationLagSpeed = 10.0f;

		m_pSpringArm->TargetArmLength = 1280.0f * 2.0f;
	}

	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));
	if (m_pCamera != NULL) {
		// �J�������X�v�����O�A�[���ɃA�^�b�`������
		m_pCamera->SetupAttachment(m_pSpringArm, USpringArmComponent::SocketName);
	}*/
}

// Called when the game starts or when spawned
void AStageSelect::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStageSelect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ���E�{�^���������ꂽ��A�X�e�[�W�؂�ւ���������s����
	if (!anyPressed)
		StageChanging(DeltaTime);

	// ���ݑI�����ꂽ�X�e�[�W
	CurrentStage();
}

// Called to bind functionality to input
void AStageSelect::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("SelectRight", IE_Pressed, this, &AStageSelect::SelectRight);
	InputComponent->BindAction("SelectLeft", IE_Pressed, this, &AStageSelect::SelectLeft);
	InputComponent->BindAction("Enter", IE_Pressed, this, &AStageSelect::SelectConfirm);
}

void AStageSelect::SelectRight()
{
	if (anyPressed) {
		//UE_LOG(LogTemp, Warning, TEXT("Right!!"));
		turnRight = true;
		anyPressed = false;
		currentStage += 1;
		if (currentStage >= stageAmount + 1) currentStage = 1;
		pressCount += 1;
	}
	/*if (canEnter) {
		currentStage += 1;
		if (currentStage >= stageAmount + 1) currentStage = 1;
		pressCount += 1;
	}*/
}

void AStageSelect::SelectLeft()
{
	if (anyPressed) {
		//UE_LOG(LogTemp, Warning, TEXT("Right!!"));
		turnRight = false;
		anyPressed = false;
		currentStage -= 1;
		if (currentStage <= 0) currentStage = stageAmount;
		pressCount += 1;
	}
	/*if (canEnter) {
		currentStage -= 1;
		if (currentStage <= 0) currentStage = stageAmount;
		pressCount += 1;
	}*/
	
}

void AStageSelect::SelectConfirm()
{
	UGameplayStatics::OpenLevel(this, "Game");

}

void AStageSelect::StageChanging(float _deltaTime)
{
	FRotator presRotation = m_SceneComponent->GetRelativeRotation();
	FVector presScale = m_SceneComponent->GetRelativeScale3D();

	//UE_LOG(LogTemp, Warning, TEXT("TurnRight:%s"), turnRight ? TEXT("TRUE") : TEXT("FALSE"));
	//UE_LOG(LogTemp, Warning, TEXT("anyPressed:%s"), anyPressed ? TEXT("TRUE") : TEXT("FALSE"));
	//UE_LOG(LogTemp, Warning, TEXT("preScale:%s"), *FString::SanitizeFloat(presScale.SizeSquared()));
	//UE_LOG(LogTemp, Warning, TEXT("finishSmall:%s"), finishSmall ? TEXT("TRUE") : TEXT("FALSE"));
	//UE_LOG(LogTemp, Warning, TEXT("finishRotating:%s"), finishRotating ? TEXT("TRUE") : TEXT("FALSE"));
	//UE_LOG(LogTemp, Warning, TEXT("presRotation:%s"), *presRotation.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("rotatingRate:%s"), *FString::SanitizeFloat(rotatingRate));

	if (turnRight) {
		// �k���J�n
		/*if (!finishSmall && presScale.SizeSquared() >= scalingRate) {
			m_SceneComponent->SetRelativeScale3D(
				FVector(presScale.X -= scalingTime * _deltaTime,
				presScale.Y -= scalingTime * _deltaTime,
				presScale.Z -= scalingTime * _deltaTime));
		}
		else if (!finishSmall) {
			finishSmall = true;
		}*/

		// ��]�J�n
		if (/*finishSmall &&*/ !finishRotating && rotatingRate < rotatingRateMax * FMath::Abs(pressCount)) {
			canEnter = true;
			startRotating = true;
			rotatingRate += _deltaTime;
			m_SceneComponent->SetRelativeRotation(
				FRotator(presRotation.Pitch, presRotation.Yaw += rotatingTime * _deltaTime, presRotation.Roll));
		}
		else if (/*finishSmall &&*/ rotatingRate >= rotatingRateMax) {
			finishRotating = false;
			startRotating = false;
			rotatingRate = 0.0f;
			pressCount = 0.0f;
			//canEnter = false;
			anyPressed = true;
		}

		// �g��J�n
		/*if (finishRotating) {
			if (presScale.SizeSquared() <= 3.0f) {
				m_SceneComponent->SetRelativeScale3D(
					FVector(presScale.X += scalingTime * _deltaTime,
					presScale.Y += scalingTime * _deltaTime,
					presScale.Z += scalingTime * _deltaTime));
			}
			else {
				m_SceneComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
				canEnter = true;
				anyPressed = true;
				finishSmall = false;
				finishRotating = false;
			}
		}*/
	}
	if (!turnRight) {
		// �k���J�n
		/*if (!finishSmall && presScale.SizeSquared() >= scalingRate) {
			m_SceneComponent->SetRelativeScale3D(
				FVector(presScale.X -= scalingTime * _deltaTime,
				presScale.Y -= scalingTime * _deltaTime,
				presScale.Z -= scalingTime * _deltaTime));
		}
		else if (!finishSmall) {
			finishSmall = true;
		}*/
		
		// ��]�J�n
		if (/*finishSmall &&*/ !finishRotating && rotatingRate <= rotatingRateMax * (float) pressCount) {
			canEnter = true;
			startRotating = true;
			rotatingRate += _deltaTime;
			m_SceneComponent->SetRelativeRotation(
				FRotator(presRotation.Pitch, presRotation.Yaw -= rotatingTime * _deltaTime, presRotation.Roll));
		}
		else if (/*finishSmall &&*/ rotatingRate > rotatingRateMax) {
			finishRotating = false;
			startRotating = false;
			rotatingRate = 0.0f;
			pressCount = 0.0f;
			//canEnter = false;
			anyPressed = true;
		}

		// �g��J�n
		/*if (finishRotating) {
			if (presScale.SizeSquared() <= 3.0f) {
				m_SceneComponent->SetRelativeScale3D(
					FVector(presScale.X += scalingTime * _deltaTime,
					presScale.Y += scalingTime * _deltaTime,
					presScale.Z += scalingTime * _deltaTime));
			}
			else {
				m_SceneComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
				canEnter = true;
				anyPressed = true;
				finishSmall = false;
				finishRotating = false;
			}
		}*/
	}
}

// ���ݑI�����ꂽ�X�e�[�W
void AStageSelect::CurrentStage()
{

}
