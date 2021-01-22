//----------------------------------------------------------
// ファイル名		：PlayerBullet.h
// 概要				：プレイヤー弾の制御
// 作成者			：19CU0220 曹飛
// 更新内容			：2020/10/02 作成　弾の生成と発射
//					：2020/11/13 更新　弾のスピードをBlueprint上で変更できるようになる
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "PlayerChara.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "PlayerBullet.generated.h"

UCLASS()
class GAME_API APlayerBullet : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlayerBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	float fps;										//	ゲームのFPS

	APlayerChara* pPlayer;

public:
	void BulletMovement();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float playerBulletSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBoxComponent* collisionBox;

	UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* HitComp,
			class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
