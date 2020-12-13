// Fill out your copyright notice in the Description page of Project Settings.


#include "MapObjectBase.h"

// Sets default values
AMapObjectBase::AMapObjectBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UStaticMeshComponent* staticMeshObj = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	staticMeshComponent = staticMeshObj;
}

// Called when the game starts or when spawned
void AMapObjectBase::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMapObjectBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
