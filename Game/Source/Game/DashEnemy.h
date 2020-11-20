//----------------------------------------------------------
// ファイル名		：DashEnemy.h
// 概要				：ダッシュエネミーの制御
// 作成者			：19CU0222 鍾家同
// 更新内容			：2020/11/20 作成　値をBlueprint上で制御するよう
//----------------------------------------------------------
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DashEnemy.generated.h"

UCLASS()
class GAME_API ADashEnemy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADashEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// 何かに載せられたのか
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
		bool isRidedOn;
	// 当たれるフラグ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
		bool isDead;

	/*UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* HitComp,
			class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);*/
};
