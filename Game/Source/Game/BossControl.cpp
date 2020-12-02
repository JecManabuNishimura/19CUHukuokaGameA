/*
 *--------------------------------------------------------
 * �{�X�̈ړ�
 * 2020/11/08 �n粗��� �쐬
 *--------------------------------------------------------
 */

#include "BossControl.h"
#include "PlayerChara.h"
#include "BossWaveControl.h"
 // GetWorld() �ɕK�v
#include "Engine/World.h"
 // ConstructorHelpers::FObjectFinder �ɕK�v
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "DrawDebugHelpers.h"

ABossControl::ABossControl()
// �S�̂̃v���p�e�B
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
	, m_BeforeCatchUpPosX(0.0f)
	, bossState(BossState::Run)
	, bossAttack(BossAttack::LightningStrike)

	// �����U���v���p�e�B
	, m_LightningStrikeMoveSpeed(75.0f)
	, m_LightningStrikeDuration(5.0f)
	, m_LightningStrikeWidth(2500.0f)
	, m_LightningStrikeInterval(2.0f)
	, m_LightningStrikeMarker(nullptr)

	// �E�F�[�u�U���v���p�e�B
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

	// �ːi�U���v���p�e�B
	, m_RushAttackMoveSpeed(200.0f)
	, m_RushAttackSpeed(200.0f)
	, m_RushReachTimeTopSpeed(3.0f)
	, m_RushAttackDuration(3.0f)
	, m_RushAttackRunningRoadPosY(500.0f)
	, m_RushTurnTime(0.5f)
	, m_IsHeadAnimEnd(false)
	, m_IsRush(false)
	, m_IsLookPlayer(false)
	, m_GetPlayerCoordinatePosX(5000.0f)
	, m_RushRunStartPos(FVector::ZeroVector)
	, m_RushStartPosX(0.0f)
	, m_RushThroughDistance(5000.0f)
	, m_CatchUpTime(5.0f)
	, m_TargetRot(180.0f)

	// BP�p�v���p�e�B
	, m_PlayerActor(nullptr)

	//TEMP////////////////////////////////////////////////////////////
	, m_TEMP_BOSS_ATTACK(BossAttack::LightningStrike)
	, m_TEMP_FENCE_POS(0.0f)
	//////////////////////////////////////////////////////////////////
{
	PrimaryActorTick.bCanEverTick = true;

	// �U���}�[�J�[�̎擾�E�ݒ�
	ConstructorHelpers::FObjectFinder<UClass> rightningStrikeMarker(TEXT("/Game/BP/RedCircleFace.RedCircleFace_C"));
	m_LightningStrikeMarker = rightningStrikeMarker.Object;

	// �E�F�[�u�U���̎擾�E�ݒ�
	ConstructorHelpers::FObjectFinder<UClass> waveAttack(TEXT("/Game/BP/BossWaveBP.BossWaveBP_C"));
	m_WaveAttackActor = waveAttack.Object;

	// �W�����v��̎擾�E�ݒ�
	ConstructorHelpers::FObjectFinder<UClass> jumppadActor(TEXT("/Game/BP/JumpPad.JumpPad_C"));
	m_JumppadActor = jumppadActor.Object;

	// �g��U���̉񐔂��Z�o
	m_WaveAttackCount = FMath::FloorToInt(m_WaveAttackDuration / m_WaveAttackInterval);
	UE_LOG(LogTemp, Verbose, TEXT("WaveAttackCount = %d"), m_WaveAttackCount);

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
		UE_LOG(LogTemp, Warning, TEXT("BossBP's Location.Y is more than 0. (Right Side.). But it is set to turn right. (m_JumpDegreeZ is more than 0.) So fix to turn left."));
		m_JumpDegreeZ *= -1.0f;
	}
	// �����ɂ���Ƃ��ɍ��ɉ�]����悤�ɂȂ��Ă�����
	else if (m_JumpDegreeZ < 0.0f && originPosY < 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("BossBP's Location.Y is less than 0. (Left Side.) But it is set to turn left. (m_JumpDegreeZ is less than 0.) So fix to turn right."));
		m_JumpDegreeZ *= -1.0f;
	}

	// m_PlayerActor��GameInstance��playerActor����
	GetInstance();	
}

void ABossControl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (bossState)
	{
		// �v���C���[�ɒǂ���
	case BossState::CatchUp:

		UE_LOG(LogTemp, Verbose, TEXT("BossEnemy state is Catch up."));

		// �v���C���[�ƈ�莞�Ԃŕ�������
		if (m_PlayerActor != nullptr && GetActorLocation().X < m_PlayerActor->GetActorLocation().X)
		{
			float newX = FMath::Lerp(m_BeforeCatchUpPosX, m_PlayerActor->GetActorLocation().X, (m_Timer / m_CatchUpTime));
			FVector newPos(newX, GetActorLocation().Y, GetActorLocation().Z);

			SetActorLocation(newPos);
		}
		// �ʏ�ړ��Ɉڍs
		else
		{
			// �����Ԃ�
			bossState = BossState::Run;

			// �^�C�}�[���Z�b�g
			m_Timer = 0.0f;
		}
		break;

		// �O�𑖂��Ă�����
	case BossState::Run:

		UE_LOG(LogTemp, Verbose, TEXT("BossEnemy state is Running."));
		// �W�����v�ڍs���ԂɂȂ�����
		if (m_Timer >= m_InsideMigrationTime)
		{
			// �W�����v��Ԃ�
			bossState = BossState::Jump_In;

			// �W�����v�J�n�ʒu��ۑ�
			m_JumpLocationY = GetActorLocation().Y;

			// �W�����v�̒��n�_��Y����ݒ�
			m_RunningRoadPosYTemp = m_RunningRoadPosY;

			// �U���̎�ނ����߂Ă����i���͏ڍ׃^�u�Ŏw�肵���U���@�݂̂ɌŒ�j
			bossAttack = m_TEMP_BOSS_ATTACK;

			// �U�����E�F�[�u�U���ł����
			if (bossAttack == BossAttack::Wave)
			{
				// ���炩���ߍU���̊Ԋu���߂Ă���
				m_attackIntervalTemp = m_WaveAttackInterval;
			}
			// �U�����ːi�U���ł����
			else if (bossAttack == BossAttack::Rush)
			{
				// �ʒu��ۑ�����
				m_RushRunStartPos = GetActorLocation();

				// �W�����v�̒��n�_���㏑��
				m_RunningRoadPosYTemp = m_RushAttackRunningRoadPosY;

				// �ːi�t���O�����낷
				m_IsRush = false;
			}

			FRotator rot(GetActorRotation().Pitch, m_JumpDegreeZ, GetActorRotation().Roll);
			SetActorRotation(rot);

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

			// �^�C�}�[���Z�b�g
			m_Timer = 0.0f;
		}
		// ��]���n�߂鎞�ԂɂȂ�����
		else if (m_Timer >= m_JumpTime * m_RotateStateRatio)
		{
			SetActorRotationLerpDegree(m_JumpDegreeZ, 0.0f, (m_Timer - m_JumpTime * m_RotateStateRatio), (m_JumpTime - m_JumpTime * m_RotateStateRatio));

			float nextPosY = FMath::Lerp(m_JumpLocationY, m_RunningRoadPosYTemp, m_Timer / m_JumpTime);

			// �ړ�����
			FVector tempPos(GetActorLocation().X + m_MoveSpeed, nextPosY, GetActorLocation().Z);
			SetActorLocation(tempPos);
		}
		// �W�����v����
		else
		{
			float nextPosY = FMath::Lerp(m_JumpLocationY, m_RunningRoadPosYTemp, m_Timer / m_JumpTime);

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
				m_attackIntervalTemp = 0.0f;
				m_Timer = 0.0f;
			}
			// �U���E�ړ�����
			else
			{
				MoveForward(m_LightningStrikeMoveSpeed, GetActorLocation());

				// �U�����ԂɂȂ��Ă�����
				if (m_Timer >= m_attackIntervalTemp)
				{
					// �U���Ԋu�𑝉�
					m_attackIntervalTemp += m_LightningStrikeInterval;

					// �U�������ʒu������
					float attackYPos = FMath::RandRange(-m_LightningStrikeWidth, m_LightningStrikeWidth);
					FVector attackPos(GetActorLocation().X, attackYPos, GetActorLocation().Z);

					// ����Actor�̐���
					SpawnAttackActor(m_LightningStrikeMarker, attackPos);

					UE_LOG(LogTemp, Verbose, TEXT("BossEnemy is Attacked! (LightningStrike)"));
				}
			}
			break;

			// �E�F�[�u�U��
		case BossAttack::Wave:

			// �U�����Ԃ��I��������
			if (m_Timer >= m_WaveAttackDuration)
			{
				bossState = BossState::RunAfterAttack;

				// �W�����v�䐶���񐔂����Z�b�g
				m_JumppadGenerateCount = 0;

				// �^�C�}�[���Z�b�g
				m_attackIntervalTemp = 0.0f;
				m_Timer = 0.0f;
			}
			// �U���E�ړ�����
			else
			{
				MoveForward(m_LightningStrikeMoveSpeed, GetActorLocation());

				// �W�����v�䐶���\���W�����v�䐶���̎��ԂŁA�Ȃ����W�����v�䐶���񐔂��U���񐔈ȉ��ł����
				if (m_IsJumppadGenerate == true && (m_attackIntervalTemp - m_Timer) < (m_WaveAttackInterval * m_JumppadSpawnRatio)
					&& m_JumppadGenerateCount <= m_WaveAttackCount - 1)
				{
					// �W�����v�䐶���ʒu������
					float jumppadYPos = FMath::RandRange(-m_JumppadGenerateWidth, m_JumppadGenerateWidth);
					FVector generatePos(GetActorLocation().X + m_JumppadPositionXAvoidWaveAttack, jumppadYPos, m_JumppadPositionZAvoidWaveAttack);

					// �A���ŃW�����v��𐶐��ł��Ȃ��悤�ɂ���
					m_IsJumppadGenerate = false;

					// �W�����v�䐶��
					AActor* jumppadTemp = SpawnAttackActor(m_JumppadActor, generatePos);

					// �W�����v��̃T�C�Y�ύX
					jumppadTemp->SetActorScale3D(m_JumppadScaleAvoidWaveAttack);

					// �W�����v���X�ʒu���i�[
					m_JumppadPosX = jumppadTemp->GetActorLocation().X;

					// �W�����v�䐶���񐔂����Z
					m_JumppadGenerateCount++;

					UE_LOG(LogTemp, Verbose, TEXT("BossEnemy Getenated Jumppad!"));
				}
				// �U�����ԂɂȂ��Ă�����
				else if (m_Timer >= m_attackIntervalTemp)
				{
					// �U���Ԋu�𑝉�
					m_attackIntervalTemp += m_WaveAttackInterval;

					// �W�����v��𐶐��\�ɂ���
					m_IsJumppadGenerate = true;

					// �E�F�[�u�U��Actor�̐���
					ABossWaveControl* waveTemp = Cast<ABossWaveControl>(SpawnAttackActor(m_WaveAttackActor, GetActorLocation()));

					// �E�F�[�u�U���ɃW�����v��̈ʒu��ۑ�
					waveTemp->SetJumppadPosX(m_JumppadPosX);

					UE_LOG(LogTemp, Verbose, TEXT("BossEnemy is Attacked! (Wave)"));
				}
			}
			break;

			// �ːi�U��
		case BossAttack::Rush:
		{
			// �{�X���ݒ肵���n�_�Ɉړ�����܂�
			if (m_Timer < m_RushAttackDuration && m_IsRush == false)
			{
				// �ʏ�ړ�����
				float moveSpeed = FMath::Lerp(m_MoveSpeed, m_RushAttackMoveSpeed, (m_Timer / m_RushReachTimeTopSpeed));

				MoveForward(moveSpeed, GetActorLocation());

				// �ʒu��ۑ�
				m_RushRunStartPos = GetActorLocation();
			}
			// �ړ����I�������
			else if (m_IsRush == false)
			{
				// �^�C�}�[���Z�b�g
				m_Timer = 0.0f;

				// �ːi��Ԃ�
				m_IsRush = true;
			}
			// �ːi�J�n
			else
			{
				// �v���C���[�̍��W���擾
				if (m_IsLookPlayer == false && m_PlayerActor != nullptr)
				{
					m_IsLookPlayer = true;

					float targetRad = GetRadianFromVector(this->GetActorLocation(), m_PlayerActor->GetActorLocation());
					m_TargetRot = FMath::RadiansToDegrees(targetRad);
					UE_LOG(LogTemp, Verbose, TEXT("Set rotation! rot = %f"), m_TargetRot);
				}
				else if (m_IsLookPlayer == false)
				{
					m_IsLookPlayer = true;

					m_TargetRot = m_RushRunStartPos.Y > 0.0f ? -180.0f : 180.0f;
					UE_LOG(LogTemp, Warning, TEXT("BossControl::m_PlayerActor is nullptr. Rush towards 180 degrees."));
				}

				// �U�����
				if (m_Timer <= m_RushTurnTime)
				{
					// �p�x�̕��
					float rotZ = FMath::Lerp(30.f, m_TargetRot, m_Timer / m_RushTurnTime);

					// �p�x�̐ݒ�
					FRotator rot(GetActorRotation().Pitch, rotZ, GetActorRotation().Roll);
					SetActorRotation(rot);

					// �ʏ�ړ�����
					MoveForward(m_RushAttackMoveSpeed, GetActorLocation());

					float posY = FMath::Lerp(m_RushRunStartPos.Y, 0.0f, m_Timer / m_RushTurnTime);
					FVector newPos(GetActorLocation().X, posY, GetActorLocation().Z);

					SetActorLocation(newPos);
				}
				// �ːi
				else
				{
					// ���U��A�j���[�V�������s���Ă��Ȃ����
					if (m_IsHeadAnim == false && m_IsHeadAnimEnd == false)
					{
						// ���U��A�j���[�V�������s��
						m_IsHeadAnim = true;
						UE_LOG(LogTemp, Warning, TEXT("Head start"));
					}
					// ���U��A�j���[�V�������I����Ă����
					else if (m_IsHeadAnimEnd)
					{
						// �v���C���[���L���ł��A�{�X���ړI�ʒu�ɓ��B���Ă��炸�A�t�F���X�̒��ɂ���
						if (m_PlayerActor != nullptr && GetActorLocation().X > m_PlayerActor->GetActorLocation().X - m_RushThroughDistance
							&& GetActorLocation().Y > -m_TEMP_FENCE_POS && GetActorLocation().Y < m_TEMP_FENCE_POS)
						{
							// ���U��A�j���[�V�������I������
							m_IsHeadAnim = false;
							// �p�x�̐ݒ�
							FRotator rot(GetActorRotation().Pitch, m_TargetRot, GetActorRotation().Roll);
							SetActorRotation(rot);

							// �ːi�ړ�
							MoveForward(m_RushAttackSpeed, GetActorLocation());
						}
						// �O�ɏo��
						else
						{
							bossState = BossState::RunAfterRush;

							// �v���C���[�̕����Ɍ����t���O�����Z�b�g
							m_IsLookPlayer = false;

							// �A�j���[�V�����֘A�̃��Z�b�g
							m_IsHeadAnim = false;
							m_IsHeadAnimEnd = false;

							// �^�C�}�[���Z�b�g
							m_Timer = 0.0f;
						}
					}
					// ���U��A�j���[�V������
					else
					{
						UE_LOG(LogTemp, Verbose, TEXT("Head keep"));
					}
				}
			}
			break;

			UE_LOG(LogTemp, Verbose, TEXT("BossEnemy is Attacked! (Rush)"));
			break;
		}

		// ���̑��̍U����i
		default:
			UE_LOG(LogTemp, Warning, TEXT("BossEnemy attack is Unknown. bossAttack index = %d."), (int)bossAttack);
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

			// �W�����v�J�n�ʒu��ۑ�
			m_JumpLocationY = GetActorLocation().Y;

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

		// �ːi��̈ړ�
	case BossState::RunAfterRush:
		UE_LOG(LogTemp, Verbose, TEXT("BossEnemy state is Runnning (after rush)."));

		// �W�����v�ڍs���ԂɂȂ�����
		if (m_Timer >= m_RunningTimeAfterAttack)
		{
			// �W�����v��Ԃ�
			bossState = BossState::Jump_Out;

			// �W�����v�J�n�ʒu��ۑ�
			m_JumpLocationY = GetActorLocation().Y;

			FRotator rot(GetActorRotation().Pitch, -360.0f, GetActorRotation().Roll);
			SetActorRotation(rot);

			// �^�C�}�[���Z�b�g
			m_Timer = 0.0f;
		}
		// ��]���n�߂鎞�ԂɂȂ�����
		else if (m_Timer >= m_RunningTimeAfterAttack * m_RotateStateRatio)
		{
			SetActorRotationLerpDegree(m_TargetRot, -360.0f, (m_Timer - m_RunningTimeAfterAttack * m_RotateStateRatio), (m_RunningTimeAfterAttack - m_RunningTimeAfterAttack * m_RotateStateRatio));

			MoveForward(m_MoveSpeed, GetActorLocation());
		}
		// ���鏈��
		else
		{
			MoveForward(m_MoveSpeed, GetActorLocation());
		}
		break;

		// �W�����v�ŊO�ɍs��
	case BossState::Jump_Out:
		UE_LOG(LogTemp, Verbose, TEXT("BossEnemy state is Jump (out)."));

		// �W�����v���Ԃ��I��������
		if (m_Timer >= m_JumpTime)
		{
			// �ːi��Ԃ�������
			if (bossAttack == BossAttack::Rush)
			{
				// �ǂ�����Ԃ�
				bossState = BossState::CatchUp;
				m_BeforeCatchUpPosX = GetActorLocation().X;
			}
			// ����ȊO�̍U��
			else
			{
				// �ʏ��Ԃ�
				bossState = BossState::Run;
			}
			SetActorRotation(FRotator::ZeroRotator);

			// �^�C�}�[���Z�b�g
			m_Timer = 0.0f;
		}
		// ��]���n�߂鎞�ԂɂȂ�����
		else if (m_Timer >= m_JumpTime * m_RotateStateRatio)
		{
			SetActorRotationLerpDegree(-m_JumpDegreeZ, 0.0f, (m_Timer - m_JumpTime * m_RotateStateRatio), (m_JumpTime - m_JumpTime * m_RotateStateRatio));

			float nextPosY = FMath::Lerp(m_JumpLocationY, originPosY, m_Timer / m_JumpTime);

			// �ړ�����
			FVector tempPos(GetActorLocation().X + m_MoveSpeed, nextPosY, GetActorLocation().Z);
			SetActorLocation(tempPos);
		}
		// �W�����v����
		else
		{
			float nextPosY = FMath::Lerp(m_JumpLocationY, originPosY, m_Timer / m_JumpTime);

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

	// �W�����v���ɂ̓R���W������؂�
	if (bossState == BossState::Jump_In || bossState == BossState::Jump_Out)
	{
		SetActorEnableCollision(false);
	}
	else
	{
		SetActorEnableCollision(true);
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

// �U�����s��Actor(TSubclassOf<>)�𐶐�
AActor* ABossControl::SpawnAttackActor(const TSubclassOf<class AActor> _attackActor, const FVector _attackPos)
{
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

	// �U��Actor�̐���
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

// �ːi�U����Ԃ��ǂ���
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

// ����U��I��������Ƃ��擾
void ABossControl::SetIsHeadEnd(bool _status)
{
	m_IsHeadAnimEnd = _status;
}

// �����ӂ��Ԃ��ǂ���
bool ABossControl::GetIsHead() const
{
	return m_IsHeadAnim;
}

// �W�����v���Ԃ̎擾
float ABossControl::GetJumpTime() const
{
	return m_JumpTime;
}

// ����A�j���[�V�����̍Đ����x�擾
float ABossControl::GetRunAnimSpeed() const
{
	return m_RunningAnimationRateSpeed;
}

// �����ړ�����
void ABossControl::MoveForward(const float& _speed, const FVector& _pos)
{
	// X���Ɉړ��ʂ����Z���AActor�̈ʒu���X�V����
	FVector vector = GetActorForwardVector() * _speed;
	FVector nextPos((_pos.X + vector.X), (_pos.Y + vector.Y), _pos.Z);

	SetActorLocation(nextPos);
}