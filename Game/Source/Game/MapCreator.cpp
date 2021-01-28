// マップの自動配置を行うクラス

// 2020/12/02 渡邊 龍音 作成
// 2020/01/28 渡邊 龍音 生成まで完了

#include "MapCreator.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "Misc/FileHelper.h"
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
	// Tickを実行する必要はないので更新を切る
	PrimaryActorTick.bCanEverTick = false;

	// RootConponentの作成
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// 文字列配列の初期化
	m_StrMapArray.Reset();

	// 生成するActorの情報を保存
	m_MapActorCreateData.Reset();

	// 縦方向に生成するものの情報リストを初期化
	m_FenceData.Reset();
	//m_ContinuousData.Reset();
	m_VerticalFenceData.Reset();
	//m_VerticalContinuousData.Reset();

	// 床Actorの初期化
	m_MapActorGround.geterateType = MapPlacementPattern::SettingLock;

	// プレイヤーActorの設定
	m_PlayerActor.geterateType = MapPlacementPattern::SettingLock;
}

void AMapCreator::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Enemy用設定
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

	// Actorのトランスフォームを固定
	// 固定するLocation
	FVector newLocation(0.0f, 0.0f, 0.0f);

	// 固定するRotation
	FRotator newRotation(0.0f, 0.0f, 0.0f);

	// 固定するScale
	FVector newScale(1.0f, 1.0f, 1.0f);

	// LocationとRotationを固定
	SetActorLocationAndRotation(newLocation, newRotation);

	// Scaleを固定
	SetActorScale3D(newScale);

	// メッシュ生成を行うかどうか
	if (m_VisibleMapWire)
	{		
		SettingMap(m_SetMapData);
	}
	m_SetMapData = false;
}

void AMapCreator::BeginPlay()
{
	Super::BeginPlay();

	SettingMap(true);
	MapCreate();
	ExportCSVFromActorArray(m_MapActorArray);
}

void AMapCreator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// X座標算出
float AMapCreator::LocationX(const int _rowIndex)
{
	return (float)_rowIndex * m_XAxis_Offset;
}

// Y座標算出
float AMapCreator::LocationY(const int _columnIndex, const int _strArrayLength)
{
	float difference = (float)_columnIndex - ((float)(_strArrayLength - 1) / 2.0f);

	return difference * m_YAxis_Offset;
}

// 連続生成ActorのYスケール算出
float AMapCreator::ContinuousScale(const int _startColumn, const int _endColumn, const float _actorScaleY)
{
	int diff = _endColumn - _startColumn;
	return _actorScaleY * diff;
}

// マップにActorを生成
AActor* AMapCreator::SpawnMapActor(FMapActorStructCpp& _spawnActor, const float _locationX, const float _locationY)
{
	if (_spawnActor.actor != nullptr)
	{
		// 生成パラメータの設定
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

// マップにスタティックメッシュを配置
void AMapCreator::AddMapInstanceStaticMesh(UInstancedStaticMeshComponent* _instancedMeshComp, FMapActorStructCpp& _spawnActor, const float _locationX, const float _locationY)
{
	_instancedMeshComp->AddInstance(
		FTransform(_spawnActor.rotation,
			FVector(_locationX, _locationY, _spawnActor.location_Z),
			_spawnActor.scale));
}

// FMapActorStructCppをリセットする関数
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

// 文字列配列にCSVファイルを格納する
bool AMapCreator::SetCSVToFString(const UDataTable* _mapData, TArray<FString>& _stringArray, const int _rowIndex)
{
	bool isLoad = true;

	// 文字配列リセット
	_stringArray.Reset();

	// マップデータがある
	if (_mapData != nullptr)
	{
		// intの行番号をFNameに変換するために、FStringに変換する（FNameへの変換はFStringからしかできないため）
		FString fStrIndex = FString::FromInt(_rowIndex + 1);

		// FStringからFNameに変換する
		FName fNameIndex = FName(*fStrIndex);

		// 行ごとのデータの取得
		FMapStructCpp* rowData = _mapData->FindRow<FMapStructCpp>(fNameIndex, FString());

		// データ行の終わり
		if (rowData == nullptr)
		{
			// データ読み込み終了
			UE_LOG(LogTemp, Warning, TEXT("[MapCreator] rowData is nullptr. (rowIndex = %s)"), *fStrIndex);
			isLoad = false;
		}
		// データがあった
		else
		{
			// FMapStructCpp構造体の要素数分の長さの配列を作成
			for (int i = 0; i < rowData->indexNum; ++i)
			{
				// FMapStructCppの最初の領域（Line_1）から最後の領域（Line_15）までの間を追加
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

			// 文字列配列の長さを代入
			m_StrMapLength = _stringArray.Num();
		}
	}
	// マップデータがない
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[MapCreator] MapData is nullptr."));
		isLoad = false;
	}

	return isLoad;
}

// 文字列の比較を行い、一致した時の情報を格納する
void AMapCreator::ComparisonChar(TArray<FMapActorStructCpp>& _generateActor, TArray<FString>& _stringArray, const int _rowIndex, TArray<FCreateData>& _generateInfoArray)
{
	// 文字列配列の要素数分検索を行う
	for (int columnIndex = 0; columnIndex < _stringArray.Num(); ++columnIndex)
	{
		// 文字列の最初
		if (columnIndex == 0)
		{
			// 床を生成しない文字ではない
			if (_stringArray[0] != m_NotGroundGenerateStr)
			{
				// 床の生成を追加
				FCreateData groundData(m_MapActorGround, _rowIndex, m_StrMapLength / 2, "Ground", MapPlacementPattern::Single);
				_generateInfoArray.Add(groundData);
			}
		}

		// 検索する文字列が空白なら飛ばす
		if (_stringArray[columnIndex] == "")
		{
			continue;
		}

		// プレイヤー生成文字と一致
		if (m_IsGeneratePlayer == false && _stringArray[columnIndex] == m_PlayerGenerateStr)
		{
			// プレイヤーを生成リストに追加してループやり直し
			FCreateData playerData(m_PlayerActor, _rowIndex, columnIndex, "Player", MapPlacementPattern::Single);
			_generateInfoArray.Add(playerData);
			continue;
		}

		// 文字列と一致するActorを検索する

		// 文字と一致フラグ
		bool isFound = false;

		// 設定した生成するActorの数だけ検索を行う
		for (int actorIndex = 0; actorIndex < _generateActor.Num(); ++actorIndex)
		{
			// 縦生成かどうか
			bool isVert = false;

			// 生成タイプによって比較する文字列を変更する
			// 単体タイプ もしくは 連続生成タイプ
			if (_generateActor[actorIndex].geterateType == MapPlacementPattern::Single/* ||
				_generateActor[actorIndex].geterateType == MapPlacementPattern::Continuous ||
				_generateActor[actorIndex].geterateType == MapPlacementPattern::V_Continuous*/)
			{
				if (_stringArray[columnIndex] == _generateActor[actorIndex].generateChar)
				{
					// 発見状態にする
					isFound = true;

					// 文字が一致した生成Actorを生成リストに追加して検索ループ終了
					FCreateData playerData(_generateActor[actorIndex], _rowIndex, columnIndex, _generateActor[actorIndex].generateChar, _generateActor[actorIndex].geterateType, false);
					_generateInfoArray.Add(playerData);
					break;
				}
			}
			// フェンス生成タイプ
			else if (_generateActor[actorIndex].geterateType == MapPlacementPattern::Fence ||
				_generateActor[actorIndex].geterateType == MapPlacementPattern::V_Fence)
			{
				if (_stringArray[columnIndex] == _generateActor[actorIndex].generateCharStart)
				{
					// 発見状態にする
					isFound = true;

					// 文字が一致した生成Actorを生成リストに追加して検索ループ終了
					FCreateData playerData(_generateActor[actorIndex], _rowIndex, columnIndex, _generateActor[actorIndex].generateCharStart, _generateActor[actorIndex].geterateType, true, true);
					_generateInfoArray.Add(playerData);
					break;
				}
				else if (_stringArray[columnIndex] == _generateActor[actorIndex].generateCharEnd)
				{
					// 発見状態にする
					isFound = true;

					// 文字が一致した生成Actorを生成リストに追加して検索ループ終了
					FCreateData playerData(_generateActor[actorIndex], _rowIndex, columnIndex, _generateActor[actorIndex].generateCharEnd, _generateActor[actorIndex].geterateType, true, false);
					_generateInfoArray.Add(playerData);
					break;
				}
			}
			// その他の場合
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[MapCreator] The \"Generate Type\" set in MapActorArray[%d] is invalid."), actorIndex);
			}
		}

		// CSVファイルの文字列と一致する文字列がマップに生成するActorに設定されていない
		if (isFound == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("[MapCreator] The string \"%s\" is set in the CSV file, but not in the MapActorArray. Check both the CSV file and the MapActorArray."), *_stringArray[columnIndex])
		}
	}
}

// 横に並んだフェンスの紐付けを行う
void AMapCreator::LinkingFence(TArray<FCreateData>& _generateInfoArray)
{
	// 生成リストにデータがない
	if (_generateInfoArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MapCreator] There is no data in _generateInfoArray."));
		return;
	}
	// データがある
	else
	{
		// 紐付けに使用する値
		int linkIndex = 0;

		// フェンス開始を保持
		bool isStart = false;

		// 行ごとに検索
		for (int row = 0; row < m_MapRowNumber; ++row)
		{
			// 生成リストの検索
			for (int actorIndex = 0; actorIndex < _generateInfoArray.Num(); ++actorIndex)
			{
				// 縦のフェンスの場合
				if (_generateInfoArray[actorIndex].generatePattern == MapPlacementPattern::Fence &&
					_generateInfoArray[actorIndex].rowIndex == row)
				{
					// フェンス生成開始の場合
					if (_generateInfoArray[actorIndex].fenceStart == true)
					{
						// 未生成であれば
						if (isStart == false)
						{
							// 生成状態に
							isStart = true;

							// 番号で紐付ける
							_generateInfoArray[actorIndex].vertLinkNum = linkIndex;

							// 開始位置の設定
							AddLinkIndex(_generateInfoArray[actorIndex].generateActorStruct, linkIndex, m_FenceData, _generateInfoArray[actorIndex].columnIndex, true);
						}
						// 生成中であればログ
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("[MapCreator] A fence generation character was found during vertical fence generation. index = %d"), )
						}
					}
					// フェンス生成終了の場合
					else if (_generateInfoArray[actorIndex].fenceStart == false)
					{
						// 生成中であれば
						if (isStart == true)
						{
							// 生成状態の終了
							isStart = false;

							// 番号で紐付ける
							_generateInfoArray[actorIndex].vertLinkNum = linkIndex;

							// 終了位置の設定
							AddLinkIndex(_generateInfoArray[actorIndex].generateActorStruct, linkIndex, m_FenceData, _generateInfoArray[actorIndex].columnIndex, false);

							// スケールの変更
							//紐付けられた番号から要素番号を取得
							int fenceDataIndex = GetLinkIndex(linkIndex, m_FenceData);

							// Y方向のスケールを変更
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

								// 紐付け番号のインクリメント
								++linkIndex;
							}
							else
							{
								UE_LOG(LogTemp, Error, TEXT("[MapCreator] fenceDataIndex is invaild."))
							}
						}
						// 生成中でなければログ
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("[MapCreator] A closing character was found before the fence generation character."))
						}
					}
				}
			}
			isStart = false;

		}
	}
}

// 縦に並んだフェンスの紐付けを行う
void AMapCreator::LinkingVerticalFence(TArray<FCreateData>& _generateInfoArray)
{
	// 生成リストにデータがない
	if (_generateInfoArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MapCreator] There is no data in _generateInfoArray."));
		return;
	}
	// データがある
	else
	{
		// 紐付けに使用する値
		int linkIndex = 0;

		// フェンス開始を保持
		bool isStart = false;

		// 列ごとに検索
		for (int column = 0; column < m_StrMapLength; ++column)
		{
			// 生成リストの検索
			for (int actorIndex = 0; actorIndex < _generateInfoArray.Num(); ++actorIndex)
			{
				// 列が一致する縦のフェンスの場合
				if (_generateInfoArray[actorIndex].generatePattern == MapPlacementPattern::V_Fence &&
					_generateInfoArray[actorIndex].columnIndex == column)
				{
					// フェンス生成開始の場合
					if (_generateInfoArray[actorIndex].fenceStart == true)
					{
						// 未生成であれば
						if (isStart == false)
						{
							// 生成状態に
							isStart = true;

							// 番号で紐付ける
							_generateInfoArray[actorIndex].vertLinkNum = linkIndex;

							// 開始位置の設定
							AddLinkIndex(_generateInfoArray[actorIndex].generateActorStruct, linkIndex, m_VerticalFenceData, _generateInfoArray[actorIndex].rowIndex, true);
						}
						// 生成中であればログ
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("[MapCreator] A fence generation character was found during vertical fence generation."))
						}
					}
					// フェンス生成終了の場合
					else if (_generateInfoArray[actorIndex].fenceStart == false)
					{
						// 生成中であれば
						if (isStart == true)
						{
							// 生成状態の終了
							isStart = false;

							// 番号で紐付ける
							_generateInfoArray[actorIndex].vertLinkNum = linkIndex;

							// 終了位置の設定
							AddLinkIndex(_generateInfoArray[actorIndex].generateActorStruct, linkIndex, m_VerticalFenceData, _generateInfoArray[actorIndex].rowIndex, false);

							// スケールの変更
							//紐付けられた番号から要素番号を取得
							int fenceDataIndex = GetLinkIndex(linkIndex, m_VerticalFenceData);

							// Y方向のスケールを変更
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

								// 紐付け番号のインクリメント
								++linkIndex;
							}
							else
							{
								UE_LOG(LogTemp, Error, TEXT("[MapCreator] fenceDataIndex is invaild."))
							}
						}
						// 生成中でなければログ
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

// ContinuousData型のTArrayに要素が含まれてるか確認する
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

// ContinuousData型のTArrayにLinkIndexの要素を代入する
void AMapCreator::AddLinkIndex(FMapActorStructCpp& _actorStruct, int _linkIndex, TArray<ContinuousData>& _array, int _value, bool isStart/* = true*/)
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

// マップ生成情報を設定
void AMapCreator::SettingMap(bool isRegenerate/* = false*/)
{
	if (m_MapActorCreateData.Num() == 0 || isRegenerate == true)
	{
		// 生成リスト初期化
		m_MapActorCreateData.Reset();

		// 文字列配列初期化
		m_StrMapArray.Reset();

		// 行番号
		for (int rowIndex = 0;; ++rowIndex)
		{
			// CSVファイルを一行文字列配列に代入することができれば
			if (SetCSVToFString(m_MapData, m_StrMapArray, rowIndex))
			{
				// 代入された文字列から生成するActorを生成リストに追加
				ComparisonChar(m_MapActorArray, m_StrMapArray, rowIndex, m_MapActorCreateData);
			}
			// できなければループ終了
			else
			{
				m_MapRowNumber = rowIndex;
				UE_LOG(LogTemp, Verbose, TEXT("[MapCreator] The number of rows in the map is %d."), m_MapRowNumber);
				break;
			}
		}
		LinkingFence(m_MapActorCreateData);
		LinkingVerticalFence(m_MapActorCreateData);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[MapCreator] Skipped adding to generating list because the list is already set."));
	}
}

// 生成を行う
void AMapCreator::MapCreate()
{
	// 連続生成の際に重複しないようにする
	int continuousLinkIndex = 0;		// 横連続
	int fenceLinkIndex = 0;		// 横フェンス
	int vertContinuousLinkIndex = 0;		// 縦連続
	int vertFenceLinkIndex = 0;		// 縦フェンス

	// 生成リストを検索
	for (int index = 0; index < m_MapActorCreateData.Num(); ++index)
	{
		switch (m_MapActorCreateData[index].generatePattern)
		{
		case MapPlacementPattern::Single:
		{
			// Actorを一つだけ生成する
			SpawnMapActor
			(
				m_MapActorCreateData[index].generateActorStruct,
				LocationX(m_MapActorCreateData[index].rowIndex),
				LocationY(m_MapActorCreateData[index].columnIndex, m_StrMapLength)
			);
			break;
		}
		case MapPlacementPattern::Fence:
		{
			// 縦のフェンス生成に使用する生成データの要素番号を格納する
			int fenceDataIndex = GetLinkIndex(m_MapActorCreateData[index].vertLinkNum, m_FenceData);

			if (fenceLinkIndex == fenceDataIndex)
			{
				FMapActorStructCpp actorTemp = m_MapActorCreateData[index].generateActorStruct;
				actorTemp.scale = m_FenceData[fenceDataIndex].scale;

				// Actorを一つだけ生成する
				SpawnMapActor
				(
					actorTemp,
					LocationX(m_MapActorCreateData[index].rowIndex),
					LocationY((m_FenceData[fenceDataIndex].startIndex + m_FenceData[fenceDataIndex].endIndex) / 2.0f, m_StrMapLength)
				);

				// 次の連続生成Actorの生成へ
				++fenceLinkIndex;
			}
			break;
		}

		case MapPlacementPattern::V_Fence:
		{
			// 縦のフェンス生成に使用する生成データの要素番号を格納する
			int fenceDataIndex = GetLinkIndex(m_MapActorCreateData[index].vertLinkNum, m_VerticalFenceData);

			if (vertFenceLinkIndex == fenceDataIndex)
			{
				FMapActorStructCpp actorTemp = m_MapActorCreateData[index].generateActorStruct;
				actorTemp.scale = m_VerticalFenceData[fenceDataIndex].scale;

				// Actorを一つだけ生成する
				SpawnMapActor
				(
					actorTemp,
					LocationX((m_VerticalFenceData[fenceDataIndex].startIndex + m_VerticalFenceData[fenceDataIndex].endIndex) / 2.0f),
					LocationY(m_MapActorCreateData[index].columnIndex, m_StrMapLength)
				);

				// 次の連続生成Actorの生成へ
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

// スタティックメッシュによる生成を行う
void AMapCreator::MapCreateEditor()
{
	// 連続生成の際に重複しないようにする
	int continuousLinkIndex = 0;		// 横連続
	int fenceLinkIndex = 0;		// 横フェンス
	int vertContinuousLinkIndex = 0;		// 縦連続
	int vertFenceLinkIndex = 0;		// 縦フェンス

	// 生成リストを検索
	for (int index = 0; index < m_MapActorCreateData.Num(); ++index)
	{
		// 何番目のActorなのかを検索
		int actorArrayIndex = GetMapActorArrayIndex(m_MapActorCreateData[index].generateActorStruct);
		if (actorArrayIndex == -1)
		{
			// 床かどうか判別
			if (m_MapActorCreateData[index].generateString != "Ground")
			{
				UE_LOG(LogTemp, Warning, TEXT("[MapCreator] Could not get the index of the Actor."));
				continue;
			}
		}
		actorArrayIndex++;

		switch (m_MapActorCreateData[index].generatePattern)
		{
		case MapPlacementPattern::Single:
		{
			// Actorを一つだけ生成する
			/*AddMapInstanceStaticMesh
			(
				m_SampleMapObject[actorArrayIndex],
				m_MapActorCreateData[index].generateActorStruct,
				LocationX(m_MapActorCreateData[index].rowIndex),
				LocationY(m_MapActorCreateData[index].columnIndex, m_StrMapLength)
			);*/
			break;
		}

		case MapPlacementPattern::Fence:
		{
			// 縦のフェンス生成に使用する生成データの要素番号を格納する
			int fenceDataIndex = GetLinkIndex(m_MapActorCreateData[index].vertLinkNum, m_FenceData);

			if (fenceLinkIndex == fenceDataIndex)
			{
				FMapActorStructCpp actorTemp = m_MapActorCreateData[index].generateActorStruct;
				actorTemp.scale = m_FenceData[fenceDataIndex].scale;

				// Actorを一つだけ生成する
				/*AddMapInstanceStaticMesh
				(
					m_SampleMapObject[actorArrayIndex],
					actorTemp,
					LocationX((m_FenceData[fenceDataIndex].startIndex + m_FenceData[fenceDataIndex].endIndex) / 2.0f),
					LocationY(m_MapActorCreateData[index].columnIndex, m_StrMapLength)
				);*/

				// 次の連続生成Actorの生成へ
				++fenceLinkIndex;
			}
			break;
		}

		case MapPlacementPattern::V_Fence:
		{
			// 縦のフェンス生成に使用する生成データの要素番号を格納する
			int fenceDataIndex = GetLinkIndex(m_MapActorCreateData[index].vertLinkNum, m_VerticalFenceData);

			if (vertFenceLinkIndex == fenceDataIndex)
			{
				FMapActorStructCpp actorTemp = m_MapActorCreateData[index].generateActorStruct;
				actorTemp.scale = m_VerticalFenceData[fenceDataIndex].scale;

				// Actorを一つだけ生成する
				/*AddMapInstanceStaticMesh
				(
					m_SampleMapObject[actorArrayIndex],
					actorTemp,
					LocationX((m_VerticalFenceData[fenceDataIndex].startIndex + m_VerticalFenceData[fenceDataIndex].endIndex) / 2.0f),
					LocationY(m_MapActorCreateData[index].columnIndex, m_StrMapLength)
				);*/

				// 次の連続生成Actorの生成へ
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

// FMapActorStructCppの要素が何番目か調べる
int AMapCreator::GetMapActorArrayIndex(FMapActorStructCpp& _mapActorStruct)
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

// CSVファイルを書き出す
bool AMapCreator::ExportCSVFromActorArray(const TArray<FMapActorStructCpp> _mapActorArray)
{	
	FString inStr = TEXT("IndexNum,ActorName,GenerateString\n");

	for (int i = 0; i < _mapActorArray.Num(); ++i)
	{
		inStr += FString::FromInt(i);
		inStr += TEXT(",");

		FString actorName;
		FString tmpLeft;
		FString tmpRight;

		_mapActorArray[i].actor.GetDefaultObject()->GetName().Split("_", &tmpLeft, &tmpRight, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		tmpLeft.Split("_", &tmpRight, &actorName, ESearchCase::IgnoreCase, ESearchDir::FromEnd);

		inStr += actorName;
		inStr += TEXT(",");

		if (_mapActorArray[i].geterateType == MapPlacementPattern::Single)
		{
			inStr += _mapActorArray[i].generateChar;
			inStr += TEXT("\n");
		}
		else
		{
			inStr += _mapActorArray[i].generateCharStart;
			inStr += TEXT(" ");
			inStr += _mapActorArray[i].generateCharEnd;
			inStr += TEXT("\n");			
		}	
	}

	FString mapName = GetWorld()->GetMapName();
	mapName = GetWorld()->RemovePIEPrefix(mapName);

	FString path = FPaths::GameDir() + (TEXT("Content/_MapCreator_MapActorCSV/"));

	FString name = TEXT("MapActorList_");
	name += mapName;
	name += TEXT(".csv");
		
	UE_LOG(LogTemp, Warning, TEXT("Path = %s"), *path);
	UE_LOG(LogTemp, Warning, TEXT("Name = %s"), *name);
	return FFileHelper::SaveStringToFile(inStr, *(path + name));
}