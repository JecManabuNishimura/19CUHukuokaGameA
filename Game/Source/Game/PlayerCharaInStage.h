//----------------------------------------------------------
// �t�@�C����		�FPlayerCharaInStage.h
// �T�v				�FStageSelect�Ńv���C���[�̐���
// �쐬��			�F19CU0222 �߉Ɠ�
// �X�V���e			�F2020/11/23 �쐬�@�v���C���[�̃J�����̓���
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharaInStage.generated.h"

UCLASS()
class GAME_API APlayerCharaInStage : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharaInStage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	float cameraRoll;
	float cameraPitch;
	// ============�y���̓o�C���h�z=============
	// ���̓o�C���h�z�J�����ړ�:���E
	void CameraHorizontal(float _axisValue);
	// ���̓o�C���h�z�J�����ړ�:�O��
	void CameraVertical(float _axisValue);
	// =========================================

};
