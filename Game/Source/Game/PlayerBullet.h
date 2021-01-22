//----------------------------------------------------------
// �t�@�C����		�FPlayerBullet.h
// �T�v				�F�v���C���[�e�̐���
// �쐬��			�F19CU0220 ����
// �X�V���e			�F2020/10/02 �쐬�@�e�̐����Ɣ���
//					�F2020/11/13 �X�V�@�e�̃X�s�[�h��Blueprint��ŕύX�ł���悤�ɂȂ�
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "PlayerChara.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "PlayerBullet.generated.h"

UCLASS()
class GAME_API APlayerBullet : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlayerBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	float fps;										//	�Q�[����FPS

	APlayerChara* pPlayer;

public:
	void BulletMovement();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float playerBulletSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBoxComponent* collisionBox;

	UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* HitComp,
			class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
