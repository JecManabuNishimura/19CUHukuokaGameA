#include "EnemyChara.h"
#include "DrawDebugHelpers.h"

AEnemyChara::AEnemyChara()
	: sinTime(0.0f)
	, originPosY(0.0f)
	, enemyType(EEnemyMoveType::None)
	, playerActor(NULL)
	, forwardSpeed(1.0f)
	, sinWaveSpeed(1.0f)
	, sinWaveFrequency(1.0f)
	, overtakeSpeed(2.0f)
	, overtakeOffset(0.0f)
	, overtakeDistance(300.0f)
	, overtakeYAxisMove(-100.0f)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyChara::BeginPlay()
{
	Super::BeginPlay();

	// 開始時点のY座標を保存
	originPosY = GetActorLocation().Y; 
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
	switch (enemyType)
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
			if (enemyType == EEnemyMoveType::Overtake_Line)
			{
				Move_Overtake_Line(overtakeSpeed, forwardSpeed, pos, playerPos);
			}

			// 内側に入る動きがなめらかの場合
			else if (enemyType == EEnemyMoveType::Overtake_Smooth)
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

	// タイプ指定なし
	default:
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
	FVector nextPos((_pos.X + _speed), _pos.Y, _pos.Z);

	SetActorLocation(nextPos);
}

// 正弦波移動処理
void AEnemyChara::Move_Sin(const float _speedForward, const float _speedSide, const float _frequency, const float _deltaTime, const FVector _pos)
{
	// 経過時間を計測し、sinの値に使用する
	// sinの値をY軸に反映させ、X軸に移動量を加算し、Actorの位置を更新する
	sinTime += _deltaTime;

	FVector nextPos(_pos.X + _speedForward, ((_pos.Y + (FMath::Sin(sinTime * _speedSide) * _frequency)) / 2.0f), _pos.Z);

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
		point[0] = FVector(_pPos.X + overtakeOffset								, originPosY								, _ePos.Z);
		point[1] = FVector(_pPos.X + overtakeOffset + (overtakeDistance / 2.0f) , originPosY								, _ePos.Z);
		point[2] = FVector(_pPos.X + overtakeOffset + (overtakeDistance / 2.0f) , originPosY + (overtakeYAxisMove / 2.0f)	, _ePos.Z);

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
		point[0] = FVector(_pPos.X + overtakeOffset + (overtakeDistance / 2.0f) , originPosY + (overtakeYAxisMove / 2.0f)	, _ePos.Z);
		point[1] = FVector(_pPos.X + overtakeOffset + (overtakeDistance / 2.0f) , originPosY + overtakeYAxisMove			, _ePos.Z);
		point[2] = FVector(_pPos.X + overtakeOffset + overtakeDistance			, originPosY + overtakeYAxisMove			, _ePos.Z);

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
	lerpPos[0] = FMath::Lerp(_startPos,	 _midPos,	 _interpolation);
	lerpPos[1] = FMath::Lerp(_midPos,	 _endPos,	 _interpolation);

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

