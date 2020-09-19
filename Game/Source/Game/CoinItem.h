//----------------------------------------------------------
// ファイル名		：SensorTest.h
// 概要				：コインの操作を行う
// 作成者			：19CU0238 渡邊龍音
//
// 更新内容			：2020/09/17 渡邊龍音 作成
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

	// 何秒でコインを取得できるか
	UPROPERTY(EditAnyWhere)
		float m_GetCoinTime;

	// コイン取得用タイマー
	float m_CoinTimer;

	// PlayerのActor
	AActor* m_playerActor;

	// もともとのScale
	FVector m_OriginScale;
};
