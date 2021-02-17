//----------------------------------------------------------
// ファイル名		：EnemyCharaATKControl.h
// 概要				：敵の攻撃を行う
// 作成者			：19CU0222 鍾家同
// 更新内容			：2020/10/02 作成　弾の生成と発射
//					：2020/11/02 変更　プレイヤー側でPlayerCharaNoSensorに関連するものを削除（PlayerCharaに統合された）
//					：2020/11/03 増加　敵撃破のエフェクトを生成
//					：2020/11/04 増加　死亡エフェクトを生成
//					：2020/11/13 変更　敵弾はゲームスタート後で発射する
//					：2020/11/15 増加　EEnemyType列挙型を追加
//					：2020/11/16 増加　EnergyEnemyの生成
//					：2020/11/16 増加　曹飛　ShotEnemyの生成
//					：2021/02/10 増加　足りないコメントを補足
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "EnemyChara.h"
#include "PlayerChara.h"
#include "EnemyCharaATKControl.generated.h"

class AActor;
class UParticleSystem;

// BulletType(弾の種類)
UENUM(BlueprintType)
enum class EEnemyAttackType : uint8
{
	Straight		UMETA(DisplayName = "straight bullet"),
	None			UMETA(DisplayName = "no attack"),
};

// EnemyType(敵の種類)
UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	ShootEnemy		UMETA(DisplayName = "Shoot Enemy"),
	EnergyEnemy		UMETA(DisplayName = "Energy Enemy"),
	DashEnemy		UMETA(DisplayName = "Dash Enemy")
};

UCLASS()
class GAME_API AEnemyCharaATKControl : public AEnemyChara
{
	GENERATED_BODY()

public:
	AEnemyCharaATKControl();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	// Enemy's Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP")
		int health;

	// Enemy type
	UPROPERTY(EditAnywhere, Category = "Enemy Type")
		EEnemyType enemyType;

	// Attacking type (攻撃タイプ)
	UPROPERTY(EditAnywhere, Category = "Attack")
		EEnemyAttackType enemyATKType;

	// The shootableDistance between Player and Enemy. (Playerとの射撃可能距離)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float shootableDistance;

	// Bullet type (弾の使用タイプ)
	UPROPERTY(EditAnywhere, Category = "Bullet")
		TSubclassOf<AActor> bulletActor;

	// Time Duration between two bullets.(発射間隔)
	UPROPERTY(EditAnywhere, Category = "Bullet")
		float bulletDuration;

	// Current time by the bullet.(弾の経過時間)
	UPROPERTY(EditAnywhere, Category = "Bullet")
		float bulletTimeCount;

	// Using the Offset to prevent the collide with enemy.(弾の生成位置補正)
	UPROPERTY(EditAnywhere, Category = "Offset")
		float bulletXOffset;

	// 死亡エフェクト
	UPROPERTY(EditAnywhere, Category = "Effects")
		UParticleSystem* DeadEffect;

	// 死亡エフェクトの生成位置
	UPROPERTY(EditAnywhere, Category = "Effects")
		FVector DeadEffectLocation;

	// Dead flag (死亡フラグ)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dead")
		bool isDead;

	// 当たり判定
	UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* HitComp,
			class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Attack flag (攻撃フラグ)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot Enemy")
		bool canAttack;

	// Moving trigger (移動フラグ)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy Enemy")
		bool isMoving;

private:
	APlayerChara* pPlayer;

	// 発射間隔カウントダウン
	bool canPlayEffect;
	bool closeToRightRoad;
	bool behindToPlayer;

	//EEnemyMoveType currentMoveType;

public:
	// 射程圏
	bool CloseToPlayer();
	// 発射開始
	void Shooting(float DeltaTime);
	// コースから離れる仕組み
	void LeaveFromRoad(float DeltaTime);
	// 死亡処理
	void Dead();
};