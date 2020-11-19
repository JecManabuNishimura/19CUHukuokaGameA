// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "PlayerChara.h"
#include "EnemyCharaATKControl.h"
#include "EnemyChara.h"
#include "EnemyBullet.generated.h"

class UMaterialInstanceDynamic;
class UParticleSystem;

UCLASS()
class GAME_API AEnemyBullet : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEnemyBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	APlayerChara* pPlayer;
	AEnemyCharaATKControl* eEnemy;

	UMaterialInstanceDynamic* DynamicMaterial;

	bool isPlayerBeGuarding;

private:
	void PlayEffects();

public:
	void BulletMovement();

	bool isReflectedByPlayer;

	FVector GetEnemyPos();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBoxComponent* collisionBox;

	UPROPERTY(EditAnywhere, Category = "Effects")
		UParticleSystem* GuardEffect;

	UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* HitComp,
			class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
