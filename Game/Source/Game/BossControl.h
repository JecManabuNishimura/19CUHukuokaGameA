/*
 *--------------------------------------------------------
 * ボスの移動
 * 2020/11/08 渡邊龍音 作成
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
	// ボスの状態
	enum BossState
	{
		Run,			// 移動
		RunAfterAttack,	// 攻撃後の移動（ジャンプ角度にする）
		Jump_In,		// ジャンプ（外→内）
		Jump_Out,		// ジャンプ（内→外）
		Attack,			// 攻撃
		Neigh,			// いななき
		Wait,			// 止まって頭を振る
	};

	// ボスの攻撃の種類
	enum BossAttack
	{
		LightningStrike,	// 落雷攻撃
	};

	// プライベート変数
private:
	UPROPERTY(EditAnyWhere)
		// 移動速度
		float m_MoveSpeed;

	UPROPERTY(EditAnyWhere)
		// 外側と内側を出入りする時間
		float m_InsideMigrationTime;

	UPROPERTY(EditAnyWhere)
		// 移動中に回転を始める時間の割合　0ならすぐに開始、 1なら最後まで回転させない（急に回転する）
		float m_RotateStateRatio;

	UPROPERTY(EditAnyWhere)
		// ジャンプしている時間
		float m_JumpTime;

	UPROPERTY(EditAnyWhere)
		// ジャンプ角度
		float m_JumpDegreeZ;

	UPROPERTY(EditAnyWhere)
		// 攻撃後の走る時間
		float m_RunningTimeAfterAttack;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack LightningStrike")
		// 移動速度
		float m_LightningStrikeMoveSpeed;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack LightningStrike")
		// 落雷攻撃を続ける時間
		float m_LightningStrikeDuration;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack LightningStrike")
		// 落雷攻撃のY軸の範囲
		float m_LightningStrikeWidth;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack LightningStrike")
		// 落雷攻撃をする間隔
		float m_LightningStrikeInterval;


	// 攻撃マーカー保存用
	TSubclassOf<class AActor> m_LightningStrikeMarker;

	// 時間計測用タイマー
	float m_Timer;

	// もともとの座標
	float originPosY;

	// 落雷攻撃の間隔の一時保存用
	float m_LightningStrikeIntervalTemp;

	// 落雷攻撃の名前に使用するインデックス
	int m_LightningStrikeIndex;

	// ボスの状態
	BossState bossState;

	// ボスの攻撃種類
	BossAttack bossAttack;

	// プライベート関数
private:
	// 2点のVector座標から角度(ラジアン角)を算出
	float GetRadianFromVector(const FVector& posA, const FVector& posB);

	// 直線移動
	void MoveForward(const float& _speed, const FVector& _pos);

	// 角度を補間して変更
	void SetActorRotationLerpDegree(const float startDeg, const float targetDeg, const float& dividend, const float& divisor);

	// パプリック関数
public:

	// アニメーションBP用関数--------------------------------------
	UFUNCTION(BlueprintCallable)
		// ジャンプ状態かどうか
		bool GetIsJump() const;

	UFUNCTION(BlueprintCallable)
		// 攻撃状態かどうか
		bool GetIsAttack() const;

	UFUNCTION(BlueprintCallable)
		// ジャンプ時間の取得
		float GetJumpTime() const;
	//-------------------------------------------------------------
};
