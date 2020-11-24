/*
 *--------------------------------------------------------
 * ボスの移動
 * 2020/11/08 渡邊龍音 作成
 *--------------------------------------------------------
 */

#include "BossControl.h"
#include "BossWaveControl.h"
 // GetWorld() に必要
#include "Engine/World.h"
 // ConstructorHelpers::FObjectFinder に必要
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "DrawDebugHelpers.h"

ABossControl::ABossControl()
// 全体のプロパティ
	: m_MoveSpeed(50.0f)
	, m_RunningAnimationRateSpeed(1.0f)
	, m_InsideMigrationTime(5.0f)
	, m_RotateStateRatio(0.8f)
	, m_JumpTime(1.833f)
	, m_JumpDegreeZ(30.0f)
	, m_RunningRoadPosY(0.0f)
	, m_RunningTimeAfterAttack(3.0f)
	, m_IsHeadAnim(false)
	, m_JumpLocationY(0.0f)
	, m_Timer(0.0f)
	, originPosY(0.0f)
	, m_attackIntervalTemp(0.0f)
	, m_RunningRoadPosYTemp(0.0f)
	, bossState(BossState::Run)
	, bossAttack(BossAttack::LightningStrike)

	// 落雷攻撃プロパティ
	, m_LightningStrikeMoveSpeed(75.0f)
	, m_LightningStrikeDuration(5.0f)
	, m_LightningStrikeWidth(2500.0f)
	, m_LightningStrikeInterval(2.0f)
	, m_LightningStrikeMarker(nullptr)

	// ウェーブ攻撃プロパティ
	, m_WaveAttackMoveSpeed(75.0f)
	, m_WaveAttackDuration(5.0f)
	, m_WaveAttackInterval(2.0f)
	, m_JumppadSpawnRatio(0.2f)
	, m_JumppadGenerateWidth(2500.0f)
	, m_JumppadPositionXAvoidWaveAttack(1000.0f)
	, m_JumppadPositionZAvoidWaveAttack(300.0f)
	, m_JumppadScaleAvoidWaveAttack(FVector::OneVector)
	, m_IsJumppadGenerate(true)
	, m_WaveAttackCount(0)
	, m_JumppadGenerateCount(0)
	, m_JumppadPosX(0.0f)
	, m_WaveAttackActor(nullptr)
	, m_JumppadActor(nullptr)

	// 突進攻撃プロパティ
	, m_RushAttackMoveSpeed(200.0f)
	, m_RushAttackSpeed(200.0f)
	, m_RushStartXAxisOffset(1000.0f)
	, m_RushAttackRunningRoadPosY(500.0f)
	, m_RushTurnTime(0.5f)
	, m_IsHeadAnimEnd(false)
	, m_IsRush(false)
	, m_IsRushTurnDirectionRight(false)
	, m_GetPlayerCoordinatePosX(5000.0f)
	, m_RushRunStartPosX(0.0f)
	, m_RushStartPosX(0.0f)
	, m_RushDistance(5000.0f)

	//TEMP////////////////////////////////////////////////////////////
	, m_TEMP_BOSS_ATTACK(BossAttack::LightningStrike)
	//////////////////////////////////////////////////////////////////
{
	PrimaryActorTick.bCanEverTick = true;

	// 攻撃マーカーの取得・設定
	ConstructorHelpers::FObjectFinder<UClass> rightningStrikeMarker(TEXT("/Game/BP/RedCircleFace.RedCircleFace_C"));
	m_LightningStrikeMarker = rightningStrikeMarker.Object;

	// ウェーブ攻撃の取得・設定
	ConstructorHelpers::FObjectFinder<UClass> waveAttack(TEXT("/Game/BP/BossWaveBP.BossWaveBP_C"));
	m_WaveAttackActor = waveAttack.Object;

	// ジャンプ台の取得・設定
	ConstructorHelpers::FObjectFinder<UClass> jumppadActor(TEXT("/Game/BP/JumpPad.JumpPad_C"));
	m_JumppadActor = jumppadActor.Object;

	// 波状攻撃の回数を算出
	m_WaveAttackCount = FMath::FloorToInt(m_WaveAttackDuration / m_WaveAttackInterval);
	UE_LOG(LogTemp, Verbose, TEXT("WaveAttackCount = %d"), m_WaveAttackCount);

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
		UE_LOG(LogTemp, Warning, TEXT("BossBP's Location.Y is more than 0. (Right Side.). But it is set to turn right. (m_JumpDegreeZ is more than 0.) So fix to turn left."));
		m_JumpDegreeZ *= -1.0f;
	}
	// 左側にいるときに左に回転するようになっていたら
	else if (m_JumpDegreeZ < 0.0f && originPosY < 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("BossBP's Location.Y is less than 0. (Left Side.) But it is set to turn left. (m_JumpDegreeZ is less than 0.) So fix to turn right."));
		m_JumpDegreeZ *= -1.0f;
	}
}

void ABossControl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (bossState)
	{
		// 外を走っている状態
	case BossState::Run:

		UE_LOG(LogTemp, Verbose, TEXT("BossEnemy state is Running."));
		// ジャンプ移行時間になったら
		if (m_Timer >= m_InsideMigrationTime)
		{
			// ジャンプ状態へ
			bossState = BossState::Jump_In;

			// ジャンプ開始位置を保存
			m_JumpLocationY = GetActorLocation().Y;

			// ジャンプの着地点のY軸を設定
			m_RunningRoadPosYTemp = m_RunningRoadPosY;

			// 攻撃の種類を決めておく（今は詳細タブで指定した攻撃法のみに固定）
			bossAttack = m_TEMP_BOSS_ATTACK;

			// 攻撃がウェーブ攻撃であれば
			if (bossAttack == BossAttack::Wave)
			{
				// あらかじめ攻撃の間隔をつめておく
				m_attackIntervalTemp = m_WaveAttackInterval;
			}
			// 攻撃が突進攻撃であれば
			else if (bossAttack == BossAttack::Rush)
			{
				// X軸の位置を保存する
				m_RushRunStartPosX = GetActorLocation().X;

				// ジャンプの着地点を上書き
				m_RunningRoadPosYTemp = m_RushAttackRunningRoadPosY;

				// 突進フラグを下ろす
				m_IsRush = false;
			}

			FRotator rot(GetActorRotation().Pitch, m_JumpDegreeZ, GetActorRotation().Roll);
			SetActorRotation(rot);

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

			// タイマーリセット
			m_Timer = 0.0f;
		}
		// 回転を始める時間になったら
		else if (m_Timer >= m_JumpTime * m_RotateStateRatio)
		{
			SetActorRotationLerpDegree(m_JumpDegreeZ, 0.0f, (m_Timer - m_JumpTime * m_RotateStateRatio), (m_JumpTime - m_JumpTime * m_RotateStateRatio));

			float nextPosY = FMath::Lerp(m_JumpLocationY, m_RunningRoadPosYTemp, m_Timer / m_JumpTime);

			// 移動する
			FVector tempPos(GetActorLocation().X + m_MoveSpeed, nextPosY, GetActorLocation().Z);
			SetActorLocation(tempPos);
		}
		// ジャンプ処理
		else
		{
			float nextPosY = FMath::Lerp(m_JumpLocationY, m_RunningRoadPosYTemp, m_Timer / m_JumpTime);

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
				m_attackIntervalTemp = 0.0f;
				m_Timer = 0.0f;
			}
			// 攻撃・移動処理
			else
			{
				MoveForward(m_LightningStrikeMoveSpeed, GetActorLocation());

				// 攻撃時間になっていたら
				if (m_Timer >= m_attackIntervalTemp)
				{
					// 攻撃間隔を増加
					m_attackIntervalTemp += m_LightningStrikeInterval;

					// 攻撃生成位置を決定
					float attackYPos = FMath::RandRange(-m_LightningStrikeWidth, m_LightningStrikeWidth);
					FVector attackPos(GetActorLocation().X, attackYPos, GetActorLocation().Z);

					// 落雷Actorの生成
					SpawnAttackActor(m_LightningStrikeMarker, attackPos);

					UE_LOG(LogTemp, Verbose, TEXT("BossEnemy is Attacked! (LightningStrike)"));
				}
			}
			break;

			// ウェーブ攻撃
		case BossAttack::Wave:

			// 攻撃時間が終了したら
			if (m_Timer >= m_WaveAttackDuration)
			{
				bossState = BossState::RunAfterAttack;

				// ジャンプ台生成回数をリセット
				m_JumppadGenerateCount = 0;

				// タイマーリセット
				m_attackIntervalTemp = 0.0f;
				m_Timer = 0.0f;
			}
			// 攻撃・移動処理
			else
			{
				MoveForward(m_LightningStrikeMoveSpeed, GetActorLocation());

				// ジャンプ台生成可能かつジャンプ台生成の時間で、なおかつジャンプ台生成回数が攻撃回数以下であれば
				if (m_IsJumppadGenerate == true && (m_attackIntervalTemp - m_Timer) < (m_WaveAttackInterval * m_JumppadSpawnRatio)
					&& m_JumppadGenerateCount <= m_WaveAttackCount - 1)
				{
					// ジャンプ台生成位置を決定
					float jumppadYPos = FMath::RandRange(-m_JumppadGenerateWidth, m_JumppadGenerateWidth);
					FVector generatePos(GetActorLocation().X + m_JumppadPositionXAvoidWaveAttack, jumppadYPos, m_JumppadPositionZAvoidWaveAttack);

					// 連続でジャンプ台を生成できないようにする
					m_IsJumppadGenerate = false;

					// ジャンプ台生成
					AActor* jumppadTemp = SpawnAttackActor(m_JumppadActor, generatePos);

					// ジャンプ台のサイズ変更
					jumppadTemp->SetActorScale3D(m_JumppadScaleAvoidWaveAttack);

					// ジャンプ台のX位置を格納
					m_JumppadPosX = jumppadTemp->GetActorLocation().X;

					// ジャンプ台生成回数を加算
					m_JumppadGenerateCount++;

					UE_LOG(LogTemp, Verbose, TEXT("BossEnemy Getenated Jumppad!"));
				}
				// 攻撃時間になっていたら
				else if (m_Timer >= m_attackIntervalTemp)
				{
					// 攻撃間隔を増加
					m_attackIntervalTemp += m_WaveAttackInterval;

					// ジャンプ台を生成可能にする
					m_IsJumppadGenerate = true;

					// ウェーブ攻撃Actorの生成
					ABossWaveControl* waveTemp = Cast<ABossWaveControl>(SpawnAttackActor(m_WaveAttackActor, GetActorLocation()));

					// ウェーブ攻撃にジャンプ台の位置を保存
					waveTemp->SetJumppadPosX(m_JumppadPosX);

					UE_LOG(LogTemp, Verbose, TEXT("BossEnemy is Attacked! (Wave)"));
				}
			}
			break;

			// 突進攻撃
		case BossAttack::Rush:
		{
			// ボスが設定した地点に移動するまで
			if (GetActorLocation().X < m_RushRunStartPosX + m_RushStartXAxisOffset && m_IsRush == false)
			{
				// 通常移動処理
				MoveForward(m_RushAttackMoveSpeed, GetActorLocation());

				// 位置を保存
				m_RushStartPosX = GetActorLocation().X;
			}
			// 移動し終わったら
			else if (m_IsRush == false)
			{
				// タイマーリセット
				m_Timer = 0.0f;

				// 突進状態に
				m_IsRush = true;
			}
			// 突進開始
			else
			{
				// 振り向きの目標地点を設定
				float targetRot = 180.0f;

				// 左回りなら-1を掛ける
				if (m_IsRushTurnDirectionRight)
				{
					targetRot *= -1.0f;
				}

				// 振り向き
				if (m_Timer <= m_RushTurnTime)
				{
					// 角度の補間
					float rotZ = FMath::Lerp(0.0f, targetRot, m_Timer / m_RushTurnTime);

					// 角度の設定
					FRotator rot(GetActorRotation().Pitch, rotZ, GetActorRotation().Roll);
					SetActorRotation(rot);
				}
				// 突進
				else
				{
					// 頭振りアニメーションを行っていなければ
					if (m_IsHeadAnim == false && m_IsHeadAnimEnd == false)
					{
						// 頭振りアニメーションを行う
						m_IsHeadAnim = true;
					}
					// 頭振りアニメーションが終わっていれば
					else if (m_IsHeadAnimEnd)
					{
						// ボスが目的位置に到達していなければ
						if (GetActorLocation().X > m_RushStartPosX - m_RushDistance)
						{
							// 頭振りアニメーションを終了する
							m_IsHeadAnim = false;

							// 角度の設定
							FRotator rot(GetActorRotation().Pitch, targetRot, GetActorRotation().Roll);
							SetActorRotation(rot);

							// 突進移動
							MoveForward(m_RushAttackSpeed, GetActorLocation());
						}
						// 外に出る
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("OUT_OF_BOSS"));
						}
					}
					// 頭振りアニメーション中
					else
					{
					}
				}
			}
			break;

			UE_LOG(LogTemp, Verbose, TEXT("BossEnemy is Attacked! (Rush)"));
			break;
		}

		// その他の攻撃手段
		default:
			UE_LOG(LogTemp, Warning, TEXT("BossEnemy attack is Unknown. bossAttack index = %d."), (int)bossAttack);
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

			// ジャンプ開始位置を保存
			m_JumpLocationY = GetActorLocation().Y;

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
			// 通常状態へ
			bossState = BossState::Run;
			SetActorRotation(FRotator::ZeroRotator);

			// タイマーリセット
			m_Timer = 0.0f;
		}
		// 回転を始める時間になったら
		else if (m_Timer >= m_JumpTime * m_RotateStateRatio)
		{
			SetActorRotationLerpDegree(-m_JumpDegreeZ, 0.0f, (m_Timer - m_JumpTime * m_RotateStateRatio), (m_JumpTime - m_JumpTime * m_RotateStateRatio));

			float nextPosY = FMath::Lerp(m_JumpLocationY, originPosY, m_Timer / m_JumpTime);

			// 移動する
			FVector tempPos(GetActorLocation().X + m_MoveSpeed, nextPosY, GetActorLocation().Z);
			SetActorLocation(tempPos);
		}
		// ジャンプ処理
		else
		{
			float nextPosY = FMath::Lerp(m_JumpLocationY, originPosY, m_Timer / m_JumpTime);

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

	// ジャンプ時にはコリジョンを切る
	if (bossState == BossState::Jump_In || bossState == BossState::Jump_Out)
	{
		SetActorEnableCollision(false);
	}
	else
	{
		SetActorEnableCollision(true);
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

// 攻撃を行うActor(TSubclassOf<>)を生成
AActor* ABossControl::SpawnAttackActor(const TSubclassOf<class AActor> _attackActor, const FVector _attackPos)
{
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

	// 攻撃Actorの生成
	if (_attackActor != nullptr)
	{
		return GetWorld()->SpawnActor<AActor>(_attackActor, _attackPos, FRotator::ZeroRotator, params);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BossControl::_attackActor is nullptr. check Constructor."));
		return nullptr;
	}
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

// 突進攻撃状態かどうか
bool ABossControl::GetIsAttack() const
{
	if (bossState == BossState::Attack && bossAttack == BossAttack::Rush)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// 頭を振り終わったことを取得
void ABossControl::SetIsHeadEnd(bool _status)
{
	m_IsHeadAnimEnd = _status;
}

// 頭をふる状態かどうか
bool ABossControl::GetIsHead() const
{
	return m_IsHeadAnim;
}

// ジャンプ時間の取得
float ABossControl::GetJumpTime() const
{
	return m_JumpTime;
}

// 走るアニメーションの再生速度取得
float ABossControl::GetRunAnimSpeed() const
{
	return m_RunningAnimationRateSpeed;
}

// 直線移動処理
void ABossControl::MoveForward(const float& _speed, const FVector& _pos)
{
	// X軸に移動量を加算し、Actorの位置を更新する
	FVector vector = GetActorForwardVector() * _speed;
	FVector nextPos((_pos.X + vector.X), (_pos.Y + vector.Y), _pos.Z);

	SetActorLocation(nextPos);
}