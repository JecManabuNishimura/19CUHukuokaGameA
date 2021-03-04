//----------------------------------------------------------
// �t�@�C����		�FPlayerChara.cpp
// �T�v				�F�v���C���[�̐���
// �쐬��			�F19CU0220 ����
// �X�V���e			�F2020/10/02 �쐬�@�v���C���[�̊e����
//					�F2020/11/12 �X�V�@�n粗����@�Z���T�[�������I�Ɍ��o����悤�ɂȂ�
//					�F2020/11/16 �ύX�@�߉Ɠ��@bulletActor��APlayerBullet�^�Ɍp������
//					�F2021/01/06 �ύX�@�߉Ɠ��@GoalWidget�̐����^�C�~���O��ύX(VR�ł̂���)
//----------------------------------------------------------

// �C���N���[�h
#include "PlayerChara.h"
#include "PlayerBullet.h"
#include "SensorManager.h"
#include "Engine.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "..\..\UE4Duino\Source\UE4Duino\Public\Serial.h"
#include <time.h>
#include "HeadMountedDisplayFunctionLibrary.h"			// VR's 

//	define�}�N��
#define ROTATOR_ARRAY_SIZE 1
#define JUMP_HEIGHT (jumpTime * jumpTime * (-gravity) / 2) + (jumpTime * jumpPower)

// �R���X�g���N�^
APlayerChara::APlayerChara()
	: m_pArduinoSerial(NULL)
	, withSensor(false)
	, serialPort(4)
	, isButtonRelerse(true)
	, isOpen(false)
	, startPosZ(0.f)
	, nowPosZ(0.f)
	, overStartHight(false)
	, hadDoOnce(false)
	, tempSpeed(0.f)
	, bulletTimeCount(0.0f)
	, bulletDuration(1.0f)
	, bulletXOffset(10.0f)
	, bulletYOffset(0.f)
	, playerSpeed(0.f)
	, playerMaxSpeed(200.f)
	, DashSpeed(1.5f)
	, gravity(700.f)
	, jumpPower(1200.f)
	, superJumpPower(2400.f)
	, tempJumpPower(0.f)
	, jumpTime(0.f)
	, nowJumpHeight(0.f)
	, prevJumpHeight(0.f)
	, isShoting(false)
	, canJump(false)
	, isJumping(false)
	, isLanding(false)
	, tempRotate(0.f)
	, isGuarding(false)
	, isDashing(false)
	, isDashLine(false)
	, tempDamageFrame(0.f)
	, canBeDamaged(true)
	, haveGuardEnergy(true)
	, haveDashEnergy(true)
	, haveShotEnergy(true)
	, isStart(false)
	, isDead(false)
	, isGoal(false)
	, haveShowedRanking(false)
	, haveShowedDeadUI(false)
	, isFirstShoting(true)
	, isDamageOver(false)
	, GoalTime(0.f)
	, IsAddScoreOver(true)
	, HP(100.f)
	, CoinCount(0)
	, CountShootEnemy(0)
	, ShotEnergy(100.f)
	, ShotMaxEnergy(100.f)
	, GuardEnergy(100.f)
	, GuardEnergyMax(100.f)
	, DashEnergy(100.f)
	, DashEnergyMax(100.f)
	, AddDashEnergy(10.f)
	, DashEffectLocationOffset(400.f, 0.f, 0.f)
	, DashEffectRotationOffset(0.f, 0.f, 0.f)
	, guardUIDownValue(10.f)
	, Shot_UIDownSpeed(0.5f)
	, Shot_UIUpSpeed(0.5f)
	, Guard_UIDownSpeed(0.5f)
	, Guard_UIUpSpeed(0.5f)
	, Dash_UIDownSpeed(0.5f)
	, Dash_UIUpSpeed(0.5f)
	, DamageFrame(50.f)
	, CoinScore(1000.f)
	, PlayerScore(0.f)
	, ScoreAnimStart(false)
	, Damage(10.f)
	, selectPlay(0)
	, tempRoll(0.f)
	, tempPitch(0.f)
	, tempYaw(0.f)
	, fps(0.f)
{
	// ���t���[���A���̃N���X��Tick()���ĂԂ��ǂ��������߂�t���O�B�K�v�ɉ����āA�p�t�H�[�}���X����̂��߂ɐ؂邱�Ƃ��ł���B
	PrimaryActorTick.bCanEverTick = true;

	// ��]�ʂ̕ۑ��p�z��̏�����
	prevRotator.Reset();

	//	�f�t�H���g�v���C���[�Ƃ��Đݒ�
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// �Q�[���X�^�[�g���A�܂��͐������ɌĂ΂�鏈��
void APlayerChara::BeginPlay()
{
	Super::BeginPlay();

	//	�f�[�^�ۑ�
	tempDamageFrame = DamageFrame;
	tempSpeed = playerMaxSpeed;

	tempJumpPower = jumpPower;
	
	playerSpeed = 0.f;

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerChara::OnBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &APlayerChara::OverlapEnds);;

	if (withSensor)
	{
		UCharacterMovementComponent* pCharMoveComp = GetCharacterMovement();
		if (pCharMoveComp != NULL)
		{
			//	�W�����v���ɂ����������ւ̈ړ��������悤�Ɂi0�`1�̊Ԃɐݒ肷�邱�Ƃňړ������𒲐��j
			pCharMoveComp->AirControl = 0.8f;
		}

		USensorManager::ConnectToSensor();

		//	PlayerChara�������Ă��郁�b�V���R���|�[�l���g�̑��Έʒu��ύX
		USkeletalMeshComponent* pMeshComp = GetMesh();
		if (pMeshComp != NULL)
		{
			//	Z���W��������
			pMeshComp->SetRelativeLocation(FVector(0.f, 0.f, -85.f));
		}

		// VR's
		FName DeviceName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName();

		if (DeviceName == "SteamVR" || DeviceName == "OculusHMD")
		{

			UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);

		} // end if()
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't find VR Origin(HMD)"));
		} // end else
	}
}

// ���t���[���̍X�V����
void APlayerChara::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//	fps���l��
	fps = 1 / DeltaTime;

	if (withSensor)
	{
		//	�Z���T�[�̍X�V����
		if (!isDead && !isGoal && isStart)
		{
			UpdateSensor(DeltaTime);

			bool left;
			bool right;
			USensorManager::GetSensorButton(left, right);

			if (right == true || left == true)
			{
				if (isButtonRelerse == true)
				{
					ShotStart(1.0f);
				}
				isButtonRelerse = false;
			}
			else
			{
				isButtonRelerse = true;
			}
		}		
	}

	if (!isDead && !isGoal && isStart)
	{
		GoalTime += DeltaTime;

		//	�ړ�����
		UpdateMove(DeltaTime);

		//	�W�����v����
		UpdateJump(DeltaTime);

		//	�K�[�h����
		UpdateGuard();

		//	�_�b�V������
		UpdateAccelerate();

		//	�ˌ�����
		Shooting(DeltaTime);

		//	�W�����v���璅�n��SE����
		GetPlayerPosZ(DeltaTime);

		//	�_���[�W���󂯂邩�ǂ����̏���
		if (!canBeDamaged)
		{
			DamageFrame -= (DeltaTime * 60);

			if (DamageFrame <= 0.f)
			{
				canBeDamaged = true;
				Player_Damage_Widget->RemoveFromViewport();
				DamageFrame = tempDamageFrame;
				isDamageOver = true;
			}
		}
	}

	//DeadCount();

	//PlayEffects();
}

//	�ړ�����
void APlayerChara::UpdateMove(float _deltaTime)
{
	FVector NewLocation = GetActorLocation();
	FVector YRotation = GetActorForwardVector();

	//	fps�ɂ���Ĉړ��ʔ{���̌v�Z
	if (fps < 59.5f || fps > 60.5f)
	{
		fps = 1.f / (fps / 60.f);
	}
	else
	{
		fps = 60.f / fps;
	}

	//	�X�^�[�g���炾�񂾂�ő呬�x�ɒH��
	if (playerSpeed < playerMaxSpeed)
	{
		playerSpeed += 0.016f * playerMaxSpeed * fps;
	}

	//	�O�Ɍ��������ƈړ�����
	if ((isDashing || isDashLine) && haveShotEnergy && !isJumping)
	{
		NewLocation.X += playerSpeed * 1.5f * fps;
	}
	else if (!isGuarding && !isDashing && haveShotEnergy || isJumping)
	{
		NewLocation.X += playerSpeed * fps;
	}
	else if (isGuarding && haveShotEnergy)
	{
		NewLocation.X += playerSpeed * 0.8f * fps;
	}
	else if(!haveShotEnergy)
	{
		NewLocation.X += playerSpeed * 0.5f * fps;
	}


	//	�L�����N�^�[��Y���ړ�
	{
		if (withSensor == false)
		{
			NewLocation.Y += 0.8f * -tempRoll * fps;
		}
		else
		{
			NewLocation.Y += 5.0f * tempRoll * fps;
		}
		SetActorLocation(NewLocation);
	}

	//	�_���[�W���󂯂��瑬�x����������
	if (isDamageOver)
	{
		if (playerSpeed >= tempSpeed)
		{
			isDamageOver = false;
			playerSpeed = tempSpeed;
		}
		else
		{
			playerSpeed += playerMaxSpeed * 0.02f * fps;
		}
	}
}

//	�W�����v����
void APlayerChara::UpdateJump(float _deltaTime)
{
	if (withSensor == false)
	{
		if (tempPitch > 30.f && canJump && !isGuarding)
		{
			canJump = false;
			isJumping = true;
			posBeforeJump = GetActorLocation();
		}
	}
	else
	{
		if (tempPitch > 10.f && canJump && !isGuarding)
		{
			canJump = false;
			isJumping = true;
			posBeforeJump = GetActorLocation();
		}
	}
	//	�W�����v�ł��邩�̔��f

	if (isJumping)
	{
		//	�W�����v�ʂ��v�Z
		nowJumpHeight = JUMP_HEIGHT;		

		FVector nowPos = GetActorLocation();

		jumpTime += _deltaTime;

		isLanding = false;

		//	�n�ʂɗ�������W�����v��~
		if (nowJumpHeight < 0.0f)
		{
			jumpPower = tempJumpPower;
			jumpTime = 0.f;
			isJumping = false;
			isLanding = true;
			//SetActorLocation(FVector(nowPos.X, nowPos.Y, posBeforeJump.Z));
		}
		else
		{
			SetActorLocation(FVector(nowPos.X, nowPos.Y, posBeforeJump.Z + nowJumpHeight), true);
		}
	}
}

//	�K�[�h����
void APlayerChara::UpdateGuard()
{
	//	�K�[�h�ł��邩�̔��f
	if (GuardEnergy <= 0.f)
	{
		isGuarding = false;
		haveGuardEnergy = false;
		tempRotate = 0.f;
	}

	FRotator nowRot = GetActorRotation();
	if ((tempYaw < -30.f || tempYaw > 30.f) && haveGuardEnergy && !isDead && !isGoal)
	{
		isGuarding = true;
	}
	else
	{
		isGuarding = false;
	}

	if (isGuarding && haveGuardEnergy)
	{
		GuardEnergy -= Guard_UIDownSpeed * fps;
	}
	else
	{ 
		if (GuardEnergy < GuardEnergyMax)
		{
			GuardEnergy += Guard_UIUpSpeed * fps;
		}
		else
		{
			haveGuardEnergy = true;
		}
	}
}

void APlayerChara::UpdateAccelerate()
{
	FRotator nowRot = GetActorRotation();

	if (withSensor == false)
	{
		if (tempPitch < -30.f && haveDashEnergy && !isDead && !isGoal)
		{
			isDashing = true;
		}
	}
	else
	{
		if (tempPitch < -10.f && haveDashEnergy && !isDead && !isGoal)
		{
			isDashing = true;
		}
	}

	if (DashEnergy <= 0.5f)
	{
		isDashing = false;
		haveDashEnergy = false;
	}

	if (isDashing && haveDashEnergy)
	{
		DashEnergy -= Dash_UIDownSpeed * fps;
	}
	else if (DashEnergy >= DashEnergyMax)
	{
		haveDashEnergy = true;
	}
}

//���ˊJ�n
void APlayerChara::Shooting(float DeltaTime)
{
	if (ShotEnergy <= 0.f)
	{
		isShoting = false;
		haveShotEnergy = false;
	}

	if (isShoting && haveShotEnergy)
	{
		ShotEnergy -= Shot_UIDownSpeed * fps;
	}
	else
	{
		if (ShotEnergy <= ShotMaxEnergy)
		{
			ShotEnergy += Shot_UIUpSpeed * fps;
		}
		else
		{
			haveShotEnergy = true;
		}
	}

	if (playerATKType == PPlayerAttackType::Straight && isShoting) {
		bulletTimeCount += DeltaTime;

		FVector currentVector = GetActorLocation();
		if (bulletTimeCount >= bulletDuration && !isJumping && !isGuarding) {
			// �e�̍쐬�FSpawnActor<��������N���X�^>(��������N���X�A�n�_���W�A�n�_��]���W)
			GetWorld()->SpawnActor<APlayerBullet>(bulletActor, currentVector + this->GetActorForwardVector() * bulletXOffset + this->GetActorUpVector() * bulletYOffset, FRotator().ZeroRotator);
			bulletTimeCount = 0.0f;
			//UE_LOG(LogTemp, Warning, TEXT("Enemy( %s ) is attacking. Using bullet type: %s"), *(this->GetName()), *(bulletActor->GetName()));
		}
	}
}

//void APlayerChara::PlayEffects()
//{
//	if (isDashing || isDashLine) {
//		if (DashEffect == nullptr) {
//			return;
//		}
//	}
//}

//	�W�����v�䂩�痎�Ƃ�����SE���o�����߂̊֐�
void APlayerChara::GetPlayerPosZ(float DeltaTime)
{
	if (startPosZ == 0.f)
	{
		startPosZ = GetActorLocation().Z;
	}

	nowPosZ = GetActorLocation().Z;

	if (nowPosZ - startPosZ > 50.f)
	{
		overStartHight = true;
		isLanding = false;
	}

	if (overStartHight && (nowPosZ - startPosZ < 0.1f))
	{
		isLanding = true;
		overStartHight = false;
	}
}

void APlayerChara::GetCoin()
{
	ScoreAnimStart = true;

	PlayerScore += CoinScore;

	CoinCount += 1;

	if (DashEnergy < DashEnergyMax)
	{
		DashEnergy += AddDashEnergy;
	}	
}

//void APlayerChara::DeadCount()
//{
//	if (HP <= 0 || isDead)
//	{
//		if (Player_Select_Widget_Class != nullptr && !hadDoOnce)
//		{
//			isDead = true;
//
//			Player_Select_Widget = CreateWidget(GetWorld(), Player_Select_Widget_Class);
//			Player_Select_Widget->AddToViewport();
//
//			hadDoOnce = true;
//		}
//	}
//}

void APlayerChara::OnBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("JumpPad"))
	{
		jumpPower = tempJumpPower;

		canJump = true;
	}

	if (OtherActor->ActorHasTag("Fence_Film") && canBeDamaged && !isDashing && !isDashLine)
	{
		if (!isGuarding)
		{
			if (Player_Damage_Widget_Class != nullptr)
			{
				Player_Damage_Widget = CreateWidget(GetWorld(), Player_Damage_Widget_Class);
				Player_Damage_Widget->AddToViewport();
			}

			playerSpeed *= 0.5f;
			canBeDamaged = false;
			HP -= Damage;
		}
		else
		{
			GuardEnergy -= guardUIDownValue;
		}
	}

	if ((OtherActor->ActorHasTag("EnemyBullet") || OtherActor->ActorHasTag("EnemyMissile") || OtherActor->ActorHasTag("ShotEnemy") || OtherActor->ActorHasTag("EnergyEnemy")) && canBeDamaged && !isDashing && !isDashLine)
	{
		if (!isGuarding)
		{
			if (Player_Damage_Widget_Class != nullptr)
			{
				Player_Damage_Widget = CreateWidget(GetWorld(), Player_Damage_Widget_Class);
				Player_Damage_Widget->AddToViewport();
			}
			playerSpeed *= 0.5f;
			canBeDamaged = false;
			HP -= Damage;
		}
		else
		{
			GuardEnergy -= guardUIDownValue;
		}
	}

	if ((OtherActor->ActorHasTag("GeneralEnemy") || OtherActor->ActorHasTag("DashEnemy") || OtherActor->ActorHasTag("ShotEnemy") || OtherActor->ActorHasTag("EnergyEnemy")) && canBeDamaged && !isDashing && !isDashLine)
	{
			if (Player_Damage_Widget_Class != nullptr)
			{
				Player_Damage_Widget = CreateWidget(GetWorld(), Player_Damage_Widget_Class);
				Player_Damage_Widget->AddToViewport();
			}
			playerSpeed *= 0.5f;
			canBeDamaged = false;
			HP -= Damage;
	}

	if (OtherActor->ActorHasTag("DashLine"))
	{
		isDashLine = true;
	}

	if (OtherActor->ActorHasTag("SuperJump"))
	{
		jumpPower = superJumpPower;

		canJump = true;
	}

	if (OtherActor->ActorHasTag("Goal"))
	{
		isGoal = true;
	}
}

void APlayerChara::OverlapEnds(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag("JumpPad"))
	{
		canJump = false;
	}

	if (OtherActor->ActorHasTag("SuperJump"))
	{
		canJump = false;
	}
}

//	�ŐV��
void APlayerChara::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	USensorManager::DisconnectToSensor();
}

//	�ŐV��
void APlayerChara::UpdateSensor(float _deltaTime)
{
	FRotator tempRot = USensorManager::GetSensorDataRotator();

	if (tempRot == SENSOR_ERROR_ROTATOR)
	{
		return;
	}
	tempRoll = tempRot.Roll;
	tempPitch = tempRot.Pitch;
	tempYaw = tempRot.Yaw;

	//	guardEnergy��0�ɂȂ�����A����]�̊p�x��������0�ɖ߂�
	if (!haveGuardEnergy)
	{
		if (tempRotate >= 85.f || tempRotate <= -85.f)
		{
			tempYaw = 0.f;
		}
		else if (tempRotate < 85.f && tempYaw < 0.f)
		{
			tempRotate += 2.f * fps;
			tempYaw += tempRotate;
		}
		else if (tempRotate > -85.f && tempYaw > 0.f)
		{
			tempRotate -= 2.f * fps;
			tempYaw += tempRotate;
		}
	}

	// Actor�ɉ�]�ʂ𔽉f
	if (isGuarding)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(tempRoll)); = tempPitch;
		tempPitch = 0.f;
	}
	else if (isDashing)
	{
		tempYaw = 0.f;
	}

	FRotator rot(tempPitch, tempYaw, tempRoll);


	SetActorRotation(rot);
}

// �e���͊֌W���\�b�h�Ƃ̃o�C���h����
void APlayerChara::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//	�ړ�
	InputComponent->BindAxis("MoveRight", this, &APlayerChara::MoveRightWithNoSensor);

	//	�K�[�h
	InputComponent->BindAxis("Guard", this, &APlayerChara::GuardStartWithNoSensor);

	//	�_�b�V��
	InputComponent->BindAxis("DashAndJump", this, &APlayerChara::DashOrJumpStartWithNoSensor);

	InputComponent->BindAxis("Shot", this, &APlayerChara::ShotStart);
}

//	�y���̓o�C���h�z�L�����ړ�:���E
void APlayerChara::MoveRightWithNoSensor(float _axisValue)
{
	if (withSensor == false)
	{
		tempRoll = FMath::Clamp(_axisValue, -1.0f, 1.0f) * 180.f;
	}
	//---------------------------------------------------------------------
}

//	�y���̓o�C���h�z�K�[�h�̐���
void APlayerChara::GuardStartWithNoSensor(float _axisValue)
{
	if (withSensor == false)
	{
		tempYaw = FMath::Clamp(_axisValue, -1.0f, 1.0f) * 45.f;
	}
	//---------------------------------------------------------------------
}

//	�y���̓o�C���h�z�L�����ړ�:�O��
void APlayerChara::DashOrJumpStartWithNoSensor(float _axisValue)
{
	if (withSensor == false)
	{
		tempPitch = FMath::Clamp(_axisValue, -1.0f, 1.0f) * 45.f;
	}
	//---------------------------------------------------------------------
}

//	�v���C���[�̒e����
void APlayerChara::ShotStart(float _axisValue)
{
	if (_axisValue == 1 && haveShotEnergy && isStart && !isGoal && !isDead)
	{
		if (isFirstShoting)
		{
			FVector currentVector = GetActorLocation();
			GetWorld()->SpawnActor<APlayerBullet>(bulletActor, currentVector + this->GetActorForwardVector() * bulletXOffset + this->GetActorUpVector() * bulletYOffset, FRotator().ZeroRotator);
			ShotEnergy -= 10.f;
			isFirstShoting = false;
		}
		isShoting = true;
	}
	else
	{
		isFirstShoting = true;
		isShoting = false;
	}
}