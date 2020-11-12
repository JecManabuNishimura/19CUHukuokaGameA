/*
 *--------------------------------------------------------
 * �{�X�̈ړ�
 * 2020/11/08 �n粗��� �쐬
 *--------------------------------------------------------
 */

#include "BossControl.h"
 // GetWorld() �ɕK�v
#include "Engine/World.h"
 // ConstructorHelpers::FObjectFinder �ɕK�v
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

	// �U���}�[�J�[�̎擾�E�ݒ�
	ConstructorHelpers::FObjectFinder<UClass> rightningStrikeMarker(TEXT("/Game/BP/RedCircleFace.RedCircleFace_C"));
	m_LightningStrikeMarker = rightningStrikeMarker.Object;
}

void ABossControl::BeginPlay()
{
	Super::BeginPlay();

	// ���Ƃ��Ƃ̍��W��ۑ�
	originPosY = GetActorLocation().Y;

	// �X�e�[�W�O����̈ړ��̉�]�ʂ��s���̏ꍇ�ɋ����I�ɕ␳

	// �E���ɂ���Ƃ��ɉE�ɉ�]����悤�ɂȂ��Ă�����
	if (m_JumpDegreeZ > 0.0f && originPosY > 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("BossBP's Location.Y is more than 0. (Right Side) But it is set to turn right. So fix to turn left."));
		m_JumpDegreeZ *= -1.0f;
	}
	// �����ɂ���Ƃ��ɍ��ɉ�]����悤�ɂȂ��Ă�����
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
		// �����Ă�����
	case BossState::Run:

		UE_LOG(LogTemp, Verbose, TEXT("BossEnemy state is Running."));
		// �U���ڍs���ԂɂȂ�����
		if (m_Timer >= m_InsideMigrationTime)
		{
			// �W�����v��Ԃ�
			bossState = BossState::Jump_In;

			FRotator rot(GetActorRotation().Pitch, m_JumpDegreeZ, GetActorRotation().Roll);
			SetActorRotation(rot);

			originPosY = GetActorLocation().Y;

			// �^�C�}�[���Z�b�g
			m_Timer = 0.0f;
		}
		// ��]���n�߂鎞�ԂɂȂ�����
		else if (m_Timer >= m_InsideMigrationTime * m_RotateStateRatio)
		{
			SetActorRotationLerpDegree(0.0f, m_JumpDegreeZ, (m_Timer - m_InsideMigrationTime * m_RotateStateRatio), (m_InsideMigrationTime - m_InsideMigrationTime * m_RotateStateRatio));

			MoveForward(m_MoveSpeed, GetActorLocation());
		}
		// ���鏈��
		else
		{
			SetActorRotation(FRotator::ZeroRotator);
			MoveForward(m_MoveSpeed, GetActorLocation());
		}
		break;

		// �W�����v���Ē��ɓ�����
	case BossState::Jump_In:
		UE_LOG(LogTemp, Verbose, TEXT("BossEnemy state is Jump (in)."));

		// �W�����v���Ԃ��I��������
		if (m_Timer >= m_JumpTime)
		{
			// �U����Ԃ�
			bossState = BossState::Attack;
			SetActorRotation(FRotator::ZeroRotator);

			// �U���̎�ނ����߂�i���͈��ނȂ̂ŌŒ�j
			bossAttack = BossAttack::LightningStrike;

			// �^�C�}�[���Z�b�g
			m_Timer = 0.0f;
		}
		// ��]���n�߂鎞�ԂɂȂ�����
		else if (m_Timer >= m_JumpTime * m_RotateStateRatio)
		{
			SetActorRotationLerpDegree(m_JumpDegreeZ, 0.0f, (m_Timer - m_JumpTime * m_RotateStateRatio), (m_JumpTime - m_JumpTime * m_RotateStateRatio));

			float nextPosY = FMath::Lerp(originPosY, 0.0f, m_Timer / m_JumpTime);

			// �ړ�����
			FVector tempPos(GetActorLocation().X + m_MoveSpeed, nextPosY, GetActorLocation().Z);
			SetActorLocation(tempPos);
		}
		// �W�����v����
		else
		{
			float nextPosY = FMath::Lerp(originPosY, 0.0f, m_Timer / m_JumpTime);

			// �ړ�����
			FVector tempPos(GetActorLocation().X + m_MoveSpeed, nextPosY, GetActorLocation().Z);
			SetActorLocation(tempPos);
		}
		break;

		// �U�����
	case BossState::Attack:
		UE_LOG(LogTemp, Verbose, TEXT("BossEnemy state is Attacking."));

		// �U���̎�ނɂ���ĕ���
		switch (bossAttack)
		{
			// �����U��
		case BossAttack::LightningStrike:

			// �U�����Ԃ��I��������
			if (m_Timer >= m_LightningStrikeDuration)
			{
				bossState = BossState::RunAfterAttack;

				// �^�C�}�[���Z�b�g
				m_LightningStrikeIntervalTemp = 0.0f;
				m_Timer = 0.0f;
			}
			// �U���E�ړ�����
			else
			{
				MoveForward(m_LightningStrikeMoveSpeed, GetActorLocation());

				// �U�����ԂɂȂ��Ă�����
				if (m_Timer >= m_LightningStrikeIntervalTemp)
				{
					// �U���Ԋu�𑝉�
					m_LightningStrikeIntervalTemp += m_LightningStrikeInterval;

					// �U�������ʒu������
					float attackYPos = FMath::RandRange(-m_LightningStrikeWidth, m_LightningStrikeWidth);
					FVector attackPos(GetActorLocation().X, attackYPos, GetActorLocation().Z);

					// �����p�����[�^�̐ݒ�
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

					// �U���}�[�J�[�i�U�����s���j�̐���
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

			// ���̑��̍U����i
		default:
			break;
		}
		break;

		// �U����̈ړ�
	case BossState::RunAfterAttack:
		UE_LOG(LogTemp, Verbose, TEXT("BossEnemy state is Runnning (after Attack)."));

		// �W�����v�ڍs���ԂɂȂ�����
		if (m_Timer >= m_RunningTimeAfterAttack)
		{
			// �W�����v��Ԃ�
			bossState = BossState::Jump_Out;

			FRotator rot(GetActorRotation().Pitch, -m_JumpDegreeZ, GetActorRotation().Roll);
			SetActorRotation(rot);

			// �^�C�}�[���Z�b�g
			m_Timer = 0.0f;
		}
		// ��]���n�߂鎞�ԂɂȂ�����
		else if (m_Timer >= m_RunningTimeAfterAttack * m_RotateStateRatio)
		{
			SetActorRotationLerpDegree(0.0f, -m_JumpDegreeZ, (m_Timer - m_RunningTimeAfterAttack * m_RotateStateRatio), (m_RunningTimeAfterAttack - m_RunningTimeAfterAttack * m_RotateStateRatio));

			MoveForward(m_MoveSpeed, GetActorLocation());
		}
		// ���鏈��
		else
		{
			SetActorRotation(FRotator::ZeroRotator);
			MoveForward(m_MoveSpeed, GetActorLocation());
		}
		break;

		// �W�����v�ŊO�ɍs��
	case BossState::Jump_Out:
		UE_LOG(LogTemp, Verbose, TEXT("BossEnemy state is Jump (out)."));

		// �W�����v���Ԃ��I��������
		if (m_Timer >= m_JumpTime)
		{
			// �U����Ԃ�
			bossState = BossState::Attack;
			SetActorRotation(FRotator::ZeroRotator);

			// �U���̎�ނ����߂�i���͈��ނȂ̂ŌŒ�j
			bossAttack = BossAttack::LightningStrike;

			// �^�C�}�[���Z�b�g
			m_Timer = 0.0f;
		}
		// ��]���n�߂鎞�ԂɂȂ�����
		else if (m_Timer >= m_JumpTime * m_RotateStateRatio)
		{
			SetActorRotationLerpDegree(m_JumpDegreeZ, 0.0f, (m_Timer - m_JumpTime * m_RotateStateRatio), (m_JumpTime - m_JumpTime * m_RotateStateRatio));

			float nextPosY = FMath::Lerp(originPosY, 0.0f, m_Timer / m_JumpTime);

			// �ړ�����
			FVector tempPos(GetActorLocation().X + m_MoveSpeed, nextPosY, GetActorLocation().Z);
			SetActorLocation(tempPos);
		}
		// �W�����v����
		else
		{
			float nextPosY = FMath::Lerp(originPosY, 0.0f, m_Timer / m_JumpTime);

			// �ړ�����
			FVector tempPos(GetActorLocation().X + m_MoveSpeed, nextPosY, GetActorLocation().Z);
			SetActorLocation(tempPos);
		}
		break;

		// ���̑��̏��
	default:
		UE_LOG(LogTemp, Warning, TEXT("BossEnemy state is Unknown. bossState index = %d."), (int)bossState);
		break;
	}

	// �^�C�}�[�J�E���g
	m_Timer += DeltaTime;
}

void ABossControl::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// 2�_��Vector���W����p�x���Z�o
float ABossControl::GetRadianFromVector(const FVector& posA, const FVector& posB)
{
	float rad = FMath::Atan2(posB.Y - posA.Y, posB.X - posA.X);
	return rad;
}

// �p�x���Ԃ��ĕύX
void ABossControl::SetActorRotationLerpDegree(const float startDeg, const float targetDeg, const float& dividend, const float& divisor)
{
	float interpolation = FMath::Lerp(0.0f, 1.0f, dividend / divisor);
	float deg = FMath::Lerp(startDeg, targetDeg, interpolation);

	FRotator rot(0.0f, deg, 0.0f);
	SetActorRotation(rot);
}

// �W�����v��Ԃ��ǂ���
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

// �W�����v��Ԃ��ǂ���
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

// �W�����v���Ԃ̎擾
float ABossControl::GetJumpTime() const
{
	return m_JumpTime;
}

// �����ړ�����
void ABossControl::MoveForward(const float& _speed, const FVector& _pos)
{
	// X���Ɉړ��ʂ����Z���AActor�̈ʒu���X�V����
	FVector vector = GetActorForwardVector() * _speed;
	FVector nextPos((_pos.X + vector.X), (_pos.Y + vector.Y), _pos.Z);

	SetActorLocation(nextPos);
}