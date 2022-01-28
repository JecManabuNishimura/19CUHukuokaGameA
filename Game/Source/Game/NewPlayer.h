// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "NewPlayer.generated.h"

class USerial;
class UCameraComponent;

// �f�o�b�O�p���C�̐ݒ�̍\����
USTRUCT(BlueprintType)
struct FDebugRayInfo
{
	GENERATED_USTRUCT_BODY();

	// ���n���背�C��\������
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		bool IsDrawRay = true;

	// ���n���背�C�̐F
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FColor DrawRayColor = FColor::Green;

	// ���n���背�C�̕\������
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		float DrawRayTime = 7.0f;
};

// ���C�i���C���g���[�X�j�ɕK�v�Ȃ��̂̍\����
USTRUCT()
struct FLinetraceInfo
{
	GENERATED_USTRUCT_BODY();

	// ���C�̂�����������
	FHitResult hitResult;

	// ���C�̊J�n�ʒu
	FVector rayStart = FVector::ZeroVector;

	// ���C�̏I���ʒu
	FVector rayEnd = FVector::ZeroVector;

	// �ǂ�Ƀ��C�������邩��ݒ�
	FCollisionObjectQueryParams objectQueueParam = FCollisionObjectQueryParams::AllObjects;

	// ���O������̂�ݒ�
	FCollisionQueryParams collisionQueueParam = FCollisionQueryParams::DefaultQueryParam;
};

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

	// FLinetraceInfo��FDebugRayInfo�����Ƀf�o�b�O�p�̃��C����\��
	void MyDrawDebugLine(const FLinetraceInfo& linetrace, const FDebugRayInfo& ray);

	// FLinetraceInfo�����Ƀ��C���΂�
	bool MyLineTrace(FLinetraceInfo& linetrace);

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

	// �W�����v���
	bool m_IsJump;

	// �O�i�̌��݂̉�����
	float m_CurrentForwardAcceleration;

	// ���E�ړ��̗�
	float m_SideValue;

	// ���E�̌��݂̉�����
	float m_CurrentSideAcceleration;

	// �}�J�[�u�̋���
	float m_CurrentSharpcurvePower;

	// �{�[�h���ڒn���Ă��Ȃ����̑��x������
	float m_AirSpeedAttenuation;

	// �ړ��ʂ̓��͂�ۑ�����
	FVector m_UpdateValue;
	
	// �i�s�����𐧌�
	FVector m_BaseForwardVector;

	// ���n���背�C�̏��
	FLinetraceInfo m_GroundRayInfo;

	// �e�[�����n���C�̏��
	FLinetraceInfo m_TailLandingRayInfo;

public:
	// �v���C���[��MovementComponent
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UFloatingPawnMovement* m_FloatingPawnMovementComponent;

	/*
	// �{�[�h�̓����蔻��Ɏg�p����i�J�v�Z���j
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UCapsuleComponent* m_BoardCapsuleCollision;

	// �{�[�h�̓����蔻��Ɏg�p����i�{�b�N�X�j
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UBoxComponent* m_BoardBoxCollision;
	*/

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

	// �X�v�����O�A�[���̂��Ƃ��Ƃ̒���
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings|Camera")
		float m_SpringArmLength;

	// �X�v�����O�A�[���̒����̒���
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings|Camera")
		float m_ArmLengthAdjust;

	// �ڒn���背�C�̊J�n�ʒu�̃I�t�Z�b�g
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings|Ground")
		FVector m_GroundRayOffset;

	// �ڒn���背�C�̒���
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings|Ground")
		float m_GroundRayLength;

	// �W�����v��̃W�����v��
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings|Jump")
		FVector m_JumpPower;

	// �e�[�����n���C�̊J�n�ʒu�̃I�t�Z�b�g
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings|Jump")
		FVector m_TailLandingRayOffset;

	// �e�[�����n���C�̒���
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings|Jump")
		float m_TailLandingRayLength;

	// �ړ��\���ǂ���
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move")
		bool m_CanMove;

	// �{�[�h���ڒn���Ă��Ȃ����̑��x������
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Meta = (ClampMin = "0", ClampMax = "1"), Category = "Move")
		float m_AirSpeedAttenuationValue;

	// �ʏ펞�̏d�S
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Move")
		FVector m_DefaultCenterOfMass;

	// �W�����v���̏d�S
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Move")
		FVector m_JumpCenterOfMass;

	// ���E�̈ړ���
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Move|Side")
		float m_SideMaxSpeed;

	// ���E�̈ړ��ʂ̉�����
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Meta = (ClampMin = "0", ClampMax = "1"), Category = "Move|Side")
		float m_SideAcceleration;

	// ���n���背�C�̐ݒ�
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Debug")
		FDebugRayInfo m_GroundDebug;

	// ���n���背�C��\������
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Debug")
		FDebugRayInfo m_TailLandingDebug;

public:
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};