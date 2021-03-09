//----------------------------------------------------------
// �t�@�C����		�FStageSelectCamera.cpp
// �T�v				�FStageSelect�Ńv���C���[�̐���
// �쐬��			�F19CU0222 �߉Ɠ�
// �X�V���e			�F2020/11/23 �쐬�@�v���C���[�̃J�����̓���
//					�F2021/01/31 �C���@SpawnActor�iStageSelectActor�j�̃R���W�������폜�iL.176�j
//----------------------------------------------------------


#include "StageSelectCamera.h"
#include "StageSelect.h"
#include "Engine.h"
#include "SensorManager.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFrameWork/Actor.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AStageSelectCamera::AStageSelectCamera() :
	//m_pSpringArm(NULL),
	//m_pCamera(NULL),
	m_cameraPitchLimit(FVector2D(-50.0f, 40.0f)),
	m_cameraYawLimit(FVector2D(-150.0f, -20.0f)),
	length(5000.0f),
	isLookAtStageSelect(false),
	SelectedActor(NULL),
	pStageSelect(NULL),
	canBPPlayAction(false),
	canDisplayUI(false),
	isPlayingAction(false),
	isHit(false),
	canSpawnAttachedActor(false),
	isEnterButtonPressed(false),
	isCancelButtonPressed(false),
	isStageSelectAttached(false),
	isReturning(false),
	isStartAttached(false),
	SpawnMainActorLocation(300.0f, 0.0f, 0.0f),
	SSCurrentRotation(0.0f, 0.0f, 0.0f),
	CurrentStageNum(1),
	currentTime(0.0f),

	sensorThreshold(5.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	// �X�v�����O�A�[���̃I�u�W�F�N�g�𐶐�
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
	}

	// �J�����̃I�u�W�F�N�g�𐶐�
	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));
	if (m_pCamera != NULL) {
		// �J�������X�v�����O�A�[���ɃA�^�b�`������
		m_pCamera->SetupAttachment(RootComponent, USpringArmComponent::SocketName);
	}*/

	if (pStageSelect == NULL) {
		pStageSelect = Cast<AStageSelect>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	}
}

// Called when the game starts or when spawned
void AStageSelectCamera::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AStageSelectCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// �J�����X�V����
	CameraMovement(DeltaTime);

	SelectObject();

	AttachObject();
	ReturnObject();

	Timer(DeltaTime);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, isHit ? TEXT("TRUE") : TEXT("FALSE"));

	if (USensorManager::GetIsOpen() == true)
	{
		bool left;
		bool right;

		USensorManager::GetSensorButton(left, right);

		if (left)
		{
			ButtonCancel();
		}
		else if (right)
		{
			ButtonEnter();
		}

		FVector sensorData = USensorManager::GetSensorDataRaw(left, right);
		float sensorDataX = sensorData.X;
		static bool isMove = false;

		UE_LOG(LogTemp, Warning, TEXT("%f"), sensorData.X);
		if (isMove == false && sensorDataX > sensorThreshold)
		{
			ButtonRight();
			isMove = true;
		}
		else if (isMove == false && sensorDataX < -sensorThreshold)
		{
			ButtonLeft();
			isMove = true;
		}
		else if (sensorDataX <= sensorThreshold && sensorDataX >= -sensorThreshold)
		{
			isMove = false;
		}
	}
}

// �y���̓o�C���h�z
void AStageSelectCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("CameraUp", this, &AStageSelectCamera::CameraVertical);
	InputComponent->BindAxis("CameraRight", this, &AStageSelectCamera::CameraHorizontal);
	InputComponent->BindAction("Enter", IE_Pressed, this, &AStageSelectCamera::ButtonEnter);
	InputComponent->BindAction("Back", IE_Pressed, this, &AStageSelectCamera::ButtonCancel);
	InputComponent->BindAction("SelectRight", IE_Pressed, this, &AStageSelectCamera::ButtonRight);
	InputComponent->BindAction("SelectLeft", IE_Pressed, this, &AStageSelectCamera::ButtonLeft);
}

// �J�����X�V����
void AStageSelectCamera::CameraMovement(float _deltaTime)
{
	// ���݂�FPS�𑪒�
	float fps = 1.0f / _deltaTime;

	// �����������Ă��A��葬�x�ŃJ���������悤�ɕ␳
	float rotateCorrection = 60.0f / fps;

	// �J�����̐V�����p�x�����߂�
	// ���݂̊p�x���擾
	FRotator NewRotation = GetActorRotation();

	// Yaw�Ɋւ��ẮA���E�̐����p�x�͈͓̔��Ő؂�
	NewRotation.Yaw = FMath::Clamp(NewRotation.Yaw + (m_cameraRotateInput.X * rotateCorrection), m_cameraYawLimit.X, m_cameraYawLimit.Y);

	// Pitch�Ɋւ��ẮA�㉺�̐����p�x�͈͓̔��Ő؂�
	NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + (m_cameraRotateInput.Y * rotateCorrection), m_cameraPitchLimit.X, m_cameraPitchLimit.Y);

	// �V�����p�x�𔽉f
	SetActorRotation(NewRotation);

}

void AStageSelectCamera::SelectObject()
{
	FVector Start = GetActorLocation();
	FVector End = GetActorLocation() + GetActorForwardVector() * length;
	FHitResult OutHit;
	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f);
	//bool isHit;
	//FCollisionQueryParams CollisionParams;
	//CollisionParams.AddIgnoredActor(this);
	if (!isReturning && !isStageSelectAttached && !isStartAttached)
		isHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_WorldStatic/*, CollisionParams*/);
	else isHit = false;
	if (isHit) {
		//UE_LOG(LogTemp, Warning, TEXT("%s"), *OutHit.GetActor()->GetName());
		if (OutHit.GetActor()->ActorHasTag("StageSelect")) {
			//UE_LOG(LogTemp, Warning, TEXT("StageSelectHasHit"));
			SelectedActor = OutHit.GetActor();
			canBPPlayAction = true;
			isStageSelectAttached = true;
		}
		else if (OutHit.GetActor()->ActorHasTag("Start")) {
			canDisplayUI = true;
			isStartAttached = true;
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("isHit:%s"), isHit ? TEXT("TRUE") : TEXT("FALSE"));

}

// �w�肳�ꂽ�I�u�W�F�N�g���J�����ɃA�^�b�`
void AStageSelectCamera::AttachObject()
{
	AStageSelect* SpawnedMainActor = NULL;
	// �w�肳�ꂽ�I�u�W�F�N�g�����ł��ꂽ�ꍇ
	if (canSpawnAttachedActor) {
		if (StageSelectActor != NULL) {
			if (!isReturning) {
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnedMainActor = GetWorld()->SpawnActor<AStageSelect>(StageSelectActor, ActorSpawnParams);
				SpawnedMainActor->SetActorRelativeLocation(SpawnMainActorLocation);
				SpawnedMainActor->SetActorRelativeRotation(SSCurrentRotation + FRotator(0.0f, 90.0f, 0.0f));
				SpawnedMainActor->SetActorRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));
				SpawnedMainActor->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
				SpawnedMainActor->currentStage = CurrentStageNum;
				isLookAtStageSelect = true;
				pStageSelect = SpawnedMainActor;
			}
			// �����ł�Blueprint��ō쐬����(StageSelectCameraBP)
			if (isReturning) {}
			canSpawnAttachedActor = false;
		}

	}
	if (canDisplayUI) {
		isStartAttached = true;
	}
}

// �I�u�W�F�N�g�����̈ʒu�ɖ߂�
void AStageSelectCamera::ReturnObject()
{
	if (isEnterButtonPressed) {
		//canBPPlayAction = true;
		isReturning = true;
		if (pStageSelect != NULL) {
			switch (pStageSelect->currentStage)
			{
			case 1:
			case 5:
				SSCurrentRotation = FRotator(0.0f, 0.0f, 0.0f);
				break;
			case 2:
			case 6:
				SSCurrentRotation = FRotator(0.0f, 90.0f, 0.0f);
				break;
			case 3:
			case 7:
				SSCurrentRotation = FRotator(0.0f, 180.0f, 0.0f);
				break;
			case 4:
			case 8:
				SSCurrentRotation = FRotator(0.0f, -90.0f, 0.0f);
				break;
			default:
				break;
			}
			CurrentStageNum = pStageSelect->currentStage;
			isStageSelectAttached = false;
			isLookAtStageSelect = false;
			pStageSelect->Destroy();
		}
	}
	if (isCancelButtonPressed) {
		//canBPPlayAction = true;
		isReturning = true;
		if (pStageSelect != NULL) {
			CurrentStageNum = pStageSelect->currentStage;
			isStageSelectAttached = false;
			isLookAtStageSelect = false;
			pStageSelect->Destroy();
		}
	}
}

// �I�����ꂽ�A�N�^�[�̃��A�N�V����
void AStageSelectCamera::ObjectEnlarge(float _deltaTime)
{
	/*if (SelectedActor->ActorHasTag("StageSelect")) FinalLocation = FVector(-1080.0f, -260.0f, 1792.0f);

	if (SelectedActor != NULL) {
		FVector currentLocation = SelectedActor->GetActorLocation();
	}*/
}

void AStageSelectCamera::Timer(float _deltaTime)
{
	// button�t���O�̐���F0.5�b��A�t���O��false(���̏�Ԃɖ߂�)
	if (isEnterButtonPressed || isCancelButtonPressed) {
		currentTime += _deltaTime * 1.0f;
		if (currentTime >= 0.5f) {
			isEnterButtonPressed = false;
			isCancelButtonPressed = false;
			currentTime = 0.0f;
		}
	}

	// �I�u�W�F�N�g�����ɖ߂�������J�����������Ȃ�����
	if (isReturning) {
		currentTime += _deltaTime * 1.0f;
		if (currentTime >= 1.0f) {
			isReturning = false;
			currentTime = 0.0f;
			if (isStageSelectAttached) isStageSelectAttached = false;
			if (isLookAtStageSelect) isLookAtStageSelect = false;
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("%s"), isEnterButtonPressed ? TEXT("TRUE") : TEXT("FALSE"));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, isEnterButtonPressed ? TEXT("true") : TEXT("false"));
}

// =======================�y���̓o�C���h�z=========================
// ���̓o�C���h�z�J�����ړ�:���E(Pitch)
void AStageSelectCamera::CameraHorizontal(float _axisValue)
{
	m_cameraRotateInput.X = _axisValue;
}
// ���̓o�C���h�z�J�����ړ�:�O��(Yaw)
void AStageSelectCamera::CameraVertical(float _axisValue)
{
	m_cameraRotateInput.Y = _axisValue;
}
void AStageSelectCamera::ButtonEnter()
{
	if (!isEnterButtonPressed && (isStageSelectAttached || isStartAttached)) {
		isEnterButtonPressed = true;
	}
}
void AStageSelectCamera::ButtonCancel()
{
	if (!isCancelButtonPressed && (isStageSelectAttached || isStartAttached)) {
		isCancelButtonPressed = true;
	}
}
void AStageSelectCamera::ButtonRight()
{
	if (pStageSelect != NULL) {
		pStageSelect->SelectRight();
	}
}
void AStageSelectCamera::ButtonLeft()
{
	if (pStageSelect != NULL) {
		pStageSelect->SelectLeft();
	}
}
// =================================================================