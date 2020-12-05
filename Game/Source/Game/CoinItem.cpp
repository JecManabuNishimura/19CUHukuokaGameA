//----------------------------------------------------------
// �t�@�C����		�FSensorTest.h
// �T�v				�F�R�C���̑�����s��
// �쐬��			�F19CU0238 �n粗���
//
// �X�V���e			�F2020/09/17 �n粗��� �쐬
//					�F2020/09/22 �n粗��� �R�C�������苗�������悤��
//					�F2020/09/23 �n粗��� �R�C����������Ƃ��ɏ�ɒ��˂�悤��
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

	// ���[�g�R���|�[�l���g����
	m_pRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = m_pRoot;

	// �R���W�����p�{�b�N�X�R���|�[�l���g����
	m_pBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	m_pBoxComp->SetupAttachment(RootComponent);

	// ���b�V���R���|�[�l���g����
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

	// ���݂̃X�P�[����ۑ�
	m_OriginScale = GetActorScale();

	// ���݂̈ʒu��ۑ�
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

		// �擾���Ă���̎��Ԃ̊������v�Z
		float ratio = FMath::Clamp((FMath::RadiansToDegrees(m_CoinTimer * m_CoinBounceSpeed) / 180.0f), 0.0f, 1.0f);

		// �ʒu���X�V
		FVector originPos = GetActorLocation();
		FVector targetPos = m_playerActor->GetActorLocation();

		originPos.X += m_CoinDistance;
		
		FVector newPos = FMath::Lerp(originPos, targetPos, ratio);

		newPos.Z = m_OriginLocation.Z + (FMath::Sin(m_CoinTimer * m_CoinBounceSpeed) * m_CoinBounce);

		// �T�C�Y�̍X�V
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
		// Actor�̃^�O��Player�ł����
		if (OtherActor->ActorHasTag("Player"))
		{
			// �v���C���[��Actor��ۑ�
			m_playerActor = OtherActor;

			APlayerChara* pPlayer = Cast<APlayerChara>(OtherActor);

			if (pPlayer)
			{
				pPlayer->GetCoin();
			}

			// �R���W����������
			this->SetActorEnableCollision(false);

			// �ړ�
			FVector newPos = GetActorLocation();
			newPos.X += m_CoinDistance;

			SetActorLocation(newPos);
		}
	}
}