//----------------------------------------------------------
// ƒtƒ@ƒCƒ‹–¼		FPlayerChara.cpp
// ŠT—v				FƒvƒŒƒCƒ„[‚Ì§Œä
// ì¬Ò			F19CU0220 ‘‚”ò
// XV“à—e			F2020/10/02 ì¬@ƒvƒŒƒCƒ„[‚ÌŠe‘€ì
//					F2020/11/12 XV@“nç²—´‰¹@ƒZƒ“ƒT[‚ğ©“®“I‚ÉŒŸo‚·‚é‚æ‚¤‚É‚È‚é
//					F2020/11/16 •ÏX@ß‰Æ“¯@bulletActor‚ğAPlayerBulletŒ^‚ÉŒp³‚·‚é
//----------------------------------------------------------

// ƒCƒ“ƒNƒ‹[ƒh
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

//	defineƒ}ƒNƒ
#define ROTATOR_ARRAY_SIZE 1
#define JUMP_HEIGHT (jumpTime * jumpTime * (-gravity) / 2) + (jumpTime * jumpPower)

// ƒRƒ“ƒXƒgƒ‰ƒNƒ^
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
	, restartLocationX(0.f)
	, isStart(false)
	, isDead(false)
	, isGoal(false)
	, haveShowedRanking(false)
	, isFirstShoting(true)
	, isDamageOver(false)
	, GoalTime(0.f)
	, tempGoalTime(0.f)
	, deadCount(0)
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
	, tempPlayerScore(0.f)
	, nowPage(1)
	, maxPage(0)
	, Damage(10.f)
	, selectPlay(0)
	, tempRoll(0.f)
	, tempPitch(0.f)
	, tempYaw(0.f)
	, tempDataOfShot(0.f)
	, tempDataOfDash(0.f)
	, tempDataOfGuard(0.f)
{
	// –ˆƒtƒŒ[ƒ€A‚±‚ÌƒNƒ‰ƒX‚ÌTick()‚ğŒÄ‚Ô‚©‚Ç‚¤‚©‚ğŒˆ‚ß‚éƒtƒ‰ƒOB•K—v‚É‰‚¶‚ÄAƒpƒtƒH[ƒ}ƒ“ƒXŒüã‚Ì‚½‚ß‚ÉØ‚é‚±‚Æ‚à‚Å‚«‚éB
	PrimaryActorTick.bCanEverTick = true;

	// ‰ñ“]—Ê‚Ì•Û‘¶—p”z—ñ‚Ì‰Šú‰»
	prevRotator.Reset();

	//	ƒfƒtƒHƒ‹ƒgƒvƒŒƒCƒ„[‚Æ‚µ‚Äİ’è
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// ƒQ[ƒ€ƒXƒ^[ƒgA‚Ü‚½‚Í¶¬‚ÉŒÄ‚Î‚ê‚éˆ—
void APlayerChara::BeginPlay()
{
	Super::BeginPlay();

	tempDamageFrame = DamageFrame;
	tempSpeed = playerMaxSpeed;

	tempDataOfShot = ShotEnergy;
	tempDataOfDash = DashEnergy;
	tempDataOfGuard = GuardEnergy;

	tempJumpPower = jumpPower;

	restartLocationX = GetActorLocation().X;

	playerSpeed = 0.f;

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerChara::OnBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &APlayerChara::OverlapEnds);;

	if (withSensor)
	{
		UCharacterMovementComponent* pCharMoveComp = GetCharacterMovement();
		if (pCharMoveComp != NULL)
		{
			//	ƒWƒƒƒ“ƒv‚É‚à…•½•ûŒü‚Ö‚ÌˆÚ“®‚ª•·‚­‚æ‚¤‚Éi0`1‚ÌŠÔ‚Éİ’è‚·‚é‚±‚Æ‚ÅˆÚ“®‚·‚é‹ï‡‚ğ’²®j
			pCharMoveComp->AirControl = 0.8f;
		}


		// 2020/11/11 “nç² ©“®ŒŸo‚É•ÏX-----------------------------------------begin--
		for (int i = 0; i < 20 && isOpen == false; ++i)
		{
			// ƒVƒŠƒAƒ‹ƒ|[ƒg‚ğŠJ‚¯‚é
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

		// 10‰ñ•ª‚Ìƒf[ƒ^‚ğ“ü‚ê‚é
		int errorCount = 0;
		for (int i = 0; i < ROTATOR_ARRAY_SIZE; ++i)
		{
			FRotator rotTemp;
			rotTemp = SensorToRotator();

			// ƒZƒ“ƒT[‚Ì’l‚ª“Ç‚İæ‚ê‚Ä‚¢‚È‚¯‚ê‚Î‚â‚è’¼‚µ
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
	}

	//	PlayerChara‚ª‚Á‚Ä‚¢‚éƒƒbƒVƒ…ƒRƒ“ƒ|[ƒlƒ“ƒg‚Ì‘Š‘ÎˆÊ’u‚ğ•ÏX
	USkeletalMeshComponent* pMeshComp = GetMesh();
	if (pMeshComp != NULL)
	{
		//	ZÀ•W‚ğ‰º‚°‚é
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

// –ˆƒtƒŒ[ƒ€‚ÌXVˆ—
void APlayerChara::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(DeltaTime * 60.f));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, isGuarding ? TEXT("true") : TEXT("false"));

	if (withSensor)
	{
		//	ƒZƒ“ƒT[‚ÌXVˆ—
		UpdateSensor(DeltaTime);
	}

	if (!isDead && !isGoal && isStart)
	{
		GoalTime += DeltaTime;

		//	ˆÚ“®ˆ—
		UpdateMove(DeltaTime);

		//	ƒWƒƒƒ“ƒvˆ—
		UpdateJump(DeltaTime);

		//	ƒK[ƒhˆ—
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

//	ˆÚ“®ˆ—
void APlayerChara::UpdateMove(float _deltaTime)
{
	FVector NewLocation = GetActorLocation();
	FVector YRotation = GetActorForwardVector();

	if (playerSpeed < playerMaxSpeed)
	{
		playerSpeed += 0.016f * playerMaxSpeed;
	}

	//	‘O‚ÉŒü‚­‚¸‚Á‚ÆˆÚ“®‚·‚é
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


	//	ƒLƒƒƒ‰ƒNƒ^[‚ÌY²ˆÚ“®
	{
		YRotation.Y = 0.f;
		NewLocation.Y += 0.4f * tempRoll;
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
			playerSpeed += playerMaxSpeed * 0.02f;
		}
	}
}

//	ƒWƒƒƒ“ƒvˆ—
void APlayerChara::UpdateJump(float _deltaTime)
{
	if (tempPitch > 30.f && canJump && !isGuarding)
	{
		canJump = false;
		isJumping = true;
		posBeforeJump = GetActorLocation();
	}

	if (isJumping)
	{
		//	ƒWƒƒƒ“ƒv—Ê‚ğŒvZ
		nowJumpHeight = JUMP_HEIGHT;		

		FVector nowPos = GetActorLocation();

		jumpTime += _deltaTime;

		isLanding = false;

		if (nowJumpHeight < 0.0f)
		{
			jumpPower = tempJumpPower;
			jumpTime = 0.f;
			isJumping = false;
			isLanding = true;
			SetActorLocation(FVector(nowPos.X, nowPos.Y, posBeforeJump.Z));
		}
		else
		{
			SetActorLocation(FVector(nowPos.X, nowPos.Y, posBeforeJump.Z + nowJumpHeight), true);
		}

		//prevJumpHeight = nowJumpHeight;
	}
}

//	ƒK[ƒhˆ—
void APlayerChara::UpdateGuard()
{
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
	if (tempPitch < -30.f && haveDashEnergy && !isDead && !isGoal)
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
	if (hadDoOnce)
	{
		SetActorLocation(FVector(restartLocationX - 30.f, 0.f, 110.f));

		HP = 90.f;

		playerSpeed = 0.f;

		ShotEnergy = tempDataOfShot;

		DashEnergy = tempDataOfDash;

		GuardEnergy = tempDataOfGuard;

		isDead = false;

		Player_Select_Widget->RemoveFromViewport();

		selectPlay = 0;

		deadCount++;

		hadDoOnce = false;
	}
}

//”­ËŠJn
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
			// ’e‚Ìì¬FSpawnActor<¶¬‚·‚éƒNƒ‰ƒXŒ^>(¶¬‚·‚éƒNƒ‰ƒXAn“_À•WAn“_‰ñ“]À•W)
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

				isStart = false;

				isDashing = false;

				isGuarding = false;

				restartLocationX = GetActorLocation().X;

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

void APlayerChara::GetCoin()
{
	PlayerScore += CoinScore;

	CoinCount += 1;
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

	if (OtherActor->ActorHasTag("DashLine"))
	{
		isDashLine = true;

		canJump = true;
	}

	if (OtherActor->ActorHasTag("SuperJump"))
	{
		jumpPower = superJumpPower;

		canJump = true;
	}

	//if (OtherActor->ActorHasTag("CheckPoint"))
	//{
	//	tempGoalTime = GoalTime;

	//	tempPlayerScore = PlayerScore;
	//}

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

	if (OtherActor->ActorHasTag("SuperJump"))
	{
		canJump = false;
	}

	if (OtherActor->ActorHasTag("DashLine"))
	{
		isDashLine = false;
	}
}

//	Ç°¤Î¥»¥ó¥µ©`°æ
void APlayerChara::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (m_pArduinoSerial != NULL)
	{
		m_pArduinoSerial->Close();
		m_pArduinoSerial = NULL;
	}

	// ‰ñ“]—Ê‚Ì•Û‘¶—p”z—ñ‚Ì‰Šú‰»
	prevRotator.Reset();
}

//	Ç°¤Î¥»¥ó¥µ©`°æ
FRotator APlayerChara::SensorToRotator()
{
	bool isRead = false;		// ƒf[ƒ^‚ğ“Ç‚İæ‚ê‚½‚©H
	FString fStr;				// “Ç‚İæ‚èƒf[ƒ^Ši”[—p
	int tryCnt = 0;				// “Ç‚İæ‚ë‚¤‚Æ‚µ‚½‰ñ”
	const int tryCntMax = 500;	// Å‘å‚Ì“Ç‚İæ‚é‰ñ”

	// ƒVƒŠƒAƒ‹‚ÌƒIƒuƒWƒFƒNƒg‚ª‚ ‚ê‚Î
	if (m_pArduinoSerial != NULL)
	{
		// ƒf[ƒ^‚Ì“Ç‚İæ‚è
		// ƒf[ƒ^‚ª“Ç‚İæ‚ê‚é‚©AÅ‘å“Ç‚İæ‚è‰ñ”‚É‚È‚é‚Ü‚ÅŒJ‚è•Ô‚·
		do
		{
			m_pArduinoSerial->Println(FString(TEXT("s")));

			fStr = m_pArduinoSerial->Readln(isRead);
			++tryCnt;
		} while (isRead == false && tryCnt < tryCntMax);

		TArray<FString> splitTextArray;
		splitTextArray.Reset();

		UE_LOG(LogTemp, VeryVerbose, TEXT("ASensorTest::SensorToRotator(): Try Read Count: %d / %d"), tryCnt, tryCntMax);

		// “Ç‚İæ‚ê‚È‚©‚Á‚½‚çZeroRotator‚ğ•Ô‚µ‚ÄI—¹
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

		// ƒZƒ“ƒT[ƒf[ƒ^‚ğƒJƒ“ƒ}‹æØ‚è‚ÅsplitTextArray‚É“ü‚ê‚é
		fStr.ParseIntoArray(splitTextArray, TEXT(","));

		// ‚»‚ê‚¼‚ê‚ğintŒ^‚É•ÏŠ·‚·‚é
		TArray<float> rotatorAxis;
		rotatorAxis.Reset();

		for (int i = 0; i < splitTextArray.Num(); ++i)
		{
			rotatorAxis.Add(FCString::Atof(*splitTextArray[i]));
		}

		// Roll(X), Pitch(Y), Yaw(Z)‚Ì—v‘fi3ŒÂ•ªj“Ç‚İæ‚ê‚Ä‚¢‚È‚¯‚ê‚ÎZeroRotator‚ğ•Ô‚·
		if (rotatorAxis.IsValidIndex(2) == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("ASensorTest::SensorToRotator(): Failed Add TArray<float> elements. return ZeroRotator."));
			withSensor = false;
			return FRotator::ZeroRotator;
		}

		UE_LOG(LogTemp, Verbose, TEXT("ASensorTest::SensorToRotator(): Rotator Roll:%f Pitch:%f Yaw:%f"), rotatorAxis[0], rotatorAxis[1], rotatorAxis[2]);

		// FRotatorŒ^‚Ì•Ï”‚ğfloatŒ^‚ğg—p‚µ‚Ä‰Šú‰»
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

//	Ç°¤Î¥»¥ó¥µ©`°æ
void APlayerChara::UpdateSensor(float _deltaTime)
{
	//FRotator tempRot = SensorManager::GetSensorDataRotator();
	//tempRoll = tempRot.Roll;
	//tempPitch = tempRot.Pitch;
	//tempYaw = tempRot.Yaw;

	//// ‰ÁZ
	for (int i = 0; i < prevRotator.Num(); ++i)
	{
		tempRoll += prevRotator[i].Roll;
		tempPitch += prevRotator[i].Pitch;
		tempYaw += prevRotator[i].Yaw;
	}

	// •½‹Ï’l‚ğZo
	tempRoll /= prevRotator.Num();
	tempPitch /= prevRotator.Num();
	tempYaw /= prevRotator.Num();

	//	guardEnergy‚ª0‚É‚È‚Á‚½‚çA‰¡‰ñ“]‚ÌŠp“x‚ğ‹­§‚É0‚É–ß‚é
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

	// Actor‚É‰ñ“]—Ê‚ğ”½‰f
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

	// ƒŠƒXƒg‚ğXV
	if (prevRotator.IsValidIndex(0) == true)
	{
		// ƒCƒ“ƒfƒbƒNƒX”Ô†0‚Ì—v‘f‚ğíœ
		prevRotator.RemoveAt(0);

		//FRotator rotTemp = SensorManager::GetSensorDataRotator();
		FRotator rotTemp = SensorToRotator();

		// ƒZƒ“ƒT[‚©‚ç‚Ì’l‚ªŠ®‘S‚É0‚©”»•Ê
		if (rotTemp == FRotator::ZeroRotator)
		{
			// Œ»İ‚Ì•½‹Ï’l‚ğ‘ã“ü
			prevRotator.Add(rot);
		}
		else
		{
			// ƒZƒ“ƒT[‚©‚ç‚Ì’l‚ğ‘ã“ü
			prevRotator.Add(rotTemp);
		}
	}

	prevDiffRot = rot;
}

//	====================================
//	ƒZƒ“ƒT[‚ª‚Á‚Ä‚È‚¢ŠÖ”


// Še“ü—ÍŠÖŒWƒƒ\ƒbƒh‚Æ‚ÌƒoƒCƒ“ƒhˆ—
void APlayerChara::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//	ˆÚ“®
	InputComponent->BindAxis("MoveRight", this, &APlayerChara::MoveRightWithNoSensor);

	//	ƒK[ƒh
	InputComponent->BindAxis("Guard", this, &APlayerChara::GuardStartWithNoSensor);

	//	ƒ_ƒbƒVƒ…
	InputComponent->BindAxis("DashAndJump", this, &APlayerChara::DashOrJumpStartWithNoSensor);

	InputComponent->BindAxis("Shot", this, &APlayerChara::ShotStart);
}

//	y“ü—ÍƒoƒCƒ“ƒhzƒLƒƒƒ‰ˆÚ“®:¶‰E
void APlayerChara::MoveRightWithNoSensor(float _axisValue)
{
	// 2020/11/11 “nç² ƒZƒ“ƒT[©“®ŒŸo‚É‚æ‚è•ÏX--------------------------
	if (withSensor == false)
	{
		tempRoll = FMath::Clamp(_axisValue, -1.0f, 1.0f) * 180.f;
	}
	//---------------------------------------------------------------------
}

//	y“ü—ÍƒoƒCƒ“ƒhzƒK[ƒh‚Ì§Œä
void APlayerChara::GuardStartWithNoSensor(float _axisValue)
{
	// 2020/11/11 “nç² ƒZƒ“ƒT[©“®ŒŸo‚É‚æ‚è•ÏX--------------------------
	if (withSensor == false)
	{
		tempYaw = FMath::Clamp(_axisValue, -1.0f, 1.0f) * 45.f;
	}
	//---------------------------------------------------------------------
}

//	y“ü—ÍƒoƒCƒ“ƒhzƒLƒƒƒ‰ˆÚ“®:‘OŒã
void APlayerChara::DashOrJumpStartWithNoSensor(float _axisValue)
{
	// 2020/11/11 “nç² ƒZƒ“ƒT[©“®ŒŸo‚É‚æ‚è•ÏX--------------------------
	if (withSensor == false)
	{
		tempPitch = FMath::Clamp(_axisValue, -1.0f, 1.0f) * 45.f;
	}
	//---------------------------------------------------------------------
}

void APlayerChara::ShotStart(float _axisValue)
{
	if (_axisValue == 1 && haveShotEnergy && isStart && !isGoal && !isDead)
	{
		if (isFirstShoting)
		{
			FVector currentVector = GetActorLocation();
			GetWorld()->SpawnActor<APlayerBullet>(bulletActor, currentVector + this->GetActorForwardVector() * bulletXOffset, FRotator().ZeroRotator);
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