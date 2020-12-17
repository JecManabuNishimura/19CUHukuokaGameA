// Fill out your copyright notice in the Description page of Project Settings.


#include "MapObjectBase.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AMapObjectBase::AMapObjectBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// RootConponent�̍쐬
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	staticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	staticMeshComponent->SetupAttachment(RootComponent);
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
