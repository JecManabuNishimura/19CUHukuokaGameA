// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossWaveControl.generated.h"

UCLASS()
class GAME_API ABossWaveControl : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABossWaveControl();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// プライベート変数
private:

	// ウェーブ全般に使用するプロパティ--------------------------------------------------------------------------------------

	// 生成されてからジャンプ台に到達する時間
	UPROPERTY(EditAnyWhere, Meta = (ToolTip = "Time to reach the jumppad after being generated."))
		float m_ReachToJumppadTime;

	// 生成された時のスケール倍率
	UPROPERTY(EditAnyWhere, Meta = (ToolTip = "Scale magnification at generation."))
		float m_ScaleAtGenerateMag;

	// 最終的なスケール倍率
	UPROPERTY(EditAnyWhere, Meta = (ToolTip = "Final scale magnification."))
		float m_FinalScaleMag;

	// タイマー
	float m_Timer;

	// 目標点となるジャンプ台のX軸の位置
	float m_JumppadPosX;

	// 生成時の位置保存用
	FVector m_OriginPos;

	// もともとのスケール保存
	FVector m_OriginScale;

	//-----------------------------------------------------------------------------------------------------------------------

	// パブリック関数
public:

	// ジャンプパッド位置の設定
	void SetJumppadPosX(float _posX);
};
