//----------------------------------------------------------
// �t�@�C����		�FDashEnemy.cpp
// �T�v				�F�_�b�V���G�l�~�[�̐���
// �쐬��			�F19CU0222 �߉Ɠ�
// �X�V���e			�F2020/11/20 �쐬�@�l��Blueprint��Ő��䂷��悤
//----------------------------------------------------------

#include "DashEnemy.h"

// Sets default values
ADashEnemy::ADashEnemy():
	isRidedOn(true),
	isDead(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADashEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADashEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/*void ADashEnemy::OnBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("PlayerBullet")) {
		OtherActor->Destroy();
	}
}*/

