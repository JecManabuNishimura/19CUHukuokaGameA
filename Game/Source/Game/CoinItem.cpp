//----------------------------------------------------------
// ファイル名		：SensorTest.h
// 概要				：コインの操作を行う
// 作成者			：19CU0238 渡邊龍音
//
// 更新内容			：2020/09/17 渡邊龍音 作成
//					：2020/09/22 渡邊龍音 コインから一定距離を取るように
//					：2020/09/23 渡邊龍音 コインを取ったときに上に跳ねるように
//----------------------------------------------------------

#include "CoinItem.h"
#include "PlayerChara.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

ACoinItem::ACoinItem()
	: m_AddScore(100)
	, m_CoinDistance(100.0f)
	, m_CoinBounce(200.0f)
	, m_CoinBounceSpeed(10.0f)
	, m_CoinTimer(0.0f)
	, m_playerActor(NULL)
	, m_OriginScale(FVector::ZeroVector)
	, m_OriginLocation(FVector::ZeroVector)
{
	PrimaryActorTick.bCanEverTick = true;

	// ルートコンポーネント生成
	m_pRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = m_pRoot;

	// コリジョン用ボックスコンポーネント生成
	m_pBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	m_pBoxComp->SetupAttachment(RootComponent);

	// メッシュコンポーネント生成
	m_pItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoinMesh"));
	m_pItemMesh->SetupAttachment(RootComponent);
}

void ACoinItem::BeginPlay()
{
	Super::BeginPlay();	

	if (m_pBoxComp != NULL)
	{
		m_pBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ACoinItem::OnOverlapBegin);
	}

	// 現在のスケールを保存
	m_OriginScale = GetActorScale();

	// 現在の位置を保存
	m_OriginLocation = GetActorLocation();
}

void ACoinItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_playerActor != NULL)
	{
		m_CoinTimer += DeltaTime;

		if (FMath::RadiansToDegrees(m_CoinTimer * m_CoinBounceSpeed) >= 180.0f)
		{
			this->Destroy();
		}

		// 取得してからの時間の割合を計算
		float ratio = FMath::Clamp((FMath::RadiansToDegrees(m_CoinTimer * m_CoinBounceSpeed) / 180.0f), 0.0f, 1.0f);

		// 位置を更新
		FVector originPos = GetActorLocation();
		FVector targetPos = m_playerActor->GetActorLocation();

		originPos.X += m_CoinDistance;
		
		FVector newPos = FMath::Lerp(originPos, targetPos, ratio);

		newPos.Z = m_OriginLocation.Z + (FMath::Sin(m_CoinTimer * m_CoinBounceSpeed) * m_CoinBounce);

		// サイズの更新
		FVector newScale = FMath::Lerp(m_OriginScale, FVector::ZeroVector, ratio);

		SetActorLocation(newPos);
		SetActorScale3D(newScale);
	}
}

void ACoinItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 Otherbodyindex, bool bFromSweep, const FHitResult& sweepResult)
{
	if (OtherActor && (OtherActor != (AActor*)this) && OtherComp)
	{
		// ActorのタグがPlayerであれば
		if (OtherActor->ActorHasTag("Player"))
		{
			// プレイヤーのActorを保存
			m_playerActor = OtherActor;

			APlayerChara* pPlayer = Cast<APlayerChara>(OtherActor);

			if (pPlayer)
			{
				pPlayer->GetCoin();
			}

			// コリジョン無効化
			this->SetActorEnableCollision(false);

			// 移動
			FVector newPos = GetActorLocation();
			newPos.X += m_CoinDistance;

			SetActorLocation(newPos);
		}
	}
}