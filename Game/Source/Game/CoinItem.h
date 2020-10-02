//----------------------------------------------------------
// ファイル名		：SensorTest.h
// 概要				：コインの操作を行う
// 作成者			：19CU0238 渡邊龍音
//
// 更新内容			：2020/09/17 渡邊龍音 作成
//					：2020/09/22 渡邊龍音 コインから一定距離を取るように
//					：2020/09/23 渡邊龍音 コインを取ったときに上に跳ねるように
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoinItem.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class GAME_API ACoinItem : public AActor
{
	GENERATED_BODY()
	
public:	
	ACoinItem();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	// オーバーラップ時に呼ばれる
	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 Otherbodyindex, bool bFromSweep, const FHitResult& sweepResult);

public:
	// コリジョン
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UBoxComponent* m_pBoxComp;

	// メッシュ
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UStaticMeshComponent* m_pItemMesh;

protected:
	// ルートコンポーネント用
	UPROPERTY()
		USceneComponent* m_pRoot;

private:
	// 加算スコア
	UPROPERTY(EditAnyWhere)
		int m_AddScore;

	// コイン取得時の距離
	UPROPERTY(EditAnyWhere)
		float m_CoinDistance;

	// 跳ねる量
	UPROPERTY(EditAnyWhere)
		float m_CoinBounce;

	// 跳ねる量
	UPROPERTY(EditAnyWhere)
		float m_CoinBounceSpeed;

	// コイン取得用タイマー
	float m_CoinTimer;

	// PlayerのActor
	AActor* m_playerActor;

	// もともとのScale
	FVector m_OriginScale;

	// もともとの位置
	FVector m_OriginLocation;
};
