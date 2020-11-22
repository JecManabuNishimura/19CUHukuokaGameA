// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBullet.h"
#include "PlayerChara.h"
#include "Materials/MaterialInstanceDynamic.h"		//	color package
#include "Materials/MaterialInterface.h"			//	color package
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Engine.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AEnemyBullet::AEnemyBullet():
	  pPlayer(NULL)
	, isPlayerBeGuarding(false)
	, isReflectedByPlayer(false)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	collisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = collisionBox;

	if (pPlayer == NULL)
		pPlayer = Cast<APlayerChara>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

// Called when the game starts or when spawned
void AEnemyBullet::BeginPlay()
{
	Super::BeginPlay();

	//GetRootComponent()->getcap;
	//GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBullet::OnBeginOverlap);
	collisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBullet::OnBeginOverlap);

	//	colorÇÃê›íË
	auto Cube = FindComponentByClass<UStaticMeshComponent>();
	auto Material = Cube->GetMaterial(0);

	DynamicMaterial = UMaterialInstanceDynamic::Create(Material, NULL);
	Cube->SetMaterial(0, DynamicMaterial);

	//	ÉvÉåÉCÉÑÅ[Ç∆ìGÇälìæ
	//eEnemy = Cast<AEnemyCharaATKControl>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	UE_LOG(LogTemp, Warning, TEXT("%s"), isReflectedByPlayer ? TEXT("TRUE") : TEXT("FALSE"));
}

// Called every frame
void AEnemyBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	BulletMovement();

	float PlayerCurrentLocationX = pPlayer->GetActorLocation().X;
	if (GetActorLocation().X < PlayerCurrentLocationX) {
		Destroy();
	}
}

void AEnemyBullet::BulletMovement()
{
	if (isPlayerBeGuarding)
	{
		FVector NewPos = GetActorLocation();
		NewPos.X += 300.0f;
		SetActorLocation(NewPos);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *(FString::SanitizeFloat(NewPos.X)));

		//	colorÇÃê›íË
		float blend = 1.0f;
		DynamicMaterial->SetScalarParameterValue(TEXT("Blend"), blend);
	}
}


//FVector AEnemyBullet::GetEnemyPos()
//{
//	FVector eEnemyPos = eEnemy->GetActorLocation();
//	
//	return eEnemyPos;
//}

void AEnemyBullet::OnBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*if (OtherActor->ActorHasTag("Guard"))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, pPlayer->m_bGuarding ? TEXT("true"): TEXT("false"));

		
		isPlayerBeGuarding = true;
		pPlayer->GuardEnergy -= pPlayer->guardBulletUIDownSpeed;

		isReflectedByPlayer = true;
	}*/
}