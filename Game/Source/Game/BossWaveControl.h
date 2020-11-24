// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossWaveControl.generated.h"

UCLASS()
class GAME_API ABossWaveControl : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABossWaveControl();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// �v���C�x�[�g�ϐ�
private:

	// �E�F�[�u�S�ʂɎg�p����v���p�e�B--------------------------------------------------------------------------------------

	// ��������Ă���W�����v��ɓ��B���鎞��
	UPROPERTY(EditAnyWhere, Meta = (ToolTip = "Time to reach the jumppad after being generated."))
		float m_ReachToJumppadTime;

	// �������ꂽ���̃X�P�[���{��
	UPROPERTY(EditAnyWhere, Meta = (ToolTip = "Scale magnification at generation."))
		float m_ScaleAtGenerateMag;

	// �ŏI�I�ȃX�P�[���{��
	UPROPERTY(EditAnyWhere, Meta = (ToolTip = "Final scale magnification."))
		float m_FinalScaleMag;

	// �^�C�}�[
	float m_Timer;

	// �ڕW�_�ƂȂ�W�����v���X���̈ʒu
	float m_JumppadPosX;

	// �������̈ʒu�ۑ��p
	FVector m_OriginPos;

	// ���Ƃ��Ƃ̃X�P�[���ۑ�
	FVector m_OriginScale;

	//-----------------------------------------------------------------------------------------------------------------------

	// �p�u���b�N�֐�
public:

	// �W�����v�p�b�h�ʒu�̐ݒ�
	void SetJumppadPosX(float _posX);
};
