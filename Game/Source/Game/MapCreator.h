// �}�b�v�̎����z�u���s���N���X

// 2020/12/02 �n� ���� �쐬

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyChara.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "MapCreator.generated.h"

class UInstancedStaticMeshComponent;
class UStaticMeshComponent;

// �}�b�v�ɔz�u���鎞�̐����p�^�[��
UENUM(BlueprintType)
enum class MapPlacementPattern : uint8
{
	SettingLock		UMETA(Hidden),									// �ݒ�s��
	Single			UMETA(DisplayName = "Single"),					// �P�̂Ŕz�u����
	Continuous		UMETA(DisplayName = "Continuous (Horizontal)"),	// �A���z�u�ň��Actor�ɂȂ�i�������j
	V_Continuous	UMETA(DisplayName = "Continuous (Vertical)"),	// �A���z�u�ň��Actor�ɂȂ�i�c�����j
	Fence			UMETA(DisplayName = "Fence (Horizontal)"),		// �n�_�ƏI�_���w�肵�Ĉ��Actor�𐶐�����i�������j
	V_Fence			UMETA(DisplayName = "Fence (Vertical)"),		// �n�_�ƏI�_���w�肵�Ĉ��Actor�𐶐�����i�c�����j
};

// �}�b�v�ɔz�u����Actor�̍\����
USTRUCT(BlueprintType)
struct FMapActorStructCpp
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		TSubclassOf<class AActor> actor = nullptr;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		float location_Z = 0.0f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FRotator rotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FVector scale = FVector::OneVector;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly,
		Meta = (EditCondition = "geterateType != MapPlacementPattern::Fence && geterateType != MapPlacementPattern::V_Fence && geterateType != MapPlacementPattern::SettingLock"))
		FString generateChar = "";

	UPROPERTY(EditAnyWhere, BlueprintReadOnly,
		Meta = (EditCondition = "geterateType == MapPlacementPattern::Fence || geterateType == MapPlacementPattern::V_Fence && geterateType != MapPlacementPattern::SettingLock"))
		FString generateCharStart = "";

	UPROPERTY(EditAnyWhere, BlueprintReadOnly,
		Meta = (EditCondition = "geterateType == MapPlacementPattern::Fence || geterateType == MapPlacementPattern::V_Fence && geterateType != MapPlacementPattern::SettingLock"))
		FString generateCharEnd = "";

	UPROPERTY(EditAnyWhere, BlueprintReadOnly,
		Meta = (EditCondition = "geterateType != MapPlacementPattern::SettingLock"))
		MapPlacementPattern geterateType = MapPlacementPattern::Single;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly,
		Meta = (EditCondition = "geterateType != MapPlacementPattern::SettingLock"))
		EEnemyMoveType enemyMoveType = EEnemyMoveType::None;
};

// �}�b�vCSV�p�\����
USTRUCT()
struct FMapStructCpp : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FString Line_1;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FString Line_2;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FString Line_3;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FString Line_4;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FString Line_5;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FString Line_6;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FString Line_7;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FString Line_8;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FString Line_9;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FString Line_10;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FString Line_11;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FString Line_12;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FString Line_13;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FString Line_14;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FString Line_15;

	UPROPERTY(BlueprintreadOnly)
		int indexNum = 15;
};

UCLASS()
class GAME_API AMapCreator : public AActor
{
	GENERATED_BODY()

public:
	AMapCreator();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	// private�ϐ�
private:
	// ���b�V����\������
	UPROPERTY(EditAnyWhere)
		bool m_VisibleMapWire;

	// �}�b�v�f�[�^
	UPROPERTY(EditAnyWhere)
		UDataTable* m_MapData;

	// �}�b�vCSV�ŏ��𐶐����Ȃ����̕�����
	UPROPERTY(EditAnyWhere)
		FString m_NotGroundGenerateStr;

	// X���̃I�t�Z�b�g
	UPROPERTY(EditAnyWhere)
		float m_XAxis_Offset;

	// Y���̃I�t�Z�b�g
	UPROPERTY(EditAnyWhere)
		float m_YAxis_Offset;

	// ����Actor
	UPROPERTY(EditAnyWhere)
		FMapActorStructCpp m_MapActorGround;

	// �v���C���[Actor�����̕���
	UPROPERTY(EditAnyWhere)
		FString m_PlayerGenerateStr;

	// �v���C���[��Actor
	UPROPERTY(EditAnyWhere)
		FMapActorStructCpp m_PlayerActor;

	// ���ȊO�̂̐���Actor
	UPROPERTY(EditAnyWhere)
		TArray<FMapActorStructCpp> m_MapActorArray;

	// �}�b�v�f�[�^��ǂݍ��ނ�
	bool m_IsLoadMapData;

	// ���ɘA���������鎞�p�̃t���O
	bool m_IsContinuous;

	// �t�F���X�𐶐����鎞�p�̃t���O
	bool m_IsFence;

	// �v���C���[�𐶐��������i�d�����Đ������Ȃ��j
	bool m_IsGeneratePlayer;

	// ���ɘA���������鎞�p�̗�ԍ��ۑ��p
	int m_ColumnStart;

	// ���ɘA���������鎞�p�̗�ԍ��ۑ��p
	TArray<int> m_ColumnStartVertArray;

	// �c�ɘA���������鎞�p�̍s�ԍ��ۑ��p
	TArray<int> m_RowStartArray;

	// �t�F���X�����p�̗�ԍ��ۑ��p
	int m_FenceStart;

	// �}�b�v�̕�������ꎞ�I�ɕۑ�
	TArray<FString> strArrayTemp;

	// ���ɘA����������Actor�̈ꎞ�ۑ�
	FMapActorStructCpp m_ContinuousActorTemp;

	// �c�ɘA����������Actor�̈ꎞ�ۑ�
	TArray<FMapActorStructCpp> m_ContinuousVertActorTempArray;

	// public�ϐ�
public:
	// ���̃T���v���ɂȂ�StaticMesh
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UInstancedStaticMeshComponent* m_SampleGround;

	// private���\�b�h
private:
	// ������z��Ɉ�s���̃}�b�v�f�[�^��ۑ�
	void SetStrArrayMapData(TArray<FString>& _strArray, const int _rowIndex, bool& _isLoad);

	// �A���z�uActor�̐������s��
	void SpawnContinuousActor(const int _rowIndex, const int _startColumn, const int _endColumn);

	// X���W�Z�o
	float LocationX(const int _rowIndex);

	// Y���W�Z�o
	float LocationY(const int _columnIndex, const int _strArrayLength);

	// �A������Actor��Y�X�P�[���Z�o
	float ContinuousScaleY(const int _startColumn, const int _endColumn, const float _actorScaleY);

	// �A������Actor��Y�ʒu�Z�o
	float ContinuousLocationY(const int _startColumn, const int _endColumn, const int _strArrayLength);

	// �}�b�v��Actor�𐶐�
	AActor* SpawnMapActor(FMapActorStructCpp _spawnActor, const float _locationX, const float _locationY);

	// FMapActorStructCpp�����Z�b�g����֐�
	FMapActorStructCpp MapActorStructCppReset();
};
