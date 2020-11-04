//----------------------------------------------------------
// �t�@�C����		�FEnemyCharaATKControl.h
// �T�v				�F�G�̍U�����s��
// �쐬��			�F19CU0222 �߉Ɠ�
// �X�V���e			�F2020/10/02 �쐬�@�e�̐����Ɣ���
//					�F2020/11/02 �ύX�@�v���C���[����PlayerCharaNoSensor�Ɋ֘A������̂��폜�iPlayerChara�ɓ������ꂽ�j
//					�F2020/11/03 �����@�G���j�̃G�t�F�N�g�𐶐�
//					�F2020/11/04 �����@���S�G�t�F�N�g�𐶐�
//----------------------------------------------------------

#include "EnemyCharaATKControl.h"
#include "PlayerChara.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"		// Player�̏��擾���邽��
#include "Engine.h"

AEnemyCharaATKControl::AEnemyCharaATKControl():
	shootableDistance(3000.0f),
	bulletTimeCount(0.0f),
	bulletDuration(1.0f),
	bulletXOffset(400.0f),
	DeadEffectLocation(0.0f,0.0f,10.0f),
	isDead(false),
	health(5),
	canPlayEffect(true)
{
}

void AEnemyCharaATKControl::BeginPlay()
{
	Super::BeginPlay();

	// PlayerChara���̏����擾
	pPlayer = Cast<APlayerChara>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharaATKControl::OnBeginOverlap);
}

void AEnemyCharaATKControl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (enemyATKType)
	{
		case EEnemyAttackType::Straight:
			if (CloseToPlayer() == true && !isDead) Shooting(DeltaTime);
			break;

		case EEnemyAttackType::None:
			UE_LOG(LogTemp, Warning, TEXT("Please give the Enemy(%s) attack type."), *(this->GetName()));
			break;

		default:
			UE_LOG(LogTemp, Warning, TEXT("There is something wrong to the EnemyATKType."));
			break;
	}

	if (isDead)
		Dead();
}

// Player�Ƃ̋������߂����ǂ���
bool AEnemyCharaATKControl::CloseToPlayer()
{
	//APlayerChara* pPlayer = Cast<APlayerChara>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	FVector vectortoPlayer = pPlayer->GetActorLocation() - this->GetActorLocation();
	float currentDistance = vectortoPlayer.Size();

	//UE_LOG(LogTemp, Warning, TEXT("value:%s"), *FString::SanitizeFloat(currentDistance));

	// �v���C���[�ƈ�苗������Δ��ˊJ�n
	if (currentDistance <= shootableDistance && vectortoPlayer.X < 0.0f) {
		//UE_LOG(LogTemp, Warning, TEXT("Enemy( %s ) can attack."), *(this->GetName()));
		return true;
	}
	else return false;
}

//���ˊJ�n
void AEnemyCharaATKControl::Shooting(float DeltaTime)
{
	bulletTimeCount += DeltaTime;

	FVector currentVector = GetActorLocation();
	FRotator currentRotator = GetActorRotation();
	if (bulletTimeCount >= bulletDuration) {		// �e�̍쐬�FSpawnActor<AActor>(��������N���X�A�n�_���W�A�n�_��]���W)
		GetWorld()->SpawnActor<AActor>(bulletActor, currentVector + this->GetActorForwardVector() * bulletXOffset, currentRotator);
		bulletTimeCount = 0.0f;
		//UE_LOG(LogTemp, Warning, TEXT("Enemy( %s ) is attacking. Using bullet type: %s"), *(this->GetName()), *(bulletActor->GetName()));
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue,);
	//UE_LOG(LogTemp, Warning, TEXT("value:%s"),*FString::SanitizeFloat(currentDistance));
}

void AEnemyCharaATKControl::Dead()
{
	if (canPlayEffect) {
		UGameplayStatics::SpawnEmitterAttached(DeadEffect, RootComponent, TEXT("Dead"), DeadEffectLocation);
		canPlayEffect = false;
	}
}

void AEnemyCharaATKControl::OnBeginOverlap(
	UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("PlayerBullet") || OtherActor->ActorHasTag("EnemyBullet")) {
		UE_LOG(LogTemp, Warning, TEXT("Hit!!"));
		if (health > 0) {
			health -= 1;
		}
		else if (health <= 0) {
			this->SetActorEnableCollision(false);
			health = 0;
			isDead = true;
		}
	}
}
