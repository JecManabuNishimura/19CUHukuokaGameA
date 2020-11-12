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
		Run,			// �ړ�
		RunAfterAttack,	// �U����̈ړ��i�W�����v�p�x�ɂ���j
		Jump_In,		// �W�����v�i�O�����j
		Jump_Out,		// �W�����v�i�����O�j
		Attack,			// �U��
		Neigh,			// ���ȂȂ�
		Wait,			// �~�܂��ē���U��
	};

	// �{�X�̍U���̎��
	enum BossAttack
	{
		LightningStrike,	// �����U��
	};

	// �v���C�x�[�g�ϐ�
private:
	UPROPERTY(EditAnyWhere)
		// �ړ����x
		float m_MoveSpeed;

	UPROPERTY(EditAnyWhere)
		// �O���Ɠ������o���肷�鎞��
		float m_InsideMigrationTime;

	UPROPERTY(EditAnyWhere)
		// �ړ����ɉ�]���n�߂鎞�Ԃ̊����@0�Ȃ炷���ɊJ�n�A 1�Ȃ�Ō�܂ŉ�]�����Ȃ��i�}�ɉ�]����j
		float m_RotateStateRatio;

	UPROPERTY(EditAnyWhere)
		// �W�����v���Ă��鎞��
		float m_JumpTime;

	UPROPERTY(EditAnyWhere)
		// �W�����v�p�x
		float m_JumpDegreeZ;

	UPROPERTY(EditAnyWhere)
		// �U����̑��鎞��
		float m_RunningTimeAfterAttack;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack LightningStrike")
		// �ړ����x
		float m_LightningStrikeMoveSpeed;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack LightningStrike")
		// �����U���𑱂��鎞��
		float m_LightningStrikeDuration;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack LightningStrike")
		// �����U����Y���͈̔�
		float m_LightningStrikeWidth;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack LightningStrike")
		// �����U��������Ԋu
		float m_LightningStrikeInterval;


	// �U���}�[�J�[�ۑ��p
	TSubclassOf<class AActor> m_LightningStrikeMarker;

	// ���Ԍv���p�^�C�}�[
	float m_Timer;

	// ���Ƃ��Ƃ̍��W
	float originPosY;

	// �����U���̊Ԋu�̈ꎞ�ۑ��p
	float m_LightningStrikeIntervalTemp;

	// �����U���̖��O�Ɏg�p����C���f�b�N�X
	int m_LightningStrikeIndex;

	// �{�X�̏��
	BossState bossState;

	// �{�X�̍U�����
	BossAttack bossAttack;

	// �v���C�x�[�g�֐�
private:
	// 2�_��Vector���W����p�x(���W�A���p)���Z�o
	float GetRadianFromVector(const FVector& posA, const FVector& posB);

	// �����ړ�
	void MoveForward(const float& _speed, const FVector& _pos);

	// �p�x���Ԃ��ĕύX
	void SetActorRotationLerpDegree(const float startDeg, const float targetDeg, const float& dividend, const float& divisor);

	// �p�v���b�N�֐�
public:

	// �A�j���[�V����BP�p�֐�--------------------------------------
	UFUNCTION(BlueprintCallable)
		// �W�����v��Ԃ��ǂ���
		bool GetIsJump() const;

	UFUNCTION(BlueprintCallable)
		// �U����Ԃ��ǂ���
		bool GetIsAttack() const;

	UFUNCTION(BlueprintCallable)
		// �W�����v���Ԃ̎擾
		float GetJumpTime() const;
	//-------------------------------------------------------------
};
