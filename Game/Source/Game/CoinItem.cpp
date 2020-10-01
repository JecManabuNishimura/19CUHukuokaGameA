//----------------------------------------------------------
// ファイル名		：SensorTest.h
// 概要				：コインの操作を行う
// 作成者			：19CU0238 渡邊龍音
//
// 更新内容			：2020/09/17 渡邊龍音 作成
//----------------------------------------------------------

#include "CoinItem.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

ACoinItem::ACoinItem()
	: m_AddScore(100)
	, m_GetCoinTime(5.0f)
	, m_CoinTimer(0.0f)
	, m_playerActor(NULL)
	, m_OriginScale(FVector::ZeroVector)
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

	// Playerと触れるまでは毎フレーム処理する必要がない
	PrimaryActorTick.SetTickFunctionEnable(false);

	// 現在のスケールを保存
	m_OriginScale = GetActorScale();
}

void ACoinItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_playerActor != NULL)
	{
		m_CoinTimer += DeltaTime;

		if (m_CoinTimer >= m_GetCoinTime)
		{
			this->Destroy();
		}

		// 取得してからの時間の割合を計算
		float ratio = FMath::Clamp((m_CoinTimer / m_GetCoinTime), 0.0f, 1.0f);

		// 位置を更新
		FVector newPos = FMath::Lerp(GetActorLocation(), m_playerActor->GetActorLocation(), ratio);

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

			// Tick()有効化
			PrimaryActorTick.SetTickFunctionEnable(true);

			// コリジョン無効化
			this->SetActorEnableCollision(false);
		}
	}
}