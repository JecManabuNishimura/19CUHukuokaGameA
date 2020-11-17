//----------------------------------------------------------
// �t�@�C����		�FSensorTest.h
// �T�v				�F�G�̑�����s��
// �쐬��			�F19CU0238 �n粗���
//
// �X�V���e			�F2020/09/13 �n粗��� �쐬
//					�F2020/09/19 �n粗��� ���߂̂���G�̓����̒ǉ�
//					�F			 �����ړ���ForwardVector�ōs��
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

	// �J�n���_��Y���W��ۑ�
	originPosY = GetActorLocation().Y;

	// �J�n���_�̉�]�ʂ�ۑ�
	originRotate = GetActorRotation();
}

void AEnemyChara::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ����Actor�̈ʒu�̍X�V
	FVector pos = GetActorLocation();

	// Player�Ƃ��Đݒ肵��Actor�̈ʒu�̍X�V�i�ݒ肵���ꍇ�̂݁j
	FVector playerPos = FVector::ZeroVector;
	if (playerActor != NULL)
	{
		playerPos = playerActor->GetActorLocation();
	}

	// �ړ��̎�ނɉ����ĕ���
	switch (enemyMoveType)
	{
		// �����ړ��^�C�v
	case EEnemyMoveType::Line:

		Move_Line(forwardSpeed, pos);
		break;


		// �����g�ړ��^�C�v
	case EEnemyMoveType::Sin:

		Move_Sin(forwardSpeed, sinWaveSpeed, sinWaveFrequency, DeltaTime, pos);
		break;


		// �ǂ��z���ē����ɓ���^�C�v
	case EEnemyMoveType::Overtake_Line:
	case EEnemyMoveType::Overtake_Smooth:

		// Player���ݒ肳��Ă��Ȃ���΃G���[���O��\�����֐����I��
		if (playerActor == NULL)
		{
			UE_LOG(LogTemp, Error, TEXT("AEnemyChara::Tick(): playerActor is NULL. Need to set Actor."));
			return;
		}

		// �v���C���[�����ɂ���i���������ړ��j
		if (pos.X < (playerPos.X + overtakeOffset))
		{
			Move_Line(overtakeSpeed, pos);
		}
		// �v���C���[���O�ɂ���i�����ɓ���j
		else if (pos.X < (playerPos.X + overtakeOffset + overtakeDistance))
		{
			// �����ɓ��铮���������I�̏ꍇ
			if (enemyMoveType == EEnemyMoveType::Overtake_Line)
			{
				Move_Overtake_Line(overtakeSpeed, forwardSpeed, pos, playerPos);
			}

			// �����ɓ��铮�����Ȃ߂炩�̏ꍇ
			else if (enemyMoveType == EEnemyMoveType::Overtake_Smooth)
			{
				Move_Overtake_Lerp(overtakeSpeed, forwardSpeed, pos, playerPos);
			}
		}
		// ��������
		else
		{
			Move_Line(forwardSpeed, pos);
		}
		break;

		// ���߂���̓ːi
	case EEnemyMoveType::BodyBlow:

		// Player���ݒ肳��Ă��Ȃ���΃G���[���O��\�����֐����I��
		if (playerActor == NULL)
		{
			UE_LOG(LogTemp, Error, TEXT("AEnemyChara::Tick(): playerActor is NULL. Need to set Actor."));
			return;
		}

		// �v���C���[�ƓG�̋����������ȏ㗣��Ă����
		if (FMath::Abs(this->GetActorLocation().X - playerActor->GetActorLocation().X) > bodyBlowDistance)
		{
			Move_Line(bodyBlowMoveSpeed, pos);
		}
		// �v���C���[�Ƃ̋��������̋����ɒB������
		else if (isBlowing == false)
		{
			Move_Line(-_TEMP_playerMoveSpeed, pos);
			chargeTime += DeltaTime;

			// �i�o�ߎ��� - ��]��x�点�鎞�ԁj���A�ݒ莞�Ԃ𒴂���܂�
			if ((chargeTime - bodyBlowTurnDelay) < bodyBlowChargeTime)
			{
				// �G�ƃv���C���[�̈ʒu����p�x���Z�o
				float distanceX = playerActor->GetActorLocation().X - this->GetActorLocation().X;
				float distanceY = playerActor->GetActorLocation().Y - this->GetActorLocation().Y;
				float radAngle = atan2(distanceY, distanceX);
				float degAngle = FMath::RadiansToDegrees(radAngle);

				FRotator targetRot(originRotate.Pitch, degAngle, originRotate.Roll);

				// ��]�𑁂߂镪���Z�o
				float shortRatio = FMath::Clamp((chargeTime - bodyBlowTurnDelay) / bodyBlowChargeTime, 0.0f, 1.0f);
				float shorten = FMath::Lerp(0.0f, bodyBlowTurnShorten, shortRatio);

				// ���Ԃɉ����ĉ�]������
				float rotatorRatio = FMath::Clamp((chargeTime - bodyBlowTurnDelay + shorten) / bodyBlowChargeTime, 0.0f, 1.0f);
				FRotator newRotate = FMath::Lerp(originRotate, targetRot, rotatorRatio);

				// �G�ɉ�]�ʂ𔽉f
				SetActorRotation(newRotate);
			}
			else
			{
				isBlowing = true;
			}
		}
		// �ːi�J�n
		else if (isBlowing == true)
		{
			Move_Line(bodyBlowSpeed, pos);
		}
		break;

		// �^�C�v�w��Ȃ�
	default:

		// �G���[���O�̕\��
		UE_LOG(LogTemp, Display, TEXT("Enemy : %s is not MoveType setting."), *(this->GetName()))
			break;
	}
}

void AEnemyChara::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// �����ړ�����
void AEnemyChara::Move_Line(const float _speed, const FVector _pos)
{
	// X���Ɉړ��ʂ����Z���AActor�̈ʒu���X�V����
	FVector vector = GetActorForwardVector() * _speed;
	FVector nextPos((_pos.X + vector.X), (_pos.Y + vector.Y), _pos.Z);

	SetActorLocation(nextPos);
}

// �����g�ړ�����
void AEnemyChara::Move_Sin(const float _speedForward, const float _speedSide, const float _frequency, const float _deltaTime, const FVector _pos)
{
	// �o�ߎ��Ԃ��v�����Asin�̒l�Ɏg�p����
	// sin�̒l��Y���ɔ��f�����AX���Ɉړ��ʂ����Z���AActor�̈ʒu���X�V����
	sinTime += _deltaTime;

	FVector nextPos(_pos.X + _speedForward, (originPosY + (FMath::Sin(sinTime * _speedSide) * _frequency)), _pos.Z);

	SetActorLocation(nextPos);
}

// �ǂ��z���E�����ł̈ړ�
void AEnemyChara::Move_Overtake_Line(const float _overtakeSpeed, const float _speed, const FVector _ePos, const FVector _pPos)
{
	// 0�Ŋ��邱�Ƃ�h��
	float t = 0.5f;
	if ((overtakeOffset + overtakeDistance) > 0)
	{
		// ���݂̈ʒu���A�ړI�n�_�ɑ΂��Ăǂ̂��炢�̊������v�Z
		t = (_ePos.X - _pPos.X - overtakeOffset) / overtakeDistance;
	}

	// �ړ����x�EY���̍��W��X���̈ړ��ʂɑ΂��Ă�����Ƃ��ψڂ�����
	float nextPosY = FMath::Lerp(originPosY, (originPosY + overtakeYAxisMove), t);
	float speed = FMath::Lerp(_overtakeSpeed, _speed, t);

	// X���W�Ɉړ��ʂ����Z���A�ۑ�����Y���W��K�p
	FVector nextPos(_ePos.X + speed, nextPosY, _ePos.Z);

	// �ʒu�̍X�V
	SetActorLocation(nextPos);
}

// �ǂ��z���E�Ȃ߂炩�Ȉړ�
void AEnemyChara::Move_Overtake_Lerp(const float _overtakeSpeed, const float _speed, const FVector _ePos, const FVector _pPos)
{
	// �ړ����x
	float speed = 0.0f;

	// Y���̈ړ���̍��W
	float nextPosY = 0.0f;

	// �ړ���̍��W
	FVector nextPos = FVector::ZeroVector;

	// �Ȃ߂炩�Ɉړ����邽�߂̊J�n�_�A�ڕW�_�A2�̒��ԓ_�̕ۑ��p
	FVector point[3];

	// �����ɓ���؂邽�߂̋����iovertakeDistance�j�̔������O�ɂ����
	if (_ePos.X < (_pPos.X + overtakeOffset + (overtakeDistance / 2.0f)))
	{
		// 3�_�̕ۑ�������
		point[0] = FVector(_pPos.X + overtakeOffset, originPosY, _ePos.Z);
		point[1] = FVector(_pPos.X + overtakeOffset + (overtakeDistance / 2.0f), originPosY, _ePos.Z);
		point[2] = FVector(_pPos.X + overtakeOffset + (overtakeDistance / 2.0f), originPosY + (overtakeYAxisMove / 2.0f), _ePos.Z);

		// ���݂̈ʒu���A�ړI�n�_�ɑ΂��Ăǂ̂��炢�̊������v�Z
		float t = (_ePos.X - _pPos.X - overtakeOffset) / (overtakeDistance / 2.0f);

		// �Ȃ߂炩�Ɉړ��������̍��W�́AY���W�݂̂�ۑ�
		CalcLerpPos(point[0], point[1], point[2], t, NULL, &nextPosY, NULL);

		// �ړ����x�̕��
		speed = FMath::Lerp(_overtakeSpeed, _speed, t);

		// X���W�Ɉړ��ʂ����Z���A�ۑ�����Y���W��K�p
		nextPos = FVector(_ePos.X + speed, nextPosY, _ePos.Z);
	}
	else
	{
		// 3�_�̕ۑ�
		point[0] = FVector(_pPos.X + overtakeOffset + (overtakeDistance / 2.0f), originPosY + (overtakeYAxisMove / 2.0f), _ePos.Z);
		point[1] = FVector(_pPos.X + overtakeOffset + (overtakeDistance / 2.0f), originPosY + overtakeYAxisMove, _ePos.Z);
		point[2] = FVector(_pPos.X + overtakeOffset + overtakeDistance, originPosY + overtakeYAxisMove, _ePos.Z);

		// ���݂̈ʒu���A�ړI�n�_�ɑ΂��Ăǂ̂��炢�̊������v�Z
		float t = (_ePos.X - _pPos.X - overtakeOffset - (overtakeDistance / 2.0f)) / (overtakeDistance / 2.0f);

		// �Ȃ߂炩�Ɉړ��������̍��W�́AY���W�݂̂�ۑ�
		CalcLerpPos(point[0], point[1], point[2], t, NULL, &nextPosY, NULL);

		// �ړ����x�̕��
		speed = FMath::Lerp(_overtakeSpeed, _speed, (t * 2.0f));

		// X���W�Ɉړ��ʂ����Z���A�ۑ�����Y���W��K�p
		nextPos = FVector(_ePos.X + speed, nextPosY, _ePos.Z);
	}

	// �ʒu�̍X�V
	SetActorLocation(nextPos);
}

// �Ȃ߂炩�Ȉړ��̍ۂɎg�������i3�_�̍��W����Ȃ߂炩�ȋȐ���`���j
FVector AEnemyChara::CalcLerpPos(const FVector _startPos, const FVector _midPos, const FVector _endPos, const float _interpolation
	, float* _posX /* = NULL */, float* _posY /* = NULL */, float* _posZ /* = NULL */)
{
	FVector lerpPos[3];

	// ��Ԃ̒l��0���� ���� 1���傫���l�ɂ��Ȃ�
	float t = FMath::Clamp(_interpolation, 0.0f, 1.0f);

	// �J�n�_���璆�ԓ_�ia�j�A���ԓ_����I���_�ib�j�����Ԃ����l��ۑ�
	lerpPos[0] = FMath::Lerp(_startPos, _midPos, _interpolation);
	lerpPos[1] = FMath::Lerp(_midPos, _endPos, _interpolation);

	// ��La, b�����Ԃ����l��ۑ��i�Ȃ߂炩�Ɉړ�����ۂ̍��W�ɂȂ�j
	lerpPos[2] = FMath::Lerp(lerpPos[0], lerpPos[1], _interpolation);

	// ���ꂼ��̃|�C���^�[�ɃA�h���X���i�[����Ă���ΑΉ������l����
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

