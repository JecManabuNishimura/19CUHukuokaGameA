// �}�b�v�̎����z�u���s���N���X

// 2020/12/02 �n� ���� �쐬

#include "MapCreator.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/DataTable.h"

AMapCreator::AMapCreator()
	: m_VisibleMapWire(false)
	, m_IsLoadMapData(false)
	, m_IsContinuous(false)
	, m_IsFence(false)
	, m_IsGeneratePlayer(false)
	, m_ColumnStart(0)
	, m_FenceStart(0)
	, m_NotGroundGenerateStr("NL")
	, m_XAxis_Offset(2500.0f)
	, m_YAxis_Offset(500.0f)
	, m_PlayerGenerateStr("s")
{
	// Tick�����s����K�v�͂Ȃ��̂ōX�V��؂�
	PrimaryActorTick.bCanEverTick = false;

	// SubObject�̍쐬
	m_SampleGround = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("SampleGround"));

	m_SampleObjectArray.Add(CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("SampleObject")));

	// ������z��̏�����
	strArrayTemp.Reset();

	// �A���z�u�p�̔z��̏�����
	m_ColumnStartVertArray.Reset();
	m_RowStartArray.Reset();
	m_ColumnStartVertFenceArray.Reset();
	m_RowStartFenceArray.Reset();

	m_MapActorGround.geterateType = MapPlacementPattern::SettingLock;

	// �v���C���[Actor�̐ݒ�
	m_PlayerActor.geterateType = MapPlacementPattern::SettingLock;

	// �}�b�vActor�ꎞ�ۑ��ϐ���������
	m_ContinuousActorTemp = MapActorStructCppReset();
	m_ContinuousVertActorTempArray.Reset();
	m_FenceActorTempArray.Reset();
}

void AMapCreator::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	// �s�ԍ�
	int rowIndex = 0;

	// Actor�̃g�����X�t�H�[�����Œ�
	// �Œ肷��Location
	FVector newLocation(0.0f, 0.0f, 0.0f);

	// �Œ肷��Rotation
	FRotator newRotation(0.0f, 0.0f, 0.0f);

	// �Œ肷��Scale
	FVector newScale(1.0f, 1.0f, 1.0f);

	// Location��Rotation���Œ�
	SetActorLocationAndRotation(newLocation, newRotation);

	// Scale���Œ�
	SetActorScale3D(newScale);

	// �C���X�^���X���b�V��������
	if (m_SampleGround != nullptr)
	{
		// �C���X�^���X���b�V���̍폜
		m_SampleGround->ClearInstances();

		// ����Actor���ݒ肳��Ă����
		if (m_MapActorGround.actor != nullptr)
		{
			// C++�ō쐬���ꂽDefaultSubObject��StaticMeshComponent�����Ă����
			m_MapActorGround.actorStaticMesh = Cast<UStaticMeshComponent>(m_MapActorGround.actor.GetDefaultObject()->GetComponentByClass(UStaticMeshComponent::StaticClass()));

			if (m_MapActorGround.actorStaticMesh != nullptr)
			{
				m_SampleGround->SetStaticMesh(m_MapActorGround.actorStaticMesh->GetStaticMesh());
				m_SampleGround->SetMaterial(0, m_MapActorGround.actorStaticMesh->GetMaterial(0));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MapCreator::m_SampleGround is null."));
	}

	// ���b�V���������s�����ǂ���
	if (m_VisibleMapWire)
	{
		// m_IsLoadMapData��false�ɂȂ�܂Ńf�[�^�̓ǂݍ���
		do
		{
			SetStrArrayMapData(strArrayTemp, rowIndex, m_IsLoadMapData);

			//	�}�b�v�ɔz�u���鏰���ݒ肳��Ă��Ȃ�
			if (m_MapActorGround.actor == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("MapCreator::Need set m_MapActorGround."));
			}
			else
			{
				// ���̈ʒu���Z�o
				FVector groundPos(rowIndex * m_XAxis_Offset, (float)strArrayTemp.Num() / 2.0f, m_MapActorGround.location_Z);
				FTransform groundTransform(m_MapActorGround.rotation, groundPos, m_MapActorGround.scale);

				// �C���X�^���X���b�V�������݂��Ă���
				if (m_SampleGround != nullptr)
				{
					m_SampleGround->AddInstance(groundTransform);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("MapCreator::Need set m_SampleGround."));
				}
			}

			// ���̍s��ǂݍ���
			strArrayTemp.Reset();
			++rowIndex;

		} while (m_IsLoadMapData);
	}
}

void AMapCreator::BeginPlay()
{
	Super::BeginPlay();

	// ���}�b�v�̃C���X�^���X���폜
	if (m_SampleGround != nullptr)
	{
		m_SampleGround->ClearInstances();
	}

	// �ϐ��錾
	bool isFenceMake = false;
	int rowIndex = 0;

	// �����z�񏉊���
	strArrayTemp.Reset();

	// CSV�t�@�C���ǂݍ���
	do
	{
		// Loading
		UE_LOG(LogTemp, Verbose, TEXT("Map Loading Now..."));

		// �}�b�v�f�[�^��ǂݍ���
		SetStrArrayMapData(strArrayTemp, rowIndex, m_IsLoadMapData);

		// �}�b�v�f�[�^���Ȃ���΂����
		if (m_IsLoadMapData == false)
		{
			// �c�����̘A���z�u�`�F�b�N
			if (m_ColumnStartVertArray.Num() > 0)
			{
				// ������z�������ǂ݂���
				FMapStructCpp mapStructTemp;
				for (int columnIndex = 0; columnIndex < mapStructTemp.indexNum; ++columnIndex)
				{
					// �ݒ肵��m_MapActorArray�̗v�f�����A�������s��
					for (int i = 0; i < m_MapActorArray.Num(); ++i)
					{
						// �c�����ɘA���z�u����Actor���`�F�b�N����
						for (int j = 0; j < m_ColumnStartVertArray.Num(); ++j)
						{
							// �񂪈�v���邩
							if (m_ColumnStartVertArray[j] == columnIndex)
							{
								m_ContinuousVertActorTempArray[j].scale.X = (rowIndex - m_RowStartArray[j]) * m_ContinuousVertActorTempArray[j].scale.X;

								SpawnMapActor(m_ContinuousVertActorTempArray[j], LocationX((rowIndex + m_RowStartArray[j]) / 2.0f), LocationY(m_ColumnStartVertArray[j], mapStructTemp.indexNum));

								// �v�f�̏���
								// �ۑ����Ă���Actor�̍폜
								m_ContinuousVertActorTempArray.RemoveAt(j);
								// ��̍폜
								m_ColumnStartVertArray.RemoveAt(j);
								// �s�̍폜
								m_RowStartArray.RemoveAt(j);
							}
						}
					}
				}
			}

			return;
		}

		// ������z�������ǂ݂���
		for (int columnIndex = 0; columnIndex < strArrayTemp.Num(); ++columnIndex)
		{
			// ��ԍŏ��̕�����NL�i�n�ʂ𐶐����Ȃ��ꍇ�j�łȂ����
			if (strArrayTemp[0] != m_NotGroundGenerateStr)
			{
				// �n�ʐ���
				if (m_MapActorGround.actor != nullptr)
				{
					SpawnMapActor(m_MapActorGround, LocationX(rowIndex), 0.0f);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("m_MapActorGround.actor is nullptr!"))
				}
			}
			// �n�ʂ𐶐����Ȃ��ꍇ�͑���Actor���������Ȃ��̂ŃX�L�b�v����
			else
			{
				break;
			}

			// �����Ȃ��̏ꍇ
			if (strArrayTemp[columnIndex] == "")
			{
				// ���ɘA���z�u�����������`�F�b�N
				if (m_IsContinuous == true)
				{
					m_IsContinuous = false;

					SpawnContinuousActor(rowIndex, m_ColumnStart, columnIndex);
				}
			}
			// �v���C���[�����̏ꍇ
			else if (strArrayTemp[columnIndex] == m_PlayerGenerateStr)
			{
				// �v���C���[�𐶐����Ă��Ȃ����
				if (m_IsGeneratePlayer == false)
				{
					// �v���C���[�𐶐�����
					m_IsGeneratePlayer = true;

					// �v���C���[�̐���
					if (m_PlayerActor.actor != nullptr)
					{
						SpawnMapActor(m_PlayerActor, LocationX(rowIndex), LocationY(columnIndex, strArrayTemp.Num()));
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("PlayerChara is nullptr!"));
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("PlayerChara was generated!"));
				}
			}
			// ��L�ȊO�̕����̏ꍇ
			else
			{
				// �}�b�v��Actor�𐶐�����
				// �ݒ肵��m_MapActorArray�̗v�f�����A�������s��
				for (int mapActorIndex = 0; mapActorIndex < m_MapActorArray.Num(); ++mapActorIndex)
				{
					// �����z��̗v�f��m_MapActorArray�ɐݒ肵����������v���Ă�����
					if (strArrayTemp[columnIndex] == m_MapActorArray[mapActorIndex].generateChar)
					{
						// �������ɘA���z�u���ŁA�A���z�u���I��������`�F�b�N
						if (m_IsContinuous == true && m_ContinuousActorTemp.actor != m_MapActorArray[mapActorIndex].actor)
						{
							m_IsContinuous = false;

							SpawnContinuousActor(rowIndex, m_ColumnStart, columnIndex);
						}

						// �c�����̘A���z�u�`�F�b�N
						if (m_ColumnStartVertArray.Num() > 0)
						{
							// �c�����ɘA���z�u����Actor���`�F�b�N����
							for (int j = 0; j < m_ColumnStartVertArray.Num(); ++j)
							{
								// �c�����ɘA���z�u���ŁA�A���z�u���I��������`�F�b�N
								if (m_ColumnStartVertArray[j] == columnIndex && m_ContinuousVertActorTempArray[j].actor != m_MapActorArray[mapActorIndex].actor)
								{
									m_ContinuousVertActorTempArray[j].scale.Y = ContinuousScale(m_ColumnStartVertArray[j], columnIndex, m_ContinuousVertActorTempArray[j].scale.Y);
									SpawnMapActor(m_ContinuousVertActorTempArray[j], LocationX(rowIndex), LocationY(columnIndex, strArrayTemp.Num()));

									// �v�f�̏���
									// �ۑ����Ă���Actor�̍폜
									m_ContinuousVertActorTempArray.RemoveAt(j);
									// ��̍폜
									m_ColumnStartVertArray.RemoveAt(j);
									// �s�̍폜
									m_RowStartArray.RemoveAt(j);
								}
							}
						}

						// �������[���ɂ���ĕ���
						switch (m_MapActorArray[mapActorIndex].geterateType)
						{
							// �P�̂Ŕz�u����
						case MapPlacementPattern::Single:

							// Actor���������������
							SpawnMapActor(m_MapActorArray[mapActorIndex], LocationX(rowIndex), LocationY(columnIndex, strArrayTemp.Num()));

							break;

							// �������ɘA���z�u�ň��Actor�ɂȂ�
						case MapPlacementPattern::Continuous:

							// ���̘A���z�u�̊J�n
							if (m_IsContinuous == false)
							{
								m_IsContinuous = true;

								// �A���z�u���J�n����Actor�̕ۑ�
								m_ContinuousActorTemp = m_MapActorArray[mapActorIndex];

								// �ʒu�̕ۑ�
								m_ColumnStart = columnIndex;
							}
							break;

							// �c�����ɘA���z�u�ň��Actor�ɂȂ�
						case MapPlacementPattern::V_Continuous:

							// �A���z�u���J�n����Actor�̕ۑ�
							m_ContinuousVertActorTempArray.Add(m_MapActorArray[mapActorIndex]);

							// �ʒu�̕ۑ�
							// ��
							m_ColumnStartVertArray.Add(columnIndex);

							// �s
							m_RowStartArray.Add(rowIndex);

							// �v�f��2�ȏ゠��ꍇ
							if (m_ColumnStartVertArray.Num() >= 2)
							{
								// ��ԐV�����ǉ������v�f��������̏ꍇ
								if (m_ColumnStartVertArray[m_ColumnStartVertArray.Num() - 1] == columnIndex)
								{
									for (int i = 0; i < m_ContinuousVertActorTempArray.Num() - 2; ++i)
									{
										// �ǉ�����Actor������Actor�ł���Δj��
										if (m_ContinuousVertActorTempArray[m_ContinuousVertActorTempArray.Num() - 1].actor == m_ContinuousVertActorTempArray[i].actor)
										{
											// �A���z�u���J�n����Actor�̕ۑ�
											m_ContinuousVertActorTempArray.RemoveAt(m_ContinuousVertActorTempArray.Num() - 1);

											// �ʒu�̕ۑ�
											// ��
											m_ColumnStartVertArray.RemoveAt(m_ColumnStartVertArray.Num() - 1);

											// �s
											m_RowStartArray.RemoveAt(m_RowStartArray.Num() - 1);

											break;
										}
									}
								}
							}
							break;

						default:
							break;
						}
					}
					// �����z��̗v�f�ƘA���z�u����m_MapActorArray�ɐݒ肵��Start�̕�������v���Ă�����
					else if (strArrayTemp[columnIndex] == m_MapActorArray[mapActorIndex].generateCharStart)
					{
						// �������[���ɂ���ĕ���
						switch (m_MapActorArray[mapActorIndex].geterateType)
						{
							// �n�_�ƏI�_���w�肵�Ĉ��Actor�𐶐�����i�������j
						case MapPlacementPattern::Fence:

							// �t�F���X�z�u�̊J�n
							if (m_IsFence == false)
							{
								m_IsFence = true;

								// �t�F���X�����J�n�̗��ۑ�
								m_FenceStart = columnIndex;
							}
							break;

							// �n�_�ƏI�_���w�肵�Ĉ��Actor�𐶐�����i�c�����j
						case MapPlacementPattern::V_Fence:

							// �t�F���X�z�u�̊J�n�n�_�̏���ۑ�

							// ����Actor�̕ۑ�
							m_FenceActorTempArray.Add(m_MapActorArray[mapActorIndex]);

							// �s�̕ۑ�
							m_RowStartFenceArray.Add(rowIndex);
							// ��̕ۑ�
							m_ColumnStartVertFenceArray.Add(columnIndex);

							break;

						default:
							break;
						}
					}
					// �����z��̗v�f�ƘA���z�u����m_MapActorArray�ɐݒ肵��End�̕�������v���Ă�����
					else if (strArrayTemp[columnIndex] == m_MapActorArray[mapActorIndex].generateCharEnd)
					{
						// �������[���ɂ���ĕ���
						switch (m_MapActorArray[mapActorIndex].geterateType)
						{
							// �n�_�ƏI�_���w�肵�Ĉ��Actor�𐶐�����i�������j
						case MapPlacementPattern::Fence:

							// �t�F���X����
							// �t�F���X�������J�n����Ă����
							if (m_IsFence == true)
							{
								m_IsFence = false;

								SpawnContinuousActor(rowIndex, m_FenceStart, columnIndex);
							}
							break;

							// �n�_�ƏI�_���w�肵�Ĉ��Actor�𐶐�����i�c�����j
						case MapPlacementPattern::V_Fence:

							// �t�F���X����
							// �������J�n�����t�F���X�������
							if (m_FenceActorTempArray.Num() > 0)
							{
								// ��v�f������
								for (int columnCheck = 0; columnCheck < m_ColumnStartVertFenceArray.Num(); ++columnCheck)
								{
									// �񂪈�v������
									if (m_ColumnStartVertFenceArray[columnCheck] == columnIndex)
									{
										// Actor����v���Ă����
										if (m_FenceActorTempArray[columnCheck].actor == m_MapActorArray[mapActorIndex].actor)
										{
											// Y�X�P�[����ݒ�
											m_FenceActorTempArray[columnCheck].scale.Y = (rowIndex - m_RowStartFenceArray[columnCheck]) * m_FenceActorTempArray[columnCheck].scale.Y;

											// ����
											SpawnMapActor(m_FenceActorTempArray[columnCheck], LocationX((rowIndex + m_RowStartFenceArray[columnCheck]) / 2.0f), LocationY(m_ColumnStartVertFenceArray[columnCheck], strArrayTemp.Num()));

											// �v�f�̍폜
											m_FenceActorTempArray.RemoveAt(columnCheck);
											m_ColumnStartVertFenceArray.RemoveAt(columnCheck);
											m_RowStartFenceArray.RemoveAt(columnCheck);
										}
									}
								}
							}
							break;

						default:
							break;
						}
					}
				}
			}
		}

		// ���̍s��ǂݍ���
		strArrayTemp.Reset();
		++rowIndex;

	} while (m_IsLoadMapData);
}

void AMapCreator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMapCreator::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e)
{
	if (e.ChangeType == EPropertyChangeType::ArrayAdd && e.GetPropertyName() == "m_SampleObjectArray")
	{
		m_SampleObjectArray[m_SampleObjectArray.Num() - 1] = m_SampleObjectArray[0];
	}
	Super::PostEditChangeChainProperty(e);
}

// ������z��Ɉ�s���̃}�b�v�f�[�^��ۑ�
void AMapCreator::SetStrArrayMapData(TArray<FString>& _strArray, const int _rowIndex, bool& _isLoad)
{
	_isLoad = true;

	// �}�b�v�f�[�^������
	if (m_MapData != nullptr)
	{
		// int�̍s�ԍ���FName�ɕϊ����邽�߂ɁAFString�ɕϊ�����iFName�ւ̕ϊ���FString���炵���ł��Ȃ����߁j
		FString fStrIndex = FString::FromInt(_rowIndex + 1);

		// FString����FName�ɕϊ�����
		FName fNameIndex = FName(*fStrIndex);

		// �s���Ƃ̃f�[�^�̎擾
		FMapStructCpp* rowData = m_MapData->FindRow<FMapStructCpp>(fNameIndex, FString());

		// �f�[�^�s�̏I���
		if (rowData == nullptr)
		{
			// �f�[�^�ǂݍ��ݏI��
			_isLoad = false;
		}
		// �f�[�^��������
		else
		{
			// FMapStructCpp�\���̗̂v�f�����̒����̔z����쐬
			for (int i = 0; i < rowData->indexNum; ++i)
			{
				// FMapStructCpp�̍ŏ��̗̈�iLine_1�j����Ō�̗̈�iLine_15�j�܂ł̊Ԃ�ǉ�
				if ((&rowData->Line_1 + i) != NULL)
				{
					_strArray.Add(*(&rowData->Line_1 + i));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Could Not Get rowData!"));
				}
			}
		}
	}
	// �}�b�v�f�[�^���Ȃ�
	else
	{
		_isLoad = false;
	}
}

void AMapCreator::SpawnContinuousActor(const int _rowIndex, const int _startColumn, const int _endColumn)
{
	// �A�������̏I��
	m_IsContinuous = false;

	// Actor�̐���
	// �X�P�[���̎Z�o
	FVector newScale(m_ContinuousActorTemp.scale.X,
		ContinuousScale(_startColumn, _endColumn, m_ContinuousActorTemp.scale.Y),
		m_ContinuousActorTemp.scale.Z);

	// �X�P�[���̓K�p
	m_ContinuousActorTemp.scale = newScale;

	// ���W���w�肵��Actor�𐶐�
	SpawnMapActor(m_ContinuousActorTemp, LocationX(_rowIndex), ContinuousLocationY(_startColumn, _endColumn, strArrayTemp.Num()));

	// Actor�̈ꎞ�ۑ����Ă������̂����Z�b�g
	m_ContinuousActorTemp = MapActorStructCppReset();
}

// X���W�Z�o
float AMapCreator::LocationX(const int _rowIndex)
{
	return (float)_rowIndex * m_XAxis_Offset;
}

// Y���W�Z�o
float AMapCreator::LocationY(const int _columnIndex, const int _strArrayLength)
{
	float difference = (float)_columnIndex - ((float)(_strArrayLength - 1) / 2.0f);

	return difference * m_YAxis_Offset;
}

// �A������Actor��Y�X�P�[���Z�o
float AMapCreator::ContinuousScale(const int _startColumn, const int _endColumn, const float _actorScaleY)
{
	int diff = _endColumn - _startColumn;
	return _actorScaleY * diff;
}

// �A������Actor��Y�ʒu�Z�o
float AMapCreator::ContinuousLocationY(const int _startColumn, const int _endColumn, const int _strArrayLength)
{
	float add = (_startColumn + _endColumn) / 2.0f;
	float halfLength = (float)(_strArrayLength - 1) / 2.0f;

	return (add - halfLength) * m_YAxis_Offset;
}

// �}�b�v��Actor�𐶐�
AActor* AMapCreator::SpawnMapActor(FMapActorStructCpp _spawnActor, const float _locationX, const float _locationY)
{
	// �����p�����[�^�̐ݒ�
	FActorSpawnParameters params;

	params.bAllowDuringConstructionScript = true;
	params.bDeferConstruction = false;
	params.bNoFail = true;
	params.Instigator = nullptr;
	params.Name = { };
	params.ObjectFlags = EObjectFlags::RF_NoFlags;
	params.OverrideLevel = nullptr;
	params.Owner = this;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	params.Template = nullptr;

	FVector pos(_locationX, _locationY, _spawnActor.location_Z);
	AActor* spawnActor = GetWorld()->SpawnActor<AActor>(_spawnActor.actor, pos, FRotator::ZeroRotator, params);

	spawnActor->SetActorScale3D(_spawnActor.scale);
	spawnActor->SetActorRotation(_spawnActor.rotation);

	return spawnActor;
}

// FMapActorStructCpp�����Z�b�g����֐�
FMapActorStructCpp AMapCreator::MapActorStructCppReset()
{
	FMapActorStructCpp params;

	params.actor = nullptr;
	params.location_Z = 0.0f;
	params.rotation = FRotator::ZeroRotator;
	params.scale = FVector::OneVector;
	params.generateChar = "";
	params.generateCharStart = "";
	params.generateCharEnd = "";
	params.geterateType = MapPlacementPattern::Single;
	params.enemyMoveType = EEnemyMoveType::None;

	return params;
}