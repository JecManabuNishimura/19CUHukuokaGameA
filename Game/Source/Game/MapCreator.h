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
	SettingLock		UMETA(Hidden),									// 設定不可、何も生成しない
	Single			UMETA(DisplayName = "Single"),					// 単体で配置する
	/*
	Continuous		UMETA(DisplayName = "Continuous (Horizontal)"),	// 連続配置で一つのActorになる（横方向）
	V_Continuous	UMETA(DisplayName = "Continuous (Vertical)"),	// 連続配置で一つのActorになる（縦方向）
	*/
	Fence			UMETA(DisplayName = "Fence (Horizontal)"),		// 始点と終点を指定して一つのActorを生成する（横方向）
	V_Fence			UMETA(DisplayName = "Fence (Vertical)"),		// 始点と終点を指定して一つのActorを生成する（縦方向）

	/*
	ContinuousEnd	UMETA(Hidden),									// 連続配置終了（横方向）
	V_ContinuousEnd	UMETA(Hidden),									// 連続配置終了（縦方向）
	FenceEnd		UMETA(Hidden),									// フェンス配置終了（横方向）
	V_FenceEnd		UMETA(Hidden),									// フェンス配置終了（縦方向）
	*/
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
		MapPlacementPattern geterateType = MapPlacementPattern::Single;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly,
		Meta = (EditCondition = "geterateType != MapPlacementPattern::SettingLock"))
		bool isEnemy = false;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly,
		Meta = (EditCondition = "geterateType != MapPlacementPattern::SettingLock && isEnemy == true"))
		EEnemyMoveType enemyMoveType = EEnemyMoveType::None;

	// スタティックメッシュコンポーネント
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
	// 生成するActor
	FMapActorStructCpp& generateActorStruct;
	// 行番号
	int rowIndex;
	// 列番号
	int columnIndex;
	// 生成する文字列
	FString generateString;
	// 生成パターン
	MapPlacementPattern generatePattern;
	// フェンスかどうか？
	bool isFence;
	// フェンスの開始文字か？
	bool fenceStart;
	// 縦方向の紐付ける数字
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

	// private変数
private:
	// メッシュを表示する
	UPROPERTY(EditAnyWhere
		, Meta = (DisplayName = "Set Map Data"))
		bool m_SetMapData;

	// メッシュを表示する
	UPROPERTY(EditAnyWhere
		, Meta = (DisplayName = "Visible Map Wire"))
		bool m_VisibleMapWire;

	// マップデータ
	UPROPERTY(EditAnyWhere
		, Meta = (DisplayName = "Map Data"))
		UDataTable* m_MapData;

	// マップCSVで床を生成しない時の文字列
	UPROPERTY(EditAnyWhere
		, Meta = (DisplayName = "Not Ground Generate Str"))
		FString m_NotGroundGenerateStr;

	// X軸のオフセット
	UPROPERTY(EditAnyWhere
		, Meta = (DisplayName = "X Axis Offset"))
		float m_XAxis_Offset;

	// Y軸のオフセット
	UPROPERTY(EditAnyWhere
		, Meta = (DisplayName = "Y Axis Offset"))
		float m_YAxis_Offset;

	// 床のActor
	UPROPERTY(EditAnyWhere
		, Meta = (DisplayName = "Map Actor Ground"))
		FMapActorStructCpp m_MapActorGround;

	// プレイヤーActor生成の文字
	UPROPERTY(EditAnyWhere
		, Meta = (DisplayName = "Player Generate Str"))
		FString m_PlayerGenerateStr;

	// プレイヤーのActor
	UPROPERTY(EditAnyWhere
		, Meta = (DisplayName = "Player Actor"))
		FMapActorStructCpp m_PlayerActor;

	// 床以外のの生成Actor
	UPROPERTY(EditAnyWhere
		, Meta = (DisplayName = "Map Actor Array"))
		TArray<FMapActorStructCpp> m_MapActorArray;

	// マップデータを読み込むか
	bool m_IsLoadMapData;

	// プレイヤーを生成したか（重複して生成しない）
	bool m_IsGeneratePlayer;

	// マップの行数
	int m_MapRowNumber;

	// マップの文字列をに保存
	TArray<FString> m_StrMapArray;

	// 文字列の数を代入
	int m_StrMapLength;

	// 生成するActorの情報を保存（生成リスト）
	TArray<CreateData> m_MapActorCreateData;

	// 生成リストに使用する縦にフェンス生成をする時の情報
	TArray<ContinuousData> m_FenceData;

	// 生成リストに使用する縦に連続生成をする時の情報
	//TArray<ContinuousData> m_ContinuousData;

	// 生成リストに使用する縦にフェンス生成をする時の情報
	TArray<ContinuousData> m_VerticalFenceData;

	// 生成リストに使用する縦に連続生成をする時の情報
	//TArray<ContinuousData> m_VerticalContinuousData;

	// サンプルになるStaticMesh
	TArray<UInstancedStaticMeshComponent*> m_SampleMapObject;

	// public変数
public:

	// privateメソッド
private:
	// X座標算出
	float LocationX(const int _rowIndex);

	// Y座標算出
	float LocationY(const int _columnIndex, const int _strArrayLength);

	// 連続生成ActorのYスケール算出
	float ContinuousScale(const int _startColumn, const int _endColumn, const float _actorScaleY);

	// マップにActorを生成
	AActor* SpawnMapActor(FMapActorStructCpp _spawnActor, const float _locationX, const float _locationY);

	// マップにスタティックメッシュを配置
	void AddMapInstanceStaticMesh(UInstancedStaticMeshComponent* _instancedMeshComp, FMapActorStructCpp _spawnActor, const float _locationX, const float _locationY);

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

	void ComparisonChar(TArray<FMapActorStructCpp>& _generateActor,TArray<FString>& _stringArray, const int _rowIndex, TArray<CreateData>& _generateInfoArray);

	// 横に並んだフェンスの紐付けを行う
	void LinkingFence(TArray<CreateData>& _generateInfoArray);

	// 縦に並んだフェンスの紐付けを行う
	void LinkingVerticalFence(TArray<CreateData>& _generateInfoArray);

	// 横への連続生成の設定を行う
	//void LinkingContinuous(TArray<CreateData>& _generateInfoArray);

	// 縦への連続生成の設定を行う
	//void LinkingVerticalContinuous(TArray<CreateData>& _generateInfoArray);

	// ContinuousData型のTArrayのLinkIndexの要素が何番目か確認する
	int GetLinkIndex(int _linkIndex, const TArray<ContinuousData> _array);

	// ContinuousData型のTArrayにLinkIndexの要素を代入する
	void AddLinkIndex(FMapActorStructCpp _actorStruct, int _linkIndex, TArray<ContinuousData>& _array, int _value, bool isStart = true);

	// マップ生成情報を設定
	void SettingMap(bool isRegenerate = false);

	// 生成を行う
	void MapCreate();

	// スタティックメッシュによる生成を行う
	void MapCreateEditor();

	// FMapActorStructCppの要素が何番目か調べる
	int GetMapActorArrayIndex(FMapActorStructCpp _mapActorStruct);
};
