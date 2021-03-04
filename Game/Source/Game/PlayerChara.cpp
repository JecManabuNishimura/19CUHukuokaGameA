//----------------------------------------------------------
// ファイル名		：PlayerChara.cpp
// 概要				：プレイヤーの制御
// 作成者			：19CU0220 曹飛
// 更新内容			：2020/10/02 作成　プレイヤーの各操作
//					：2020/11/12 更新　渡邊龍音　センサーを自動的に検出するようになる
//					：2020/11/16 変更　鍾家同　bulletActorをAPlayerBullet型に継承する
//					：2021/01/06 変更　鍾家同　GoalWidgetの生成タイミングを変更(VR版のため)
//----------------------------------------------------------

// インクルード
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

//	defineマクロ
#define ROTATOR_ARRAY_SIZE 1
#define JUMP_HEIGHT (jumpTime * jumpTime * (-gravity) / 2) + (jumpTime * jumpPower)

// コンストラクタ
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
	// 毎フレーム、このクラスのTick()を呼ぶかどうかを決めるフラグ。必要に応じて、パフォーマンス向上のために切ることもできる。
	PrimaryActorTick.bCanEverTick = true;

	// 回転量の保存用配列の初期化
	prevRotator.Reset();

	//	デフォルトプレイヤーとして設定
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// ゲームスタート時、または生成時に呼ばれる処理
void APlayerChara::BeginPlay()
{
	Super::BeginPlay();

	//	データ保存
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
			//	ジャンプ時にも水平方向への移動が聞くように（0～1の間に設定することで移動する具合を調整）
			pCharMoveComp->AirControl = 0.8f;
		}

		USensorManager::ConnectToSensor();

		//	PlayerCharaが持っているメッシュコンポーネントの相対位置を変更
		USkeletalMeshComponent* pMeshComp = GetMesh();
		if (pMeshComp != NULL)
		{
			//	Z座標を下げる
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

// 毎フレームの更新処理
void APlayerChara::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//	fpsを獲得
	fps = 1 / DeltaTime;

	if (withSensor)
	{
		//	センサーの更新処理
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

		//	移動処理
		UpdateMove(DeltaTime);

		//	ジャンプ処理
		UpdateJump(DeltaTime);

		//	ガード処理
		UpdateGuard();

		//	ダッシュ処理
		UpdateAccelerate();

		//	射撃処理
		Shooting(DeltaTime);

		//	ジャンプから着地のSE処理
		GetPlayerPosZ(DeltaTime);

		//	ダメージが受けるかどうかの処理
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

//	移動処理
void APlayerChara::UpdateMove(float _deltaTime)
{
	FVector NewLocation = GetActorLocation();
	FVector YRotation = GetActorForwardVector();

	//	fpsによって移動量倍数の計算
	if (fps < 59.5f || fps > 60.5f)
	{
		fps = 1.f / (fps / 60.f);
	}
	else
	{
		fps = 60.f / fps;
	}

	//	スタートからだんだん最大速度に辿る
	if (playerSpeed < playerMaxSpeed)
	{
		playerSpeed += 0.016f * playerMaxSpeed * fps;
	}

	//	前に向くずっと移動する
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


	//	キャラクターのY軸移動
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

	//	ダメージが受けたら速度が落ち処理
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

//	ジャンプ処理
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
	//	ジャンプできるかの判断

	if (isJumping)
	{
		//	ジャンプ量を計算
		nowJumpHeight = JUMP_HEIGHT;		

		FVector nowPos = GetActorLocation();

		jumpTime += _deltaTime;

		isLanding = false;

		//	地面に落ちたらジャンプ停止
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

//	ガード処理
void APlayerChara::UpdateGuard()
{
	//	ガードできるかの判断
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

//発射開始
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
			// 弾の作成：SpawnActor<生成するクラス型>(生成するクラス、始点座標、始点回転座標)
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

//	ジャンプ台から落としたらSEを出すための関数
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

//	最新版
void APlayerChara::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	USensorManager::DisconnectToSensor();
}

//	最新版
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

	//	guardEnergyが0になったら、横回転の角度を強制に0に戻る
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

	// Actorに回転量を反映
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

// 各入力関係メソッドとのバインド処理
void APlayerChara::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//	移動
	InputComponent->BindAxis("MoveRight", this, &APlayerChara::MoveRightWithNoSensor);

	//	ガード
	InputComponent->BindAxis("Guard", this, &APlayerChara::GuardStartWithNoSensor);

	//	ダッシュ
	InputComponent->BindAxis("DashAndJump", this, &APlayerChara::DashOrJumpStartWithNoSensor);

	InputComponent->BindAxis("Shot", this, &APlayerChara::ShotStart);
}

//	【入力バインド】キャラ移動:左右
void APlayerChara::MoveRightWithNoSensor(float _axisValue)
{
	if (withSensor == false)
	{
		tempRoll = FMath::Clamp(_axisValue, -1.0f, 1.0f) * 180.f;
	}
	//---------------------------------------------------------------------
}

//	【入力バインド】ガードの制御
void APlayerChara::GuardStartWithNoSensor(float _axisValue)
{
	if (withSensor == false)
	{
		tempYaw = FMath::Clamp(_axisValue, -1.0f, 1.0f) * 45.f;
	}
	//---------------------------------------------------------------------
}

//	【入力バインド】キャラ移動:前後
void APlayerChara::DashOrJumpStartWithNoSensor(float _axisValue)
{
	if (withSensor == false)
	{
		tempPitch = FMath::Clamp(_axisValue, -1.0f, 1.0f) * 45.f;
	}
	//---------------------------------------------------------------------
}

//	プレイヤーの弾処理
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