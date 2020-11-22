//----------------------------------------------------------
// ファイル名		：EnemyCharaATKControl.h
// 概要				：敵の攻撃を行う
// 作成者			：19CU0222 鍾家同
// 更新内容			：2020/10/02 作成　弾の生成と発射
//					：2020/11/02 変更　プレイヤー側でPlayerCharaNoSensorに関連するものを削除（PlayerCharaに統合された）
//					：2020/11/03 増加　敵撃破のエフェクトを生成
//					：2020/11/04 増加　死亡エフェクトを生成
//					：2020/11/13 変更　敵弾はゲームスタート後で発射する
//					：2020/11/15 増加　EEnemyType列挙型を追加
//					：2020/11/16 増加　EnergyEnemyの生成
//					：2020/11/16 増加　曹飛　ShotEnemyの生成
//----------------------------------------------------------

#include "EnemyCharaATKControl.h"
#include "PlayerChara.h"
#include "EnemyBullet.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"		// Playerの情報取得するため
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

	// PlayerChara側の情報を取得
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

	// エネミーの死亡処理
	if (health <= 0) {
		// 動かせない
		enemyMoveType = EEnemyMoveType::None;
		// 当たり判定をオフ
		SetActorEnableCollision(false);
		isMoving = false;
		isDead = true;
		Dead();
	}
}

// Playerとの距離が近いかどうか
bool AEnemyCharaATKControl::CloseToPlayer()
{
	//pPlayer = Cast<APlayerChara>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	FVector vectortoPlayer = pPlayer->GetActorLocation() - this->GetActorLocation();
	float currentDistance = vectortoPlayer.Size();

	//UE_LOG(LogTemp, Warning, TEXT("value:%s"), *FString::SanitizeFloat(currentDistance));

	// プレイヤーと一定距離に入れば発射開始
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

//発射開始
void AEnemyCharaATKControl::Shooting(float DeltaTime)
{
	bulletTimeCount += DeltaTime;

	FVector currentVector = GetActorLocation();
	FRotator currentRotator = GetActorRotation();

	if (bulletTimeCount >= bulletDuration && pPlayer->isStart) {
		// 弾の作成：SpawnActor<生成するクラス型>(生成するクラス、始点座標、始点回転座標)
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
