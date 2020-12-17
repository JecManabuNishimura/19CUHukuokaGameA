//----------------------------------------------------------
// �t�@�C����		�FSensorTest.h
// �T�v				�F�G�̑�����s��
// �쐬��			�F19CU0238 �n粗���
//
// �X�V���e			�F2020/09/13 �n粗��� �쐬
//					�F2020/09/19 �n粗��� ���߂̂���G�̓����̒ǉ�
//					�F			 �����ړ���ForwardVector�ōs��
//----------------------------------------------------------
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyChara.generated.h"

UENUM(BlueprintType)
enum class EEnemyMoveType : uint8
{
	Line			UMETA(DisplayName = "Line Move"),
	Sin				UMETA(DisplayName = "Sine Wave Move"),
	BodyBlow		UMETA(DisplayName = "BodyBlow"),
	Overtake_Line	UMETA(DisplayName = "Overtake (Line)"),
	Overtake_Smooth	UMETA(DisplayName = "Overtake (Smooth)"),
	None			UMETA(DisplayName = "None Move"),
};

UCLASS()

class GAME_API AEnemyChara : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyChara();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// �����g�Ɏg�����Ԍv���p
	float sinTime;

	// ���Ƃ��Ƃ�Y���̈ʒu
	float originPosY;

	// ���Ƃ��Ƃ̉�]��
	FRotator originRotate;

	// �ːi�̎��Ԍv���p
	float chargeTime;

	// �ːi���n�߂邩�ǂ���
	bool isBlowing;

public:
	// �G�̈ړ��̎��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Move Generic")
		EEnemyMoveType enemyMoveType;

	// �����̈ړ��̑���
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Enemy Move Generic")
		float forwardSpeed;

	// �����g�ړ��̂Ƃ��̉��ړ��̑���
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Enemy Move Sine")
		float sinWaveSpeed;

	// �����g�ړ��̂Ƃ��̐U�ꕝ
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Enemy Move Sine")
		float sinWaveFrequency;

	// �i���j�v���C���[�̈ړ����x
	// �{���̓v���C���[����ړ����x���擾�ł���悤�ɂ�����
	UPROPERTY(EditAnyWhere, Category = "Enemy Body Blow")
		float _TEMP_playerMoveSpeed;

	// �ːi�̎��Ɏ~�܂�v���C���[�Ƃ̋���
	UPROPERTY(EditAnyWhere, Category = "Enemy Body Blow")
		float bodyBlowDistance;

	// �ːi�̎��̌������Ă��鑬�x
	UPROPERTY(EditAnyWhere, Category = "Enemy Body Blow")
		float bodyBlowMoveSpeed;

	// �ːi�̎��̑��x
	UPROPERTY(EditAnyWhere, Category = "Enemy Body Blow")
		float bodyBlowSpeed;

	// �ːi�̎��̗��ߎ���
	UPROPERTY(EditAnyWhere, Category = "Enemy Body Blow")
		float bodyBlowChargeTime;

	// �ːi�̎��̉�]���J�n����܂ł�x�点��
	UPROPERTY(EditAnyWhere, Category = "Enemy Body Blow")
		float bodyBlowTurnDelay;

	// �ːi�̎��̉�]���I������܂ł𑁂߂�
	UPROPERTY(EditAnyWhere, Category = "Enemy Body Blow")
		float bodyBlowTurnShorten;

	// Player����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Move Overtake")
		AActor* playerActor;

	// �ǂ������Ƃ��̑��x
	UPROPERTY(EditAnyWhere, Category = "Enemy Move Overtake")
		float overtakeSpeed;

	// ���ɓ���n�߂�I�t�Z�b�g�l
	UPROPERTY(EditAnyWhere, Category = "Enemy Move Overtake")
		float overtakeOffset;

	// �v���C���[�̈ʒu�ɗ��Ă�������ɓ���܂ł̋���
	UPROPERTY(EditAnyWhere, Category = "Enemy Move Overtake")
		float overtakeDistance;

	// �ǂ����������Ƃ�Y�����Ɉړ�����ʁi�����ɓ���ʁj
	UPROPERTY(EditAnyWhere, Category = "Enemy Move Overtake")
		float overtakeYAxisMove;

	// �����ړ�����
	//
	// _speed�F�ړ����x
	// _pos	 �F���݂̈ʒu
	void Move_Line(const float _speed, const FVector _pos);


	// �����g�ړ�����
	// 
	// _speedForward�F�O�ւ̈ړ����x
	// _speedSide	�F���ւ̈ړ����x
	// _frequency	�F���̐U�ꕝ
	// _deltaTime	�Fsin�̌v�Z�Ɏg�p����B�@Tick()��DeltaTime����
	// _pos			�F���݂̈ʒu
	void Move_Sin(const float _speedForward, const float _speedSide, const float _frequency, const float _deltaTime, const FVector _pos);


	// �ǂ��z���E�����ł̈ړ�
	// �����ɓ��鎞�ɑ��x�����X�ɕ�ԁi_overtakeSpeed -> _speed�j���܂��B
	//
	// _overtakeSpeed�F�ǂ��z�����̃X�s�[�h
	// _speed		 �F�ǂ��z�����I��������̑��x
	// _ePos		 �F����Actor�̌��݂̈ʒu
	// _pPos		 �FPlayer�ɐݒ肵��Actor�̌��݂̈ʒu
	void Move_Overtake_Line(const float _overtakeSpeed, const float _speed, const FVector _ePos, const FVector _pPos);


	// �ǂ��z���E�Ȃ߂炩�Ȉړ�
	// �����ɓ��鎞�ɑ��x�����X�ɕ�ԁi_overtakeSpeed -> _speed�j���܂��B
	//
	// _overtakeSpeed�F�ǂ��z�����̃X�s�[�h
	// _speed		 �F�ǂ��z�����I��������̑��x
	// _ePos		 �F����Actor�̌��݂̈ʒu
	// _pPos		 �FPlayer�ɐݒ肵��Actor�̌��݂̈ʒu
	void Move_Overtake_Lerp(const float _overtakeSpeed, const float _speed, const FVector _ePos, const FVector _pPos);


	// �Ȃ߂炩�Ȉړ��̍ۂɎg�������i3�_�̍��W����Ȃ߂炩�ȋȐ���`���j
	//
	// _startPos		�F3�_�̂����A�J�n�n�_�ɓ�������W
	// _midPos			�F3�_�̂����A���Ԓn�_�ɓ�������W
	// _endPos			�F3�_�̂����A�ڕW�n�_�ɓ�������W
	// _interpolation	�F���ꂼ��̍��W��0�`1�Ƃ������̊���
	// _posX			�F�i�C�Ӂjfloat�^�̕ϐ��̃A�h���X���w�肷��΁A���̃A�h���X�̕ϐ���X���W�݂̂�Ԃ�
	// _posY			�F�i�C�Ӂjfloat�^�̕ϐ��̃A�h���X���w�肷��΁A���̃A�h���X�̕ϐ���Y���W�݂̂�Ԃ�
	// _posZ			�F�i�C�Ӂjfloat�^�̕ϐ��̃A�h���X���w�肷��΁A���̃A�h���X�̕ϐ���Z���W�݂̂�Ԃ�
	FVector CalcLerpPos(const FVector _startPos, const FVector _midPos, const FVector _endPos, const float _interpolation
		, float* _posX = NULL, float* _posY = NULL, float* _posZ = NULL);

	// �G�̈ړ����@��ύX����
	void SetEnemyMoveType(EEnemyMoveType _moveType);

	// �v���C���[��Actor��GameInstance����擾����
	UFUNCTION(BlueprintImplementableEvent)
		void GetPlayerFromGameInstance();
};
