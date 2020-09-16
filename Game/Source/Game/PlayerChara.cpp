// インクルード
#include "PlayerChara.h"
#include "Engine.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "..\..\UE4Duino\Source\UE4Duino\Public\Serial.h"
#include <time.h>

//	defineマクロ
#define ROTATOR_ARRAY_SIZE 1
#define JUMP_HEIGHT (m_jumpTime * m_jumpTime * (-m_gravity) / 2) + (m_jumpTime * m_jumpPower)

// コンストラクタ
APlayerChara::APlayerChara()
	: m_pArduinoSerial(NULL)
	, serialPort(4)
	, isOpen(false)
	, m_pSpringArm(NULL)
	, m_pCamera(NULL)
	, m_gravity(600.f)
	, m_jumpPower(1800.f)
	, m_jumpTime(0.f)
	, m_nowJumpHeight(0.f)
	, m_prevJumpHeight(0.f)
	, m_bJumping(false) 
	, m_bGuarding(false)
	, m_bAccelerate(false)
	, m_bCanControl(true)
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

	//	スプリングアームのオブジェクトを生成
	m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("m_pSpringArm"));
	if (m_pSpringArm != NULL)
	{
		m_pSpringArm->SetupAttachment(RootComponent);

		//	アームの長さを設定
		//	カメラの子リジョンテストを行うかを設定
		m_pSpringArm->bDoCollisionTest = false;
		//	カメラ追従ラグを使うかを設定
		m_pSpringArm->bEnableCameraLag = true;
		//	カメラ追従ラグの速度を設定
		m_pSpringArm->CameraLagSpeed = 20.f;
		//	カメラ回転ラグを使うかを設定
		m_pSpringArm->bEnableCameraRotationLag = true;
		//	カメラ回転ラグの速度を設定
		m_pSpringArm->CameraRotationLagSpeed = 20.f;		
	}

	//	カメラのオブジェクトを生成
	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));
	if (m_pCamera != NULL)
	{
		//	カメラをスプリングアームにアタッチさせる
		m_pCamera->SetupAttachment(m_pSpringArm, USpringArmComponent::SocketName);
	}
}

// ゲームスタート時、または生成時に呼ばれる処理
void APlayerChara::BeginPlay()
{
	Super::BeginPlay();

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

	// シリアルポートを開ける
	m_pArduinoSerial = USerial::OpenComPort(isOpen, serialPort, 115200);

	if (isOpen == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASensorTest::BeginPlay(): COM Port:%d is failed open. Please check the connection and COM Port number."), serialPort);
		return;
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("ASensorTest::BeginPlay(): COM Port:%d is Successfully Open."), serialPort);
	}

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
}

// 毎フレームの更新処理
void APlayerChara::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//	カメラ更新処理
	UpdateCamera(DeltaTime);

	//	移動処理
	UpdateMove(DeltaTime);

	//	ジャンプ処理
	UpdateJump(DeltaTime);

	//	ガード処理
	UpdateGuard();

	UpdateAccelerate();

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

	//GEngine->AddOnScreenDebugMessage(-1, 3.0, FColor::Red, FString::FromInt(tempYaw));

	// Actorに回転量を反映
	FRotator rot(tempPitch, tempYaw, tempRoll);

	UE_LOG(LogTemp, Error, TEXT("Roll : %f Pitch : %f Yaw : %f"), (rot - prevDiffRot).Roll, (rot - prevDiffRot).Pitch, (rot - prevDiffRot).Yaw);

	float angle = 5.0f;
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

//	カメラ更新処理
void APlayerChara::UpdateCamera(float _deltaTime)
{
	//	ルートオブジェクトを中心に、スプリングアームについているカメラを回転させる
	USpringArmComponent* pSpringArm = m_pSpringArm;
	if (pSpringArm != NULL)
	{
		//	現在のFPSを測定
		float fps = 1.0f / _deltaTime;

		//	処理落ちしても、一定速度でカメラが回るように補正
		float rotateCorrection = 60.f / fps;
	}
}

//	移動処理
void APlayerChara::UpdateMove(float _deltaTime)
{
	FVector NewLocation = GetActorLocation();
	//	前に向くずっと移動する
	if (m_bAccelerate)
	{
		NewLocation.X += 30.f;
	}
	else if (!m_bGuarding && !m_bAccelerate)
	{
		NewLocation.X += 20.f;
	}
	else
	{
		NewLocation.X += 12.f;
	}

	//	キャラクターの移動
	{
		//	SpringArmが向く方向に、入力による移動量をPawnMovementComponentに渡す
		NewLocation.Y += tempRoll * 0.2f;
		SetActorLocation(NewLocation);
	}
}

//	ジャンプ処理
void APlayerChara::UpdateJump(float _deltaTime)
{
	if(tempPitch > 45.f)
	{
		//	ジャンプ量を計算
		m_nowJumpHeight = JUMP_HEIGHT;

		//	ジャンプ時間を増加
		m_jumpTime += _deltaTime;

		//	Actorの現在の座標を取得
		FVector nowPos = GetActorLocation();

		//	着地時（=ジャンプ量がマイナスに転じた時）、ジャンプ前状態に戻す
		if (m_nowJumpHeight < 0.0f)
		{
			m_bJumping = false;
			m_jumpTime = 0.0f;

			SetActorLocation(FVector(nowPos.X, nowPos.Y, m_posBeforeJump.Z));
		}
		//	それ以外は、ジャンプしているため座標を反映
		else
		{
			//	現在の座標にジャンプ量を足す
			SetActorLocation(FVector(nowPos.X, nowPos.Y, m_posBeforeJump.Z + m_nowJumpHeight), true);
		}

		//	ジャンプ量を保持
		m_prevJumpHeight = m_nowJumpHeight;
	}
}

//	ガード処理
void APlayerChara::UpdateGuard()
{
		FRotator nowRot = GetActorRotation();
		if (tempYaw < -30.f)
		{
			m_bGuarding = true;
		}
		else
		{
			m_bGuarding = false;
		}
}

void APlayerChara::UpdateAccelerate()
{
	FRotator nowRot = GetActorRotation();
	if (tempPitch < -30.f || tempPitch > 30.f)
	{
		m_bAccelerate = true;
	}
	else
	{
		m_bAccelerate = false;
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
		return FRotator::ZeroRotator;
	}
}