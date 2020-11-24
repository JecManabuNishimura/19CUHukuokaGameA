//----------------------------------------------------------
// �t�@�C����		�FPlayerCharaInStage.h
// �T�v				�FStageSelect�Ńv���C���[�̐���
// �쐬��			�F19CU0222 �߉Ɠ�
// �X�V���e			�F2020/11/23 �쐬�@�v���C���[�̃J�����̓���
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "StageSelectCamera.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class GAME_API AStageSelectCamera : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AStageSelectCamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// �X�v�����O�A�[��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* m_pSpringArm;
	// �J����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		UCameraComponent* m_pCamera;
	// �J�����̃s�b�`�͈�(�㉺)
	UPROPERTY(EditAnywhere, Category = "Camera")
		FVector2D m_cameraPitchLimit;
	// �J�����̃��[�͈�(���E)
	UPROPERTY(EditAnywhere, Category = "Camera")
		FVector2D m_cameraYawLimit;
	UPROPERTY(EditAnywhere, Category = "LineTrace")
		float length;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action")
		bool isLookAtStageSelect;
	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "Action")
		bool isPlayingAction;
	UPROPERTY(EditAnywhere, Category = "Action")
		FVector FinalLocation;


private:
	FVector2D m_cameraRotateInput;

	void CameraMovement(float _deltaTime);

	void ObjectSelect();

	// �I�����ꂽ�A�N�^�[�̃��A�N�V����
	void ObjectEnlarge(float _deltaTime);

	AActor* SelectedActor;

	// ============�y���̓o�C���h�z=============
	// ���̓o�C���h�z�J�����ړ�:���E
	void CameraHorizontal(float _axisValue);
	// ���̓o�C���h�z�J�����ړ�:�O��
	void CameraVertical(float _axisValue);
	// =========================================

};
