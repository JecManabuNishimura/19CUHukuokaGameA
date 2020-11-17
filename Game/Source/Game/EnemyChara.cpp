//----------------------------------------------------------
// ファイル名		：SensorTest.h
// 概要				：敵の操作を行う
// 作成者			：19CU0238 渡邊龍音
//
// 更新内容			：2020/09/13 渡邊龍音 作成
//					：2020/09/19 渡邊龍音 溜めのある敵の動きの追加
//					：			 直線移動をForwardVectorで行う
//----------------------------------------------------------
#include "EnemyChara.h"
#include "DrawDebugHelpers.h"

AEnemyChara::AEnemyChara()
	: sinTime(0.0f)
	, originPosY(0.0f)
	, chargeTime(0.0f)
	, isBlowing(false)
	, enemyMoveType(EEnemyMoveType::None)
	, playerActor(NULL)
	, forwardSpeed(1.0f)
	, sinWaveSpeed(1.0f)
	, sinWaveFrequency(1.0f)
	, bodyBlowDistance(100.0f)
	, bodyBlowMoveSpeed(0.5f)
	, bodyBlowSpeed(2.0f)
	, bodyBlowChargeTime(2.0f)
	, bodyBlowTurnDelay(0.5f)
	, bodyBlowTurnShorten(0.5f)
	, overtakeSpeed(2.0f)
	, overtakeOffset(0.0f)
	, overtakeDistance(300.0f)
	, overtakeYAxisMove(-100.0f)

	, _TEMP_playerMoveSpeed(5.0f)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyChara::BeginPlay()
{
	Super::BeginPlay();

	// 開始時点のY座標を保存
	originPosY = GetActorLocation().Y;

	// 開始時点の回転量を保存
	originRotate = GetActorRotation();
}

void AEnemyChara::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// このActorの位置の更新
	FVector pos = GetActorLocation();

	// Playerとして設定したActorの位置の更新（設定した場合のみ）
	FVector playerPos = FVector::ZeroVector;
	if (playerActor != NULL)
	{
		playerPos = playerActor->GetActorLocation();
	}

	// 移動の種類に応じて分岐
	switch (enemyMoveType)
	{
		// 直線移動タイプ
	case EEnemyMoveType::Line:

		Move_Line(forwardSpeed, pos);
		break;


		// 正弦波移動タイプ
	case EEnemyMoveType::Sin:

		Move_Sin(forwardSpeed, sinWaveSpeed, sinWaveFrequency, DeltaTime, pos);
		break;


		// 追い越して内側に入るタイプ
	case EEnemyMoveType::Overtake_Line:
	case EEnemyMoveType::Overtake_Smooth:

		// Playerが設定されていなければエラーログを表示し関数を終了
		if (playerActor == NULL)
		{
			UE_LOG(LogTemp, Error, TEXT("AEnemyChara::Tick(): playerActor is NULL. Need to set Actor."));
			return;
		}

		// プレイヤーより後ろにいる（早い直線移動）
		if (pos.X < (playerPos.X + overtakeOffset))
		{
			Move_Line(overtakeSpeed, pos);
		}
		// プレイヤーより前にいる（内側に入る）
		else if (pos.X < (playerPos.X + overtakeOffset + overtakeDistance))
		{
			// 内側に入る動きが直線的の場合
			if (enemyMoveType == EEnemyMoveType::Overtake_Line)
			{
				Move_Overtake_Line(overtakeSpeed, forwardSpeed, pos, playerPos);
			}

			// 内側に入る動きがなめらかの場合
			else if (enemyMoveType == EEnemyMoveType::Overtake_Smooth)
			{
				Move_Overtake_Lerp(overtakeSpeed, forwardSpeed, pos, playerPos);
			}
		}
		// 並走する
		else
		{
			Move_Line(forwardSpeed, pos);
		}
		break;

		// 溜めからの突進
	case EEnemyMoveType::BodyBlow:

		// Playerが設定されていなければエラーログを表示し関数を終了
		if (playerActor == NULL)
		{
			UE_LOG(LogTemp, Error, TEXT("AEnemyChara::Tick(): playerActor is NULL. Need to set Actor."));
			return;
		}

		// プレイヤーと敵の距離が距離以上離れていれば
		if (FMath::Abs(this->GetActorLocation().X - playerActor->GetActorLocation().X) > bodyBlowDistance)
		{
			Move_Line(bodyBlowMoveSpeed, pos);
		}
		// プレイヤーとの距離が一定の距離に達したら
		else if (isBlowing == false)
		{
			Move_Line(-_TEMP_playerMoveSpeed, pos);
			chargeTime += DeltaTime;

			// （経過時間 - 回転を遅らせる時間）が、設定時間を超えるまで
			if ((chargeTime - bodyBlowTurnDelay) < bodyBlowChargeTime)
			{
				// 敵とプレイヤーの位置から角度を算出
				float distanceX = playerActor->GetActorLocation().X - this->GetActorLocation().X;
				float distanceY = playerActor->GetActorLocation().Y - this->GetActorLocation().Y;
				float radAngle = atan2(distanceY, distanceX);
				float degAngle = FMath::RadiansToDegrees(radAngle);

				FRotator targetRot(originRotate.Pitch, degAngle, originRotate.Roll);

				// 回転を早める分を算出
				float shortRatio = FMath::Clamp((chargeTime - bodyBlowTurnDelay) / bodyBlowChargeTime, 0.0f, 1.0f);
				float shorten = FMath::Lerp(0.0f, bodyBlowTurnShorten, shortRatio);

				// 時間に応じて回転させる
				float rotatorRatio = FMath::Clamp((chargeTime - bodyBlowTurnDelay + shorten) / bodyBlowChargeTime, 0.0f, 1.0f);
				FRotator newRotate = FMath::Lerp(originRotate, targetRot, rotatorRatio);

				// 敵に回転量を反映
				SetActorRotation(newRotate);
			}
			else
			{
				isBlowing = true;
			}
		}
		// 突進開始
		else if (isBlowing == true)
		{
			Move_Line(bodyBlowSpeed, pos);
		}
		break;

		// タイプ指定なし
	default:

		// エラーログの表示
		UE_LOG(LogTemp, Display, TEXT("Enemy : %s is not MoveType setting."), *(this->GetName()))
			break;
	}
}

void AEnemyChara::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// 直線移動処理
void AEnemyChara::Move_Line(const float _speed, const FVector _pos)
{
	// X軸に移動量を加算し、Actorの位置を更新する
	FVector vector = GetActorForwardVector() * _speed;
	FVector nextPos((_pos.X + vector.X), (_pos.Y + vector.Y), _pos.Z);

	SetActorLocation(nextPos);
}

// 正弦波移動処理
void AEnemyChara::Move_Sin(const float _speedForward, const float _speedSide, const float _frequency, const float _deltaTime, const FVector _pos)
{
	// 経過時間を計測し、sinの値に使用する
	// sinの値をY軸に反映させ、X軸に移動量を加算し、Actorの位置を更新する
	sinTime += _deltaTime;

	FVector nextPos(_pos.X + _speedForward, (originPosY + (FMath::Sin(sinTime * _speedSide) * _frequency)), _pos.Z);

	SetActorLocation(nextPos);
}

// 追い越し・直線での移動
void AEnemyChara::Move_Overtake_Line(const float _overtakeSpeed, const float _speed, const FVector _ePos, const FVector _pPos)
{
	// 0で割ることを防ぐ
	float t = 0.5f;
	if ((overtakeOffset + overtakeDistance) > 0)
	{
		// 現在の位置が、目的地点に対してどのぐらいの割合か計算
		t = (_ePos.X - _pPos.X - overtakeOffset) / overtakeDistance;
	}

	// 移動速度・Y軸の座標をX軸の移動量に対してちょっとずつ変移させる
	float nextPosY = FMath::Lerp(originPosY, (originPosY + overtakeYAxisMove), t);
	float speed = FMath::Lerp(_overtakeSpeed, _speed, t);

	// X座標に移動量を加算し、保存したY座標を適用
	FVector nextPos(_ePos.X + speed, nextPosY, _ePos.Z);

	// 位置の更新
	SetActorLocation(nextPos);
}

// 追い越し・なめらかな移動
void AEnemyChara::Move_Overtake_Lerp(const float _overtakeSpeed, const float _speed, const FVector _ePos, const FVector _pPos)
{
	// 移動速度
	float speed = 0.0f;

	// Y軸の移動後の座標
	float nextPosY = 0.0f;

	// 移動後の座標
	FVector nextPos = FVector::ZeroVector;

	// なめらかに移動するための開始点、目標点、2つの中間点の保存用
	FVector point[3];

	// 内側に入り切るための距離（overtakeDistance）の半分より前にいれば
	if (_ePos.X < (_pPos.X + overtakeOffset + (overtakeDistance / 2.0f)))
	{
		// 3点の保存をする
		point[0] = FVector(_pPos.X + overtakeOffset, originPosY, _ePos.Z);
		point[1] = FVector(_pPos.X + overtakeOffset + (overtakeDistance / 2.0f), originPosY, _ePos.Z);
		point[2] = FVector(_pPos.X + overtakeOffset + (overtakeDistance / 2.0f), originPosY + (overtakeYAxisMove / 2.0f), _ePos.Z);

		// 現在の位置が、目的地点に対してどのぐらいの割合か計算
		float t = (_ePos.X - _pPos.X - overtakeOffset) / (overtakeDistance / 2.0f);

		// なめらかに移動した時の座標の、Y座標のみを保存
		CalcLerpPos(point[0], point[1], point[2], t, NULL, &nextPosY, NULL);

		// 移動速度の補間
		speed = FMath::Lerp(_overtakeSpeed, _speed, t);

		// X座標に移動量を加算し、保存したY座標を適用
		nextPos = FVector(_ePos.X + speed, nextPosY, _ePos.Z);
	}
	else
	{
		// 3点の保存
		point[0] = FVector(_pPos.X + overtakeOffset + (overtakeDistance / 2.0f), originPosY + (overtakeYAxisMove / 2.0f), _ePos.Z);
		point[1] = FVector(_pPos.X + overtakeOffset + (overtakeDistance / 2.0f), originPosY + overtakeYAxisMove, _ePos.Z);
		point[2] = FVector(_pPos.X + overtakeOffset + overtakeDistance, originPosY + overtakeYAxisMove, _ePos.Z);

		// 現在の位置が、目的地点に対してどのぐらいの割合か計算
		float t = (_ePos.X - _pPos.X - overtakeOffset - (overtakeDistance / 2.0f)) / (overtakeDistance / 2.0f);

		// なめらかに移動した時の座標の、Y座標のみを保存
		CalcLerpPos(point[0], point[1], point[2], t, NULL, &nextPosY, NULL);

		// 移動速度の補間
		speed = FMath::Lerp(_overtakeSpeed, _speed, (t * 2.0f));

		// X座標に移動量を加算し、保存したY座標を適用
		nextPos = FVector(_ePos.X + speed, nextPosY, _ePos.Z);
	}

	// 位置の更新
	SetActorLocation(nextPos);
}

// なめらかな移動の際に使う処理（3点の座標からなめらかな曲線を描く）
FVector AEnemyChara::CalcLerpPos(const FVector _startPos, const FVector _midPos, const FVector _endPos, const float _interpolation
	, float* _posX /* = NULL */, float* _posY /* = NULL */, float* _posZ /* = NULL */)
{
	FVector lerpPos[3];

	// 補間の値を0未満 かつ 1より大きい値にしない
	float t = FMath::Clamp(_interpolation, 0.0f, 1.0f);

	// 開始点から中間点（a）、中間点から終了点（b）を線補間した値を保存
	lerpPos[0] = FMath::Lerp(_startPos, _midPos, _interpolation);
	lerpPos[1] = FMath::Lerp(_midPos, _endPos, _interpolation);

	// 上記a, bを線補間した値を保存（なめらかに移動する際の座標になる）
	lerpPos[2] = FMath::Lerp(lerpPos[0], lerpPos[1], _interpolation);

	// それぞれのポインターにアドレスが格納されていれば対応した値を代入
	if (_posX != NULL)
	{
		*_posX = lerpPos[2].X;
	}
	if (_posY != NULL)
	{
		*_posY = lerpPos[2].Y;
	}
	if (_posZ != NULL)
	{
		*_posZ = lerpPos[2].Z;
	}

	return lerpPos[2];
}

