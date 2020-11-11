// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBullet.h"
#include "PlayerChara.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

// Sets default values
APlayerBullet::APlayerBullet()
	: pPlayer(NULL)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	collisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = collisionBox;
}

// Called when the game starts or when spawned
void APlayerBullet::BeginPlay()
{
	Super::BeginPlay();

	collisionBox->OnComponentBeginOverlap.AddDynamic(this, &APlayerBullet::OnBeginOverlap);

	//	ÉvÉåÉCÉÑÅ[Çälìæ
	pPlayer = Cast<APlayerChara>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

// Called every frame
void APlayerBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	BulletMovement();
}

void APlayerBullet::BulletMovement()
{
	FVector NewPos = GetActorLocation();
	NewPos.X += 100.f;
	SetActorLocation(NewPos);
}

void APlayerBullet::OnBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Enemy"))
	{
		pPlayer->PlayerScore += pPlayer->EnemyScore;

		pPlayer->CountShootEnemy += 1;

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, pPlayer->m_bGuarding ? TEXT("true"): TEXT("false"));
		//OtherActor->Destroy();

		this->Destroy();
	}
}


