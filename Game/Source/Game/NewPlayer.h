// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "NewPlayer.generated.h"

class USerial;
class UCameraComponent;

UCLASS()
class GAME_API ANewPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANewPlayer();

private:
	// �ړ��l���͏����i�R���g���[���[�j
	void InputSensor();

	// �ړ��l���͏����i�L�[�{�[�h�E�p�b�h�j
	void InputKeyboard(float _axisValue);

	// �ړ�����
	void UpdateMove();

	// �������Z�b�g
	void ResetAcceleration();

	// �}�J�[�u�i�h���t�g�j��Ԃɂ���
	void SetDrift(bool _status);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//	�I����
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// �R���g���[���[���ڑ�����Ă��邩
	bool m_IsSensor;

	// �h���t�g��Ԃ��ǂ���
	bool m_IsSharpcurve;

	// �O�i�̌��݂̉�����
	float m_CurrentForwardAcceleration;

	// ���E�ړ��̗�
	float m_SideValue;

	// ���E�̌��݂̉�����
	float m_CurrentSideAcceleration;

	// �}�J�[�u�̋���
	float m_CurrentSharpcurvePower;

	// �ړ��ʂ̓��͂�ۑ�����
	FVector m_UpdateValue;

public:
	// �v���C���[��MovementComponent
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UFloatingPawnMovement* m_FloatingPawnMovementComponent;

	// �{�[�h�̓����蔻��Ɏg�p����
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UCapsuleComponent* m_BoardCapsuleCollision;

	// �{�[�h�̃��b�V��
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UStaticMeshComponent* m_BoardMesh;

	// �v���C���[�̃��b�V��
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		USkeletalMeshComponent* m_PlayerMesh;

	// �X�v�����O�A�[��
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		USpringArmComponent* m_SpringArm;

	// �J����
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UCameraComponent* m_PlayerCamera;

	// �R���W����
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UBoxComponent* m_BoxCollision;

	// ���[�g�R���|�[�l���g��Y��]
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings")
		float m_RootRotationY;

	// �X�v�����O�A�[���̂��Ƃ��Ƃ̒���
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings|Camera")
		float m_SpringArmLength;

	// �X�v�����O�A�[���̒����̒���
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings|Camera")
		float m_ArmLengthAdjust;

	// �ړ��\���ǂ���
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move")
		bool m_CanMove;

	// ���E�̈ړ���
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Move|Side")
		float m_SideMaxSpeed;

	// ���E�̈ړ��ʂ̉�����
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Meta = (ClampMin = "0", ClampMax = "1"), Category = "Move|Side")
		float m_SideAcceleration;

public:
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};