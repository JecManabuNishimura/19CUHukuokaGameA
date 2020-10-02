#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ParticleControl.generated.h"

UCLASS()
class GAME_API AParticleControl : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AParticleControl();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// パーティクルのActor
	UPROPERTY(EditAnyWhere, Category = "ParticleSetting")
		AActor* m_pVolt;

	// 移動速度
	UPROPERTY(EditAnyWhere, Category = "ParticleSetting")
		float m_MoveSpeed;

	// 振れ幅の最低
	UPROPERTY(EditAnyWhere, Category = "ParticleSetting")
		float m_FrequencyMin;

	// 振れ幅の最高
	UPROPERTY(EditAnyWhere, Category = "ParticleSetting")
		float m_FrequencyMax;

	// 何秒で一周波するか
	UPROPERTY(EditAnyWhere, Category = "ParticleSetting")
		float m_FrequencyTime;

	// もともとの位置
	FVector m_originPos;

	// 時間計測用タイマー
	float m_timer;

	// 上向き下向きか
	bool m_isDirectionUp;

private:
	// Actor移動処理
	void MoveParticle(float _deltaTime);
};
