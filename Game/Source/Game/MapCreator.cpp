// �}�b�v�̎����z�u���s���N���X

// 2020/12/02 �n� ���� �쐬

#include "MapCreator.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/DataTable.h"
#include <time.h>

AMapCreator::AMapCreator()
	: m_SetMapData(false)
	, m_VisibleMapWire(false)
	, m_IsLoadMapData(false)
	, m_IsGeneratePlayer(false)
	, m_MapRowNumber(0)
	, m_StrMapLength(0)
	, m_NotGroundGenerateStr("NL")
	, m_XAxis_Offset(2500.0f)
	, m_YAxis_Offset(500.0f)
	, m_PlayerGenerateStr("s")
{
	// Tick�����s����K�v�͂Ȃ��̂ōX�V��؂�
	PrimaryActorTick.bCanEverTick = false;

	// RootConponent�̍쐬
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// ������z��̏�����
	m_StrMapArray.Reset();

	// ��������Actor�̏���ۑ�
	m_MapActorCreateData.Reset();

	// �c�����ɐ���������̂̏�񃊃X�g��������
	m_FenceData.Reset();
	//m_ContinuousData.Reset();
	m_VerticalFenceData.Reset();
	//m_VerticalContinuousData.Reset();

	// ��Actor�̏�����
	m_MapActorGround.geterateType = MapPlacementPattern::SettingLock;

	// �v���C���[Actor�̐ݒ�
	m_PlayerActor.geterateType = MapPlacementPattern::SettingLock;
}

void AMapCreator::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Enemy�p�ݒ�
	for (int i = 0; i < m_MapActorArray.Num(); ++i)
	{
		if (m_MapActorArray[i].isEnemy == true)
		{
			m_MapActorArray[i].geterateType = MapPlacementPattern::Single;
		}
		else
		{
			m_MapActorArray[i].enemyMoveType = EEnemyMoveType::None;
		}
	}

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

	// �}�b�v�I�u�W�F�N�g�p�C���X�^���X���b�V���z��̏�����
	if (m_SampleMapObject.Num() > 0)
	{
		for (int i = 0; i < m_SampleMapObject.Num(); ++i)
		{
			if (m_SampleMapObject[i] != nullptr)
			{
				// �C���X�^���X�폜
				m_SampleMapObject[i]->ClearInstances();
			}
		}
	}

	// �}�b�v�I�u�W�F�N�g�p�C���X�^���X���b�V���z�񃊃Z�b�g
	m_SampleMapObject.Reset();

	// ���b�V���������s�����ǂ���
	if (m_VisibleMapWire)
	{
		// ���ȊO��Actor�̃��b�V�����C���X�^���X���b�V���ɐݒ肷��
		for (int i = 0; i < m_MapActorArray.Num() + 1; ++i)
		{
			// �C���X�^���X���b�V���R���|�[�l���g�̐ݒ�
			m_SampleMapObject.Add(NewObject<UInstancedStaticMeshComponent>(this));
			m_SampleMapObject[i]->RegisterComponent();
			m_SampleMapObject[i]->AttachToComponent(RootComponent, { EAttachmentRule::SnapToTarget, true });

			// �C���X�^���X���b�V���R���|�[�l���g���ݒ�ł��Ă����
			if (m_SampleMapObject[i] != nullptr)
			{
				// �ŏ��ł���Ώ���ݒ�
				if (i == 0)
				{
					// ��Actor���ݒ肳��Ă����
					if (m_MapActorGround.actor != nullptr)
					{
						// C++�ō쐬���ꂽActor��DefaultSubObject��StaticMeshComponent�����Ă����
						m_MapActorGround.actorStaticMesh = Cast<UStaticMeshComponent>(m_MapActorGround.actor.GetDefaultObject()->GetComponentByClass(UStaticMeshComponent::StaticClass()));

						// �X�^�e�B�b�N���b�V�����擾�ł��Ă����

						if (m_MapActorGround.actorStaticMesh != nullptr)
						{
							// �C���X�^���X���b�V���ɃX�^�e�B�b�N���b�V���ƃ}�e���A����ݒ�

							m_SampleMapObject[i]->SetStaticMesh(m_MapActorGround.actorStaticMesh->GetStaticMesh());
							m_SampleMapObject[i]->SetMaterial(i, m_MapActorGround.actorStaticMesh->GetMaterial(0));
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("[MapCreator] Could not get static mesh for actor \"%s\""),*(m_MapActorGround.actor.GetDefaultObject()->GetName()))
						}
					}
				}
				else
				{
					// ����Actor���ݒ肳��Ă����
					if (m_MapActorArray[i - 1].actor != nullptr)
					{
						// C++�ō쐬���ꂽActor��DefaultSubObject��StaticMeshComponent�����Ă����
						m_MapActorArray[i - 1].actorStaticMesh = Cast<UStaticMeshComponent>(m_MapActorArray[i - 1].actor.GetDefaultObject()->GetComponentByClass(UStaticMeshComponent::StaticClass()));

						// �X�^�e�B�b�N���b�V�����擾�ł��Ă����

						if (m_MapActorArray[i - 1].actorStaticMesh != nullptr)
						{
							// �C���X�^���X���b�V���ɃX�^�e�B�b�N���b�V���ƃ}�e���A����ݒ�

							m_SampleMapObject[i]->SetStaticMesh(m_MapActorArray[i - 1].actorStaticMesh->GetStaticMesh());
							m_SampleMapObject[i]->SetMaterial(i, m_MapActorArray[i - 1].actorStaticMesh->GetMaterial(0));
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("[MapCreator] Could not get static mesh for actor \"%s\""), *(m_MapActorArray[i - 1].actor.GetDefaultObject()->GetName()))
						}
					}
				}
			}
		}
		SettingMap(m_SetMapData);

		MapCreateEditor();
		UE_LOG(LogTemp, Log, TEXT("HELLO!"));
	}
	m_SetMapData = false;
}

void AMapCreator::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < m_SampleMapObject.Num(); ++i)
	{
		if (m_SampleMapObject[i] != nullptr)
		{
			// �C���X�^���X�폜
			m_SampleMapObject[i]->ClearInstances();
		}
	}
	
	SettingMap();
	
	MapCreate();
}

void AMapCreator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

// �}�b�v��Actor�𐶐�
AActor* AMapCreator::SpawnMapActor(FMapActorStructCpp _spawnActor, const float _locationX, const float _locationY)
{
	if (_spawnActor.actor != nullptr)
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
	else return nullptr;
}

// �}�b�v�ɃX�^�e�B�b�N���b�V����z�u
void AMapCreator::AddMapInstanceStaticMesh(UInstancedStaticMeshComponent* _instancedMeshComp, FMapActorStructCpp _spawnActor, const float _locationX, const float _locationY)
{
	_instancedMeshComp->AddInstance(
		FTransform(_spawnActor.rotation,
			FVector(_locationX, _locationY, _spawnActor.location_Z),
			_spawnActor.scale));
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

// ������z���CSV�t�@�C�����i�[����
bool AMapCreator::SetCSVToFString(const UDataTable* _mapData, TArray<FString>& _stringArray, const int _rowIndex)
{
	bool isLoad = true;

	// �����z�񃊃Z�b�g
	_stringArray.Reset();

	// �}�b�v�f�[�^������
	if (_mapData != nullptr)
	{
		// int�̍s�ԍ���FName�ɕϊ����邽�߂ɁAFString�ɕϊ�����iFName�ւ̕ϊ���FString���炵���ł��Ȃ����߁j
		FString fStrIndex = FString::FromInt(_rowIndex + 1);

		// FString����FName�ɕϊ�����
		FName fNameIndex = FName(*fStrIndex);

		// �s���Ƃ̃f�[�^�̎擾
		FMapStructCpp* rowData = _mapData->FindRow<FMapStructCpp>(fNameIndex, FString());

		// �f�[�^�s�̏I���
		if (rowData == nullptr)
		{
			// �f�[�^�ǂݍ��ݏI��
			UE_LOG(LogTemp, Warning, TEXT("[MapCreator] rowData is nullptr. (rowIndex = %s)"), *fStrIndex);
			isLoad = false;
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
					_stringArray.Add(*(&rowData->Line_1 + i));
					UE_LOG(LogTemp, Verbose, TEXT("[MapCreator] Set Data. (%s)"), *(*(&rowData->Line_1 + i)));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("[MapCreator] Could Not Get rowData!"));
				}
			}

			// ������z��̒�������
			m_StrMapLength = _stringArray.Num();
		}
	}
	// �}�b�v�f�[�^���Ȃ�
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[MapCreator] MapData is nullptr."));
		isLoad = false;
	}

	return isLoad;
}

// ������̔�r���s���A��v�������̏����i�[����
void AMapCreator::ComparisonChar(TArray<FMapActorStructCpp>& _generateActor, TArray<FString>& _stringArray, const int _rowIndex, TArray<CreateData>& _generateInfoArray)
{
	// ������z��̗v�f�����������s��
	for (int columnIndex = 0; columnIndex < _stringArray.Num(); ++columnIndex)
	{
		// ������̍ŏ�
		if (columnIndex == 0)
		{
			// ���𐶐����Ȃ������ł͂Ȃ�
			if (_stringArray[0] != m_NotGroundGenerateStr)
			{
				// ���̐�����ǉ�
				CreateData groundData(m_MapActorGround, _rowIndex, m_StrMapLength / 2, "Ground", MapPlacementPattern::Single);
				_generateInfoArray.Add(groundData);
			}
		}

		// �������镶���񂪋󔒂Ȃ��΂�
		if (_stringArray[columnIndex] == "")
		{
			continue;
		}

		// �v���C���[���������ƈ�v
		if (m_IsGeneratePlayer == false && _stringArray[columnIndex] == m_PlayerGenerateStr)
		{
			// �v���C���[�𐶐����X�g�ɒǉ����ă��[�v��蒼��
			CreateData playerData(m_PlayerActor, _rowIndex, columnIndex, "Player", MapPlacementPattern::Single);
			_generateInfoArray.Add(playerData);
			continue;
		}

		// ������ƈ�v����Actor����������

		// �����ƈ�v�t���O
		bool isFound = false;

		// �ݒ肵����������Actor�̐������������s��
		for (int actorIndex = 0; actorIndex < _generateActor.Num(); ++actorIndex)
		{
			// �c�������ǂ���
			bool isVert = false;

			// �����^�C�v�ɂ���Ĕ�r���镶�����ύX����
			// �P�̃^�C�v �������� �A�������^�C�v
			if (_generateActor[actorIndex].geterateType == MapPlacementPattern::Single/* ||
				_generateActor[actorIndex].geterateType == MapPlacementPattern::Continuous ||
				_generateActor[actorIndex].geterateType == MapPlacementPattern::V_Continuous*/)
			{
				if (_stringArray[columnIndex] == _generateActor[actorIndex].generateChar)
				{
					// ������Ԃɂ���
					isFound = true;

					// ��������v��������Actor�𐶐����X�g�ɒǉ����Č������[�v�I��
					CreateData playerData(_generateActor[actorIndex], _rowIndex, columnIndex, _generateActor[actorIndex].generateChar, _generateActor[actorIndex].geterateType, false);
					_generateInfoArray.Add(playerData);
					break;
				}
			}
			// �t�F���X�����^�C�v
			else if (_generateActor[actorIndex].geterateType == MapPlacementPattern::Fence ||
				_generateActor[actorIndex].geterateType == MapPlacementPattern::V_Fence)
			{
				if (_stringArray[columnIndex] == _generateActor[actorIndex].generateCharStart)
				{
					// ������Ԃɂ���
					isFound = true;

					// ��������v��������Actor�𐶐����X�g�ɒǉ����Č������[�v�I��
					CreateData playerData(_generateActor[actorIndex], _rowIndex, columnIndex, _generateActor[actorIndex].generateCharStart, _generateActor[actorIndex].geterateType, true, true);
					_generateInfoArray.Add(playerData);
					break;
				}
				else if (_stringArray[columnIndex] == _generateActor[actorIndex].generateCharEnd)
				{
					// ������Ԃɂ���
					isFound = true;

					// ��������v��������Actor�𐶐����X�g�ɒǉ����Č������[�v�I��
					CreateData playerData(_generateActor[actorIndex], _rowIndex, columnIndex, _generateActor[actorIndex].generateCharEnd, _generateActor[actorIndex].geterateType, true, false);
					_generateInfoArray.Add(playerData);
					break;
				}
			}
			// ���̑��̏ꍇ
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[MapCreator] The \"Generate Type\" set in MapActorArray[%d] is invalid."), actorIndex);
			}
		}

		// CSV�t�@�C���̕�����ƈ�v���镶���񂪃}�b�v�ɐ�������Actor�ɐݒ肳��Ă��Ȃ�
		if (isFound == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("[MapCreator] The string \"%s\" is set in the CSV file, but not in the MapActorArray. Check both the CSV file and the MapActorArray."), *_stringArray[columnIndex])
		}
	}
}

// ���ɕ��񂾃t�F���X�̕R�t�����s��
void AMapCreator::LinkingFence(TArray<CreateData>& _generateInfoArray)
{
	// �������X�g�Ƀf�[�^���Ȃ�
	if (_generateInfoArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MapCreator] There is no data in _generateInfoArray."));
		return;
	}
	// �f�[�^������
	else
	{
		// �R�t���Ɏg�p����l
		int linkIndex = 0;

		// �t�F���X�J�n��ێ�
		bool isStart = false;

		// �s���ƂɌ���
		for (int row = 0; row < m_MapRowNumber; ++row)
		{
			// �������X�g�̌���
			for (int actorIndex = _generateInfoArray.Num() - 1; actorIndex >= 0; --actorIndex)
			{
				// �c�̃t�F���X�̏ꍇ
				if (_generateInfoArray[actorIndex].generatePattern == MapPlacementPattern::Fence &&
					_generateInfoArray[actorIndex].rowIndex == row)
				{
					// �t�F���X�����J�n�̏ꍇ
					if (_generateInfoArray[actorIndex].fenceStart == true)
					{
						// �������ł����
						if (isStart == false)
						{
							// ������Ԃ�
							isStart = true;

							// �ԍ��ŕR�t����
							_generateInfoArray[actorIndex].vertLinkNum = linkIndex;

							// �J�n�ʒu�̐ݒ�
							AddLinkIndex(_generateInfoArray[actorIndex].generateActorStruct, linkIndex, m_FenceData, _generateInfoArray[actorIndex].columnIndex, true);
						}
						// �������ł���΃��O
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("[MapCreator] A fence generation character was found during vertical fence generation."))
						}
					}
					// �t�F���X�����I���̏ꍇ
					else if (_generateInfoArray[actorIndex].fenceStart == false)
					{
						// �������ł����
						if (isStart == true)
						{
							// ������Ԃ̏I��
							isStart = false;

							// �ԍ��ŕR�t����
							_generateInfoArray[actorIndex].vertLinkNum = linkIndex;

							// �I���ʒu�̐ݒ�
							AddLinkIndex(_generateInfoArray[actorIndex].generateActorStruct, linkIndex, m_FenceData, _generateInfoArray[actorIndex].columnIndex, false);

							// �X�P�[���̕ύX
							//�R�t����ꂽ�ԍ�����v�f�ԍ����擾
							int fenceDataIndex = GetLinkIndex(linkIndex, m_FenceData);

							// Y�����̃X�P�[����ύX
							if (fenceDataIndex >= 0 && fenceDataIndex < m_FenceData.Num())
							{
								if (_generateInfoArray[actorIndex].generateActorStruct.isScaleXAxis)
								{
									m_FenceData[fenceDataIndex].scale.X =
										ContinuousScale(m_FenceData[fenceDataIndex].startIndex, m_FenceData[fenceDataIndex].endIndex, _generateInfoArray[actorIndex].generateActorStruct.scale.X);
								}
								else
								{
									m_FenceData[fenceDataIndex].scale.Y =
										ContinuousScale(m_FenceData[fenceDataIndex].startIndex, m_FenceData[fenceDataIndex].endIndex, _generateInfoArray[actorIndex].generateActorStruct.scale.Y);
								}

								// �R�t���ԍ��̃C���N�������g
								++linkIndex;
							}
							else
							{
								UE_LOG(LogTemp, Error, TEXT("[MapCreator] fenceDataIndex is invaild."))
							}
						}
						// �������łȂ���΃��O
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("[MapCreator] A closing character was found before the fence generation character."))
						}
					}
				}
			}
		}
	}
}

// ���ւ̘A�������̐ݒ���s��
/*void AMapCreator::LinkingContinuous(TArray<CreateData>& _generateInfoArray)
{
	// �������X�g�Ƀf�[�^���Ȃ�
	if (_generateInfoArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("There is no data in _generateInfoArray."));
		return;
	}
	// �f�[�^������
	else
	{
		// �R�t���Ɏg�p����l
		int linkIndex = 0;
		// �A�������J�n��ێ�
		bool isStart = false;
		// ������ێ�
		FString stringTemp = "";
		// �s��ێ�
		int columnIndex = 0;
		// �O��Actor�̔z��ԍ���ێ�
		int prevIndex = 0;

		// �s���ƂɌ���
		for (int row = m_MapRowNumber; row >= 0; --row)
		{
			// �������X�g�̌���
			for (int actorIndex = _generateInfoArray.Num() - 1; actorIndex >= 0; --actorIndex)
			{
				// �s����v���鉡�̘A�������̏ꍇ
				if (_generateInfoArray[actorIndex].generatePattern == MapPlacementPattern::Continuous &&
					_generateInfoArray[actorIndex].rowIndex == row)
				{
					// �ێ����Ă��镶������
					if (stringTemp == "")
					{
						// �A�������̊J�n
						isStart = true;
						// �����̕ۑ�
						stringTemp = _generateInfoArray[actorIndex].generateString;
						// ��̕ۑ�
						columnIndex = _generateInfoArray[actorIndex].columnIndex;
						// �z��ԍ��̕ۑ�
						prevIndex = actorIndex;
						// �J�n�ʒu�̐ݒ�
						AddLinkIndex(_generateInfoArray[actorIndex].generateActorStruct, linkIndex, m_ContinuousData, columnIndex, true);

						UE_LOG(LogTemp, Error, TEXT("start value = %d"), columnIndex);

						// �R�t����
						_generateInfoArray[actorIndex].vertLinkNum = linkIndex;

						// ���[�v����蒼��
						continue;
					}

					// ��ԍ����A�ԂɂȂ��Ă��Ȃ� �������� �������ȑO�ƈႤ�i�A���������r�؂ꂽ�j
					if ((columnIndex + 1) != _generateInfoArray[actorIndex].rowIndex ||
						stringTemp != _generateInfoArray[actorIndex].generateString)
					{
						// �A�������̏I��
						isStart = false;
						// �O��Actor�̏I���ʒu�̐ݒ�
						AddLinkIndex(_generateInfoArray[actorIndex].generateActorStruct, linkIndex, m_ContinuousData, columnIndex + 1, false);

						UE_LOG(LogTemp, Error, TEXT("end value = %d"), columnIndex);

						_generateInfoArray[prevIndex].vertLinkNum = linkIndex;

						// �X�P�[���̕ύX
						//�R�t����ꂽ�ԍ�����v�f�ԍ����擾
						int continuousDataIndex = GetLinkIndex(linkIndex, m_ContinuousData);

						// Y�����̃X�P�[����ύX
						m_ContinuousData[continuousDataIndex].scale.Y =
							ContinuousScale(m_ContinuousData[continuousDataIndex].startIndex, m_ContinuousData[continuousDataIndex].endIndex, _generateInfoArray[actorIndex].generateActorStruct.scale.Y);

						// �قȂ�Actor�ōĐݒ肷��
						// �R�t���ԍ��̃C���N�������g
						++linkIndex;

						// �����̕ۑ�
						stringTemp = _generateInfoArray[actorIndex].generateString;
						// �s�̕ۑ�
						columnIndex = _generateInfoArray[actorIndex].rowIndex;
						// �z��ԍ��̕ۑ�
						prevIndex = actorIndex;
						// �J�n�ʒu�̐ݒ�
						AddLinkIndex(_generateInfoArray[actorIndex].generateActorStruct, linkIndex, m_ContinuousData, columnIndex, true);
						// �R�t����
						_generateInfoArray[actorIndex].vertLinkNum = linkIndex;
					}
					// �O�Ɠ���������
					else
					{
						// �A���������
						isStart = true;
						// �s�̕ۑ�
						columnIndex = _generateInfoArray[actorIndex].rowIndex;
						// �z��ԍ��̕ۑ�
						prevIndex = actorIndex;
						// �R�t����
						_generateInfoArray[actorIndex].vertLinkNum = linkIndex;
					}

				}
			}
			// ���[�v���I����Ă��������I����Ă��Ȃ��ꍇ
			if (isStart == true)
			{
				// �A�������̏I��
				isStart = false;
				// �O��Actor�̏I���ʒu�̐ݒ�
				AddLinkIndex(_generateInfoArray[prevIndex].generateActorStruct, linkIndex, m_ContinuousData, columnIndex + 1, false);
				// �O��Actor��R�t����
				_generateInfoArray[prevIndex].vertLinkNum = linkIndex;

				// �X�P�[���̕ύX
				//�R�t����ꂽ�ԍ�����v�f�ԍ����擾
				int continuousDataIndex = GetLinkIndex(linkIndex, m_ContinuousData);

				// Y�����̃X�P�[����ύX
				m_ContinuousData[continuousDataIndex].scale.Y =
					ContinuousScale(m_ContinuousData[continuousDataIndex].startIndex, m_ContinuousData[continuousDataIndex].endIndex, _generateInfoArray[prevIndex].generateActorStruct.scale.Y);


				++linkIndex;
			}
		}
	}
}*/

// �c�ɕ��񂾃t�F���X�̕R�t�����s��
void AMapCreator::LinkingVerticalFence(TArray<CreateData>& _generateInfoArray)
{
	// �������X�g�Ƀf�[�^���Ȃ�
	if (_generateInfoArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MapCreator] There is no data in _generateInfoArray."));
		return;
	}
	// �f�[�^������
	else
	{
		// �R�t���Ɏg�p����l
		int linkIndex = 0;

		// �t�F���X�J�n��ێ�
		bool isStart = false;

		// �񂲂ƂɌ���
		for (int column = 0; column < m_StrMapLength; ++column)
		{
			// �������X�g�̌���
			for (int actorIndex = 0; actorIndex < _generateInfoArray.Num(); ++actorIndex)
			{
				// �񂪈�v����c�̃t�F���X�̏ꍇ
				if (_generateInfoArray[actorIndex].generatePattern == MapPlacementPattern::V_Fence &&
					_generateInfoArray[actorIndex].columnIndex == column)
				{
					// �t�F���X�����J�n�̏ꍇ
					if (_generateInfoArray[actorIndex].fenceStart == true)
					{
						// �������ł����
						if (isStart == false)
						{
							// ������Ԃ�
							isStart = true;

							// �ԍ��ŕR�t����
							_generateInfoArray[actorIndex].vertLinkNum = linkIndex;

							// �J�n�ʒu�̐ݒ�
							AddLinkIndex(_generateInfoArray[actorIndex].generateActorStruct, linkIndex, m_VerticalFenceData, _generateInfoArray[actorIndex].rowIndex, true);
						}
						// �������ł���΃��O
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("[MapCreator] A fence generation character was found during vertical fence generation."))
						}
					}
					// �t�F���X�����I���̏ꍇ
					else if (_generateInfoArray[actorIndex].fenceStart == false)
					{
						// �������ł����
						if (isStart == true)
						{
							// ������Ԃ̏I��
							isStart = false;

							// �ԍ��ŕR�t����
							_generateInfoArray[actorIndex].vertLinkNum = linkIndex;

							// �I���ʒu�̐ݒ�
							AddLinkIndex(_generateInfoArray[actorIndex].generateActorStruct, linkIndex, m_VerticalFenceData, _generateInfoArray[actorIndex].rowIndex, false);

							// �X�P�[���̕ύX
							//�R�t����ꂽ�ԍ�����v�f�ԍ����擾
							int fenceDataIndex = GetLinkIndex(linkIndex, m_VerticalFenceData);

							// Y�����̃X�P�[����ύX
							if (fenceDataIndex >= 0 && fenceDataIndex < m_FenceData.Num())
							{
								if (_generateInfoArray[actorIndex].generateActorStruct.isScaleXAxis)
								{
									m_VerticalFenceData[fenceDataIndex].scale.X =
										ContinuousScale(m_VerticalFenceData[fenceDataIndex].startIndex, m_VerticalFenceData[fenceDataIndex].endIndex, _generateInfoArray[actorIndex].generateActorStruct.scale.X);
								}
								else
								{
									m_VerticalFenceData[fenceDataIndex].scale.Y =
										ContinuousScale(m_VerticalFenceData[fenceDataIndex].startIndex, m_VerticalFenceData[fenceDataIndex].endIndex, _generateInfoArray[actorIndex].generateActorStruct.scale.Y);
								}

								// �R�t���ԍ��̃C���N�������g
								++linkIndex;
							}
							else
							{
								UE_LOG(LogTemp, Error, TEXT("[MapCreator] fenceDataIndex is invaild."))
							}
						}
						// �������łȂ���΃��O
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("[MapCreator] A closing character was found before the vertical fence generation character."))
						}
					}
				}
			}
		}
	}
}

// �c�ւ̘A�������̐ݒ���s��
/*void AMapCreator::LinkingVerticalContinuous(TArray<CreateData>& _generateInfoArray)
{
	// �������X�g�Ƀf�[�^���Ȃ�
	if (_generateInfoArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("There is no data in _generateInfoArray."));
		return;
	}
	// �f�[�^������
	else
	{
		// �R�t���Ɏg�p����l
		int linkIndex = 0;
		// �A�������J�n��ێ�
		bool isStart = false;
		// ������ێ�
		FString stringTemp = "";
		// �s��ێ�
		int rowTemp = 0;
		// �O��Actor�̔z��ԍ���ێ�
		int prevIndex = 0;

		// �񂲂ƂɌ���
		for (int column = 0; column < m_StrMapLength; ++column)
		{
			// �������X�g�̌���
			for (int actorIndex = 0; actorIndex < _generateInfoArray.Num(); ++actorIndex)
			{
				// �񂪈�v����c�̘A�������̏ꍇ
				if (_generateInfoArray[actorIndex].generatePattern == MapPlacementPattern::V_Continuous &&
					_generateInfoArray[actorIndex].columnIndex == column)
				{
					// �ێ����Ă��镶������
					if (stringTemp == "")
					{
						// �A�������̊J�n
						isStart = true;
						// �����̕ۑ�
						stringTemp = _generateInfoArray[actorIndex].generateString;
						// �s�̕ۑ�
						rowTemp = _generateInfoArray[actorIndex].rowIndex;
						// �z��ԍ��̕ۑ�
						prevIndex = actorIndex;
						// �J�n�ʒu�̐ݒ�
						AddLinkIndex(_generateInfoArray[actorIndex].generateActorStruct, linkIndex, m_VerticalContinuousData, rowTemp, true);

						// �R�t����
						_generateInfoArray[actorIndex].vertLinkNum = linkIndex;

						// ���[�v����蒼��
						continue;
					}

					// �s�ԍ����A�ԂɂȂ��Ă��Ȃ� �������� �������ȑO�ƈႤ�i�A���������r�؂ꂽ�j
					if ((rowTemp + 1) != _generateInfoArray[actorIndex].rowIndex ||
						stringTemp != _generateInfoArray[actorIndex].generateString)
					{
						// �A�������̏I��
						isStart = false;
						// �O��Actor�̏I���ʒu�̐ݒ�
						AddLinkIndex(_generateInfoArray[actorIndex].generateActorStruct, linkIndex, m_VerticalContinuousData, rowTemp + 1, false);
						// �O��Actor��R�t����
						_generateInfoArray[prevIndex].vertLinkNum = linkIndex;

						// �X�P�[���̕ύX
						//�R�t����ꂽ�ԍ�����v�f�ԍ����擾
						int continuousDataIndex = GetLinkIndex(linkIndex, m_VerticalContinuousData);

						// Y�����̃X�P�[����ύX
						m_VerticalContinuousData[continuousDataIndex].scale.X =
							ContinuousScale(m_VerticalContinuousData[continuousDataIndex].startIndex, m_VerticalContinuousData[continuousDataIndex].endIndex, _generateInfoArray[actorIndex].generateActorStruct.scale.Y);

						// �قȂ�Actor�ōĐݒ肷��
						// �R�t���ԍ��̃C���N�������g
						++linkIndex;

						// �����̕ۑ�
						stringTemp = _generateInfoArray[actorIndex].generateString;
						// �s�̕ۑ�
						rowTemp = _generateInfoArray[actorIndex].rowIndex;
						// �z��ԍ��̕ۑ�
						prevIndex = actorIndex;
						// �J�n�ʒu�̐ݒ�
						AddLinkIndex(_generateInfoArray[actorIndex].generateActorStruct, linkIndex, m_VerticalContinuousData, rowTemp, true);
						// �R�t����
						_generateInfoArray[actorIndex].vertLinkNum = linkIndex;
					}
					// �O�Ɠ���������
					else
					{
						// �A���������
						isStart = true;
						// �s�̕ۑ�
						rowTemp = _generateInfoArray[actorIndex].rowIndex;
						// �z��ԍ��̕ۑ�
						prevIndex = actorIndex;
						// �R�t����
						_generateInfoArray[actorIndex].vertLinkNum = linkIndex;
					}

				}
			}
			// ���[�v���I����Ă��������I����Ă��Ȃ��ꍇ
			if (isStart == true)
			{
				// �A�������̏I��
				isStart = false;
				// �O��Actor�̏I���ʒu�̐ݒ�
				AddLinkIndex(_generateInfoArray[prevIndex].generateActorStruct, linkIndex, m_VerticalContinuousData, rowTemp + 1, false);
				// �O��Actor��R�t����
				_generateInfoArray[prevIndex].vertLinkNum = linkIndex;
			}
		}
	}
}*/

// ContinuousData�^��TArray�ɗv�f���܂܂�Ă邩�m�F����
int AMapCreator::GetLinkIndex(int _linkIndex, const TArray<ContinuousData> _array)
{
	int result = -1;

	for (int i = 0; result == -1 && i < _array.Num(); ++i)
	{
		if (_array[i].linkIndex == _linkIndex)
		{
			result = i;
		}
	}

	return result;
}

// ContinuousData�^��TArray��LinkIndex�̗v�f��������
void AMapCreator::AddLinkIndex(FMapActorStructCpp _actorStruct, int _linkIndex, TArray<ContinuousData>& _array, int _value, bool isStart/* = true*/)
{
	ContinuousData vertTemp = { _linkIndex , 0, 0, _actorStruct.scale };

	int loopCnt = 0;
	int index = GetLinkIndex(_linkIndex, _array);

	while (index == -1 && loopCnt < 100)
	{
		_array.Add(vertTemp);
		index = GetLinkIndex(_linkIndex, _array);
		++loopCnt;
	}

	if (index == -1)
	{
		UE_LOG(LogTemp, Error, TEXT("[MapCreator] THE END"));
	}

	if (isStart)
	{
		_array[index].startIndex = _value;
	}
	else
	{
		_array[index].endIndex = _value;
	}

	UE_LOG(LogTemp, Verbose, TEXT("[MapCreator] Array[%d] element  link : %d, start : %d, end : %d"),
		index, _array[index].linkIndex, _array[index].startIndex, _array[index].endIndex);
}

// �}�b�v��������ݒ�
void AMapCreator::SettingMap(bool isRegenerate/* = false*/)
{
	if (m_MapActorCreateData.Num() == 0 || isRegenerate == true)
	{
		// �������X�g������
		m_MapActorCreateData.Reset();

		// ������z�񏉊���
		m_StrMapArray.Reset();

		// �s�ԍ�
		for (int rowIndex = 0;; ++rowIndex)
		{
			// CSV�t�@�C������s������z��ɑ�����邱�Ƃ��ł����
			if (SetCSVToFString(m_MapData, m_StrMapArray, rowIndex))
			{
				// ������ꂽ�����񂩂琶������Actor�𐶐����X�g�ɒǉ�
				ComparisonChar(m_MapActorArray, m_StrMapArray, rowIndex, m_MapActorCreateData);
			}
			// �ł��Ȃ���΃��[�v�I��
			else
			{
				m_MapRowNumber = rowIndex;
				UE_LOG(LogTemp, Verbose, TEXT("[MapCreator] The number of rows in the map is %d."), m_MapRowNumber);
				break;
			}
		}
		LinkingFence(m_MapActorCreateData);
		//LinkingContinuous(m_MapActorCreateData);
		LinkingVerticalFence(m_MapActorCreateData);
		//LinkingVerticalContinuous(m_MapActorCreateData);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[MapCreator] Skipped adding to generating list because the list is already set."));
	}
}

// �������s��
void AMapCreator::MapCreate()
{
	// �A�������̍ۂɏd�����Ȃ��悤�ɂ���
	int continuousLinkIndex = 0;		// ���A��
	int fenceLinkIndex = 0;		// ���t�F���X
	int vertContinuousLinkIndex = 0;		// �c�A��
	int vertFenceLinkIndex = 0;		// �c�t�F���X

	// �������X�g������
	for (int index = 0; index < m_MapActorCreateData.Num(); ++index)
	{
		switch (m_MapActorCreateData[index].generatePattern)
		{
		case MapPlacementPattern::Single:
		{
			// Actor���������������
			SpawnMapActor
			(
				m_MapActorCreateData[index].generateActorStruct,
				LocationX(m_MapActorCreateData[index].rowIndex),
				LocationY(m_MapActorCreateData[index].columnIndex, m_StrMapLength)
			);
			break;
		}

		/*
		case MapPlacementPattern::Continuous:
		{
			// ���̘A�������Ɏg�p���鐶���f�[�^�̗v�f�ԍ����i�[����
			int continuousDataIndex = GetLinkIndex(m_MapActorCreateData[index].vertLinkNum, m_ContinuousData);

			if (continuousLinkIndex == continuousDataIndex)
			{
				FMapActorStructCpp actorTemp = m_MapActorCreateData[index].generateActorStruct;
				actorTemp.scale = m_ContinuousData[continuousDataIndex].scale;

				// Actor���������������
				SpawnMapActor
				(
					actorTemp,
					LocationX(m_MapActorCreateData[index].rowIndex),
					LocationY((m_ContinuousData[continuousDataIndex].startIndex + m_ContinuousData[continuousDataIndex].endIndex) / 2.0f, m_StrMapLength)
				);

				UE_LOG(LogTemp, Error, TEXT("start = %d, end = %d"), m_ContinuousData[continuousDataIndex].startIndex, m_ContinuousData[continuousDataIndex].endIndex);

				// ���̘A������Actor�̐�����
				++continuousLinkIndex;
			}
			break;
		}

		case MapPlacementPattern::V_Continuous:
		{
			// �c�̘A�������Ɏg�p���鐶���f�[�^�̗v�f�ԍ����i�[����
			int continuousDataIndex = GetLinkIndex(m_MapActorCreateData[index].vertLinkNum, m_VerticalContinuousData);

			if (vertContinuousLinkIndex == continuousDataIndex)
			{
				FMapActorStructCpp actorTemp = m_MapActorCreateData[index].generateActorStruct;
				actorTemp.scale = m_VerticalContinuousData[continuousDataIndex].scale;

				// Actor���������������
				SpawnMapActor
				(
					actorTemp,
					LocationX((m_VerticalContinuousData[continuousDataIndex].startIndex + m_VerticalContinuousData[continuousDataIndex].endIndex) / 2.0f),
					LocationY(m_MapActorCreateData[index].columnIndex, m_StrMapLength)
				);

				// ���̘A������Actor�̐�����
				++vertContinuousLinkIndex;
			}
			break;
		}*/

		case MapPlacementPattern::Fence:
		{
			// �c�̃t�F���X�����Ɏg�p���鐶���f�[�^�̗v�f�ԍ����i�[����
			int fenceDataIndex = GetLinkIndex(m_MapActorCreateData[index].vertLinkNum, m_FenceData);

			if (fenceLinkIndex == fenceDataIndex)
			{
				FMapActorStructCpp actorTemp = m_MapActorCreateData[index].generateActorStruct;
				actorTemp.scale = m_FenceData[fenceDataIndex].scale;

				// Actor���������������
				SpawnMapActor
				(
					actorTemp,
					LocationX((m_FenceData[fenceDataIndex].startIndex + m_FenceData[fenceDataIndex].endIndex) / 2.0f),
					LocationY(m_MapActorCreateData[index].columnIndex, m_StrMapLength)
				);

				// ���̘A������Actor�̐�����
				++fenceLinkIndex;
			}
			break;
		}

		case MapPlacementPattern::V_Fence:
		{
			// �c�̃t�F���X�����Ɏg�p���鐶���f�[�^�̗v�f�ԍ����i�[����
			int fenceDataIndex = GetLinkIndex(m_MapActorCreateData[index].vertLinkNum, m_VerticalFenceData);

			if (vertFenceLinkIndex == fenceDataIndex)
			{
				FMapActorStructCpp actorTemp = m_MapActorCreateData[index].generateActorStruct;
				actorTemp.scale = m_VerticalFenceData[fenceDataIndex].scale;

				// Actor���������������
				SpawnMapActor
				(
					actorTemp,
					LocationX((m_VerticalFenceData[fenceDataIndex].startIndex + m_VerticalFenceData[fenceDataIndex].endIndex) / 2.0f),
					LocationY(m_MapActorCreateData[index].columnIndex, m_StrMapLength)
				);

				// ���̘A������Actor�̐�����
				++vertFenceLinkIndex;
			}
			break;
		}

		default:
			UE_LOG(LogTemp, Warning, TEXT("[MapCreator] MapPlacementPattern is invalid! enum index number : %d"), (int)m_MapActorCreateData[index].generatePattern);
			break;
		}
	}
}

// �X�^�e�B�b�N���b�V���ɂ�鐶�����s��
void AMapCreator::MapCreateEditor()
{
	// �A�������̍ۂɏd�����Ȃ��悤�ɂ���
	int continuousLinkIndex = 0;		// ���A��
	int fenceLinkIndex = 0;		// ���t�F���X
	int vertContinuousLinkIndex = 0;		// �c�A��
	int vertFenceLinkIndex = 0;		// �c�t�F���X
	
	// �������X�g������
	for (int index = 0; index < m_MapActorCreateData.Num(); ++index)
	{
		UE_LOG(LogTemp, Error, TEXT("[MapCreator] index = %d."), index)
		// ���Ԗڂ�Actor�Ȃ̂�������
		int actorArrayIndex = GetMapActorArrayIndex(m_MapActorCreateData[index].generateActorStruct);
		if (actorArrayIndex == -1)
		{
			// �����ǂ�������
			if (m_MapActorCreateData[index].generateString != "Ground")
			{
				UE_LOG(LogTemp, Warning, TEXT("[MapCreator] Could not get the index of the Actor."));
					continue;
			}
		}
		actorArrayIndex++;
		UE_LOG(LogTemp, Error, TEXT("[MapCreator] actorArrayIndex = %d."), actorArrayIndex)

		switch (m_MapActorCreateData[index].generatePattern)
		{
		case MapPlacementPattern::Single:
		{
			// Actor���������������
			AddMapInstanceStaticMesh
			(
				m_SampleMapObject[actorArrayIndex],
				m_MapActorCreateData[index].generateActorStruct,
				LocationX(m_MapActorCreateData[index].rowIndex),
				LocationY(m_MapActorCreateData[index].columnIndex, m_StrMapLength)
			);
			break;
		}

		case MapPlacementPattern::Fence:
		{
			// �c�̃t�F���X�����Ɏg�p���鐶���f�[�^�̗v�f�ԍ����i�[����
			int fenceDataIndex = GetLinkIndex(m_MapActorCreateData[index].vertLinkNum, m_FenceData);

			if (fenceLinkIndex == fenceDataIndex)
			{
				FMapActorStructCpp actorTemp = m_MapActorCreateData[index].generateActorStruct;
				actorTemp.scale = m_FenceData[fenceDataIndex].scale;

				// Actor���������������
				AddMapInstanceStaticMesh
				(
					m_SampleMapObject[actorArrayIndex],
					actorTemp,
					LocationX((m_FenceData[fenceDataIndex].startIndex + m_FenceData[fenceDataIndex].endIndex) / 2.0f),
					LocationY(m_MapActorCreateData[index].columnIndex, m_StrMapLength)
				);

				// ���̘A������Actor�̐�����
				++fenceLinkIndex;
			}
			break;
		}

		case MapPlacementPattern::V_Fence:
		{
			// �c�̃t�F���X�����Ɏg�p���鐶���f�[�^�̗v�f�ԍ����i�[����
			int fenceDataIndex = GetLinkIndex(m_MapActorCreateData[index].vertLinkNum, m_VerticalFenceData);

			if (vertFenceLinkIndex == fenceDataIndex)
			{
				FMapActorStructCpp actorTemp = m_MapActorCreateData[index].generateActorStruct;
				actorTemp.scale = m_VerticalFenceData[fenceDataIndex].scale;

				// Actor���������������
				AddMapInstanceStaticMesh
				(
					m_SampleMapObject[actorArrayIndex],
					actorTemp,
					LocationX((m_VerticalFenceData[fenceDataIndex].startIndex + m_VerticalFenceData[fenceDataIndex].endIndex) / 2.0f),
					LocationY(m_MapActorCreateData[index].columnIndex, m_StrMapLength)
				);

				// ���̘A������Actor�̐�����
				++vertFenceLinkIndex;
			}
			break;
		}

		default:
			UE_LOG(LogTemp, Warning, TEXT("[MapCreator] MapPlacementPattern is invalid! enum index number : %d"), (int)m_MapActorCreateData[index].generatePattern);
			break;
		}
	}
}

// FMapActorStructCpp�̗v�f�����Ԗڂ����ׂ�
int AMapCreator::GetMapActorArrayIndex(FMapActorStructCpp _mapActorStruct)
{
	for (int i = 0; i < m_MapActorArray.Num(); ++i)
	{
		if (_mapActorStruct == m_MapActorArray[i])
		{
			return i;
		}
	}

	return -1;
}