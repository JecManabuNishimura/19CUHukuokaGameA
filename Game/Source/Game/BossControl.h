/*
 *--------------------------------------------------------
 * �{�X�̈ړ�
 * 2020/11/08 �n粗��� �쐬
 *--------------------------------------------------------
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BossControl.generated.h"

class ABossWaveControl;

// �{�X�̍U���̎��
UENUM(BlueprintType)
enum class BossAttack : uint8
{
	LightningStrike,	// �����U��
	Wave,				// �E�F�[�u�U��
	Rush,				// �ːi
};

UCLASS()
class GAME_API ABossControl : public ACharacter
{
	GENERATED_BODY()

public:
	ABossControl();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// �{�X�̏��
	enum BossState
	{
		CatchUp,		// �ǂ���
		Run,			// �ړ�
		RunAfterAttack,	// �U����̈ړ��i�W�����v�p�x�ɂ���j
		RunAfterRush,	// �ːi��̈ړ��i�W�����v�p�x�ɂ���j
		Jump_In,		// �W�����v�i�O�����j
		Jump_Out,		// �W�����v�i�����O�j
		Attack,			// �U��
		Neigh,			// ���ȂȂ�
		Wait,			// �~�܂��ē���U��
	};

	// �v���C�x�[�g�ϐ�
private:

	//-TEMP---------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnyWhere, Category = "TEMP", Meta = (ToolTip = "Temporarily specify the type of boss attack."))
		// �E�F�[�u�U�����̈ړ����x
		BossAttack m_TEMP_BOSS_ATTACK;
	
	UPROPERTY(EditAnyWhere, Category = "TEMP", Meta = (ToolTip = "Temporary fence position."))
		// �E�F�[�u�U�����̈ړ����x
		float m_TEMP_FENCE_POS;
	//--------------------------------------------------------------------------------------------------------------------------------------------

	// �{�X�S�ʂɎg�p����v���p�e�B------------------------------------------------------------------------------------------

		// �ړ����x
	UPROPERTY(EditAnyWhere, Meta = (ToolTip = "Moving Speed."))
		float m_MoveSpeed;

	// ����A�j���[�V�����̍Đ����x
	UPROPERTY(EditAnyWhere, Meta = (ToolTip = "Playback speed of running animation."))
		float m_RunningAnimationRateSpeed;

	// �O���Ɠ������o���肷�鎞��
	UPROPERTY(EditAnyWhere, Meta = (ToolTip = "Time to go in and out of the outside and inside."))
		float m_InsideMigrationTime;

	UPROPERTY(EditAnyWhere, Meta = (ClampMin = "0", ClampMax = "1", ToolTip = "Percentage of time to start spinning while moving."))
		// �ړ����ɉ�]���n�߂鎞�Ԃ̊����@0�Ȃ炷���ɊJ�n�A 1�Ȃ�Ō�܂ŉ�]�����Ȃ��i�}�ɉ�]����j
		float m_RotateStateRatio;

	UPROPERTY(EditAnyWhere, Meta = (ToolTip = "Jump duration."))
		// �W�����v�p������
		float m_JumpTime;

	UPROPERTY(EditAnyWhere, Meta = (ToolTip = "Jump angle."))
		// �W�����v�p�x
		float m_JumpDegreeZ;

	UPROPERTY(EditAnyWhere, Meta = (ToolTip = "Position of Y-axis running on the road."))
		// ���𑖂�Y���̈ʒu
		float m_RunningRoadPosY;

	UPROPERTY(EditAnyWhere, Meta = (ToolTip = "Time to run after an attack."))
		// �U����̑��鎞��
		float m_RunningTimeAfterAttack;

	// �{�X�̓����ӂ�A�j���[�V�������Đ����邩�ǂ���
	bool m_IsHeadAnim;

	// �W�����v���Ɏg�p����Y���W�ۑ��p
	float m_JumpLocationY;

	// ���Ԍv���p�^�C�}�[
	float m_Timer;

	// ���Ƃ��Ƃ̍��W
	float originPosY;

	// �U���̊Ԋu�̈ꎞ�ۑ��p
	float m_attackIntervalTemp;

	// ���𑖂�Y���̈ʒu�̈ꎞ�ۑ��p
	float m_RunningRoadPosYTemp;

	// �ǂ����ړ��̍ۂ̈ʒu�ۑ�
	float m_BeforeCatchUpPosX;

	// �{�X�̏��
	BossState bossState;

	// �{�X�̍U�����
	BossAttack bossAttack;

	//-----------------------------------------------------------------------------------------------------------------------

	// �����U���̃v���p�e�B--------------------------------------------------------------------------------------------------

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack LightningStrike", Meta = (ToolTip = "Movement speed during a lightning strike."))
		// �����U�����̈ړ����x
		float m_LightningStrikeMoveSpeed;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack LightningStrike", Meta = (ToolTip = "Time to continue a lightning strike."))
		// �����U���𑱂��鎞��
		float m_LightningStrikeDuration;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack LightningStrike", Meta = (ToolTip = "Y-axis range of lightning strikes."))
		// �����U����Y���͈̔�
		float m_LightningStrikeWidth;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack LightningStrike", Meta = (ToolTip = "Interval for lightning strikes."))
		// �����U��������Ԋu
		float m_LightningStrikeInterval;

	// �U���}�[�J�[�ۑ��p
	TSubclassOf<class AActor> m_LightningStrikeMarker;

	//-----------------------------------------------------------------------------------------------------------------------

	// �E�F�[�u�U���̃v���p�e�B----------------------------------------------------------------------------------------------

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Wave", Meta = (ToolTip = "Movement speed during a wave attack."))
		// �E�F�[�u�U�����̈ړ����x
		float m_WaveAttackMoveSpeed;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Wave", Meta = (ToolTip = "Time to continue a wave attack."))
		// �E�F�[�u�U�������鎞��
		float m_WaveAttackDuration;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Wave", Meta = (ToolTip = "Interval for wave attacks."))
		// �E�F�[�u�U��������Ԋu
		float m_WaveAttackInterval;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Wave", Meta = (ClampMin = "0", ClampMax = "1", ToolTip = "The ratio of the time to generate a jump pad to \"Wave Attack Interval\"."))
		// �E�F�[�u�U��������Ԋu�ɑ΂���A�W�����v�䐶�������鎞�Ԃ̊���
		float m_JumppadSpawnRatio;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Wave", Meta = (ToolTip = "Y-axis range of Jump pad generation."))
		// �����U����Y���͈̔�
		float m_JumppadGenerateWidth;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Wave", Meta = (ToolTip = "Jump pad generation position X to avoid wave attacks."))
		// �E�F�[�u�U��������邽�߂̃W�����v�䐶����X���̃I�t�Z�b�g
		float m_JumppadPositionXAvoidWaveAttack;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Wave", Meta = (ToolTip = "Jump pad generation position Z to avoid wave attacks."))
		// �E�F�[�u�U��������邽�߂̃W�����v���Z���̈ʒu
		float m_JumppadPositionZAvoidWaveAttack;

	UPROPERTY(EditAnyWhere, Category = "BossAttack|Boss Attack Wave", Meta = (ToolTip = "Jump pad generation scale to avoid wave attacks."))
		// �E�F�[�u�U��������邽�߂̃W�����v��̃X�P�[��
		FVector m_JumppadScaleAvoidWaveAttack;

	// �E�F�[�u�U�����ɃW�����v��̐���������t���O
	bool m_IsJumppadGenerate;

	// �E�F�[�u�U���̉�
	int m_WaveAttackCount;

	// �W�����v��𐶐�����
	int  m_JumppadGenerateCount;

	// �W�����v��ʒu�ۑ��p
	float m_JumppadPosX;

	// �E�F�[�u�U���ۑ��p
	TSubclassOf<class ABossWaveControl> m_WaveAttackActor;

	// �W�����v��ۑ��p
	TSubclassOf<class AActor> m_JumppadActor;

	//-----------------------------------------------------------------------------------------------------------------------

	// �ːi�U���̃v���p�e�B--------------------------------------------------------------------------------------------------

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Rush", Meta = (ToolTip = "Movement speed during a rush attack (When leaving)"))
		// �ːi�U�����̒ʏ�ړ����x
		float m_RushAttackMoveSpeed;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Rush", Meta = (ToolTip = "Movement speed during a rush attack (When rush)"))
		// �ːi�U�����̓ːi�ړ����x
		float m_RushAttackSpeed;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Rush", Meta = (ToolTip = "How many seconds does it take to reach top speed?"))
		// ���b�ōő呬�x�ɂȂ邩
		float m_RushReachTimeTopSpeed;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Rush", Meta = (ToolTip = "Interval for rush attack"))
		// �ːi�U�������鎞��
		float m_RushAttackDuration;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Rush", Meta = (ToolTip = "Position of Y-axis running on the road during a rush attack."))
		// �ːi�U�����̓��𑖂�Y���̈ʒu
		float m_RushAttackRunningRoadPosY;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Rush", Meta = (ToolTip = "Time to turn around before rushing"))
		// �ːi�O�̐U������ɂ����鎞��
		float m_RushTurnTime;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Rush", Meta = (ToolTip = "X-axis range to get the player's coordinates"))
		// �v���C���[�̍��W���擾����X���͈̔�
		float m_GetPlayerCoordinatePosX;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Rush", Meta = (ToolTip = "Distance that BOSS passes through the player"))
		// �v���C���[��˂���������̈ړ�����
		float m_RushThroughDistance;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Rush", Meta = (ToolTip = "Movement speed after rush"))
		// �ːi��A���b�Ńv���C���[�ɒǂ�����
		float m_CatchUpTime;

	// ����U��A�j���[�V�������I���������ǂ����󂯎��
	bool m_IsHeadAnimEnd;

	// �ːi�t���O
	bool m_IsRush;

	// �v���C���[�ɑ̂����������ǂ���
	bool m_IsLookPlayer;

	// �ːi�̂��߂̈ړ����n�߂�O��X���̈ʒu�ۑ�
	FVector m_RushRunStartPos;

	// �ːi���n�߂�X���̈ʒu��ۑ�
	float m_RushStartPosX;

	// �U������̖ڕW�n�_��ݒ�
	float m_TargetRot;
	//-----------------------------------------------------------------------------------------------------------------------

	// BP�p�ϐ�
protected:

	// GameInstance�Őݒ肷��v���C���[��Actor
	UPROPERTY(BlueprintReadWrite)
		AActor* m_PlayerActor;

	// �v���C�x�[�g�֐�
private:
	// 2�_��Vector���W����p�x(���W�A���p)���Z�o
	float GetRadianFromVector(const FVector& posA, const FVector& posB);

	// �����ړ�
	void MoveForward(const float& _speed, const FVector& _pos);

	// �p�x���Ԃ��ĕύX
	void SetActorRotationLerpDegree(const float startDeg, const float targetDeg, const float& dividend, const float& divisor);

	// �U�����s��Actor(TSubclassOf<>)�𐶐�
	AActor* SpawnAttackActor(const TSubclassOf<class AActor> _attackActor, const FVector _attackPos);

	// �p�v���b�N�֐�
public:

	// �A�j���[�V����BP�p�֐�------------------------------------------------------------------------------------------------

	UFUNCTION(BlueprintCallable)
		// �W�����v��Ԃ��ǂ���
		bool GetIsJump() const;

	UFUNCTION(BlueprintCallable)
		// �U����Ԃ��ǂ���
		bool GetIsAttack() const;

	UFUNCTION(BlueprintCallable)
		// ����U��I��������Ƃ��擾
		void SetIsHeadEnd(bool _status);

	UFUNCTION(BlueprintCallable)
		// �����ӂ��Ԃ��ǂ���
		bool GetIsHead() const;

	UFUNCTION(BlueprintCallable)
		// �W�����v���Ԃ̎擾
		float GetJumpTime() const;

	UFUNCTION(BlueprintCallable)
		// ����A�j���[�V�����̍Đ����x�擾
		float GetRunAnimSpeed() const;

	//-----------------------------------------------------------------------------------------------------------------------

	// BP���̊֐����Ăяo��
	UFUNCTION(BlueprintImplementableEvent)
		void GetInstance();
};
