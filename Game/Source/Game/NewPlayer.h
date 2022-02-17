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

// ���C�i���C���g���[�X�j�ɕK�v�Ȃ��̂̍\����
USTRUCT(BlueprintType)
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

	// ���C�̊J�n�ʒu�̃I�t�Z�b�g
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FVector RayStartOffset;

	// ���n���背�C��\������
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		bool IsDrawRay = true;

	// ���C�̒���
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		float RayLength = 100.0f;

	// ���n���背�C�̐F
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FColor DrawRayColor = FColor::Green;

	// ���n���背�C�̕\������
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		float DrawRayTime = 7.0f;
};

UCLASS()
class GAME_API ANewPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANewPlayer();

private:
	// �ړ��l���͏����i�L�[�{�[�h�E�p�b�h�j
	void InputKeyboard(float _axisValue);

	// �ړ�����
	void UpdateMove(const float deltaTime);

	// �������Z�b�g
	void ResetAcceleration();

	// �}�J�[�u�i�h���t�g�j��Ԃɂ���
	void SetDrift(bool _status);

	// FLinetraceInfo��FDebugRayInfo�����Ƀf�o�b�O�p�̃��C����\��
	void MyDrawDebugLine(const FLinetraceInfo& linetrace);

	// FLinetraceInfo�����Ƀ��C���΂�
	bool MyLineTrace(FLinetraceInfo& linetrace);

	// FCollisionQueryParams����ignoreActor���������g�ȊO����
	void ClearIgnoreActor(FCollisionQueryParams& collisionQueryParams);

	// ���̊p�x���Œ肷��
	void LockAngle(float& originRot, const float lockAxis, const float targetAxis, const float tolerance);

	// �f�o�b�O���[�v
	void DebugWarp();

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

	// ���E�ړ��̎擾�p
	UFUNCTION(BlueprintPure)
		float GetSideMoveValue();

	// �ڒn���Ă��邩
	UFUNCTION(BlueprintPure)
		bool GetIsLanding();

private:
	// �v���C���[�R���g���[���[
	APlayerController* m_PlayerController;

	// �R���g���[���[���ڑ�����Ă��邩
	bool m_IsSensor;

	// �h���t�g��Ԃ��ǂ���
	bool m_IsSharpcurve;

	// ���݂̏d��
	float m_CurrentGravity;

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

	// �W�����v�R�}���h�ۑ��p
	TArray<FKey> m_InputKeys;

protected:
	UPROPERTY(BlueprintReadOnly)
		// �W�����v���
		bool m_IsJump;

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
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings|Camera"
		, Meta = (DisplayName = "Spring Arm Length", ToolTip = "Original length of spring arm"))
		float m_SpringArmLength;

	// �X�v�����O�A�[���̒����̒���
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings|Camera"
		, Meta = (DisplayName = "Spring Arm Length Adjust", ToolTip = "Adjusting the length of the spring arm"))
		float m_ArmLengthAdjust;

	// �W�����v��̃W�����v��
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings|Jump"
		, Meta = (DisplayName = "Jump Power", ToolTip = "Jumping power of the jumping platform"))
		FVector m_JumpPower;

	// �ړ��\���ǂ���
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move"
		, Meta = (DisplayName = "Can Move", ToolTip = "It can be moved"))
		bool m_CanMove;

	// �{�[�h���ڒn���Ă��Ȃ����̑��x������
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move"
		, Meta = (DisplayName = "Air Speed Attenuation Value", ToolTip = "Velocity attenuation when the board is not grounded", ClampMin = "0", ClampMax = "1"))
		float m_AirSpeedAttenuationValue;

	// �ō����x
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move"
		, Meta = (DisplayName = "Max Speed", ToolTip = "Max speed"))
		float m_MaxSpeed;

	// �W�����v���̍ō����x
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move"
		, Meta = (DisplayName = "Max Speed During Jump", ToolTip = "Maximum speed during jump"))
		float m_MaxJumpSpeed;

	// �{�[�h�̕����́i��ւ̗͂̋����j
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move"
		, Meta = (DisplayName = "Float Power", ToolTip = "The floating force of the board"))
		float m_FloatPower;

	// �W�����v���̏d�͂̋���
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move"
		, Meta = (DisplayName = "Gravity During Jump", ToolTip = "Gravity during jump"))
		float m_JumpGravity;

	// �������̏d�͂̋���
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move"
		, Meta = (DisplayName = "Gravity During Falling", ToolTip = "Gravity during falling"))
		float m_FallGravity;

	// �W�����v���ɉ�����d�͂̋���
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move"
		, Meta = (DisplayName = "Gravitational Acceleration During Jump", ToolTip = "Gravitational acceleration during jump"))
		float m_AddJumpGravity;

	// �W�����v���ɉ�����d�͂̋���
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move"
		, Meta = (DisplayName = "Gravitational Acceleration During Falling", ToolTip = "Gravitational acceleration during falling"))
		float m_AddFallGravity;

	// �z�o�[�̑��x
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move"
		, Meta = (DisplayName = "Hover Lerp Speed", ToolTip = "Amount of change in hover speed"))
		float m_HoverLerpSpeed;

	// �n�ʂɑ΂���v���C���[�̊p�x�ύX�̑��x
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move"
		, Meta = (DisplayName = "Angle Lerp Speed", ToolTip = "Amount of change in angle"))
		float m_AngleLerpSpeed;

	// ���E�̈ړ���
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Move|Side"
		, Meta = (DisplayName = "Side Max Speed", ToolTip = "Side max speed"))
		float m_SideMaxSpeed;

	// ���E�̈ړ��ʂ̉�����
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Move|Side"
		, Meta = (DisplayName = "Side Acceleration", ToolTip = "Side acceleration", ClampMin = "0", ClampMax = "1"))
		float m_SideAcceleration;

	// �z�o�[�ړ��̃��C
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Ray"
		, Meta = (DisplayName = "Hover Ray"))
		FLinetraceInfo m_HoverRay;

	// �p�x�擾���C�i�O�j
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Ray"
		, Meta = (DisplayName = "Hover Angle Front Ray"))
		FLinetraceInfo m_HoverAngleFrontRay;

	// �p�x�擾���C�i���j
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Ray"
		, Meta = (DisplayName = "Hover Angle Rear Ray"))
		FLinetraceInfo m_HoverAngleRearRay;

	// �Փˉ�����C
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Ray"
		, Meta = (DisplayName = "Avoid Ray"))
		FLinetraceInfo m_AvoidRay;

	// �f�o�b�O���[�v�|�C���g
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Debug"
		, Meta = (DisplayName = "Warp Points", ToolTip = "[Debug] Actor that warps when you press a number key"))
		TArray<AActor*> m_WarpPoints;
 

public:
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		void OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};