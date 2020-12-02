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

class ABossWaveControl;

// ボスの攻撃の種類
UENUM(BlueprintType)
enum class BossAttack : uint8
{
	LightningStrike,	// 落雷攻撃
	Wave,				// ウェーブ攻撃
	Rush,				// 突進
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
	// ボスの状態
	enum BossState
	{
		CatchUp,		// 追いつく
		Run,			// 移動
		RunAfterAttack,	// 攻撃後の移動（ジャンプ角度にする）
		RunAfterRush,	// 突進後の移動（ジャンプ角度にする）
		Jump_In,		// ジャンプ（外→内）
		Jump_Out,		// ジャンプ（内→外）
		Attack,			// 攻撃
		Neigh,			// いななき
		Wait,			// 止まって頭を振る
	};

	// プライベート変数
private:

	//-TEMP---------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnyWhere, Category = "TEMP", Meta = (ToolTip = "Temporarily specify the type of boss attack."))
		// ウェーブ攻撃時の移動速度
		BossAttack m_TEMP_BOSS_ATTACK;
	
	UPROPERTY(EditAnyWhere, Category = "TEMP", Meta = (ToolTip = "Temporary fence position."))
		// ウェーブ攻撃時の移動速度
		float m_TEMP_FENCE_POS;
	//--------------------------------------------------------------------------------------------------------------------------------------------

	// ボス全般に使用するプロパティ------------------------------------------------------------------------------------------

		// 移動速度
	UPROPERTY(EditAnyWhere, Meta = (ToolTip = "Moving Speed."))
		float m_MoveSpeed;

	// 走るアニメーションの再生速度
	UPROPERTY(EditAnyWhere, Meta = (ToolTip = "Playback speed of running animation."))
		float m_RunningAnimationRateSpeed;

	// 外側と内側を出入りする時間
	UPROPERTY(EditAnyWhere, Meta = (ToolTip = "Time to go in and out of the outside and inside."))
		float m_InsideMigrationTime;

	UPROPERTY(EditAnyWhere, Meta = (ClampMin = "0", ClampMax = "1", ToolTip = "Percentage of time to start spinning while moving."))
		// 移動中に回転を始める時間の割合　0ならすぐに開始、 1なら最後まで回転させない（急に回転する）
		float m_RotateStateRatio;

	UPROPERTY(EditAnyWhere, Meta = (ToolTip = "Jump duration."))
		// ジャンプ継続時間
		float m_JumpTime;

	UPROPERTY(EditAnyWhere, Meta = (ToolTip = "Jump angle."))
		// ジャンプ角度
		float m_JumpDegreeZ;

	UPROPERTY(EditAnyWhere, Meta = (ToolTip = "Position of Y-axis running on the road."))
		// 道を走るY軸の位置
		float m_RunningRoadPosY;

	UPROPERTY(EditAnyWhere, Meta = (ToolTip = "Time to run after an attack."))
		// 攻撃後の走る時間
		float m_RunningTimeAfterAttack;

	// ボスの頭をふるアニメーションを再生するかどうか
	bool m_IsHeadAnim;

	// ジャンプ時に使用するY座標保存用
	float m_JumpLocationY;

	// 時間計測用タイマー
	float m_Timer;

	// もともとの座標
	float originPosY;

	// 攻撃の間隔の一時保存用
	float m_attackIntervalTemp;

	// 道を走るY軸の位置の一時保存用
	float m_RunningRoadPosYTemp;

	// 追いつく移動の際の位置保存
	float m_BeforeCatchUpPosX;

	// ボスの状態
	BossState bossState;

	// ボスの攻撃種類
	BossAttack bossAttack;

	//-----------------------------------------------------------------------------------------------------------------------

	// 落雷攻撃のプロパティ--------------------------------------------------------------------------------------------------

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack LightningStrike", Meta = (ToolTip = "Movement speed during a lightning strike."))
		// 落雷攻撃時の移動速度
		float m_LightningStrikeMoveSpeed;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack LightningStrike", Meta = (ToolTip = "Time to continue a lightning strike."))
		// 落雷攻撃を続ける時間
		float m_LightningStrikeDuration;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack LightningStrike", Meta = (ToolTip = "Y-axis range of lightning strikes."))
		// 落雷攻撃のY軸の範囲
		float m_LightningStrikeWidth;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack LightningStrike", Meta = (ToolTip = "Interval for lightning strikes."))
		// 落雷攻撃をする間隔
		float m_LightningStrikeInterval;

	// 攻撃マーカー保存用
	TSubclassOf<class AActor> m_LightningStrikeMarker;

	//-----------------------------------------------------------------------------------------------------------------------

	// ウェーブ攻撃のプロパティ----------------------------------------------------------------------------------------------

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Wave", Meta = (ToolTip = "Movement speed during a wave attack."))
		// ウェーブ攻撃時の移動速度
		float m_WaveAttackMoveSpeed;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Wave", Meta = (ToolTip = "Time to continue a wave attack."))
		// ウェーブ攻撃をする時間
		float m_WaveAttackDuration;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Wave", Meta = (ToolTip = "Interval for wave attacks."))
		// ウェーブ攻撃をする間隔
		float m_WaveAttackInterval;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Wave", Meta = (ClampMin = "0", ClampMax = "1", ToolTip = "The ratio of the time to generate a jump pad to \"Wave Attack Interval\"."))
		// ウェーブ攻撃をする間隔に対する、ジャンプ台生成をする時間の割合
		float m_JumppadSpawnRatio;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Wave", Meta = (ToolTip = "Y-axis range of Jump pad generation."))
		// 落雷攻撃のY軸の範囲
		float m_JumppadGenerateWidth;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Wave", Meta = (ToolTip = "Jump pad generation position X to avoid wave attacks."))
		// ウェーブ攻撃を避けるためのジャンプ台生成のX軸のオフセット
		float m_JumppadPositionXAvoidWaveAttack;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Wave", Meta = (ToolTip = "Jump pad generation position Z to avoid wave attacks."))
		// ウェーブ攻撃を避けるためのジャンプ台のZ軸の位置
		float m_JumppadPositionZAvoidWaveAttack;

	UPROPERTY(EditAnyWhere, Category = "BossAttack|Boss Attack Wave", Meta = (ToolTip = "Jump pad generation scale to avoid wave attacks."))
		// ウェーブ攻撃を避けるためのジャンプ台のスケール
		FVector m_JumppadScaleAvoidWaveAttack;

	// ウェーブ攻撃時にジャンプ台の生成をするフラグ
	bool m_IsJumppadGenerate;

	// ウェーブ攻撃の回数
	int m_WaveAttackCount;

	// ジャンプ台を生成下数
	int  m_JumppadGenerateCount;

	// ジャンプ台位置保存用
	float m_JumppadPosX;

	// ウェーブ攻撃保存用
	TSubclassOf<class ABossWaveControl> m_WaveAttackActor;

	// ジャンプ台保存用
	TSubclassOf<class AActor> m_JumppadActor;

	//-----------------------------------------------------------------------------------------------------------------------

	// 突進攻撃のプロパティ--------------------------------------------------------------------------------------------------

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Rush", Meta = (ToolTip = "Movement speed during a rush attack (When leaving)"))
		// 突進攻撃時の通常移動速度
		float m_RushAttackMoveSpeed;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Rush", Meta = (ToolTip = "Movement speed during a rush attack (When rush)"))
		// 突進攻撃時の突進移動速度
		float m_RushAttackSpeed;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Rush", Meta = (ToolTip = "How many seconds does it take to reach top speed?"))
		// 何秒で最大速度になるか
		float m_RushReachTimeTopSpeed;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Rush", Meta = (ToolTip = "Interval for rush attack"))
		// 突進攻撃をする時間
		float m_RushAttackDuration;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Rush", Meta = (ToolTip = "Position of Y-axis running on the road during a rush attack."))
		// 突進攻撃時の道を走るY軸の位置
		float m_RushAttackRunningRoadPosY;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Rush", Meta = (ToolTip = "Time to turn around before rushing"))
		// 突進前の振り向きにかける時間
		float m_RushTurnTime;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Rush", Meta = (ToolTip = "X-axis range to get the player's coordinates"))
		// プレイヤーの座標を取得するX軸の範囲
		float m_GetPlayerCoordinatePosX;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Rush", Meta = (ToolTip = "Distance that BOSS passes through the player"))
		// プレイヤーを突き抜けた後の移動距離
		float m_RushThroughDistance;

	UPROPERTY(EditAnyWhere, Category = "Boss Attack|Boss Attack Rush", Meta = (ToolTip = "Movement speed after rush"))
		// 突進後、何秒でプレイヤーに追いつくか
		float m_CatchUpTime;

	// 頭を振るアニメーションが終了したかどうか受け取る
	bool m_IsHeadAnimEnd;

	// 突進フラグ
	bool m_IsRush;

	// プレイヤーに体を向けたかどうか
	bool m_IsLookPlayer;

	// 突進のための移動を始める前のX軸の位置保存
	FVector m_RushRunStartPos;

	// 突進を始めたX軸の位置を保存
	float m_RushStartPosX;

	// 振り向きの目標地点を設定
	float m_TargetRot;
	//-----------------------------------------------------------------------------------------------------------------------

	// BP用変数
protected:

	// GameInstanceで設定するプレイヤーのActor
	UPROPERTY(BlueprintReadWrite)
		AActor* m_PlayerActor;

	// プライベート関数
private:
	// 2点のVector座標から角度(ラジアン角)を算出
	float GetRadianFromVector(const FVector& posA, const FVector& posB);

	// 直線移動
	void MoveForward(const float& _speed, const FVector& _pos);

	// 角度を補間して変更
	void SetActorRotationLerpDegree(const float startDeg, const float targetDeg, const float& dividend, const float& divisor);

	// 攻撃を行うActor(TSubclassOf<>)を生成
	AActor* SpawnAttackActor(const TSubclassOf<class AActor> _attackActor, const FVector _attackPos);

	// パプリック関数
public:

	// アニメーションBP用関数------------------------------------------------------------------------------------------------

	UFUNCTION(BlueprintCallable)
		// ジャンプ状態かどうか
		bool GetIsJump() const;

	UFUNCTION(BlueprintCallable)
		// 攻撃状態かどうか
		bool GetIsAttack() const;

	UFUNCTION(BlueprintCallable)
		// 頭を振り終わったことを取得
		void SetIsHeadEnd(bool _status);

	UFUNCTION(BlueprintCallable)
		// 頭をふる状態かどうか
		bool GetIsHead() const;

	UFUNCTION(BlueprintCallable)
		// ジャンプ時間の取得
		float GetJumpTime() const;

	UFUNCTION(BlueprintCallable)
		// 走るアニメーションの再生速度取得
		float GetRunAnimSpeed() const;

	//-----------------------------------------------------------------------------------------------------------------------

	// BP側の関数を呼び出す
	UFUNCTION(BlueprintImplementableEvent)
		void GetInstance();
};
