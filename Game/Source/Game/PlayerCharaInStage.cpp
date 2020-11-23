//----------------------------------------------------------
// �t�@�C����		�FPlayerCharaInStage.cpp
// �T�v				�FStageSelect�Ńv���C���[�̐���
// �쐬��			�F19CU0222 �߉Ɠ�
// �X�V���e			�F2020/11/23 �쐬�@�v���C���[�̃J�����̓���
//----------------------------------------------------------


#include "PlayerCharaInStage.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APlayerCharaInStage::APlayerCharaInStage()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerCharaInStage::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerCharaInStage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharaInStage::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("CameraUp", this, &APlayerCharaInStage::CameraVertical);
	InputComponent->BindAxis("CameraRight", this, &APlayerCharaInStage::CameraHorizontal);
}

// =======================�y���̓o�C���h�z=========================
// ���̓o�C���h�z�J�����ړ�:���E
void APlayerCharaInStage::CameraHorizontal(float _axisValue)
{
	cameraRoll = _axisValue;
}
// ���̓o�C���h�z�J�����ړ�:�O��
void APlayerCharaInStage::CameraVertical(float _axisValue)
{
	cameraPitch = _axisValue;
}
// =================================================================

