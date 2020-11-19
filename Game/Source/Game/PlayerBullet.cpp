//----------------------------------------------------------
// �t�@�C����		�FPlayerBullet.cpp
// �T�v				�F�v���C���[�e�̐���
// �쐬��			�F19CU0220 ����
// �X�V���e			�F2020/10/02 �쐬�@�e�̐����Ɣ���
//					�F2020/11/13 �X�V�@�e�̃X�s�[�h��Blueprint��ŕύX�ł���悤�ɂȂ�
//----------------------------------------------------------

#include "PlayerBullet.h"
#include "PlayerChara.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

// Sets default values
APlayerBullet::APlayerBullet()
	: pPlayer(NULL)
	, playerBulletSpeed(100.f)
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

	//	�v���C���[���l��
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
	NewPos.X += playerBulletSpeed;
	SetActorLocation(NewPos);
}

void APlayerBullet::OnBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("ShotEnemy"))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, pPlayer->m_bGuarding ? TEXT("true"): TEXT("false"));
		//OtherActor->Destroy();

		this->Destroy();
	}
}