//----------------------------------------------------------
// �t�@�C����		�FPlayerCharaInStage.cpp
// �T�v				�FStageSelect�Ńv���C���[�̐���
// �쐬��			�F19CU0222 �߉Ɠ�
// �X�V���e			�F2020/11/23 �쐬�@�v���C���[�̃J�����̓���
//----------------------------------------------------------


#include "StageSelectCamera.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AStageSelectCamera::AStageSelectCamera() :
	m_pSpringArm(NULL),
	m_pCamera(NULL)
{
	PrimaryActorTick.bCanEverTick = true;

	// �X�v�����O�A�[���̃I�u�W�F�N�g�𐶐�
	m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("m_pSpringArm"));
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
		m_pCamera->SetupAttachment(m_pSpringArm, USpringArmComponent::SocketName);
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

}

// Called to bind functionality to input
void AStageSelectCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("CameraUp", this, &AStageSelectCamera::CameraVertical);
	InputComponent->BindAxis("CameraRight", this, &AStageSelectCamera::CameraHorizontal);
}

// =======================�y���̓o�C���h�z=========================
// ���̓o�C���h�z�J�����ړ�:���E
void AStageSelectCamera::CameraHorizontal(float _axisValue)
{
	cameraRoll = _axisValue;
}
// ���̓o�C���h�z�J�����ړ�:�O��
void AStageSelectCamera::CameraVertical(float _axisValue)
{
	cameraPitch = _axisValue;
}
// =================================================================