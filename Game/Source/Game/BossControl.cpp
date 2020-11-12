/*
 *--------------------------------------------------------
 * ボスの移動
 * 2020/11/08 渡邊龍音 作成
 *--------------------------------------------------------
 */

#include "BossControl.h"
 // GetWorld() に必要
#include "Engine/World.h"
 // ConstructorHelpers::FObjectFinder に必要
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

ABossControl::ABossControl()
	: m_MoveSpeed(50.0f)
	, m_InsideMigrationTime(5.0f)
	, m_RotateStateRatio(0.8f)
	, m_JumpTime(1.833f)
	, m_JumpDegreeZ(30.0f)
	, m_RunningTimeAfterAttack(3.0f)
	, m_LightningStrikeMoveSpeed(75.0f)
	, m_LightningStrikeDuration(5.0f)
	, m_LightningStrikeWidth(2500.0f)
	, m_LightningStrikeInterval(2.0f)
	, m_LightningStrikeMarker(nullptr)
	, m_Timer(0.0f)
	, originPosY(0.0f)
	, m_LightningStrikeIntervalTemp(0.0f)
	, m_LightningStrikeIndex(0)
	, bossState(BossState::Run)
	, bossAttack(BossAttack::LightningStrike)
{
	PrimaryActorTick.bCanEverTick = true;

	// 攻撃マーカーの取得・設定
	ConstructorHelpers::FObjectFinder<UClass> rightningStrikeMarker(TEXT("/Game/BP/RedCircleFace.RedCircleFace_C"));
	m_LightningStrikeMarker = rightningStrikeMarker.Object;
}

void ABossControl::BeginPlay()
{
	Super::BeginPlay();

	// もともとの座標を保存
	originPosY = GetActorLocation().Y;

	// ステージ外からの移動の回転量が不正の場合に強制的に補正

	// 右側にいるときに右に回転するようになっていたら
	if (m_JumpDegreeZ > 0.0f && originPosY > 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("BossBP's Location.Y is more than 0. (Right Side) But it is set to turn right. So fix to turn left."));
		m_JumpDegreeZ *= -1.0f;
	}
	// 左側にいるときに左に回転するようになっていたら
	else if (m_JumpDegreeZ < 0.0f && originPosY < 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("BossBP's Location.Y is less than 0. (Left Side) But it is set to turn left. So fix to turn right."));
		m_JumpDegreeZ *= -1.0f;
	}
}

void ABossControl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (bossState)
	{
		// 走っている状態
	case BossState::Run:

		UE_LOG(LogTemp, Verbose, TEXT("BossEnemy state is Running."));
		// 攻撃移行時間になったら
		if (m_Timer >= m_InsideMigrationTime)
		{
			// ジャンプ状態へ
			bossState = BossState::Jump_In;

			FRotator rot(GetActorRotation().Pitch, m_JumpDegreeZ, GetActorRotation().Roll);
			SetActorRotation(rot);

			originPosY = GetActorLocation().Y;

			// タイマーリセット
			m_Timer = 0.0f;
		}
		// 回転を始める時間になったら
		else if (m_Timer >= m_InsideMigrationTime * m_RotateStateRatio)
		{
			SetActorRotationLerpDegree(0.0f, m_JumpDegreeZ, (m_Timer - m_InsideMigrationTime * m_RotateStateRatio), (m_InsideMigrationTime - m_InsideMigrationTime * m_RotateStateRatio));

			MoveForward(m_MoveSpeed, GetActorLocation());
		}
		// 走る処理
		else
		{
			SetActorRotation(FRotator::ZeroRotator);
			MoveForward(m_MoveSpeed, GetActorLocation());
		}
		break;

		// ジャンプして中に入る状態
	case BossState::Jump_In:
		UE_LOG(LogTemp, Verbose, TEXT("BossEnemy state is Jump (in)."));

		// ジャンプ時間が終了したら
		if (m_Timer >= m_JumpTime)
		{
			// 攻撃状態へ
			bossState = BossState::Attack;
			SetActorRotation(FRotator::ZeroRotator);

			// 攻撃の種類を決める（今は一種類なので固定）
			bossAttack = BossAttack::LightningStrike;

			// タイマーリセット
			m_Timer = 0.0f;
		}
		// 回転を始める時間になったら
		else if (m_Timer >= m_JumpTime * m_RotateStateRatio)
		{
			SetActorRotationLerpDegree(m_JumpDegreeZ, 0.0f, (m_Timer - m_JumpTime * m_RotateStateRatio), (m_JumpTime - m_JumpTime * m_RotateStateRatio));

			float nextPosY = FMath::Lerp(originPosY, 0.0f, m_Timer / m_JumpTime);

			// 移動する
			FVector tempPos(GetActorLocation().X + m_MoveSpeed, nextPosY, GetActorLocation().Z);
			SetActorLocation(tempPos);
		}
		// ジャンプ処理
		else
		{
			float nextPosY = FMath::Lerp(originPosY, 0.0f, m_Timer / m_JumpTime);

			// 移動する
			FVector tempPos(GetActorLocation().X + m_MoveSpeed, nextPosY, GetActorLocation().Z);
			SetActorLocation(tempPos);
		}
		break;

		// 攻撃状態
	case BossState::Attack:
		UE_LOG(LogTemp, Verbose, TEXT("BossEnemy state is Attacking."));

		// 攻撃の種類によって分岐
		switch (bossAttack)
		{
			// 落雷攻撃
		case BossAttack::LightningStrike:

			// 攻撃時間が終了したら
			if (m_Timer >= m_LightningStrikeDuration)
			{
				bossState = BossState::RunAfterAttack;

				// タイマーリセット
				m_LightningStrikeIntervalTemp = 0.0f;
				m_Timer = 0.0f;
			}
			// 攻撃・移動処理
			else
			{
				MoveForward(m_LightningStrikeMoveSpeed, GetActorLocation());

				// 攻撃時間になっていたら
				if (m_Timer >= m_LightningStrikeIntervalTemp)
				{
					// 攻撃間隔を増加
					m_LightningStrikeIntervalTemp += m_LightningStrikeInterval;

					// 攻撃生成位置を決定
					float attackYPos = FMath::RandRange(-m_LightningStrikeWidth, m_LightningStrikeWidth);
					FVector attackPos(GetActorLocation().X, attackYPos, GetActorLocation().Z);

					// 生成パラメータの設定
					FActorSpawnParameters params;

					params.bAllowDuringConstructionScript = true;
					params.bDeferConstruction = false;
					params.bNoFail = true;
					params.Instigator = this;
					params.Name = { };
					params.ObjectFlags = EObjectFlags::RF_NoFlags;
					params.OverrideLevel = nullptr;
					params.Owner = this;
					params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					params.Template = nullptr;

					// 攻撃マーカー（攻撃も行う）の生成
					if (m_LightningStrikeMarker != nullptr)
					{
						GetWorld()->SpawnActor<AActor>(m_LightningStrikeMarker, attackPos, FRotator::ZeroRotator, params);
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("BossControl::m_LightningStrikeMarker is nullptr. check Constructor."));
					}

					UE_LOG(LogTemp, VeryVerbose, TEXT("BossEnemy is Attacked! (LightningStrike)"));
				}
			}
			break;

			// その他の攻撃手段
		default:
			break;
		}
		break;

		// 攻撃後の移動
	case BossState::RunAfterAttack:
		UE_LOG(LogTemp, Verbose, TEXT("BossEnemy state is Runnning (after Attack)."));

		// ジャンプ移行時間になったら
		if (m_Timer >= m_RunningTimeAfterAttack)
		{
			// ジャンプ状態へ
			bossState = BossState::Jump_Out;

			FRotator rot(GetActorRotation().Pitch, -m_JumpDegreeZ, GetActorRotation().Roll);
			SetActorRotation(rot);

			// タイマーリセット
			m_Timer = 0.0f;
		}
		// 回転を始める時間になったら
		else if (m_Timer >= m_RunningTimeAfterAttack * m_RotateStateRatio)
		{
			SetActorRotationLerpDegree(0.0f, -m_JumpDegreeZ, (m_Timer - m_RunningTimeAfterAttack * m_RotateStateRatio), (m_RunningTimeAfterAttack - m_RunningTimeAfterAttack * m_RotateStateRatio));

			MoveForward(m_MoveSpeed, GetActorLocation());
		}
		// 走る処理
		else
		{
			SetActorRotation(FRotator::ZeroRotator);
			MoveForward(m_MoveSpeed, GetActorLocation());
		}
		break;

		// ジャンプで外に行く
	case BossState::Jump_Out:
		UE_LOG(LogTemp, Verbose, TEXT("BossEnemy state is Jump (out)."));

		// ジャンプ時間が終了したら
		if (m_Timer >= m_JumpTime)
		{
			// 攻撃状態へ
			bossState = BossState::Attack;
			SetActorRotation(FRotator::ZeroRotator);

			// 攻撃の種類を決める（今は一種類なので固定）
			bossAttack = BossAttack::LightningStrike;

			// タイマーリセット
			m_Timer = 0.0f;
		}
		// 回転を始める時間になったら
		else if (m_Timer >= m_JumpTime * m_RotateStateRatio)
		{
			SetActorRotationLerpDegree(m_JumpDegreeZ, 0.0f, (m_Timer - m_JumpTime * m_RotateStateRatio), (m_JumpTime - m_JumpTime * m_RotateStateRatio));

			float nextPosY = FMath::Lerp(originPosY, 0.0f, m_Timer / m_JumpTime);

			// 移動する
			FVector tempPos(GetActorLocation().X + m_MoveSpeed, nextPosY, GetActorLocation().Z);
			SetActorLocation(tempPos);
		}
		// ジャンプ処理
		else
		{
			float nextPosY = FMath::Lerp(originPosY, 0.0f, m_Timer / m_JumpTime);

			// 移動する
			FVector tempPos(GetActorLocation().X + m_MoveSpeed, nextPosY, GetActorLocation().Z);
			SetActorLocation(tempPos);
		}
		break;

		// その他の状態
	default:
		UE_LOG(LogTemp, Warning, TEXT("BossEnemy state is Unknown. bossState index = %d."), (int)bossState);
		break;
	}

	// タイマーカウント
	m_Timer += DeltaTime;
}

void ABossControl::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// 2点のVector座標から角度を算出
float ABossControl::GetRadianFromVector(const FVector& posA, const FVector& posB)
{
	float rad = FMath::Atan2(posB.Y - posA.Y, posB.X - posA.X);
	return rad;
}

// 角度を補間して変更
void ABossControl::SetActorRotationLerpDegree(const float startDeg, const float targetDeg, const float& dividend, const float& divisor)
{
	float interpolation = FMath::Lerp(0.0f, 1.0f, dividend / divisor);
	float deg = FMath::Lerp(startDeg, targetDeg, interpolation);

	FRotator rot(0.0f, deg, 0.0f);
	SetActorRotation(rot);
}

// ジャンプ状態かどうか
bool ABossControl::GetIsJump() const
{
	if (bossState == BossState::Jump_In || bossState == BossState::Jump_Out)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// ジャンプ状態かどうか
bool ABossControl::GetIsAttack() const
{
	if (bossState == BossState::Attack)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// ジャンプ時間の取得
float ABossControl::GetJumpTime() const
{
	return m_JumpTime;
}

// 直線移動処理
void ABossControl::MoveForward(const float& _speed, const FVector& _pos)
{
	// X軸に移動量を加算し、Actorの位置を更新する
	FVector vector = GetActorForwardVector() * _speed;
	FVector nextPos((_pos.X + vector.X), (_pos.Y + vector.Y), _pos.Z);

	SetActorLocation(nextPos);
}