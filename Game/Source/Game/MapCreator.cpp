// マップの自動配置を行うクラス

// 2020/12/02 渡邊 龍音 作成

#include "MapCreator.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/DataTable.h"
#include <time.h>

AMapCreator::AMapCreator()
	: m_VisibleMapWire(false)
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

	// SubObjectの作成
	m_SampleGround = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("SampleGround"));
	m_SampleGround->SetupAttachment(RootComponent);

	// 文字列配列の初期化
	m_StrMapArray.Reset();

	// 生成するActorの情報を保存
	m_MapActorCreateData.Reset();

	// 縦方向に生成するものの情報リストを初期化
	m_VerticalFenceData.Reset();
	m_VerticalContinuousData.Reset();

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

	// 床のインスタンスメッシュの削除
	if (m_SampleGround != nullptr)
	{
		m_SampleGround->ClearInstances();
	}

	// マップオブジェクト用インスタンスメッシュ配列の初期化
	if (m_SampleMapObject.Num() > 0)
	{
		for (int i = 0; i < m_SampleMapObject.Num(); ++i)
		{
			if (m_SampleMapObject[i] != nullptr)
			{
				// インスタンス削除
				m_SampleMapObject[i]->ClearInstances();
			}
		}
	}
	// マップオブジェクト用インスタンスメッシュ配列リセット
	m_SampleMapObject.Reset();

	// メッシュ生成を行うかどうか
	if (m_VisibleMapWire)
	{
		SettingMap();
	}
}

void AMapCreator::BeginPlay()
{
	Super::BeginPlay();

	// 仮マップのインスタンスを削除
	if (m_SampleGround != nullptr)
	{
		m_SampleGround->ClearInstances();
	}

	SettingMap();

	MapCreate();
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
AActor* AMapCreator::SpawnMapActor(FMapActorStructCpp _spawnActor, const float _locationX, const float _locationY)
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
			UE_LOG(LogTemp, Warning, TEXT("rowData is nullptr. (rowIndex = %s)"), *fStrIndex);
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
					UE_LOG(LogTemp, Verbose, TEXT("Set Data. (%s)"), *(*(&rowData->Line_1 + i)));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Could Not Get rowData!"));
				}
			}

			// 文字列配列の長さを代入
			m_StrMapLength = _stringArray.Num();
		}
	}
	// マップデータがない
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MapData is nullptr."));
		isLoad = false;
	}

	return isLoad;
}

// 文字列の比較を行い、一致した時の情報を格納する
void AMapCreator::ComparisonChar(TArray<FMapActorStructCpp>& _generateActor, TArray<FString>& _stringArray, const int _rowIndex, TArray<CreateData>& _generateInfoArray)
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
				CreateData groundData(m_MapActorGround, _rowIndex, m_StrMapLength / 2, "Ground", MapPlacementPattern::Single);
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
			CreateData playerData(m_PlayerActor, _rowIndex, columnIndex, "Player", MapPlacementPattern::Single);
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
			if (_generateActor[actorIndex].geterateType == MapPlacementPattern::Single ||
				_generateActor[actorIndex].geterateType == MapPlacementPattern::Continuous ||
				_generateActor[actorIndex].geterateType == MapPlacementPattern::V_Continuous)
			{
				if (_stringArray[columnIndex] == _generateActor[actorIndex].generateChar)
				{
					// 発見状態にする
					isFound = true;

					// 文字が一致した生成Actorを生成リストに追加して検索ループ終了
					CreateData playerData(_generateActor[actorIndex], _rowIndex, columnIndex, _generateActor[actorIndex].generateChar, _generateActor[actorIndex].geterateType, false);
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
					// 文字が一致した生成Actorを生成リストに追加して検索ループ終了
					CreateData playerData(_generateActor[actorIndex], _rowIndex, columnIndex, _generateActor[actorIndex].generateCharStart, _generateActor[actorIndex].geterateType, true, true);
					_generateInfoArray.Add(playerData);
					break;
				}
				else if (_stringArray[columnIndex] == _generateActor[actorIndex].generateCharEnd)
				{
					// 文字が一致した生成Actorを生成リストに追加して検索ループ終了
					CreateData playerData(_generateActor[actorIndex], _rowIndex, columnIndex, _generateActor[actorIndex].generateCharEnd, _generateActor[actorIndex].geterateType, true, false);
					_generateInfoArray.Add(playerData);
					break;
				}
			}
			// その他の場合
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("The \"Generate Type\" set in MapActorArray[%d] is invalid."), actorIndex);
			}
		}

		// CSVファイルの文字列と一致する文字列がマップに生成するActorに設定されていない
		if (isFound == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("The string \"%s\" is set in the CSV file, but not in the MapActorArray. Check both the CSV file and the MapActorArray."), *_stringArray[columnIndex])
		}
	}
}

// 縦に並んだフェンスの紐付けを行う
void AMapCreator::LinkingVerticalFence(TArray<CreateData>& _generateInfoArray)
{
	// 生成リストにデータがない
	if (_generateInfoArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("There is no data in _generateInfoArray."));
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
							UE_LOG(LogTemp, Warning, TEXT("A fence generation character was found during vertical fence generation."))
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
							m_VerticalFenceData[fenceDataIndex].scale.X =
								ContinuousScale(m_VerticalFenceData[fenceDataIndex].startIndex, m_VerticalFenceData[fenceDataIndex].endIndex, _generateInfoArray[actorIndex].generateActorStruct.scale.Y);

							// 紐付け番号のインクリメント
							++linkIndex;
						}
						// 生成中でなければログ
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("A closing character was found before the vertical fence generation character."))
						}
					}
				}
			}
		}
	}
}

// 縦への連続生成の設定を行う
void AMapCreator::LinkingVerticalContinuous(TArray<CreateData>& _generateInfoArray)
{
	// 生成リストにデータがない
	if (_generateInfoArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("There is no data in _generateInfoArray."));
		return;
	}
	// データがある
	else
	{
		// 紐付けに使用する値
		int linkIndex = 0;
		// 連続生成開始を保持
		bool isStart = false;
		// 文字を保持
		FString stringTemp = "";
		// 行を保持
		int rowTemp = 0;
		// 前のActorの配列番号を保持
		int prevIndex = 0;

		// 列ごとに検索
		for (int column = 0; column < m_StrMapLength; ++column)
		{
			// 生成リストの検索
			for (int actorIndex = 0; actorIndex < _generateInfoArray.Num(); ++actorIndex)
			{
				// 列が一致する縦の連続生成の場合
				if (_generateInfoArray[actorIndex].generatePattern == MapPlacementPattern::V_Continuous &&
					_generateInfoArray[actorIndex].columnIndex == column)
				{
					// 保持している文字が空白
					if (stringTemp == "")
					{
						// 連続生成の開始
						isStart = true;
						// 文字の保存
						stringTemp = _generateInfoArray[actorIndex].generateString;
						// 行の保存
						rowTemp = _generateInfoArray[actorIndex].rowIndex;
						// 配列番号の保存
						prevIndex = actorIndex;
						// 開始位置の設定
						AddLinkIndex(_generateInfoArray[actorIndex].generateActorStruct, linkIndex, m_VerticalContinuousData, rowTemp, true);

						// 紐付ける
						_generateInfoArray[actorIndex].vertLinkNum = linkIndex;

						// ループをやり直す
						continue;
					}

					// 行番号が連番になっていない もしくは 文字が以前と違う（連続生成が途切れた）
					if ((rowTemp + 1) != _generateInfoArray[actorIndex].rowIndex ||
						stringTemp != _generateInfoArray[actorIndex].generateString)
					{
						// 連続生成の終了
						isStart = false;
						// 前のActorの終了位置の設定
						AddLinkIndex(_generateInfoArray[actorIndex].generateActorStruct, linkIndex, m_VerticalContinuousData, rowTemp + 1, false);
						// 前のActorを紐付ける
						_generateInfoArray[prevIndex].vertLinkNum = linkIndex;

						// スケールの変更
						//紐付けられた番号から要素番号を取得
						int continuousDataIndex = GetLinkIndex(linkIndex, m_VerticalContinuousData);

						// Y方向のスケールを変更
						m_VerticalContinuousData[continuousDataIndex].scale.X =
							ContinuousScale(m_VerticalContinuousData[continuousDataIndex].startIndex, m_VerticalContinuousData[continuousDataIndex].endIndex, _generateInfoArray[actorIndex].generateActorStruct.scale.Y);

						// 異なるActorで再設定する
						// 紐付け番号のインクリメント
						++linkIndex;

						// 文字の保存
						stringTemp = _generateInfoArray[actorIndex].generateString;
						// 行の保存
						rowTemp = _generateInfoArray[actorIndex].rowIndex;
						// 配列番号の保存
						prevIndex = actorIndex;
						// 開始位置の設定
						AddLinkIndex(_generateInfoArray[actorIndex].generateActorStruct, linkIndex, m_VerticalContinuousData, rowTemp, true);
						// 紐付ける
						_generateInfoArray[actorIndex].vertLinkNum = linkIndex;
					}
					// 前と同じ文字列
					else
					{
						// 連続生成状態
						isStart = true;
						// 行の保存
						rowTemp = _generateInfoArray[actorIndex].rowIndex;
						// 配列番号の保存
						prevIndex = actorIndex;
						// 紐付ける
						_generateInfoArray[actorIndex].vertLinkNum = linkIndex;
					}

				}
			}
			// ループが終わっても生成が終わっていない場合
			if (isStart == true)
			{
				// 連続生成の終了
				isStart = false;
				// 前のActorの終了位置の設定
				AddLinkIndex(_generateInfoArray[prevIndex].generateActorStruct, linkIndex, m_VerticalContinuousData, rowTemp + 1, false);
				// 前のActorを紐付ける
				_generateInfoArray[prevIndex].vertLinkNum = linkIndex;
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
void AMapCreator::AddLinkIndex(FMapActorStructCpp _actorStruct, int _linkIndex, TArray<ContinuousData>& _array, int _value, bool isStart/* = true*/)
{
	ContinuousData vertTemp = { _linkIndex , 0, 0, _actorStruct.scale};

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
		UE_LOG(LogTemp, Error, TEXT("THE END"));
	}

	if (isStart)
	{
		_array[index].startIndex = _value;
	}
	else
	{
		_array[index].endIndex = _value;
	}

	UE_LOG(LogTemp, Verbose, TEXT("Array[%d] element  link : %d, start : %d, end : %d"),
		index, _array[index].linkIndex, _array[index].startIndex, _array[index].endIndex);
}

// マップ生成情報を設定
void AMapCreator::SettingMap()
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
			UE_LOG(LogTemp, Verbose, TEXT("The number of rows in the map is %d."), m_MapRowNumber);
			break;
		}
	}
	LinkingVerticalFence(m_MapActorCreateData);
	LinkingVerticalContinuous(m_MapActorCreateData);
}

// 生成を行う
void AMapCreator::MapCreate()
{
	// 連続生成の際に重複しないようにする
	int continuousLinkIndex = 0;
	int fenceLinkIndex = 0;

	// 生成リストを検索
	for (int index = 0; index < m_MapActorCreateData.Num(); ++index)
	{
		// 連続生成に使用する生成データの要素番号を格納する
		int continuousDataIndex = GetLinkIndex(m_MapActorCreateData[index].vertLinkNum, m_VerticalContinuousData);
		// フェンス生成に使用する生成データの要素番号を格納する
		int fenceDataIndex = GetLinkIndex(m_MapActorCreateData[index].vertLinkNum, m_VerticalFenceData);

		switch (m_MapActorCreateData[index].generatePattern)
		{
		case MapPlacementPattern::Single:
			// Actorを一つだけ生成する
			SpawnMapActor
			(
				m_MapActorCreateData[index].generateActorStruct,
				LocationX(m_MapActorCreateData[index].rowIndex),
				LocationY(m_MapActorCreateData[index].columnIndex, m_StrMapLength)
			);
			break;

		case MapPlacementPattern::V_Continuous:

			if (continuousLinkIndex == continuousDataIndex)
			{
				FMapActorStructCpp actorTemp = m_MapActorCreateData[index].generateActorStruct;
				actorTemp.scale = m_VerticalContinuousData[continuousDataIndex].scale;

				// Actorを一つだけ生成する
				SpawnMapActor
				(
					actorTemp,
					LocationX((m_VerticalContinuousData[continuousDataIndex].startIndex + m_VerticalContinuousData[continuousDataIndex].endIndex) / 2.0f),
					LocationY(m_MapActorCreateData[index].columnIndex, m_StrMapLength)
				);

				// 次の連続生成Actorの生成へ
				++continuousLinkIndex;
			}
			break;
			
		case MapPlacementPattern::V_Fence:

			if (fenceLinkIndex == fenceDataIndex)
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
				++fenceLinkIndex;
			}
			break;

		default:
			UE_LOG(LogTemp, Warning, TEXT("MapPlacementPattern is invalid! enum index number : %d"), (int)m_MapActorCreateData[index].generatePattern);
			break;
		}
	}
}