//----------------------------------------------------------
// ファイル名		：PlayerChara.cpp
// 概要				：プレイヤーの制御
// 作成者			：19CU0220 曹飛
// 更新内容			：2020/10/02 作成　プレイヤーの各操作
//					：2020/11/12 更新　渡邊龍音　センサーを自動的に検出するようになる
//					：2020/11/16 変更　鍾家同　bulletActorをAPlayerBullet型に継承する
//----------------------------------------------------------

// インクルード
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

//	defineマクロ
#define ROTATOR_ARRAY_SIZE 1
#define JUMP_HEIGHT (jumpTime * jumpTime * (-gravity) / 2) + (jumpTime * jumpPower)

// コンストラクタ
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

	tempDamageFrame = DamageFrame;
	tempSpeed = playerSpeed;

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerChara::OnBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &APlayerChara::OverlapEnds);;

	//	PlayerCharaが持っているメッシュコンポーネントの相対位置を変更
	USkeletalMeshComponent* pMeshComp = GetMesh();
	if (pMeshComp != NULL)
	{
		//	Z座標を下げる
		pMeshComp->SetRelativeLocation(FVector(0.f, 0.f, -85.f));
	}

	UCharacterMovementComponent* pCharMoveComp = GetCharacterMovement();
	if (pCharMoveComp != NULL)
	{
		//	ジャンプ時にも水平方向への移動が聞くように（0～1の間に設定することで移動する具合を調整）
		pCharMoveComp->AirControl = 0.8f;
	}


	// 2020/11/11 渡邊 自動検出に変更-----------------------------------------begin--
	for (int i = 0; i < 20 && isOpen == false; ++i)
	{
		// シリアルポートを開ける
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

	// 10回分のデータを入れる
	int errorCount = 0;
	for (int i = 0; i < ROTATOR_ARRAY_SIZE; ++i)
	{
		FRotator rotTemp;
		rotTemp = SensorToRotator();

		// センサーの値が読み取れていなければやり直し
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

// 毎フレームの更新処理
void APlayerChara::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(DeltaTime * 60.f));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, isGuarding ? TEXT("true") : TEXT("false"));

	if (withSensor)
	{
		//	センサーの更新処理
		UpdateSensor(DeltaTime);
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

//	移動処理
void APlayerChara::UpdateMove(float _deltaTime)
{
	FVector NewLocation = GetActorLocation();
	FVector YRotation = GetActorForwardVector();

	//	前に向くずっと移動する
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


	//	キャラクターのY軸移動
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

//	ジャンプ処理
void APlayerChara::UpdateJump(float _deltaTime)
{
	if (tempPitch > 30.f && !isJumping && canJump && !isGuarding)
	{
		isJumping = true;
		posBeforeJump = GetActorLocation();
	}

	if (isJumping)
	{
		//	ジャンプ量を計算
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

//	ガード処理
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
			// 弾の作成：SpawnActor<生成するクラス型>(生成するクラス、始点座標、始点回転座標)
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

	// 回転量の保存用配列の初期化
	prevRotator.Reset();
}

//	センサーの更新処理
void APlayerChara::UpdateSensor(float _deltaTime)
{
	tempRoll = 0.f;
	tempPitch = 0.f;
	tempYaw = 0.f;

	// 加算
	for (int i = 0; i < prevRotator.Num(); ++i)
	{
		tempRoll += prevRotator[i].Roll;
		tempPitch += prevRotator[i].Pitch;
		tempYaw += prevRotator[i].Yaw;
	}

	// 平均値を算出
	tempRoll /= prevRotator.Num();
	tempPitch /= prevRotator.Num();
	tempYaw /= prevRotator.Num();

	//	guardEnergyが0になったら、横回転の角度を強制に0に戻る
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

	float angle = 5.f;
	if (FMath::Abs((rot - prevDiffRot).Roll) < angle && FMath::Abs((rot - prevDiffRot).Pitch) < angle && FMath::Abs((rot - prevDiffRot).Yaw) < angle)
	{
		rot = prevDiffRot;
	}

	SetActorRotation(rot);

	// リストを更新
	if (prevRotator.IsValidIndex(0) == true)
	{
		// インデックス番号0の要素を削除
		prevRotator.RemoveAt(0);

		FRotator rotTemp = SensorToRotator();

		// センサーからの値が完全に0か判別
		if (rotTemp == FRotator::ZeroRotator)
		{
			// 現在の平均値を代入
			prevRotator.Add(rot);
		}
		else
		{
			// センサーからの値を代入
			prevRotator.Add(rotTemp);
		}
	}

	prevDiffRot = rot;
}


FRotator APlayerChara::SensorToRotator()
{
	bool isRead = false;		// データを読み取れたか？
	FString fStr;				// 読み取りデータ格納用
	int tryCnt = 0;				// 読み取ろうとした回数
	const int tryCntMax = 500;	// 最大の読み取る回数

	// シリアルのオブジェクトがあれば
	if (m_pArduinoSerial != NULL)
	{
		// データの読み取り
		// データが読み取れるか、最大読み取り回数になるまで繰り返す
		do
		{
			m_pArduinoSerial->Println(FString(TEXT("s")));

			fStr = m_pArduinoSerial->Readln(isRead);
			++tryCnt;
		} while (isRead == false && tryCnt < tryCntMax);

		TArray<FString> splitTextArray;
		splitTextArray.Reset();

		UE_LOG(LogTemp, VeryVerbose, TEXT("ASensorTest::SensorToRotator(): Try Read Count: %d / %d"), tryCnt, tryCntMax);

		// 読み取れなかったらZeroRotatorを返して終了
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

		// センサーデータをカンマ区切りでsplitTextArrayに入れる
		fStr.ParseIntoArray(splitTextArray, TEXT(","));

		// それぞれをint型に変換する
		TArray<float> rotatorAxis;
		rotatorAxis.Reset();

		for (int i = 0; i < splitTextArray.Num(); ++i)
		{
			rotatorAxis.Add(FCString::Atof(*splitTextArray[i]));
		}

		// Roll(X), Pitch(Y), Yaw(Z)の要素（3個分）読み取れていなければZeroRotatorを返す
		if (rotatorAxis.IsValidIndex(2) == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("ASensorTest::SensorToRotator(): Failed Add TArray<float> elements. return ZeroRotator."));
			withSensor = false;
			return FRotator::ZeroRotator;
		}

		UE_LOG(LogTemp, Verbose, TEXT("ASensorTest::SensorToRotator(): Rotator Roll:%f Pitch:%f Yaw:%f"), rotatorAxis[0], rotatorAxis[1], rotatorAxis[2]);

		// FRotator型の変数をfloat型を使用して初期化
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
//	センサーが持ってない関数


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
}

//	【入力バインド】キャラ移動:左右
void APlayerChara::MoveRightWithNoSensor(float _axisValue)
{
	// 2020/11/11 渡邊 センサー自動検出により変更--------------------------
	if (withSensor == false)
	{
		tempRoll = FMath::Clamp(_axisValue, -1.0f, 1.0f) * 180.f;
	}
	//---------------------------------------------------------------------
}

//	【入力バインド】ガードの制御
void APlayerChara::GuardStartWithNoSensor(float _axisValue)
{
	// 2020/11/11 渡邊 センサー自動検出により変更--------------------------
	if (withSensor == false)
	{
		tempYaw = FMath::Clamp(_axisValue, -1.0f, 1.0f) * 45.f;
	}
	//---------------------------------------------------------------------
}

//	【入力バインド】キャラ移動:前後
void APlayerChara::DashOrJumpStartWithNoSensor(float _axisValue)
{
	// 2020/11/11 渡邊 センサー自動検出により変更--------------------------
	if (withSensor == false)
	{
		tempPitch = FMath::Clamp(_axisValue, -1.0f, 1.0f) * 45.f;
	}
	//---------------------------------------------------------------------
}