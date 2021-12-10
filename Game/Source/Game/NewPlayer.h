// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NewPlayer.generated.h"

class USerial;
class UCameraComponent;

UCLASS()
class GAME_API ANewPlayer : public ACharacter
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
	// �J����
	UCameraComponent* m_PlayerCamera;

	// �R���g���[���[���ڑ�����Ă��邩
	bool m_IsSensor;

	// �O�i�̌��݂̉�����
	float m_CurrentForwardAcceleration;

	// ���E�̌��݂̉�����
	float m_CurrentSideAcceleration;

	// �ړ��ʂ̓��͂�ۑ�����
	FVector m_UpdateValue;

public:
	// �ړ��\���ǂ���
	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
		bool m_CanMove;

	// �O�i�̍ő呬�x
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Move|Forward")
		float m_ForwardMaxSpeed;

	// �O�i�̉�����
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Meta = (ClampMin = "0", ClampMax = "1"), Category = "Move|Forward")
		float m_ForwardAcceleration;

	// ���E�̈ړ���
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Move|Side")
		float m_SideMaxSpeed;

	// ���E�̈ړ��ʂ̉�����
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Meta = (ClampMin = "0", ClampMax = "1"), Category = "Move|Side")
		float m_SideAcceleration;

	// ���E�ړ��̒�~���̋���
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Meta = (ClampMin = "0", ClampMax = "1"), Category = "Move|Side")
		float m_BrakePower;
};