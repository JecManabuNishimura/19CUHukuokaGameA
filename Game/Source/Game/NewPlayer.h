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

// トリックの際に以上・以下を指定する用
UENUM(BlueprintType)
enum class EComp : uint8
{
	Auto,
	OrMore,
	MoreThan,
	LessThan,
	OrLess,
};

// トリックの際の左スティックの軸の方向
UENUM(BlueprintType)
enum class EInputAxis : uint8
{
	X,
	Y,
};

// トリックの種類
UENUM(BlueprintType)
enum class ETrickType : uint8
{
	Trick1,
	Trick2,
	Trick3,
	Trick4,
	None,
};

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

	void SetStart(UPrimitiveComponent* CurrentComp)
	{
		this->rayStart = CurrentComp->GetComponentLocation() + (CurrentComp->GetForwardVector() * this->RayStartOffset.X) + (CurrentComp->GetRightVector() * this->RayStartOffset.Y) + (CurrentComp->GetUpVector() * this->RayStartOffset.Z);
	}

	void SetEnd(UPrimitiveComponent* CurrentComp)
	{
		this->rayEnd = this->rayStart - (CurrentComp->GetUpVector() * this->RayLength);
	}
};

// トリックと入力の関連付け
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
	// X軸の入力処理
	void InputAxisX(const float _axisValue);

	// Y軸の入力処理
	void InputAxisY(const float _axisValue);

	// ホバー処理
	void Hover(const float _deltaTime);

	// 2つのレイからプレイヤーの角度を変更
	void SetRotationWithRay(const float _deltaTime);

	// 移動処理
	void UpdateMove(const float _deltaTime);

	// トリックボタンの入力を受け付ける
	void SetTrick(const bool _status);

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

	// どのトリックを決めているか
	UFUNCTION(BlueprintPure)
		ETrickType GetTrickType();

private:
	// プレイヤーコントローラー
	APlayerController* m_PlayerController;

	// トリックボタンが押されているか
	bool m_IsTrick;

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
		
	// 入力の保存
	FVector2D m_InputAxisValue;

	// 現在のトリック
	ETrickType m_CurrentTrick;

protected:
	UPROPERTY(BlueprintReadOnly)
		// ジャンプ状態
		bool m_IsJump;

public:

	// プレイヤーのMovementComponent
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UFloatingPawnMovement* m_FloatingPawnMovementComponent;

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
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Camera"
		, Meta = (DisplayName = "Spring Arm Length", ToolTip = "Original length of spring arm"))
		float m_SpringArmLength;

	// スプリングアームの長さの調整
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Camera"
		, Meta = (DisplayName = "Adjust Speed Spring Arm Length", ToolTip = "Amount to adjust the length of the spring arm when the speed is high"))
		float m_ArmLengthAdjust;

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
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move|Gravity"
		, Meta = (DisplayName = "Gravity During Jump", ToolTip = "Gravity during jump"))
		float m_JumpGravity;

	// 落下中の重力の強さ
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move|Gravity"
		, Meta = (DisplayName = "Gravity During Falling", ToolTip = "Gravity during falling"))
		float m_FallGravity;

	// ジャンプ中に加える重力の強さ
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move|Gravity"
		, Meta = (DisplayName = "Gravitational Acceleration During Jump", ToolTip = "Gravitational acceleration during jump"))
		float m_AddJumpGravity;

	// ジャンプ中に加える重力の強さ
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move|Gravity"
		, Meta = (DisplayName = "Gravitational Acceleration During Falling", ToolTip = "Gravitational acceleration during falling"))
		float m_AddFallGravity;

	// ホバー量の変更する速度
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

	// トリックのバインド
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Trick"
		, Meta = (DisplayName = "Trick Binding"))
		TArray<FTrickBind> m_TrickBind;

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