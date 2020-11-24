//----------------------------------------------------------
// �t�@�C����		�FPlayerCharaInStage.cpp
// �T�v				�FStageSelect�Ńv���C���[�̐���
// �쐬��			�F19CU0222 �߉Ɠ�
// �X�V���e			�F2020/11/23 �쐬�@�v���C���[�̃J�����̓���
//----------------------------------------------------------


#include "StageSelectCamera.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AStageSelectCamera::AStageSelectCamera() :
	m_pSpringArm(NULL),
	m_pCamera(NULL),
	m_cameraPitchLimit(FVector2D(-50.0f, 40.0f)),
	m_cameraYawLimit(FVector2D(-150.0f, -20.0f)),
	length(5000.0f),
	isLookAtStageSelect(false),
	SelectedActor(NULL),
	canPlayAction(false),
	isPlayingAction(false),
	isHit(false)
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
	}*/

	// �J�����̃I�u�W�F�N�g�𐶐�
	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));
	if (m_pCamera != NULL) {
		// �J�������X�v�����O�A�[���ɃA�^�b�`������
		m_pCamera->SetupAttachment(RootComponent, USpringArmComponent::SocketName);
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

	if (!isPlayingAction) ObjectSelect();
	else ObjectEnlarge(DeltaTime);
}

// �y���̓o�C���h�z
void AStageSelectCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("CameraUp", this, &AStageSelectCamera::CameraVertical);
	InputComponent->BindAxis("CameraRight", this, &AStageSelectCamera::CameraHorizontal);
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

void AStageSelectCamera::ObjectSelect()
{
	FVector Start = GetActorLocation();
	FVector End = GetActorLocation() + GetActorForwardVector() * length;
	FHitResult OutHit;
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f);
	//bool isHit;
	//FCollisionQueryParams CollisionParams;
	//CollisionParams.AddIgnoredActor(this);
	if (!isPlayingAction)
		isHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_WorldStatic/*, CollisionParams*/);
	else isHit = false;
	if (isHit) {
		UE_LOG(LogTemp, Warning, TEXT("%s"), *OutHit.GetActor()->GetName());
		if (OutHit.GetActor()->ActorHasTag("StageSelect")) {
			SelectedActor = OutHit.GetActor();
			isLookAtStageSelect = true;
			//canPlayAction = true;
			isPlayingAction = true;
		}
		else isLookAtStageSelect = false;
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
// =================================================================