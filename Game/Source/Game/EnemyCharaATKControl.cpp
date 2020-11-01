//----------------------------------------------------------
// �t�@�C����		�FEnemyCharaATKControl.h
// �T�v				�F�G�̍U�����s��
// �쐬��			�F19CU0222 �߉Ɠ�
// �X�V���e			�F2020/10/02 �쐬 �e�̐����Ɣ���
//					�F
//----------------------------------------------------------

#include "EnemyCharaATKControl.h"
#include "PlayerChara.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"		// Player�̏��擾���邽��
#include "Engine.h"

AEnemyCharaATKControl::AEnemyCharaATKControl():
	shootableDistance(3000.0f),
	bulletTimeCount(0.0f),
	bulletDuration(1.0f),
	bulletXOffset(10.0f)
{
}

void AEnemyCharaATKControl::BeginPlay()
{
	Super::BeginPlay();

	// PlayerChara���̏����擾
	pPlayer = Cast<APlayerChara>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void AEnemyCharaATKControl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (enemyATKType)
	{
		case EEnemyAttackType::Straight:
			if (CloseToPlayer() == true) Shooting(DeltaTime);
			break;

		case EEnemyAttackType::None:
			UE_LOG(LogTemp, Warning, TEXT("Please give the Enemy(%s) attack type."), *(this->GetName()));
			break;

		default:
			UE_LOG(LogTemp, Warning, TEXT("There is something wrong to the EnemyATKType."));
			break;
	}
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
		UE_LOG(LogTemp, Warning, TEXT("Enemy( %s ) can attack."), *(this->GetName()));
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
		UE_LOG(LogTemp, Warning, TEXT("Enemy( %s ) is attacking. Using bullet type: %s"), *(this->GetName()), *(bulletActor->GetName()));
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue,);
	//UE_LOG(LogTemp, Warning, TEXT("value:%s"),*FString::SanitizeFloat(currentDistance));
}