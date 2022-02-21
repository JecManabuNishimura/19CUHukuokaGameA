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

// �g���b�N�̍ۂɈȏ�E�ȉ����w�肷��p
UENUM(BlueprintType)
enum class EComp : uint8
{
	Auto,
	OrMore,
	MoreThan,
	LessThan,
	OrLess,
};

// �g���b�N�̍ۂ̍��X�e�B�b�N�̎��̕���
UENUM(BlueprintType)
enum class EInputAxis : uint8
{
	X,
	Y,
};

// �g���b�N�̎��
UENUM(BlueprintType)
enum class ETrickType : uint8
{
	Trick1,
	Trick2,
	Trick3,
	Trick4,
	None,
};

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

	void SetStart(UPrimitiveComponent* CurrentComp)
	{
		this->rayStart = CurrentComp->GetComponentLocation() + (CurrentComp->GetForwardVector() * this->RayStartOffset.X) + (CurrentComp->GetRightVector() * this->RayStartOffset.Y) + (CurrentComp->GetUpVector() * this->RayStartOffset.Z);
	}

	void SetEnd(UPrimitiveComponent* CurrentComp)
	{
		this->rayEnd = this->rayStart - (CurrentComp->GetUpVector() * this->RayLength);
	}
};

// �g���b�N�Ɠ��͂̊֘A�t��
USTRUCT(BlueprintType)
struct FTrickBind
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		EInputAxis AxisDirection = EInputAxis::X;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Meta = (ClampMin = "-1", ClampMax = "1"))
		float InputAxis = 0.0f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		EComp ValueComparisonType = EComp::OrMore;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		ETrickType Trick = ETrickType::None;
};

UCLASS()
class GAME_API ANewPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANewPlayer();

private:
	// X���̓��͏���
	void InputAxisX(const float _axisValue);

	// Y���̓��͏���
	void InputAxisY(const float _axisValue);

	// �z�o�[����
	void Hover(const float _deltaTime);

	// 2�̃��C����v���C���[�̊p�x��ύX
	void SetRotationWithRay(const float _deltaTime);

	// �ړ�����
	void UpdateMove(const float _deltaTime);

	// �g���b�N�{�^���̓��͂��󂯕t����
	void SetTrick(const bool _status);

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

	// �ǂ̃g���b�N�����߂Ă��邩
	UFUNCTION(BlueprintPure)
		ETrickType GetTrickType();

private:
	// �v���C���[�R���g���[���[
	APlayerController* m_PlayerController;

	// �g���b�N�{�^����������Ă��邩
	bool m_IsTrick;

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
		
	// ���͂̕ۑ�
	FVector2D m_InputAxisValue;

	// ���݂̃g���b�N
	ETrickType m_CurrentTrick;

protected:
	UPROPERTY(BlueprintReadOnly)
		// �W�����v���
		bool m_IsJump;

public:

	// �v���C���[��MovementComponent
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UFloatingPawnMovement* m_FloatingPawnMovementComponent;

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
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Camera"
		, Meta = (DisplayName = "Spring Arm Length", ToolTip = "Original length of spring arm"))
		float m_SpringArmLength;

	// �X�v�����O�A�[���̒����̒���
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Camera"
		, Meta = (DisplayName = "Adjust Speed Spring Arm Length", ToolTip = "Amount to adjust the length of the spring arm when the speed is high"))
		float m_ArmLengthAdjust;

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
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move|Gravity"
		, Meta = (DisplayName = "Gravity During Jump", ToolTip = "Gravity during jump"))
		float m_JumpGravity;

	// �������̏d�͂̋���
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move|Gravity"
		, Meta = (DisplayName = "Gravity During Falling", ToolTip = "Gravity during falling"))
		float m_FallGravity;

	// �W�����v���ɉ�����d�͂̋���
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move|Gravity"
		, Meta = (DisplayName = "Gravitational Acceleration During Jump", ToolTip = "Gravitational acceleration during jump"))
		float m_AddJumpGravity;

	// �W�����v���ɉ�����d�͂̋���
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move|Gravity"
		, Meta = (DisplayName = "Gravitational Acceleration During Falling", ToolTip = "Gravitational acceleration during falling"))
		float m_AddFallGravity;

	// �z�o�[�ʂ̕ύX���鑬�x
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

	// �g���b�N�̃o�C���h
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Trick"
		, Meta = (DisplayName = "Trick Binding"))
		TArray<FTrickBind> m_TrickBind;

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