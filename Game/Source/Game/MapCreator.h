// マップの自動配置を行うクラス

// 2020/12/02 渡邊 龍音 作成

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyChara.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "MapCreator.generated.h"

class UInstancedStaticMeshComponent;
class UStaticMeshComponent;

// マップに配置する時の生成パターン
UENUM(BlueprintType)
enum class MapPlacementPattern : uint8
{
	SettingLock		UMETA(Hidden),									// 設定不可
	Single			UMETA(DisplayName = "Single"),					// 単体で配置する
	Continuous		UMETA(DisplayName = "Continuous (Horizontal)"),	// 連続配置で一つのActorになる（横方向）
	V_Continuous	UMETA(DisplayName = "Continuous (Vertical)"),	// 連続配置で一つのActorになる（縦方向）
	Fence			UMETA(DisplayName = "Fence (Horizontal)"),		// 始点と終点を指定して一つのActorを生成する（横方向）
	V_Fence			UMETA(DisplayName = "Fence (Vertical)"),		// 始点と終点を指定して一つのActorを生成する（縦方向）
};

// マップに配置するActorの構造体
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

// マップCSV用構造体
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

	// private変数
private:
	// メッシュを表示する
	UPROPERTY(EditAnyWhere)
		bool m_VisibleMapWire;

	// マップデータ
	UPROPERTY(EditAnyWhere)
		UDataTable* m_MapData;

	// マップCSVで床を生成しない時の文字列
	UPROPERTY(EditAnyWhere)
		FString m_NotGroundGenerateStr;

	// X軸のオフセット
	UPROPERTY(EditAnyWhere)
		float m_XAxis_Offset;

	// Y軸のオフセット
	UPROPERTY(EditAnyWhere)
		float m_YAxis_Offset;

	// 床のActor
	UPROPERTY(EditAnyWhere)
		FMapActorStructCpp m_MapActorGround;

	// プレイヤーActor生成の文字
	UPROPERTY(EditAnyWhere)
		FString m_PlayerGenerateStr;

	// プレイヤーのActor
	UPROPERTY(EditAnyWhere)
		FMapActorStructCpp m_PlayerActor;

	// 床以外のの生成Actor
	UPROPERTY(EditAnyWhere)
		TArray<FMapActorStructCpp> m_MapActorArray;

	// マップデータを読み込むか
	bool m_IsLoadMapData;

	// 横に連続生成する時用のフラグ
	bool m_IsContinuous;

	// フェンスを生成する時用のフラグ
	bool m_IsFence;

	// プレイヤーを生成したか（重複して生成しない）
	bool m_IsGeneratePlayer;

	// 横に連続生成する時用の列番号保存用
	int m_ColumnStart;

	// 横に連続生成する時用の列番号保存用
	TArray<int> m_ColumnStartVertArray;

	// 縦に連続生成する時用の行番号保存用
	TArray<int> m_RowStartArray;

	// フェンス生成用の列番号保存用
	int m_FenceStart;

	// マップの文字列を一時的に保存
	TArray<FString> strArrayTemp;

	// 横に連続生成するActorの一時保存
	FMapActorStructCpp m_ContinuousActorTemp;

	// 縦に連続生成するActorの一時保存
	TArray<FMapActorStructCpp> m_ContinuousVertActorTempArray;

	// public変数
public:
	// 床のサンプルになるStaticMesh
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UInstancedStaticMeshComponent* m_SampleGround;

	// privateメソッド
private:
	// 文字列配列に一行分のマップデータを保存
	void SetStrArrayMapData(TArray<FString>& _strArray, const int _rowIndex, bool& _isLoad);

	// 連続配置Actorの生成を行う
	void SpawnContinuousActor(const int _rowIndex, const int _startColumn, const int _endColumn);

	// X座標算出
	float LocationX(const int _rowIndex);

	// Y座標算出
	float LocationY(const int _columnIndex, const int _strArrayLength);

	// 連続生成ActorのYスケール算出
	float ContinuousScaleY(const int _startColumn, const int _endColumn, const float _actorScaleY);

	// 連続生成ActorのY位置算出
	float ContinuousLocationY(const int _startColumn, const int _endColumn, const int _strArrayLength);

	// マップにActorを生成
	AActor* SpawnMapActor(FMapActorStructCpp _spawnActor, const float _locationX, const float _locationY);

	// FMapActorStructCppをリセットする関数
	FMapActorStructCpp MapActorStructCppReset();
};
