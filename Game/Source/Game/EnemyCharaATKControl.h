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
//					�F2021/02/10 �����@����Ȃ��R�����g��⑫
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "EnemyChara.h"
#include "PlayerChara.h"
#include "EnemyCharaATKControl.generated.h"

class AActor;
class UParticleSystem;

// BulletType(�e�̎��)
UENUM(BlueprintType)
enum class EEnemyAttackType : uint8
{
	Straight		UMETA(DisplayName = "straight bullet"),
	None			UMETA(DisplayName = "no attack"),
};

// EnemyType(�G�̎��)
UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	ShootEnemy		UMETA(DisplayName = "Shoot Enemy"),
	EnergyEnemy		UMETA(DisplayName = "Energy Enemy"),
	DashEnemy		UMETA(DisplayName = "Dash Enemy")
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP")
		int health;

	// Enemy type
	UPROPERTY(EditAnywhere, Category = "Enemy Type")
		EEnemyType enemyType;

	// Attacking type (�U���^�C�v)
	UPROPERTY(EditAnywhere, Category = "Attack")
		EEnemyAttackType enemyATKType;

	// The shootableDistance between Player and Enemy. (Player�Ƃ̎ˌ��\����)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float shootableDistance;

	// Bullet type (�e�̎g�p�^�C�v)
	UPROPERTY(EditAnywhere, Category = "Bullet")
		TSubclassOf<AActor> bulletActor;

	// Time Duration between two bullets.(���ˊԊu)
	UPROPERTY(EditAnywhere, Category = "Bullet")
		float bulletDuration;

	// Current time by the bullet.(�e�̌o�ߎ���)
	UPROPERTY(EditAnywhere, Category = "Bullet")
		float bulletTimeCount;

	// Using the Offset to prevent the collide with enemy.(�e�̐����ʒu�␳)
	UPROPERTY(EditAnywhere, Category = "Offset")
		float bulletXOffset;

	// ���S�G�t�F�N�g
	UPROPERTY(EditAnywhere, Category = "Effects")
		UParticleSystem* DeadEffect;

	// ���S�G�t�F�N�g�̐����ʒu
	UPROPERTY(EditAnywhere, Category = "Effects")
		FVector DeadEffectLocation;

	// Dead flag (���S�t���O)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dead")
		bool isDead;

	// �����蔻��
	UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* HitComp,
			class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Attack flag (�U���t���O)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot Enemy")
		bool canAttack;

	// Moving trigger (�ړ��t���O)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy Enemy")
		bool isMoving;

private:
	APlayerChara* pPlayer;

	// ���ˊԊu�J�E���g�_�E��
	bool canPlayEffect;
	bool closeToRightRoad;
	bool behindToPlayer;

	//EEnemyMoveType currentMoveType;

public:
	// �˒���
	bool CloseToPlayer();
	// ���ˊJ�n
	void Shooting(float DeltaTime);
	// �R�[�X���痣���d�g��
	void LeaveFromRoad(float DeltaTime);
	// ���S����
	void Dead();
};