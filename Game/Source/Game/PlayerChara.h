//----------------------------------------------------------
// �t�@�C����		�FPlayerChara.h
// �T�v				�F�v���C���[�̐���
// �쐬��			�F19CU0220 ����
// �X�V���e			�F2020/10/02 �쐬�@�v���C���[�̊e����
//					�F2020/11/12 �X�V�@�n粗����@�Z���T�[�������I�Ɍ��o����悤�ɂȂ�
//					�F2020/11/16 �ύX�@�߉Ɠ��@bulletActor��APlayerBullet�^�Ɍp������
//----------------------------------------------------------

// �C���N���[�h�K�[�h
#pragma once

// �C���N���[�h
#include "CoreMinimal.h"
#include "GameFramework/Character.h"	// ACharacter���p�����Ă��邽��
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Blueprint/UserWidget.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerChara.generated.h"

//	�O���錾
class USerial;
class USpringArmComponent;
class UCameraComponent;
class AActor;
class APlayerBullet;

UENUM(BlueprintType)
enum class PPlayerAttackType : uint8
{
	Straight		UMETA(DisplayName = "straight bullet"),
	None			UMETA(DisplayName = "no attack"),
};

UCLASS()
class GAME_API APlayerChara : public ACharacter
{
	// UE4�̃I�u�W�F�N�g�N���X�ŕK���擪�ɏ����}�N��
	GENERATED_BODY()

public:
	// �R���X�g���N�^
	APlayerChara();

protected:
	// �Q�[���X�^�[�g���A�܂��͐������ɌĂ΂�鏈��
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
public:
	// ���t���[���̍X�V����
	virtual void Tick(float DeltaTime) override;

	// �Z���T�[�̒l��Rotator�ɕϊ�
	FRotator SensorToRotator();

	// �e���͊֌W���\�b�h�Ƃ̃o�C���h����
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	//	�J�����X�V����
	void UpdateSensor(float _deltaTime);

	//	�ړ�����
	void UpdateMove(float _deltaTime);

	//	�W�����v����
	void UpdateJump(float _deltaTime);

	//	�K�[�h����
	void UpdateGuard();

	//	��������
	void UpdateAccelerate();

	//	���X�^�[�g
	void RestartGame();

	//	���S�J�E���g
	void DeadCount();

	//	�v���C���[��Z���l��
	void GetPlayerPosZ(float DeltaTime);

	//	�v���C���[�̃G�t�F�N�g
	void PlayEffects();

	//	====================================
	//	�Z���T�[�������ĂȂ��֐�

	void Shooting(float DeltaTime);

	//	�y���̓o�C���h�z�L�����ړ�:���E
	void MoveRightWithNoSensor(float _axisValue);

	//	�y���̓o�C���h�z�K�[�h�J�n
	void GuardStartWithNoSensor(float _axisValue);

	//	�y���̓o�C���h�z�_�b�V���J�n
	void DashOrJumpStartWithNoSensor(float _axisValue);

	void ShotStart(float _axisValue);
	//	====================================

private:
	// Arduino�̃V���A���ʐM�ۑ��p
	USerial* m_pArduinoSerial;

	// ��]�ʂ̕ۑ��i�Ȃ߂炩�Ɉړ�����悤�Ɂj
	TArray<FRotator> prevRotator;
	FRotator prevDiffRot;

	int serialPort;
	//----------------------------------------------------------------------------------
	bool isButtonRelerse;

	//	UPROPERTY�ɂ��邱�ƂŁA�u���[�v�����g��ŕϐ��̊m�F�A�ҏW�Ȃǂ��ł���
	//	�uBlueprintReadOnly�v�Ɏw�肵�Ă��邽�߁A�u���[�v�����g�Ō��邱�Ƃ����\�ŁA�ҏW�͂ł��Ȃ�
	UPROPERTY(EditAnywhere, Category = "Jump")
		float gravity;								//	�d��

	UPROPERTY(EditAnywhere, Category = "Jump")		//	�W�����v��
		float jumpPower;

	UPROPERTY(EditAnywhere, Category = "Jump")		//	�W�����v��
		float superJumpPower;

	float tempJumpPower;

	float jumpTime;									//	�W�����v����
	float nowJumpHeight;							//	���݃t���[���̃W�����v��
	float prevJumpHeight;							//	�O�t���[���̃W�����v��

	bool canJump;
	FVector posBeforeJump;							//	�W�������J�n�O�̃L�����N�^�[���W

	float startPosZ;								//	�J�n����Z���W
	float nowPosZ;									//	���̍��W
	bool overStartHight;							//	�n�_�̍��x���ׂ�flag

	bool hadDoOnce;

	float tempRotate;								//�@����Ԃɖ߂��̉�]�p�x

	bool haveGuardEnergy;							//	�K�[�h�Q�[�W���܂����邩

	float bulletTimeCount;							// ���ˊԊu�J�E���g�_�E��

	float tempSpeed;								//	�ő呬�x��ۑ�
	bool isDamageOver;								//	�_���[�W���󂯂��邩�ǂ���

	bool haveDashEnergy;							//	�_�b�V���̃G�l���M�[

	float tempDamageFrame;							//	�_���[�W���󂯂���̖��G����

	//	�Z���T�[��PC���Ńe�X�g���邽�߂�X,Y,Z�p�x�v���p�e�B
	float tempPitch;								
	float tempYaw;
	float tempRoll;

	float fps;										//	�Q�[����FPS

public:
	//	�Z���T�[�������Ă��܂���
	UPROPERTY(EditAnywhere, Category = "Sensor")
		bool withSensor;
	//------------------------------------------------------

	// Attacking type (�U���^�C�v)
	UPROPERTY(EditAnywhere, Category = "Attack")
		PPlayerAttackType playerATKType;

	// Bullet type (�e�̎g�p�^�C�v)
	UPROPERTY(EditAnywhere, Category = "Bullet")
		TSubclassOf<APlayerBullet> bulletActor;

	// Time Duration between two bullets.(���ˊԊu)
	UPROPERTY(EditAnywhere, Category = "Bullet")
		float bulletDuration;

	// Using the Offset to prevent the collide with enemy.(�e�̐����ʒu�␳)
	UPROPERTY(EditAnywhere, Category = "Offset")
		float bulletXOffset;

	UPROPERTY(EditAnywhere, Category = "Offset")
		float bulletYOffset;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	Dead��UI
		TSubclassOf<UUserWidget> Player_Select_Widget_Class;
	UUserWidget* Player_Select_Widget;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	Damage��UI
		TSubclassOf<UUserWidget> Player_Damage_Widget_Class;
	UUserWidget* Player_Damage_Widget;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	Goal��UI
		TSubclassOf<UUserWidget> Player_Goal_Widget_Class;
	UUserWidget* Player_Goal_Widget;

	UPROPERTY(EditAnywhere, Category = "Effects")	//	�_�b�V���G�t�F�N�g
		UNiagaraSystem* DashEffect;

	//	�_�b�V���G�t�F�N�g�������鎞�̈ʒu�␳
	UPROPERTY(EditAnywhere, Category = "Effects")	
		FVector DashEffectLocationOffset;
	UPROPERTY(EditAnywhere, Category = "Effects")
		FRotator DashEffectRotationOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	�v���C���[���񂾌�̑I����
		int32 selectPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	�K�[�h����������Q�[�W�̉������
		float guardBulletUIDownSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	�V���[�g�Q�[�W�̉�����X�s�[�h
		float Shot_UIDownSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	�V���[�g�Q�[�W�̉񕜃X�s�[�h
		float Shot_UIUpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	�K�[�h��Ԃ̃Q�[�W��������X�s�[�h
		float Guard_UIDownSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	�K�[�h�Q�[�W�̉񕜃X�s�[�h
		float Guard_UIUpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	�_�b�V���Q�[�W��������X�s�[�h
		float Dash_UIDownSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	�_�b�V���Q�[�W�̉񕜃X�s�[�h
		float Dash_UIUpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	�v���C���[�̃X�s�[�h
		float playerSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	�v���C���[�̍ő�X�s�[�h
		float playerMaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	�_�b�V�����鎞�̑��x�i�l�̓v���C���[�̒ʏ�X�s�[�h�Ɗ|�����������j
		float DashSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	�S�[�������܂ł̎���
		float GoalTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	�v���C���[��HP
		float HP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	�l�������R�C���̐�
		int32 CoinCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	�G�����ł�����
		int32 CountShootEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	����Ԓe�̃G�l���M�[
		float ShotEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	�e�̍ő�G�l���M�[
		float ShotMaxEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	����ԃK�[�h�̃G�l���M�[
		float GuardEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	�K�[�h�̍ő�G�l���M�[
		float GuardEnergyMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	����ԃ_�b�V���̃G�l���M�[
		float DashEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)		//	
		float DashEnergyMax;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float AddDashEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float DamageFrame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)			//	�R�C����Score
		float CoinScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float PlayerScore;								//	Player�l���̑�Score

	UPROPERTY(EditAnywhere, BlueprintReadWrite)			//	ScoreUI�̃A�j���[�V����flag
		bool ScoreAnimStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsAddScoreOver;

	//	=============================================================
	//	�v���C���[�̏��
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isGoal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool haveShowedRanking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isFirstShoting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isShoting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool haveShotEnergy;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		bool isJumping;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		bool isLanding;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		bool isDashing;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		bool isDashLine;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		bool isGuarding;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		bool canBeDamaged;

	UPROPERTY(EditAnywhere)
		float Damage;

	// Is Open Com Port
	UPROPERTY(BlueprintReadOnly, Category = "Sensor")
		bool isOpen;

	void GetCoin();

	UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* HitComp,
			class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OverlapEnds(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};