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
	// 移動値入力処理（コントローラー）
	void InputSensor();

	// 移動値入力処理（キーボード・パッド）
	void InputKeyboard(float _axisValue);

	// 移動処理
	void UpdateMove();

	// 加速リセット
	void ResetAcceleration();

	// 急カーブ（ドリフト）状態にする
	void SetDrift(bool _status);

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
	// コントローラーが接続されているか
	bool m_IsSensor;

	// ドリフト状態かどうか
	bool m_IsSharpcurve;

	// 前進の現在の加速量
	float m_CurrentForwardAcceleration;

	// 左右移動の量
	float m_SideValue;

	// 左右の現在の加速量
	float m_CurrentSideAcceleration;

	// 急カーブの強さ
	float m_CurrentSharpcurvePower;

	// 移動量の入力を保存する
	FVector m_UpdateValue;

public:
	// プレイヤーのMovementComponent
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UFloatingPawnMovement* m_FloatingPawnMovementComponent;

	// ボードの当たり判定に使用する
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UCapsuleComponent* m_BoardCapsuleCollision;

	// ボードのメッシュ
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UStaticMeshComponent* m_BoardMesh;

	// プレイヤーのメッシュ
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		USkeletalMeshComponent* m_PlayerMesh;

	// スプリングアーム
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		USpringArmComponent* m_SpringArm;

	// カメラ
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UCameraComponent* m_PlayerCamera;

	// コリジョン
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UBoxComponent* m_BoxCollision;

	// ルートコンポーネントのY回転
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings")
		float m_RootRotationY;

	// スプリングアームのもともとの長さ
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings|Camera")
		float m_SpringArmLength;

	// スプリングアームの長さの調整
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings|Camera")
		float m_ArmLengthAdjust;

	// 移動可能かどうか
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move")
		bool m_CanMove;

	// 左右の移動量
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Move|Side")
		float m_SideMaxSpeed;

	// 左右の移動量の加速量
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