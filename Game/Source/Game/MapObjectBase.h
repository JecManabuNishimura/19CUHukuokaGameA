// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapObjectBase.generated.h"

UCLASS()
class GAME_API AMapObjectBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMapObjectBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// protected�ϐ�
protected:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UStaticMeshComponent* staticMeshComponent;
};
