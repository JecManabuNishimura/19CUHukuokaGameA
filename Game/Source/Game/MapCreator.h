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
	SettingLock		UMETA(Hidden),									// �ݒ�s�A�����������Ȃ�
	Single			UMETA(DisplayName = "Single"),					// �P�̂Ŕz�u����
	/*
	Continuous		UMETA(DisplayName = "Continuous (Horizontal)"),	// �A���z�u�ň��Actor�ɂȂ�i�������j
	V_Continuous	UMETA(DisplayName = "Continuous (Vertical)"),	// �A���z�u�ň��Actor�ɂȂ�i�c�����j
	*/
	Fence			UMETA(DisplayName = "Fence (Horizontal)"),		// �n�_�ƏI�_���w�肵�Ĉ��Actor�𐶐�����i�������j
	V_Fence			UMETA(DisplayName = "Fence (Vertical)"),		// �n�_�ƏI�_���w�肵�Ĉ��Actor�𐶐�����i�c�����j

	/*
	ContinuousEnd	UMETA(Hidden),									// �A���z�u�I���i�������j
	V_ContinuousEnd	UMETA(Hidden),									// �A���z�u�I���i�c�����j
	FenceEnd		UMETA(Hidden),									// �t�F���X�z�u�I���i�������j
	V_FenceEnd		UMETA(Hidden),									// �t�F���X�z�u�I���i�c�����j
	*/
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

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		bool isScaleXAxis = true;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly,
		Meta = (EditCondition = "geterateType != MapPlacementPattern::Fence && geterateType != MapPlacementPattern::V_Fence && geterateType != MapPlacementPattern::SettingLock"))
		FString generateChar = "";

	UPROPERTY(EditAnyWhere, BlueprintReadOnly,
		Meta = (EditCondition = "geterateType == MapPlacementPattern::Fence || geterateType == MapPlacementPattern::V_Fence && geterateType != MapPlacementPattern::SettingLock && isEnemy == false"))
		FString generateCharStart = "";

	UPROPERTY(EditAnyWhere, BlueprintReadOnly,
		Meta = (EditCondition = "geterateType == MapPlacementPattern::Fence || geterateType == MapPlacementPattern::V_Fence && geterateType != MapPlacementPattern::SettingLock && isEnemy == false"))
		FString generateCharEnd = "";

	UPROPERTY(EditAnyWhere, BlueprintReadOnly,
		Meta = (EditCondition = "geterateType != MapPlacementPattern::SettingLock && isEnemy == false"))
		MapPlacementPattern geterateType = MapPlacementPattern::Single;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly,
		Meta = (EditCondition = "geterateType != MapPlacementPattern::SettingLock"))
		bool isEnemy = false;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly,
		Meta = (EditCondition = "geterateType != MapPlacementPattern::SettingLock && isEnemy == true"))
		EEnemyMoveType enemyMoveType = EEnemyMoveType::None;

	// �X�^�e�B�b�N���b�V���R���|�[�l���g
	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
		UStaticMeshComponent* actorStaticMesh;

	bool operator== (const FMapActorStructCpp& mapActor) const
	{
		return (actor				==	mapActor.actor &&
				generateChar		==	mapActor.generateChar &&
				generateCharStart	==	mapActor.generateCharStart &&
				generateCharEnd		==	mapActor.generateCharEnd);
	}
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

	UPROPERTY(BlueprintReadOnly)
		int indexNum = 15;
};

class CreateData
{
public:
	CreateData(FMapActorStructCpp& act, int row = 0, int column = 0, FString str = "",
		MapPlacementPattern pat = MapPlacementPattern::SettingLock, bool fence = false , bool start = false, int lnk = -1)
		: generateActorStruct(act)
		, rowIndex(row)
		, columnIndex(column)
		, generateString(str)
		, generatePattern(pat)
		, isFence(fence)
		, fenceStart(start)
		, vertLinkNum(lnk)
	{}

public:
	// ��������Actor
	FMapActorStructCpp& generateActorStruct;
	// �s�ԍ�
	int rowIndex;
	// ��ԍ�
	int columnIndex;
	// �������镶����
	FString generateString;
	// �����p�^�[��
	MapPlacementPattern generatePattern;
	// �t�F���X���ǂ����H
	bool isFence;
	// �t�F���X�̊J�n�������H
	bool fenceStart;
	// �c�����̕R�t���鐔��
	int vertLinkNum;
};

struct ContinuousData
{
	int linkIndex;
	int startIndex;
	int endIndex;
	FVector scale;
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
	UPROPERTY(EditAnyWhere
		, Meta = (DisplayName = "Set Map Data"))
		bool m_SetMapData;

	// ���b�V����\������
	UPROPERTY(EditAnyWhere
		, Meta = (DisplayName = "Visible Map Wire"))
		bool m_VisibleMapWire;

	// �}�b�v�f�[�^
	UPROPERTY(EditAnyWhere
		, Meta = (DisplayName = "Map Data"))
		UDataTable* m_MapData;

	// �}�b�vCSV�ŏ��𐶐����Ȃ����̕�����
	UPROPERTY(EditAnyWhere
		, Meta = (DisplayName = "Not Ground Generate Str"))
		FString m_NotGroundGenerateStr;

	// X���̃I�t�Z�b�g
	UPROPERTY(EditAnyWhere
		, Meta = (DisplayName = "X Axis Offset"))
		float m_XAxis_Offset;

	// Y���̃I�t�Z�b�g
	UPROPERTY(EditAnyWhere
		, Meta = (DisplayName = "Y Axis Offset"))
		float m_YAxis_Offset;

	// ����Actor
	UPROPERTY(EditAnyWhere
		, Meta = (DisplayName = "Map Actor Ground"))
		FMapActorStructCpp m_MapActorGround;

	// �v���C���[Actor�����̕���
	UPROPERTY(EditAnyWhere
		, Meta = (DisplayName = "Player Generate Str"))
		FString m_PlayerGenerateStr;

	// �v���C���[��Actor
	UPROPERTY(EditAnyWhere
		, Meta = (DisplayName = "Player Actor"))
		FMapActorStructCpp m_PlayerActor;

	// ���ȊO�̂̐���Actor
	UPROPERTY(EditAnyWhere
		, Meta = (DisplayName = "Map Actor Array"))
		TArray<FMapActorStructCpp> m_MapActorArray;

	// �}�b�v�f�[�^��ǂݍ��ނ�
	bool m_IsLoadMapData;

	// �v���C���[�𐶐��������i�d�����Đ������Ȃ��j
	bool m_IsGeneratePlayer;

	// �}�b�v�̍s��
	int m_MapRowNumber;

	// �}�b�v�̕�������ɕۑ�
	TArray<FString> m_StrMapArray;

	// ������̐�����
	int m_StrMapLength;

	// ��������Actor�̏���ۑ��i�������X�g�j
	TArray<CreateData> m_MapActorCreateData;

	// �������X�g�Ɏg�p����c�Ƀt�F���X���������鎞�̏��
	TArray<ContinuousData> m_FenceData;

	// �������X�g�Ɏg�p����c�ɘA�����������鎞�̏��
	//TArray<ContinuousData> m_ContinuousData;

	// �������X�g�Ɏg�p����c�Ƀt�F���X���������鎞�̏��
	TArray<ContinuousData> m_VerticalFenceData;

	// �������X�g�Ɏg�p����c�ɘA�����������鎞�̏��
	//TArray<ContinuousData> m_VerticalContinuousData;

	// �T���v���ɂȂ�StaticMesh
	TArray<UInstancedStaticMeshComponent*> m_SampleMapObject;

	// public�ϐ�
public:

	// private���\�b�h
private:
	// X���W�Z�o
	float LocationX(const int _rowIndex);

	// Y���W�Z�o
	float LocationY(const int _columnIndex, const int _strArrayLength);

	// �A������Actor��Y�X�P�[���Z�o
	float ContinuousScale(const int _startColumn, const int _endColumn, const float _actorScaleY);

	// �}�b�v��Actor�𐶐�
	AActor* SpawnMapActor(FMapActorStructCpp _spawnActor, const float _locationX, const float _locationY);

	// �}�b�v�ɃX�^�e�B�b�N���b�V����z�u
	void AddMapInstanceStaticMesh(UInstancedStaticMeshComponent* _instancedMeshComp, FMapActorStructCpp _spawnActor, const float _locationX, const float _locationY);

	// FMapActorStructCpp�����Z�b�g����֐�
	FMapActorStructCpp MapActorStructCppReset();

	// ������z���CSV�t�@�C�����i�[����

	// ������		�}�b�v��CSV�t�@�C��
	// ������		�}�b�v�ۑ��p������z��
	// ��O����		�s�ԍ�

	bool SetCSVToFString(const UDataTable* _mapData, TArray<FString>& _stringArray, const int _rowIndex);

	// ������̔�r���s���A��v�������̏����i�[����

	// ������		����������Actor���i�[����Ă���z��
	// ������		�}�b�v�p������z��
	// ��O����		�s�ԍ�
	// ��l����		�������̕ۑ���ɂȂ�z��

	void ComparisonChar(TArray<FMapActorStructCpp>& _generateActor,TArray<FString>& _stringArray, const int _rowIndex, TArray<CreateData>& _generateInfoArray);

	// ���ɕ��񂾃t�F���X�̕R�t�����s��
	void LinkingFence(TArray<CreateData>& _generateInfoArray);

	// �c�ɕ��񂾃t�F���X�̕R�t�����s��
	void LinkingVerticalFence(TArray<CreateData>& _generateInfoArray);

	// ���ւ̘A�������̐ݒ���s��
	//void LinkingContinuous(TArray<CreateData>& _generateInfoArray);

	// �c�ւ̘A�������̐ݒ���s��
	//void LinkingVerticalContinuous(TArray<CreateData>& _generateInfoArray);

	// ContinuousData�^��TArray��LinkIndex�̗v�f�����Ԗڂ��m�F����
	int GetLinkIndex(int _linkIndex, const TArray<ContinuousData> _array);

	// ContinuousData�^��TArray��LinkIndex�̗v�f��������
	void AddLinkIndex(FMapActorStructCpp _actorStruct, int _linkIndex, TArray<ContinuousData>& _array, int _value, bool isStart = true);

	// �}�b�v��������ݒ�
	void SettingMap(bool isRegenerate = false);

	// �������s��
	void MapCreate();

	// �X�^�e�B�b�N���b�V���ɂ�鐶�����s��
	void MapCreateEditor();

	// FMapActorStructCpp�̗v�f�����Ԗڂ����ׂ�
	int GetMapActorArrayIndex(FMapActorStructCpp _mapActorStruct);
};
