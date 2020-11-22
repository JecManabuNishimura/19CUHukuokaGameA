//----------------------------------------------------------
// �t�@�C����		�FEnemyCharaATKControl.h
// �T�v				�F�G�̍U�����s��
// �쐬��			�F19CU0222 �߉Ɠ�
// �X�V���e			�F2020/10/02 �쐬�@�e�̐����Ɣ���
//					�F2020/11/02 �ύX�@�v���C���[����PlayerCharaNoSensor�Ɋ֘A������̂��폜�iPlayerChara�ɓ������ꂽ�j
//					�F2020/11/03 �����@�G���j�̃G�t�F�N�g�𐶐�
//					�F2020/11/04 �����@���S�G�t�F�N�g�𐶐�
//					�F2020/11/13 �ύX�@�G�e�̓Q�[���X�^�[�g��Ŕ��˂���
//					�F2020/11/15 �����@EEnemyType�񋓌^��ǉ�
//					�F2020/11/16 �����@EnergyEnemy�̐���
//					�F2020/11/16 �����@����@ShotEnemy�̐���
//----------------------------------------------------------

#include "EnemyCharaATKControl.h"
#include "PlayerChara.h"
#include "EnemyBullet.h"
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
	canPlayEffect(true),
	canAttack(false),
	closeToRightRoad(true),
	behindToPlayer(false),
	isMoving(true),
	energyEnemyScore(200)
{
}

void AEnemyCharaATKControl::BeginPlay()
{
	Super::BeginPlay();

	// PlayerChara���̏����擾
	pPlayer = Cast<APlayerChara>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharaATKControl::OnBeginOverlap);

	//currentMoveType = enemyMoveType;
}

void AEnemyCharaATKControl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (enemyType)
	{
	case EEnemyType::ShootEnemy:
		switch (enemyATKType)
		{
		case EEnemyAttackType::Straight:
			if (canAttack) Shooting(DeltaTime);
			break;

		case EEnemyAttackType::None:
			UE_LOG(LogTemp, Warning, TEXT("Please give the Enemy(%s) attack type."), *(this->GetName()));
			break;

		default:
			UE_LOG(LogTemp, Warning, TEXT("There is something wrong to the EnemyATKType."));
			break;
		}
		break;
	case EEnemyType::EnergyEnemy:
		if (behindToPlayer && !isDead) {
			LeaveFromRoad(DeltaTime);
		}
		switch (enemyATKType)
		{
		case EEnemyAttackType::Straight:
			if (CloseToPlayer() == false && !isDead) {
				canAttack = false;
				isMoving = true;
			}
			else if (CloseToPlayer() == true && !isDead) {
				//UE_LOG(LogTemp, Warning, TEXT("closeToPlayer==true!!!!"));
				canAttack = true;
				isMoving = true;
				Shooting(DeltaTime);
			}
			break;

		case EEnemyAttackType::None:
			UE_LOG(LogTemp, Warning, TEXT("Please give the Enemy(%s) attack type."), *(this->GetName()));
			break;

		default:
			UE_LOG(LogTemp, Warning, TEXT("There is something wrong to the EnemyATKType."));
			break;
		}
		break;
	}

	// �G�l�~�[�̎��S����
	if (health <= 0) {
		// �������Ȃ�
		enemyMoveType = EEnemyMoveType::None;
		// �����蔻����I�t
		SetActorEnableCollision(false);
		isMoving = false;
		isDead = true;
		Dead();
	}
}

// Player�Ƃ̋������߂����ǂ���
bool AEnemyCharaATKControl::CloseToPlayer()
{
	//pPlayer = Cast<APlayerChara>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	FVector vectortoPlayer = pPlayer->GetActorLocation() - this->GetActorLocation();
	float currentDistance = vectortoPlayer.Size();

	//UE_LOG(LogTemp, Warning, TEXT("value:%s"), *FString::SanitizeFloat(currentDistance));

	// �v���C���[�ƈ�苗���ɓ���Δ��ˊJ�n
	if (currentDistance <= shootableDistance && vectortoPlayer.X < 0.0f) {
		//UE_LOG(LogTemp, Warning, TEXT("Enemy( %s ) can attack."), *(this->GetName()));
		
		return true;
	}
	else if (vectortoPlayer.X > 0.0f) {
		//UE_LOG(LogTemp, Warning, TEXT("Enemy( %s ) can attack."), *(this->GetName()));
		behindToPlayer = true;
		return false;
	}
	else return false;
}

//���ˊJ�n
void AEnemyCharaATKControl::Shooting(float DeltaTime)
{
	bulletTimeCount += DeltaTime;

	FVector currentVector = GetActorLocation();
	FRotator currentRotator = GetActorRotation();

	if (bulletTimeCount >= bulletDuration && pPlayer->isStart) {
		// �e�̍쐬�FSpawnActor<��������N���X�^>(��������N���X�A�n�_���W�A�n�_��]���W)
		if (bulletActor != NULL)
			GetWorld()->SpawnActor<AActor>(bulletActor, currentVector + this->GetActorForwardVector() * bulletXOffset, currentRotator);
		bulletTimeCount = 0.0f;
		//UE_LOG(LogTemp, Warning, TEXT("Enemy( %s ) is attacking. Using bullet type: %s"), *(this->GetName()), *(bulletActor->GetName()));
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue,);
	//UE_LOG(LogTemp, Warning, TEXT("value:%s"),*FString::SanitizeFloat(currentDistance));
}

void AEnemyCharaATKControl::LeaveFromRoad(float DeltaTime)
{
	FRotator currentRotation = GetActorRotation();
	if (this->GetActorLocation().Y >= 0) {
		closeToRightRoad = true;
		SetActorRotation(FMath::Lerp(FQuat(currentRotation), FQuat(FRotator(0.0f, 90.0f, 0.0f)), 0.1));
		if (GetActorLocation().Y >= 1900.0f) Destroy();
	}
	else if (this->GetActorLocation().Y < 0) {
		closeToRightRoad = false;
		SetActorRotation(FMath::Lerp(FQuat(currentRotation), FQuat(FRotator(0.0f, -90.0f, 0.0f)), 0.1));
		if (GetActorLocation().Y <= -1900.0f) Destroy();
	}
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
	if (OtherActor->ActorHasTag("PlayerBullet")) {
		if (health > 0) {
			health -= 1;
		}
		else if (health <= 0) {
			health = 0;
		}
	}

	AEnemyBullet* pEnemyBullet = Cast<AEnemyBullet>(OtherActor);
	if (OtherActor->ActorHasTag("EnemyBullet") && pEnemyBullet->isReflectedByPlayer) {
		UE_LOG(LogTemp, Warning, TEXT("ReflectHit"));
		if (this->ActorHasTag("EnergyEnemy")) pPlayer->PlayerScore += energyEnemyScore;
		if (health > 0) {
			health -= 1;
		}
		else if (health <= 0) {
			health = 0;
		}
	}
}
