// Fill out your copyright notice in the Description page of Project Settings.


#include "BossWaveControl.h"

// Sets default values
ABossWaveControl::ABossWaveControl()
	: m_ReachToJumppadTime(2.0f)
	, m_ScaleAtGenerateMag(0.1f)
	, m_FinalScaleMag(1.0f)
	, m_Timer(0.0f)
	, m_JumppadPosX(0.0f)
	, m_OriginPos(FVector::ZeroVector)
	, m_OriginScale(FVector::OneVector)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABossWaveControl::BeginPlay()
{
	Super::BeginPlay();
	
	m_OriginPos = GetActorLocation();
	m_OriginScale = GetActorScale3D();
}

// Called every frame
void ABossWaveControl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	{
		m_Timer += DeltaTime;

		if (m_Timer > m_ReachToJumppadTime)
		{
			Destroy();
		}

		float nextPosX = FMath::Lerp(m_OriginPos.X, m_JumppadPosX, m_Timer / m_ReachToJumppadTime);
		float scaleMag = FMath::Lerp(m_ScaleAtGenerateMag, m_FinalScaleMag, m_Timer / m_ReachToJumppadTime);

		FVector nextPos(nextPosX, GetActorLocation().Y, GetActorLocation().Z);

		SetActorLocation(nextPos);
		SetActorScale3D(m_OriginScale * scaleMag);
	}
}

// ジャンプパッド位置の設定
void ABossWaveControl::SetJumppadPosX(float _posX)
{
	m_JumppadPosX = _posX;
}

