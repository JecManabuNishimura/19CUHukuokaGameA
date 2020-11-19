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
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "EnemyChara.h"
#include "PlayerChara.h"
#include "EnemyCharaATKControl.generated.h"

class AActor;
class UParticleSystem;
class AEnemyBullet;

UENUM(BlueprintType)
enum class EEnemyAttackType : uint8
{
	Straight		UMETA(DisplayName = "straight bullet"),
	None			UMETA(DisplayName = "no attack"),
};

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	ShootEnemy		UMETA(DisplayName = "Shoot Enemy"),
	EnergyEnemy		UMETA(DisplayName = "Energy Enemy")
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
	UPROPERTY(EditAnywhere, Category = "HP")
		int health;

	// Enemy type
	UPROPERTY(EditAnywhere, Category = "Enemy Type")
		EEnemyType enemyType;

	// Attacking type (攻撃タイプ)
	UPROPERTY(EditAnywhere, Category = "Attack")
		EEnemyAttackType enemyATKType;

	// The shootableDistance between Player and Enemy. (Playerとの射撃可能距離)
	UPROPERTY(EditAnywhere, Category = "Attack")
		float shootableDistance;

	// Bullet type (弾の使用タイプ)
	UPROPERTY(EditAnywhere, Category = "Bullet")
		TSubclassOf<AActor> bulletActor;

	// Time Duration between two bullets.(発射間隔)
	UPROPERTY(EditAnywhere, Category = "Bullet")
		float bulletDuration;

	// Using the Offset to prevent the collide with enemy.(弾の生成位置補正)
	UPROPERTY(EditAnywhere, Category = "Offset")
		float bulletXOffset;

	UPROPERTY(EditAnywhere, Category = "Effects")
		UParticleSystem* DeadEffect;

	UPROPERTY(EditAnywhere, Category = "Effects")
		FVector DeadEffectLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dead")
		bool isDead;

	UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* HitComp,
			class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot Enemy")
		bool canAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy Enemy")
		bool isMoving;

	UPROPERTY(EditAnywhere, Category = "Energy Enemy")
		float energyEnemyScore;

private:

	APlayerChara* pPlayer;

	// 発射間隔カウントダウン
	float bulletTimeCount;

	bool canPlayEffect;
	bool closeToRightRoad;
	bool behindToPlayer;

	EEnemyMoveType currentMoveType;

public:
	// Playerとの距離が近いかどうか
	bool CloseToPlayer();
	// 発射開始
	void Shooting(float DeltaTime);
	// コースから離れる
	void LeaveFromRoad(float DeltaTime);

	void Dead();
	
};