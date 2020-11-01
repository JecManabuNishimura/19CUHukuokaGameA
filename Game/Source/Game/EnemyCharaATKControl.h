//----------------------------------------------------------
// �t�@�C����		�FEnemyCharaATKControl.h
// �T�v				�F�G�̍U�����s��
// �쐬��			�F19CU0222 �߉Ɠ�
// �X�V���e			�F2020/10/02 �e�̐����Ɣ���
//					�F
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "EnemyChara.h"
#include "PlayerChara.h"
#include "EnemyCharaATKControl.generated.h"

class AActor;

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


private:

	APlayerChara* pPlayer;

	// ���ˊԊu�J�E���g�_�E��
	float bulletTimeCount;

public:
	// Player�Ƃ̋������߂����ǂ���
	bool CloseToPlayer();
	//���ˊJ�n
	void Shooting(float DeltaTime);

};