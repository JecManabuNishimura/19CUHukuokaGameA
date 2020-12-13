// マップの自動配置を行うクラス

// 2020/12/02 渡邊 龍音 作成

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
	// Tickを実行する必要はないので更新を切る
	PrimaryActorTick.bCanEverTick = false;

	// SubObjectの作成
	m_SampleGround = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("SampleGround"));

	m_SampleObjectArray.Add(CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("SampleObject")));

	// 文字列配列の初期化
	strArrayTemp.Reset();

	// 連続配置用の配列の初期化
	m_ColumnStartVertArray.Reset();
	m_RowStartArray.Reset();
	m_ColumnStartVertFenceArray.Reset();
	m_RowStartFenceArray.Reset();

	m_MapActorGround.geterateType = MapPlacementPattern::SettingLock;

	// プレイヤーActorの設定
	m_PlayerActor.geterateType = MapPlacementPattern::SettingLock;

	// マップActor一時保存変数を初期化
	m_ContinuousActorTemp = MapActorStructCppReset();
	m_ContinuousVertActorTempArray.Reset();
	m_FenceActorTempArray.Reset();
}

void AMapCreator::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	// 行番号
	int rowIndex = 0;

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

	// インスタンスメッシュがある
	if (m_SampleGround != nullptr)
	{
		// インスタンスメッシュの削除
		m_SampleGround->ClearInstances();

		// 床のActorが設定されていれば
		if (m_MapActorGround.actor != nullptr)
		{
			// C++で作成されたDefaultSubObjectにStaticMeshComponentがついていれば
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

	// メッシュ生成を行うかどうか
	if (m_VisibleMapWire)
	{
		// m_IsLoadMapDataがfalseになるまでデータの読み込み
		do
		{
			SetStrArrayMapData(strArrayTemp, rowIndex, m_IsLoadMapData);

			//	マップに配置する床が設定されていない
			if (m_MapActorGround.actor == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("MapCreator::Need set m_MapActorGround."));
			}
			else
			{
				// 床の位置を算出
				FVector groundPos(rowIndex * m_XAxis_Offset, (float)strArrayTemp.Num() / 2.0f, m_MapActorGround.location_Z);
				FTransform groundTransform(m_MapActorGround.rotation, groundPos, m_MapActorGround.scale);

				// インスタンスメッシュが存在している
				if (m_SampleGround != nullptr)
				{
					m_SampleGround->AddInstance(groundTransform);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("MapCreator::Need set m_SampleGround."));
				}
			}

			// 次の行を読み込む
			strArrayTemp.Reset();
			++rowIndex;

		} while (m_IsLoadMapData);
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

	// 変数宣言
	bool isFenceMake = false;
	int rowIndex = 0;

	// 文字配列初期化
	strArrayTemp.Reset();

	// CSVファイル読み込み
	do
	{
		// Loading
		UE_LOG(LogTemp, Verbose, TEXT("Map Loading Now..."));

		// マップデータを読み込む
		SetStrArrayMapData(strArrayTemp, rowIndex, m_IsLoadMapData);

		// マップデータがなければおわり
		if (m_IsLoadMapData == false)
		{
			// 縦方向の連続配置チェック
			if (m_ColumnStartVertArray.Num() > 0)
			{
				// 文字列配列を一つずつ読みこむ
				FMapStructCpp mapStructTemp;
				for (int columnIndex = 0; columnIndex < mapStructTemp.indexNum; ++columnIndex)
				{
					// 設定したm_MapActorArrayの要素数分、検索を行う
					for (int i = 0; i < m_MapActorArray.Num(); ++i)
					{
						// 縦方向に連続配置中のActor分チェックする
						for (int j = 0; j < m_ColumnStartVertArray.Num(); ++j)
						{
							// 列が一致するか
							if (m_ColumnStartVertArray[j] == columnIndex)
							{
								m_ContinuousVertActorTempArray[j].scale.X = (rowIndex - m_RowStartArray[j]) * m_ContinuousVertActorTempArray[j].scale.X;

								SpawnMapActor(m_ContinuousVertActorTempArray[j], LocationX((rowIndex + m_RowStartArray[j]) / 2.0f), LocationY(m_ColumnStartVertArray[j], mapStructTemp.indexNum));

								// 要素の消去
								// 保存していたActorの削除
								m_ContinuousVertActorTempArray.RemoveAt(j);
								// 列の削除
								m_ColumnStartVertArray.RemoveAt(j);
								// 行の削除
								m_RowStartArray.RemoveAt(j);
							}
						}
					}
				}
			}

			return;
		}

		// 文字列配列を一つずつ読みこむ
		for (int columnIndex = 0; columnIndex < strArrayTemp.Num(); ++columnIndex)
		{
			// 一番最初の文字がNL（地面を生成しない場合）でなければ
			if (strArrayTemp[0] != m_NotGroundGenerateStr)
			{
				// 地面生成
				if (m_MapActorGround.actor != nullptr)
				{
					SpawnMapActor(m_MapActorGround, LocationX(rowIndex), 0.0f);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("m_MapActorGround.actor is nullptr!"))
				}
			}
			// 地面を生成しない場合は他のActorも生成しないのでスキップする
			else
			{
				break;
			}

			// 文字なしの場合
			if (strArrayTemp[columnIndex] == "")
			{
				// 横に連続配置中だったかチェック
				if (m_IsContinuous == true)
				{
					m_IsContinuous = false;

					SpawnContinuousActor(rowIndex, m_ColumnStart, columnIndex);
				}
			}
			// プレイヤー生成の場合
			else if (strArrayTemp[columnIndex] == m_PlayerGenerateStr)
			{
				// プレイヤーを生成していなければ
				if (m_IsGeneratePlayer == false)
				{
					// プレイヤーを生成した
					m_IsGeneratePlayer = true;

					// プレイヤーの生成
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
			// 上記以外の文字の場合
			else
			{
				// マップにActorを生成する
				// 設定したm_MapActorArrayの要素数分、検索を行う
				for (int mapActorIndex = 0; mapActorIndex < m_MapActorArray.Num(); ++mapActorIndex)
				{
					// 文字配列の要素とm_MapActorArrayに設定した文字が一致していたら
					if (strArrayTemp[columnIndex] == m_MapActorArray[mapActorIndex].generateChar)
					{
						// 横方向に連続配置中で、連続配置が終わったかチェック
						if (m_IsContinuous == true && m_ContinuousActorTemp.actor != m_MapActorArray[mapActorIndex].actor)
						{
							m_IsContinuous = false;

							SpawnContinuousActor(rowIndex, m_ColumnStart, columnIndex);
						}

						// 縦方向の連続配置チェック
						if (m_ColumnStartVertArray.Num() > 0)
						{
							// 縦方向に連続配置中のActor分チェックする
							for (int j = 0; j < m_ColumnStartVertArray.Num(); ++j)
							{
								// 縦方向に連続配置中で、連続配置が終わったかチェック
								if (m_ColumnStartVertArray[j] == columnIndex && m_ContinuousVertActorTempArray[j].actor != m_MapActorArray[mapActorIndex].actor)
								{
									m_ContinuousVertActorTempArray[j].scale.Y = ContinuousScale(m_ColumnStartVertArray[j], columnIndex, m_ContinuousVertActorTempArray[j].scale.Y);
									SpawnMapActor(m_ContinuousVertActorTempArray[j], LocationX(rowIndex), LocationY(columnIndex, strArrayTemp.Num()));

									// 要素の消去
									// 保存していたActorの削除
									m_ContinuousVertActorTempArray.RemoveAt(j);
									// 列の削除
									m_ColumnStartVertArray.RemoveAt(j);
									// 行の削除
									m_RowStartArray.RemoveAt(j);
								}
							}
						}

						// 生成ルールによって分岐
						switch (m_MapActorArray[mapActorIndex].geterateType)
						{
							// 単体で配置する
						case MapPlacementPattern::Single:

							// Actorを一つだけ生成する
							SpawnMapActor(m_MapActorArray[mapActorIndex], LocationX(rowIndex), LocationY(columnIndex, strArrayTemp.Num()));

							break;

							// 横方向に連続配置で一つのActorになる
						case MapPlacementPattern::Continuous:

							// 横の連続配置の開始
							if (m_IsContinuous == false)
							{
								m_IsContinuous = true;

								// 連続配置を開始したActorの保存
								m_ContinuousActorTemp = m_MapActorArray[mapActorIndex];

								// 位置の保存
								m_ColumnStart = columnIndex;
							}
							break;

							// 縦方向に連続配置で一つのActorになる
						case MapPlacementPattern::V_Continuous:

							// 連続配置を開始したActorの保存
							m_ContinuousVertActorTempArray.Add(m_MapActorArray[mapActorIndex]);

							// 位置の保存
							// 列
							m_ColumnStartVertArray.Add(columnIndex);

							// 行
							m_RowStartArray.Add(rowIndex);

							// 要素が2つ以上ある場合
							if (m_ColumnStartVertArray.Num() >= 2)
							{
								// 一番新しく追加した要素が同じ列の場合
								if (m_ColumnStartVertArray[m_ColumnStartVertArray.Num() - 1] == columnIndex)
								{
									for (int i = 0; i < m_ContinuousVertActorTempArray.Num() - 2; ++i)
									{
										// 追加したActorが同じActorであれば破棄
										if (m_ContinuousVertActorTempArray[m_ContinuousVertActorTempArray.Num() - 1].actor == m_ContinuousVertActorTempArray[i].actor)
										{
											// 連続配置を開始したActorの保存
											m_ContinuousVertActorTempArray.RemoveAt(m_ContinuousVertActorTempArray.Num() - 1);

											// 位置の保存
											// 列
											m_ColumnStartVertArray.RemoveAt(m_ColumnStartVertArray.Num() - 1);

											// 行
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
					// 文字配列の要素と連続配置するm_MapActorArrayに設定したStartの文字が一致していたら
					else if (strArrayTemp[columnIndex] == m_MapActorArray[mapActorIndex].generateCharStart)
					{
						// 生成ルールによって分岐
						switch (m_MapActorArray[mapActorIndex].geterateType)
						{
							// 始点と終点を指定して一つのActorを生成する（横方向）
						case MapPlacementPattern::Fence:

							// フェンス配置の開始
							if (m_IsFence == false)
							{
								m_IsFence = true;

								// フェンス生成開始の列を保存
								m_FenceStart = columnIndex;
							}
							break;

							// 始点と終点を指定して一つのActorを生成する（縦方向）
						case MapPlacementPattern::V_Fence:

							// フェンス配置の開始地点の情報を保存

							// 生成Actorの保存
							m_FenceActorTempArray.Add(m_MapActorArray[mapActorIndex]);

							// 行の保存
							m_RowStartFenceArray.Add(rowIndex);
							// 列の保存
							m_ColumnStartVertFenceArray.Add(columnIndex);

							break;

						default:
							break;
						}
					}
					// 文字配列の要素と連続配置するm_MapActorArrayに設定したEndの文字が一致していたら
					else if (strArrayTemp[columnIndex] == m_MapActorArray[mapActorIndex].generateCharEnd)
					{
						// 生成ルールによって分岐
						switch (m_MapActorArray[mapActorIndex].geterateType)
						{
							// 始点と終点を指定して一つのActorを生成する（横方向）
						case MapPlacementPattern::Fence:

							// フェンス生成
							// フェンス生成が開始されていれば
							if (m_IsFence == true)
							{
								m_IsFence = false;

								SpawnContinuousActor(rowIndex, m_FenceStart, columnIndex);
							}
							break;

							// 始点と終点を指定して一つのActorを生成する（縦方向）
						case MapPlacementPattern::V_Fence:

							// フェンス生成
							// 生成を開始したフェンスがあれば
							if (m_FenceActorTempArray.Num() > 0)
							{
								// 列要素分検索
								for (int columnCheck = 0; columnCheck < m_ColumnStartVertFenceArray.Num(); ++columnCheck)
								{
									// 列が一致したら
									if (m_ColumnStartVertFenceArray[columnCheck] == columnIndex)
									{
										// Actorも一致していれば
										if (m_FenceActorTempArray[columnCheck].actor == m_MapActorArray[mapActorIndex].actor)
										{
											// Yスケールを設定
											m_FenceActorTempArray[columnCheck].scale.Y = (rowIndex - m_RowStartFenceArray[columnCheck]) * m_FenceActorTempArray[columnCheck].scale.Y;

											// 生成
											SpawnMapActor(m_FenceActorTempArray[columnCheck], LocationX((rowIndex + m_RowStartFenceArray[columnCheck]) / 2.0f), LocationY(m_ColumnStartVertFenceArray[columnCheck], strArrayTemp.Num()));

											// 要素の削除
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

		// 次の行を読み込む
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

// 文字列配列に一行分のマップデータを保存
void AMapCreator::SetStrArrayMapData(TArray<FString>& _strArray, const int _rowIndex, bool& _isLoad)
{
	_isLoad = true;

	// マップデータがある
	if (m_MapData != nullptr)
	{
		// intの行番号をFNameに変換するために、FStringに変換する（FNameへの変換はFStringからしかできないため）
		FString fStrIndex = FString::FromInt(_rowIndex + 1);

		// FStringからFNameに変換する
		FName fNameIndex = FName(*fStrIndex);

		// 行ごとのデータの取得
		FMapStructCpp* rowData = m_MapData->FindRow<FMapStructCpp>(fNameIndex, FString());

		// データ行の終わり
		if (rowData == nullptr)
		{
			// データ読み込み終了
			_isLoad = false;
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
					_strArray.Add(*(&rowData->Line_1 + i));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Could Not Get rowData!"));
				}
			}
		}
	}
	// マップデータがない
	else
	{
		_isLoad = false;
	}
}

void AMapCreator::SpawnContinuousActor(const int _rowIndex, const int _startColumn, const int _endColumn)
{
	// 連続生成の終了
	m_IsContinuous = false;

	// Actorの生成
	// スケールの算出
	FVector newScale(m_ContinuousActorTemp.scale.X,
		ContinuousScale(_startColumn, _endColumn, m_ContinuousActorTemp.scale.Y),
		m_ContinuousActorTemp.scale.Z);

	// スケールの適用
	m_ContinuousActorTemp.scale = newScale;

	// 座標を指定してActorを生成
	SpawnMapActor(m_ContinuousActorTemp, LocationX(_rowIndex), ContinuousLocationY(_startColumn, _endColumn, strArrayTemp.Num()));

	// Actorの一時保存していたものをリセット
	m_ContinuousActorTemp = MapActorStructCppReset();
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

// 連続生成ActorのY位置算出
float AMapCreator::ContinuousLocationY(const int _startColumn, const int _endColumn, const int _strArrayLength)
{
	float add = (_startColumn + _endColumn) / 2.0f;
	float halfLength = (float)(_strArrayLength - 1) / 2.0f;

	return (add - halfLength) * m_YAxis_Offset;
}

// マップにActorを生成
AActor* AMapCreator::SpawnMapActor(FMapActorStructCpp _spawnActor, const float _locationX, const float _locationY)
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