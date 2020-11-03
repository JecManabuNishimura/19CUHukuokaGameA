//----------------------------------------------------------
// ファイル名		：EnemyCharaATKControl.h
// 概要				：敵の攻撃を行う
// 作成者			：19CU0222 鍾家同
// 更新内容			：2020/10/02 作成 弾の生成と発射
//					：
//----------------------------------------------------------

#include "EnemyCharaATKControl.h"
#include "PlayerChara.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"		// Playerの情報取得するため
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

	// PlayerChara側の情報を取得
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

// Playerとの距離が近いかどうか
bool AEnemyCharaATKControl::CloseToPlayer()
{
	//APlayerChara* pPlayer = Cast<APlayerChara>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	FVector vectortoPlayer = pPlayer->GetActorLocation() - this->GetActorLocation();
	float currentDistance = vectortoPlayer.Size();

	//UE_LOG(LogTemp, Warning, TEXT("value:%s"), *FString::SanitizeFloat(currentDistance));

	// プレイヤーと一定距離入れば発射開始
	if (currentDistance <= shootableDistance && vectortoPlayer.X < 0.0f) {
		UE_LOG(LogTemp, Warning, TEXT("Enemy( %s ) can attack."), *(this->GetName()));
		return true;
	}
	else return false;
}

//発射開始
void AEnemyCharaATKControl::Shooting(float DeltaTime)
{
	bulletTimeCount += DeltaTime;

	FVector currentVector = GetActorLocation();
	FRotator currentRotator = GetActorRotation();
	if (bulletTimeCount >= bulletDuration) {		// 弾の作成：SpawnActor<AActor>(生成するクラス、始点座標、始点回転座標)
		GetWorld()->SpawnActor<AActor>(bulletActor, currentVector + this->GetActorForwardVector() * bulletXOffset, currentRotator);
		bulletTimeCount = 0.0f;
		UE_LOG(LogTemp, Warning, TEXT("Enemy( %s ) is attacking. Using bullet type: %s"), *(this->GetName()), *(bulletActor->GetName()));
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue,);
	//UE_LOG(LogTemp, Warning, TEXT("value:%s"),*FString::SanitizeFloat(currentDistance));
}