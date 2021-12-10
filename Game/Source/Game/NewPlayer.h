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
	// 移動値入力処理（コントローラー）
	void InputSensor();

	// 移動値入力処理（キーボード・パッド）
	void InputKeyboard(float _axisValue);

	// 移動処理
	void UpdateMove();

	// 加速リセット
	void ResetAcceleration();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//	終了時
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// カメラ
	UCameraComponent* m_PlayerCamera;

	// コントローラーが接続されているか
	bool m_IsSensor;

	// 前進の現在の加速量
	float m_CurrentForwardAcceleration;

	// 左右の現在の加速量
	float m_CurrentSideAcceleration;

	// 移動量の入力を保存する
	FVector m_UpdateValue;

public:
	// 移動可能かどうか
	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
		bool m_CanMove;

	// 前進の最大速度
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Move|Forward")
		float m_ForwardMaxSpeed;

	// 前進の加速量
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Meta = (ClampMin = "0", ClampMax = "1"), Category = "Move|Forward")
		float m_ForwardAcceleration;

	// 左右の移動量
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Move|Side")
		float m_SideMaxSpeed;

	// 左右の移動量の加速量
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Meta = (ClampMin = "0", ClampMax = "1"), Category = "Move|Side")
		float m_SideAcceleration;

	// 左右移動の停止時の強さ
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Meta = (ClampMin = "0", ClampMax = "1"), Category = "Move|Side")
		float m_BrakePower;
};