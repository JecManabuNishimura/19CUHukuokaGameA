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
enum class EMapPlacementPattern : uint8
{
	SettingLock		UMETA(Hidden),									// 設定不可、何も生成しない
	Single			UMETA(DisplayName = "Single"),					// 単体で配置する
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
		EMapPlacementPattern geterateType = EMapPlacementPattern::Single;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly,
		Meta = (EditCondition = "geterateType != MapPlacementPattern::SettingLock"))
		bool isEnemy = false;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly,
		Meta = (EditCondition = "geterateType != MapPlacementPattern::SettingLock && isEnemy == true"))
		EEnemyMoveType enemyMoveType = EEnemyMoveType::None;

	bool operator== (const FMapActorStructCpp& mapActor) const
	{
		return (actor == mapActor.actor &&
			generateChar == mapActor.generateChar &&
			generateCharStart == mapActor.generateCharStart &&
			generateCharEnd == mapActor.generateCharEnd);
	}
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

	UPROPERTY(BlueprintReadOnly)
		int indexNum = 15;
};

USTRUCT(BlueprintType)
struct FCreateData
{
	GENERATED_USTRUCT_BODY();

	FCreateData() {}

	FCreateData(FMapActorStructCpp& act, int row = 0, int column = 0, FString str = "",
		EMapPlacementPattern pat = EMapPlacementPattern::SettingLock, bool fence = false, bool start = false, int lnk = -1)
		: generateActorStruct(act)
		, rowIndex(row)
		, columnIndex(column)
		, generateString(str)
		, generatePattern(pat)
		, isFence(fence)
		, fenceStart(start)
		, vertLinkNum(lnk)
	{}


	// 生成するActor
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
		FMapActorStructCpp generateActorStruct;

	// 行番号
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
		int rowIndex;

	// 列番号
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
		int columnIndex;

	// 生成する文字列
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
		FString generateString;

	// 生成パターン
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
		EMapPlacementPattern generatePattern;

	// フェンスかどうか？
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
		bool isFence;

	// フェンスの開始文字か？
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
		bool fenceStart;

	// 縦方向の紐付ける数字
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
		int vertLinkNum;
};

USTRUCT(BlueprintType)
struct FContinuousData
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(BlueprintReadOnly)
		int linkIndex;
	UPROPERTY(BlueprintReadOnly)
		int startIndex;
	UPROPERTY(BlueprintReadOnly)
		int endIndex;
	UPROPERTY(BlueprintReadOnly)
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

	// private変数
private:

	// プレイヤーを生成したか（重複して生成しない）
	bool m_IsGeneratePlayer;

	// マップの行数
	int m_MapRowNumber;

	// マップの文字列をに保存
	TArray<FString> m_StrMapArray;

	// public変数
public:// メッシュを表示する
	UPROPERTY(EditAnyWhere, BlueprintReadOnly
		, Meta = (DisplayName = "Set Map Data"))
		bool m_SetMapData;

	// メッシュを表示する
	UPROPERTY(EditAnyWhere, BlueprintReadOnly
		, Meta = (DisplayName = "Visible Map Wire"))
		bool m_VisibleMapWire;

	// マップデータ
	UPROPERTY(EditAnyWhere, BlueprintReadOnly
		, Meta = (DisplayName = "Map Data"))
		UDataTable* m_MapData;

	// マップCSVで床を生成しない時の文字列
	UPROPERTY(EditAnyWhere, BlueprintReadOnly
		, Meta = (DisplayName = "Not Ground Generate Str"))
		FString m_NotGroundGenerateStr;

	// X軸のオフセット
	UPROPERTY(EditAnyWhere, BlueprintReadOnly
		, Meta = (DisplayName = "X Axis Offset"))
		float m_XAxis_Offset;

	// Y軸のオフセット
	UPROPERTY(EditAnyWhere, BlueprintReadOnly
		, Meta = (DisplayName = "Y Axis Offset"))
		float m_YAxis_Offset;

	// 床のActor
	UPROPERTY(EditAnyWhere, BlueprintReadOnly
		, Meta = (DisplayName = "Map Actor Ground"))
		FMapActorStructCpp m_MapActorGround;

	// プレイヤーActor生成の文字
	UPROPERTY(EditAnyWhere, BlueprintReadOnly
		, Meta = (DisplayName = "Player Generate Str"))
		FString m_PlayerGenerateStr;

	// プレイヤーのActor
	UPROPERTY(EditAnyWhere, BlueprintReadOnly
		, Meta = (DisplayName = "Player Actor"))
		FMapActorStructCpp m_PlayerActor;

	// 床以外のの生成Actor
	UPROPERTY(EditAnyWhere, BlueprintReadOnly
		, Meta = (DisplayName = "Map Actor Array"))
		TArray<FMapActorStructCpp> m_MapActorArray;

	// 生成するActorの情報を保存（生成リスト）
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite,
		Meta = (DisplayName = "Generate Actor List"))
		TArray<FCreateData> m_MapActorCreateData;

	UPROPERTY(BlueprintReadOnly,
		Meta = (DisplayName = "StrMapLength"))
		// 文字列の数を代入
		int m_StrMapLength;

	UPROPERTY(BlueprintReadWrite,
		Meta = (DisplayName = "FenceData"))
		// 生成リストに使用する縦にフェンス生成をする時の情報
		TArray<FContinuousData> m_FenceData;

	UPROPERTY(BlueprintReadWrite,
		Meta = (DisplayName = "VerticalFenceData"))
		// 生成リストに使用する縦にフェンス生成をする時の情報
		TArray<FContinuousData> m_VerticalFenceData;

	// privateメソッド
private:
	// X座標算出
	float LocationX(const int _rowIndex);

	// Y座標算出
	float LocationY(const int _columnIndex, const int _strArrayLength);

	// 連続生成ActorのYスケール算出
	float ContinuousScale(const int _startColumn, const int _endColumn, const float _actorScaleY);

	// マップにActorを生成
	AActor* SpawnMapActor(FMapActorStructCpp& _spawnActor, const float _locationX, const float _locationY);

	// マップにスタティックメッシュを配置
	void AddMapInstanceStaticMesh(UInstancedStaticMeshComponent* _instancedMeshComp, FMapActorStructCpp& _spawnActor, const float _locationX, const float _locationY);

	// FMapActorStructCppをリセットする関数
	FMapActorStructCpp MapActorStructCppReset();

	// 文字列配列にCSVファイルを格納する

	// 第一引数		マップのCSVファイル
	// 第二引数		マップ保存用文字列配列
	// 第三引数		行番号

	bool SetCSVToFString(const UDataTable* _mapData, TArray<FString>& _stringArray, const int _rowIndex);

	// 文字列の比較を行い、一致した時の情報を格納する

	// 第一引数		生成したいActorが格納されている配列
	// 第二引数		マップ用文字列配列
	// 第三引数		行番号
	// 第四引数		生成情報の保存先になる配列

	void ComparisonChar(TArray<FMapActorStructCpp>& _generateActor, TArray<FString>& _stringArray, const int _rowIndex, TArray<FCreateData>& _generateInfoArray);

	// 横に並んだフェンスの紐付けを行う
	void LinkingFence(TArray<FCreateData>& _generateInfoArray);

	// 縦に並んだフェンスの紐付けを行う
	void LinkingVerticalFence(TArray<FCreateData>& _generateInfoArray);

	// FContinuousData型のTArrayにLinkIndexの要素を代入する
	void AddLinkIndex(FMapActorStructCpp& _actorStruct, int _linkIndex, TArray<FContinuousData>& _array, int _value, bool isStart = true);

	// スタティックメッシュによる生成を行う
	void MapCreateEditor();

	// FMapActorStructCppの要素が何番目か調べる
	int GetMapActorArrayIndex(FMapActorStructCpp& _mapActorStruct);

	// CSVファイルを書き出す
	bool ExportCSVFromActorArray(const TArray<FMapActorStructCpp> _mapActorArray);

	// publicメソッド
public:
	UFUNCTION(BlueprintCallable)
		// 生成を行う
		void MapCreate();

	UFUNCTION(BlueprintCallable)
		// マップ生成情報を設定
		void SettingMap(bool isRegenerate = false);

	UFUNCTION(BlueprintCallable)
		// FContinuousData型のTArrayのLinkIndexの要素が何番目か確認する
		int GetLinkIndex(int _linkIndex, const TArray<FContinuousData> _array);
};
