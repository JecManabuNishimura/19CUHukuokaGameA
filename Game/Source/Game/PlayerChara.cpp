//----------------------------------------------------------
// �t�@�C����		�FPlayerChara.cpp
// �T�v				�F�v���C���[�̐���
// �쐬��			�F19CU0220 ����
// �X�V���e			�F2020/10/02 �쐬�@�v���C���[�̊e����
//					�F2020/11/12 �X�V�@�n粗����@�Z���T�[�������I�Ɍ��o����悤�ɂȂ�
//					�F2020/11/16 �ύX�@�߉Ɠ��@bulletActor��APlayerBullet�^�Ɍp������
//----------------------------------------------------------

// �C���N���[�h
#include "PlayerChara.h"
#include "PlayerBullet.h"
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
	, isOpen(false)
	, startPosZ(0.f)
	, nowPosZ(0.f)
	, countPosZTime(0.f)
	, overStartHight(false)
	, hadDoOnce(false)
	, tempSpeed(0.f)
	, bulletTimeCount(0.0f)
	, bulletDuration(1.0f)
	, bulletXOffset(10.0f)
	, playerSpeed(10.f)
	, DashSpeed(1.5f)
	, gravity(700.f)
	, jumpPower(1200.f)
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
	, isDamageOver(false)
	, GoalTime(0.f)
	, HP(100.f)
	, CoinCount(0)
	, CountShootEnemy(0)
	, ShotEnergy(100.f)
	, ShotMaxEnergy(100.f)
	, GuardEnergy(100.f)
	, GuardEnergyMax(100.f)
	, DashEnergy(100.f)
	, DashEnergyMax(100.f)
	, DashEffectLocationOffset(400.f, 0.f, 0.f)
	, DashEffectRotationOffset(0.f, 0.f, 0.f)
	, guardBulletUIDownSpeed(10.f)
	, Shot_UIDownSpeed(0.5f)
	, Shot_UIUpSpeed(0.5f)
	, Guard_UIDownSpeed(0.5f)
	, Guard_UIUpSpeed(0.5f)
	, Dash_UIDownSpeed(0.5f)
	, Dash_UIUpSpeed(0.5f)
	, DamageFrame(50.f)
	, CoinScore(1000.f)
	, EnemyScore(2000.f)
	, PlayerScore(0.f)
	, nowPage(1)
	, maxPage(0)
	, Damage(10.f)
	, selectPlay(0)
	, tempRoll(0.f)
	, tempPitch(0.f)
	, tempYaw(0.f)
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

	tempDamageFrame = DamageFrame;
	tempSpeed = playerSpeed;

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerChara::OnBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &APlayerChara::OverlapEnds);;

	//	PlayerChara�������Ă��郁�b�V���R���|�[�l���g�̑��Έʒu��ύX
	USkeletalMeshComponent* pMeshComp = GetMesh();
	if (pMeshComp != NULL)
	{
		//	Z���W��������
		pMeshComp->SetRelativeLocation(FVector(0.f, 0.f, -85.f));
	}

	UCharacterMovementComponent* pCharMoveComp = GetCharacterMovement();
	if (pCharMoveComp != NULL)
	{
		//	�W�����v���ɂ����������ւ̈ړ��������悤�Ɂi0�`1�̊Ԃɐݒ肷�邱�Ƃňړ������𒲐��j
		pCharMoveComp->AirControl = 0.8f;
	}


	// 2020/11/11 �n� �������o�ɕύX-----------------------------------------begin--
	for (int i = 0; i < 20 && isOpen == false; ++i)
	{
		// �V���A���|�[�g���J����
		//m_pArduinoSerial = USerial::OpenComPort(isOpen, serialPort, 115200);
		m_pArduinoSerial = USerial::OpenComPort(isOpen, i, 115200);

		if (isOpen == false)
		{
			UE_LOG(LogTemp, Error, TEXT("ASensorTest::BeginPlay(): COM Port:%d is failed open. Please check the connection and COM Port number."), i);
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("ASensorTest::BeginPlay(): COM Port:%d is Successfully Open."), i);
			withSensor = true;
			serialPort = i;
		}
	}
	//---------------------------------------------------------------------------end---

	// 10�񕪂̃f�[�^������
	int errorCount = 0;
	for (int i = 0; i < ROTATOR_ARRAY_SIZE; ++i)
	{
		FRotator rotTemp;
		rotTemp = SensorToRotator();

		// �Z���T�[�̒l���ǂݎ��Ă��Ȃ���΂�蒼��
		if (rotTemp == FRotator::ZeroRotator)
		{
			UE_LOG(LogTemp, Warning, TEXT("ASensorTest::BeginPlay(): Failed Read."));
			++errorCount;
			if (errorCount >= 10)
			{
				UE_LOG(LogTemp, Error, TEXT("ASensorTest::BeginPlay(): Failed to read the sensor more than 10 times. Please check the connection."));
				break;
			}
		}
		else
		{
			UE_LOG(LogTemp, Verbose, TEXT("ASensorTest::BeginPlay(): SuccessFully Read."));
		}

		prevRotator.Add(rotTemp);
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

// ���t���[���̍X�V����
void APlayerChara::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(DeltaTime * 60.f));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, isGuarding ? TEXT("true") : TEXT("false"));

	if (withSensor)
	{
		//	�Z���T�[�̍X�V����
		UpdateSensor(DeltaTime);
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

		UpdateAccelerate();

		Shooting(DeltaTime);

		GetPlayerPosZ(DeltaTime);

		if (!canBeDamaged)
		{
			DamageFrame -= (DeltaTime * 60);

			if (DamageFrame <= 0.f)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(DamageFrame));
				canBeDamaged = true;
				Player_Damage_Widget->RemoveFromViewport();
				DamageFrame = tempDamageFrame;
				isDamageOver = true;
			}
		}
	}

	if (selectPlay == 1)
	{
		RestartGame();
	}

	DeadCount();

	PlayEffects();
}

//	�ړ�����
void APlayerChara::UpdateMove(float _deltaTime)
{
	FVector NewLocation = GetActorLocation();
	FVector YRotation = GetActorForwardVector();

	//	�O�Ɍ��������ƈړ�����
	if ((isDashing || isDashLine) && haveShotEnergy)
	{
		NewLocation.X += playerSpeed * 1.5f;
	}
	else if (!isGuarding && !isDashing && haveShotEnergy)
	{
		NewLocation.X += playerSpeed;
	}
	else if (isGuarding && haveShotEnergy)
	{
		NewLocation.X += playerSpeed * 0.8f;
	}
	else if(!haveShotEnergy)
	{
		NewLocation.X += playerSpeed * 0.5f;
	}


	//	�L�����N�^�[��Y���ړ�
	{
		YRotation.Y = 0.f;
		NewLocation.Y += 0.2f * tempRoll;
		SetActorLocation(NewLocation);
	}

	if (isDamageOver)
	{
		if (playerSpeed >= tempSpeed)
		{
			isDamageOver = false;
			playerSpeed = tempSpeed;
		}
		else
		{
			playerSpeed += tempSpeed * 0.02f;
		}
	}
}

//	�W�����v����
void APlayerChara::UpdateJump(float _deltaTime)
{
	if (tempPitch > 30.f && !isJumping && canJump && !isGuarding)
	{
		isJumping = true;
		posBeforeJump = GetActorLocation();
	}

	if (isJumping)
	{
		//	�W�����v�ʂ��v�Z
		nowJumpHeight = JUMP_HEIGHT;

		FVector nowPos = GetActorLocation();

		jumpTime += _deltaTime;

		isLanding = false;

		if (nowJumpHeight < 0.0f)
		{
			jumpTime = 0.f;
			isJumping = false;
			isLanding = true;
			SetActorLocation(FVector(nowPos.X, nowPos.Y, posBeforeJump.Z));
		}
		else
		{
			SetActorLocation(FVector(nowPos.X, nowPos.Y, posBeforeJump.Z + nowJumpHeight), true);
		}

		prevJumpHeight = nowJumpHeight;
	}
}

//	�K�[�h����
void APlayerChara::UpdateGuard()
{
	if (GuardEnergy <= 0.f)
	{
		isGuarding = false;
		haveGuardEnergy = false;
		tempRotate = 0.f;
	}

	FRotator nowRot = GetActorRotation();
	if ((tempYaw < -30.f || tempYaw > 30.f) && haveGuardEnergy)
	{
		isGuarding = true;
	}
	else
	{
		isGuarding = false;
	}

	if (isGuarding && haveGuardEnergy)
	{
		GuardEnergy -= Guard_UIDownSpeed;
	}
	else
	{
		if (GuardEnergy <= GuardEnergyMax)
		{
			GuardEnergy += Guard_UIUpSpeed;
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
	if (tempPitch < -30.f && haveDashEnergy)
	{
		isDashing = true;
	}

	if (DashEnergy <= 0.f)
	{
		isDashing = false;
		haveDashEnergy = false;
	}

	if (isDashing && haveDashEnergy)
	{
		DashEnergy -= Dash_UIDownSpeed;
	}
	else
	{
		if (DashEnergy <= DashEnergyMax)
		{
			DashEnergy += Dash_UIUpSpeed;
		}
		else
		{
			haveDashEnergy = true;
		}
	}
}

void APlayerChara::RestartGame()
{
	FVector restartLocation = GetActorLocation();

	SetActorLocation(FVector(restartLocation.X - 3000.f, -10.f, 30.f));

	HP = 100.f;

	isDead = false;

	Player_Select_Widget->RemoveFromViewport();

	selectPlay = 0;

	hadDoOnce = false;
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
		ShotEnergy -= Shot_UIDownSpeed;
	}
	else
	{
		if (ShotEnergy <= ShotMaxEnergy)
		{
			ShotEnergy += Shot_UIUpSpeed;
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
			GetWorld()->SpawnActor<APlayerBullet>(bulletActor, currentVector + this->GetActorForwardVector() * bulletXOffset, FRotator().ZeroRotator);
			bulletTimeCount = 0.0f;
			//UE_LOG(LogTemp, Warning, TEXT("Enemy( %s ) is attacking. Using bullet type: %s"), *(this->GetName()), *(bulletActor->GetName()));
		}
	}
}

void APlayerChara::DeadCount()
{
	if (HP <= 0 || isDead)
	{		
			if (Player_Select_Widget_Class != nullptr && !hadDoOnce)
			{
				isDead = true;

				Player_Select_Widget = CreateWidget(GetWorld(), Player_Select_Widget_Class);
				Player_Select_Widget->AddToViewport();

				hadDoOnce = true;
			}

			//GetMesh()->SetSimulatePhysics(true);
	}
}

void APlayerChara::PlayEffects()
{
	if (isDashing || isDashLine) {
		if (DashEffect == nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("DashEffect is not assetted."));
			return;
		}
		//DashEffectRotationOffset = FRotator(0.0f, 0.0f, DashEffectRotationOffset.Roll - 500.0f);
		//UNiagaraFunctionLibrary::SpawnSystemAttached(
		//	DashEffect, RootComponent, NAME_None, DashEffectLocationOffset, DashEffectRotationOffset, EAttachLocation::KeepRelativeOffset, true);
		
		//UE_LOG(LogTemp, Warning, TEXT("%s"), );
	}

}

void APlayerChara::GetPlayerPosZ(float DeltaTime)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(startPosZ));
	if (startPosZ == 0.f)
	{
		startPosZ = GetActorLocation().Z;
	}
	countPosZTime += DeltaTime;
	nowPosZ = GetActorLocation().Z;


	if (nowPosZ - startPosZ > 50.f)
	{
		overStartHight = true;
		isLanding = false;
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(nowPosZ));
	//GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Red, nowPosZ == startPosZ ? TEXT("true") : TEXT("false"));

	if (overStartHight && (nowPosZ - startPosZ < 0.1f))
	{
		isLanding = true;
		overStartHight = false;
	}
}



void APlayerChara::OnBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("JumpPad"))
	{
		canJump = true;
	}

	if (OtherActor->ActorHasTag("Fence_Film") && canBeDamaged && !isDashing && !isGuarding)
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
			GuardEnergy -= guardBulletUIDownSpeed;
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

	if (OtherActor->ActorHasTag("Coin"))
	{
		PlayerScore += CoinScore;

		CoinCount += 1;
	}

	if (OtherActor->ActorHasTag("DashLine"))
	{
		isDashLine = true;
	}

	if (OtherActor->ActorHasTag("Goal"))
	{
		if (Player_Goal_Widget_Class != nullptr)
		{
			Player_Goal_Widget = CreateWidget(GetWorld(), Player_Goal_Widget_Class);
			Player_Goal_Widget->AddToViewport();
		}

		isGoal = true;
	}
}

void APlayerChara::OverlapEnds(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag("JumpPad"))
	{
		canJump = false;
	}

	if (OtherActor->ActorHasTag("DashLine"))
	{
		isDashLine = false;
	}
}

void APlayerChara::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (m_pArduinoSerial != NULL)
	{
		m_pArduinoSerial->Close();
		m_pArduinoSerial = NULL;
	}

	// ��]�ʂ̕ۑ��p�z��̏�����
	prevRotator.Reset();
}

//	�Z���T�[�̍X�V����
void APlayerChara::UpdateSensor(float _deltaTime)
{
	tempRoll = 0.f;
	tempPitch = 0.f;
	tempYaw = 0.f;

	// ���Z
	for (int i = 0; i < prevRotator.Num(); ++i)
	{
		tempRoll += prevRotator[i].Roll;
		tempPitch += prevRotator[i].Pitch;
		tempYaw += prevRotator[i].Yaw;
	}

	// ���ϒl���Z�o
	tempRoll /= prevRotator.Num();
	tempPitch /= prevRotator.Num();
	tempYaw /= prevRotator.Num();

	//	guardEnergy��0�ɂȂ�����A����]�̊p�x��������0�ɖ߂�
	if (!haveGuardEnergy)
	{
		if (tempRotate >= 85.f || tempRotate <= -85.f)
		{
			tempYaw = 0.f;
		}
		else if (tempRotate < 85.f && tempYaw < 0.f)
		{
			tempRotate += 2.f;
			tempYaw += tempRotate;
		}
		else if (tempRotate > -85.f && tempYaw > 0.f)
		{
			tempRotate -= 2.f;
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

	float angle = 5.f;
	if (FMath::Abs((rot - prevDiffRot).Roll) < angle && FMath::Abs((rot - prevDiffRot).Pitch) < angle && FMath::Abs((rot - prevDiffRot).Yaw) < angle)
	{
		rot = prevDiffRot;
	}

	SetActorRotation(rot);

	// ���X�g���X�V
	if (prevRotator.IsValidIndex(0) == true)
	{
		// �C���f�b�N�X�ԍ�0�̗v�f���폜
		prevRotator.RemoveAt(0);

		FRotator rotTemp = SensorToRotator();

		// �Z���T�[����̒l�����S��0������
		if (rotTemp == FRotator::ZeroRotator)
		{
			// ���݂̕��ϒl����
			prevRotator.Add(rot);
		}
		else
		{
			// �Z���T�[����̒l����
			prevRotator.Add(rotTemp);
		}
	}

	prevDiffRot = rot;
}


FRotator APlayerChara::SensorToRotator()
{
	bool isRead = false;		// �f�[�^��ǂݎ�ꂽ���H
	FString fStr;				// �ǂݎ��f�[�^�i�[�p
	int tryCnt = 0;				// �ǂݎ�낤�Ƃ�����
	const int tryCntMax = 500;	// �ő�̓ǂݎ���

	// �V���A���̃I�u�W�F�N�g�������
	if (m_pArduinoSerial != NULL)
	{
		// �f�[�^�̓ǂݎ��
		// �f�[�^���ǂݎ��邩�A�ő�ǂݎ��񐔂ɂȂ�܂ŌJ��Ԃ�
		do
		{
			m_pArduinoSerial->Println(FString(TEXT("s")));

			fStr = m_pArduinoSerial->Readln(isRead);
			++tryCnt;
		} while (isRead == false && tryCnt < tryCntMax);

		TArray<FString> splitTextArray;
		splitTextArray.Reset();

		UE_LOG(LogTemp, VeryVerbose, TEXT("ASensorTest::SensorToRotator(): Try Read Count: %d / %d"), tryCnt, tryCntMax);

		// �ǂݎ��Ȃ�������ZeroRotator��Ԃ��ďI��
		if (isRead == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("ASensorTest::SensorToRotator(): No Data From Sensor. return ZeroRotator."));
			withSensor = false;
			return FRotator::ZeroRotator;
		}
		else
		{
			UE_LOG(LogTemp, Verbose, TEXT("ASensorTest::SensorToRotator(): Get Data From Sensor."));
		}

		// �Z���T�[�f�[�^���J���}��؂��splitTextArray�ɓ����
		fStr.ParseIntoArray(splitTextArray, TEXT(","));

		// ���ꂼ���int�^�ɕϊ�����
		TArray<float> rotatorAxis;
		rotatorAxis.Reset();

		for (int i = 0; i < splitTextArray.Num(); ++i)
		{
			rotatorAxis.Add(FCString::Atof(*splitTextArray[i]));
		}

		// Roll(X), Pitch(Y), Yaw(Z)�̗v�f�i3���j�ǂݎ��Ă��Ȃ����ZeroRotator��Ԃ�
		if (rotatorAxis.IsValidIndex(2) == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("ASensorTest::SensorToRotator(): Failed Add TArray<float> elements. return ZeroRotator."));
			withSensor = false;
			return FRotator::ZeroRotator;
		}

		UE_LOG(LogTemp, Verbose, TEXT("ASensorTest::SensorToRotator(): Rotator Roll:%f Pitch:%f Yaw:%f"), rotatorAxis[0], rotatorAxis[1], rotatorAxis[2]);

		// FRotator�^�̕ϐ���float�^���g�p���ď�����
		FRotator rot(rotatorAxis[1], rotatorAxis[2], -rotatorAxis[0]);

		return rot;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ASensorTest::SensorToRotator(): ASensorTest::m_pArduinoSerial is NULL."));
		withSensor = false;
		return FRotator::ZeroRotator;
	}
}

//	====================================
//	�Z���T�[�������ĂȂ��֐�


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
}

//	�y���̓o�C���h�z�L�����ړ�:���E
void APlayerChara::MoveRightWithNoSensor(float _axisValue)
{
	// 2020/11/11 �n� �Z���T�[�������o�ɂ��ύX--------------------------
	if (withSensor == false)
	{
		tempRoll = FMath::Clamp(_axisValue, -1.0f, 1.0f) * 180.f;
	}
	//---------------------------------------------------------------------
}

//	�y���̓o�C���h�z�K�[�h�̐���
void APlayerChara::GuardStartWithNoSensor(float _axisValue)
{
	// 2020/11/11 �n� �Z���T�[�������o�ɂ��ύX--------------------------
	if (withSensor == false)
	{
		tempYaw = FMath::Clamp(_axisValue, -1.0f, 1.0f) * 45.f;
	}
	//---------------------------------------------------------------------
}

//	�y���̓o�C���h�z�L�����ړ�:�O��
void APlayerChara::DashOrJumpStartWithNoSensor(float _axisValue)
{
	// 2020/11/11 �n� �Z���T�[�������o�ɂ��ύX--------------------------
	if (withSensor == false)
	{
		tempPitch = FMath::Clamp(_axisValue, -1.0f, 1.0f) * 45.f;
	}
	//---------------------------------------------------------------------
}