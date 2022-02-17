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

// レイ（ライントレース）に必要なものの構造体
USTRUCT(BlueprintType)
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

	// レイの開始位置のオフセット
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FVector RayStartOffset;

	// 着地判定レイを表示する
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		bool IsDrawRay = true;

	// レイの長さ
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		float RayLength = 100.0f;

	// 着地判定レイの色
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FColor DrawRayColor = FColor::Green;

	// 着地判定レイの表示時間
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
	// 移動値入力処理（キーボード・パッド）
	void InputKeyboard(float _axisValue);

	// 移動処理
	void UpdateMove(const float deltaTime);

	// 加速リセット
	void ResetAcceleration();

	// 急カーブ（ドリフト）状態にする
	void SetDrift(bool _status);

	// FLinetraceInfoとFDebugRayInfoを元にデバッグ用のラインを表示
	void MyDrawDebugLine(const FLinetraceInfo& linetrace);

	// FLinetraceInfoを元にレイを飛ばす
	bool MyLineTrace(FLinetraceInfo& linetrace);

	// FCollisionQueryParamsをのignoreActorを自分自身以外解除
	void ClearIgnoreActor(FCollisionQueryParams& collisionQueryParams);

	// 軸の角度を固定する
	void LockAngle(float& originRot, const float lockAxis, const float targetAxis, const float tolerance);

	// デバッグワープ
	void DebugWarp();

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

	// 左右移動の取得用
	UFUNCTION(BlueprintPure)
		float GetSideMoveValue();

	// 接地しているか
	UFUNCTION(BlueprintPure)
		bool GetIsLanding();

private:
	// プレイヤーコントローラー
	APlayerController* m_PlayerController;

	// コントローラーが接続されているか
	bool m_IsSensor;

	// ドリフト状態かどうか
	bool m_IsSharpcurve;

	// 現在の重力
	float m_CurrentGravity;

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

	// ジャンプコマンド保存用
	TArray<FKey> m_InputKeys;

protected:
	UPROPERTY(BlueprintReadOnly)
		// ジャンプ状態
		bool m_IsJump;

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
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings|Camera"
		, Meta = (DisplayName = "Spring Arm Length", ToolTip = "Original length of spring arm"))
		float m_SpringArmLength;

	// スプリングアームの長さの調整
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings|Camera"
		, Meta = (DisplayName = "Spring Arm Length Adjust", ToolTip = "Adjusting the length of the spring arm"))
		float m_ArmLengthAdjust;

	// ジャンプ台のジャンプ力
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Settings|Jump"
		, Meta = (DisplayName = "Jump Power", ToolTip = "Jumping power of the jumping platform"))
		FVector m_JumpPower;

	// 移動可能かどうか
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move"
		, Meta = (DisplayName = "Can Move", ToolTip = "It can be moved"))
		bool m_CanMove;

	// ボードが接地していない時の速度減衰量
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move"
		, Meta = (DisplayName = "Air Speed Attenuation Value", ToolTip = "Velocity attenuation when the board is not grounded", ClampMin = "0", ClampMax = "1"))
		float m_AirSpeedAttenuationValue;

	// 最高速度
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move"
		, Meta = (DisplayName = "Max Speed", ToolTip = "Max speed"))
		float m_MaxSpeed;

	// ジャンプ中の最高速度
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move"
		, Meta = (DisplayName = "Max Speed During Jump", ToolTip = "Maximum speed during jump"))
		float m_MaxJumpSpeed;

	// ボードの浮く力（上への力の強さ）
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move"
		, Meta = (DisplayName = "Float Power", ToolTip = "The floating force of the board"))
		float m_FloatPower;

	// ジャンプ中の重力の強さ
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move"
		, Meta = (DisplayName = "Gravity During Jump", ToolTip = "Gravity during jump"))
		float m_JumpGravity;

	// 落下中の重力の強さ
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move"
		, Meta = (DisplayName = "Gravity During Falling", ToolTip = "Gravity during falling"))
		float m_FallGravity;

	// ジャンプ中に加える重力の強さ
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move"
		, Meta = (DisplayName = "Gravitational Acceleration During Jump", ToolTip = "Gravitational acceleration during jump"))
		float m_AddJumpGravity;

	// ジャンプ中に加える重力の強さ
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move"
		, Meta = (DisplayName = "Gravitational Acceleration During Falling", ToolTip = "Gravitational acceleration during falling"))
		float m_AddFallGravity;

	// ホバーの速度
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move"
		, Meta = (DisplayName = "Hover Lerp Speed", ToolTip = "Amount of change in hover speed"))
		float m_HoverLerpSpeed;

	// 地面に対するプレイヤーの角度変更の速度
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move"
		, Meta = (DisplayName = "Angle Lerp Speed", ToolTip = "Amount of change in angle"))
		float m_AngleLerpSpeed;

	// 左右の移動量
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Move|Side"
		, Meta = (DisplayName = "Side Max Speed", ToolTip = "Side max speed"))
		float m_SideMaxSpeed;

	// 左右の移動量の加速量
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Move|Side"
		, Meta = (DisplayName = "Side Acceleration", ToolTip = "Side acceleration", ClampMin = "0", ClampMax = "1"))
		float m_SideAcceleration;

	// ホバー移動のレイ
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Ray"
		, Meta = (DisplayName = "Hover Ray"))
		FLinetraceInfo m_HoverRay;

	// 角度取得レイ（前）
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Ray"
		, Meta = (DisplayName = "Hover Angle Front Ray"))
		FLinetraceInfo m_HoverAngleFrontRay;

	// 角度取得レイ（後ろ）
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Ray"
		, Meta = (DisplayName = "Hover Angle Rear Ray"))
		FLinetraceInfo m_HoverAngleRearRay;

	// 衝突回避レイ
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Ray"
		, Meta = (DisplayName = "Avoid Ray"))
		FLinetraceInfo m_AvoidRay;

	// デバッグワープポイント
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