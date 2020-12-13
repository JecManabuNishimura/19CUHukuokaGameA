//----------------------------------------------------------
// �t�@�C����		�FStageSelectCamera.h
// �T�v				�FStageSelect�Ńv���C���[�̐���
// �쐬��			�F19CU0222 �߉Ɠ�
// �X�V���e			�F2020/11/23 �쐬�@�v���C���[�̃J�����̓���
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "StageSelect.h"
#include "StageSelectCamera.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AActor;
class AStageSelect;

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
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* m_pSpringArm;
	// �J����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		UCameraComponent* m_pCamera;*/
		// �J�����̃s�b�`�͈�(�㉺)
	UPROPERTY(EditAnywhere, Category = "Camera")
		FVector2D m_cameraPitchLimit;
	// �J�����̃��[�͈�(���E)
	UPROPERTY(EditAnywhere, Category = "Camera")
		FVector2D m_cameraYawLimit;
	UPROPERTY(EditAnywhere, Category = "LineTrace")
		float length;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FindObject")
		bool isLookAtStageSelect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FindObject")
		bool isPlayingAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FindObject")
		bool canBPPlayAction;
	UPROPERTY(EditAnywhere, Category = "FindObject")
		FVector FinalLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Found StageSelectBP")
		bool canSpawnAttachedActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Found StageSelectBP")
		bool isStageSelectAttached;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Found StartBP")
		bool canDisplayUI;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Found StartBP")
		bool isStartAttached;
	// Returning flag(after pressing "enter" key)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
		bool isReturning;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
		bool isEnterButtonPressed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
		bool isCancelButtonPressed;
	UPROPERTY(EditAnywhere, Category = "Object")
		TSubclassOf<AStageSelect> StageSelectActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
		FRotator SSCurrentRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
		int CurrentStageNum;
	UPROPERTY(EditAnywhere, Category = "Offset")
		FVector SpawnMainActorLocation;

private:
	FVector2D m_cameraRotateInput;

	void CameraMovement(float _deltaTime);

	void SelectObject();
	// �w�肳�ꂽ�I�u�W�F�N�g���J�����ɃA�^�b�`
	void AttachObject();
	// �I�u�W�F�N�g�����̈ʒu�ɖ߂�
	void ReturnObject();

private:
	bool isHit;



	// values for Timer()
	float currentTime;

	// �I�����ꂽ�A�N�^�[�̃��A�N�V����
	void ObjectEnlarge(float _deltaTime);

	void Timer(float _deltaTime);

	AActor* SelectedActor;
	AStageSelect* pStageSelect;

	// ============�y���̓o�C���h�z=============
	// �J�����ړ��F���E
	void CameraHorizontal(float _axisValue);
	// �J�����ړ��F�O��
	void CameraVertical(float _axisValue);
	// �{�^������F�m��
	void ButtonEnter();
	// �{�^������F�폜
	void ButtonCancel();
	// �{�^������F���ڂ̐؂�ւ�
	void ButtonRight();
	// �{�^������F���ڂ̐؂�ւ�
	void ButtonLeft();
	// =========================================

};
