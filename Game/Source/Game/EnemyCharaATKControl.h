//----------------------------------------------------------
// �t�@�C����		�FEnemyCharaATKControl.h
// �T�v				�F�G�̍U�����s��
// �쐬��			�F19CU0222 �߉Ɠ�
// �X�V���e			�F2020/10/02 �쐬�@�e�̐����Ɣ���
//					�F2020/11/02 �ύX�@�v���C���[����PlayerCharaNoSensor�Ɋ֘A������̂��폜�iPlayerChara�ɓ������ꂽ�j
//					�F2020/11/03 �����@�G���j�̃G�t�F�N�g�𐶐�
//					�F2020/11/04 �����@���S�G�t�F�N�g�𐶐�
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "EnemyChara.h"
#include "PlayerChara.h"
#include "EnemyCharaATKControl.generated.h"

class AActor;
class UParticleSystem;

UENUM(BlueprintType)
enum class EEnemyAttackType : uint8
{
	Straight		UMETA(DisplayName = "straight bullet"),
	None			UMETA(DisplayName = "no attack"),
};

UCLASS()
class GAME_API AEnemyCharaATKControl : public AEnemyChara
{
	GENERATED_BODY()
	
public:
	AEnemyCharaATKControl();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	// Enemy's Health
	UPROPERTY(EditAnywhere,Category="HP")
		int health;

	// Attacking type (�U���^�C�v)
	UPROPERTY(EditAnywhere, Category = "Attack")
		EEnemyAttackType enemyATKType;

	// The shootableDistance between Player and Enemy. (Player�Ƃ̎ˌ��\����)
	UPROPERTY(EditAnywhere, Category = "Attack")
		float shootableDistance;

	// Bullet type (�e�̎g�p�^�C�v)
	UPROPERTY(EditAnywhere, Category = "Bullet")
		TSubclassOf<AActor> bulletActor;

	// Time Duration between two bullets.(���ˊԊu)
	UPROPERTY(EditAnywhere, Category = "Bullet")
		float bulletDuration;

	// Using the Offset to prevent the collide with enemy.(�e�̐����ʒu�␳)
	UPROPERTY(EditAnywhere, Category = "Offset")
		float bulletXOffset;

	UPROPERTY(EditAnywhere, Category = "Effects")
		UParticleSystem* DeadEffect;

	UPROPERTY(EditAnywhere, Category = "Effects")
		FVector DeadEffectLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dead")
		bool isDead;

	UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* HitComp,
			class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:

	APlayerChara* pPlayer;

	// ���ˊԊu�J�E���g�_�E��
	float bulletTimeCount;

	bool canPlayEffect;

public:
	// Player�Ƃ̋������߂����ǂ���
	bool CloseToPlayer();
	//���ˊJ�n
	void Shooting(float DeltaTime);

	void Dead();

};