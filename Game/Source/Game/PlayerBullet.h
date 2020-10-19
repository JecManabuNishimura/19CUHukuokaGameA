// Fill out your copyright notice in the Description page of Project Settings.

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
	APlayerChara* pPlayer;

public:
	void BulletMovement();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBoxComponent* collisionBox;

	UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* HitComp,
			class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
