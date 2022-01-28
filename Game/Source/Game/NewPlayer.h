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

// デバッグ用レイの設定の構造体
USTRUCT(BlueprintType)
struct FDebugRayInfo
{
	GENERATED_USTRUCT_BODY();

	// 着地判定レイを表示する
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		bool IsDrawRay = true;

	// 着地判定レイの色
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FColor DrawRayColor = FColor::Green;

	// 着地判定レイの表示時間
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		float DrawRayTime = 7.0f;
};

// レイ（ライントレース）に必要なものの構造体
USTRUCT()
struct FLinetraceInfo
{
	GENERATED_USTRUCT_BODY();

	// レイのあたった結果
	FHitResult hitResult;

	// レイの開始位置
	FVector rayStart = FVector::ZeroVector;

	// レイの終了位置
	FVector rayEnd = FVector::ZeroVector;

	// どれにレイが当たるかを設定
	FCollisionObjectQueryParams objectQueueParam = FCollisionObjectQueryParams::AllObjects;

	// 除外するものを設定
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

	// FLinetraceInfoとFDebugRayInfoを元にデバッグ用のラインを表示
	void MyDrawDebugLine(const FLinetraceInfo& linetrace, const FDebugRayInfo& ray);

	// FLinetraceInfoを元にレイを飛ばす
	bool MyLineTrace(FLinetraceInfo& linetrace);

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

	// ジャンプ状態
	bool m_IsJump;

	// 前進の現在の加速量
	float m_CurrentForwardAcceleration;

	// 左右移動の量
	float m_SideValue;

	// 左右の現在の加速量
	float m_CurrentSideAcceleration;

	// 急カーブの強さ
	float m_CurrentSharpcurvePower;

	// ボードが接地していない時の速度減衰量
	float m_AirSpeedAttenuation;

	// 移動量の入力を保存する
	FVector m_UpdateValue;
	
	// 進行方向を制御
	FVector m_BaseForwardVector;

	// 着地判定レイの情報
	FLinetraceInfo m_GroundRayInfo;

	// テール着地レイの情報
	FLinetraceInfo m_TailLandingRayInfo;

public:
	// プレイヤーのMovementComponent
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UFloatingPawnMovement* m_FloatingPawnMovementComponent;

	/*
	// ボードの当たり判定に使用する（カプセル）
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UCapsuleComponent* m_BoardCapsuleCollision;

	// ボードの当たり判定に使用する（ボックス）
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UBoxComponent* m_BoardBoxCollision;
	*/

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

	// スプリングアームのもともとの長さ
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings|Camera")
		float m_SpringArmLength;

	// スプリングアームの長さの調整
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings|Camera")
		float m_ArmLengthAdjust;

	// 接地判定レイの開始位置のオフセット
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings|Ground")
		FVector m_GroundRayOffset;

	// 接地判定レイの長さ
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings|Ground")
		float m_GroundRayLength;

	// ジャンプ台のジャンプ力
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings|Jump")
		FVector m_JumpPower;

	// テール着地レイの開始位置のオフセット
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings|Jump")
		FVector m_TailLandingRayOffset;

	// テール着地レイの長さ
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings|Jump")
		float m_TailLandingRayLength;

	// 移動可能かどうか
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move")
		bool m_CanMove;

	// ボードが接地していない時の速度減衰量
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Meta = (ClampMin = "0", ClampMax = "1"), Category = "Move")
		float m_AirSpeedAttenuationValue;

	// 通常時の重心
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Move")
		FVector m_DefaultCenterOfMass;

	// ジャンプ時の重心
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Move")
		FVector m_JumpCenterOfMass;

	// 左右の移動量
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Move|Side")
		float m_SideMaxSpeed;

	// 左右の移動量の加速量
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Meta = (ClampMin = "0", ClampMax = "1"), Category = "Move|Side")
		float m_SideAcceleration;

	// 着地判定レイの設定
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Debug")
		FDebugRayInfo m_GroundDebug;

	// 着地判定レイを表示する
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