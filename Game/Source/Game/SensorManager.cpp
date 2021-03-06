// センサーの管理を行うクラス

// 2020/12/08 渡邊 龍音 作成

#include "SensorManager.h"
#include "BlueprintUtility.h"

USerial* USensorManager::m_ArduinoSerial = NULL;
bool	 USensorManager::m_IsOpen = false;
int		 USensorManager::m_SerialPort = -1;
FVector	 USensorManager::m_Standard = FVector::ZeroVector;
FVector  USensorManager::m_MaxIncline = FVector::ZeroVector;
FVector	 USensorManager::m_Deadzone = FVector::ZeroVector;
bool	 USensorManager::isFlipX = false;
bool	 USensorManager::isFlipY = false;
bool	 USensorManager::isFlipZ = false;
float	 USensorManager::m_XYFreezeZAxisRotation = 0.0f;
FVector	 USensorManager::m_MaxVariation = FVector(10.0f, 10.0f, 10.0f);

// センサーとの接続
bool USensorManager::ConnectToSensor(int _maxSerialPort /* = 20*/, int _checkSensorNum/* = 500*/, int _tryConnectNum /* = 1*/, bool _isResetStandard/* = true*/, bool _isResetMaxIncline/* = true*/, bool _isResetDeadZone/* = true*/)
{
	DisconnectToSensor(_isResetStandard, _isResetMaxIncline, _isResetDeadZone);
	m_IsOpen = false;

	// 繰り返し接続を試みる回数
	for (int tryCnt = 0; tryCnt < _tryConnectNum; ++tryCnt)
	{
		// シリアルポートを探す
		for (int i = 0; i < _maxSerialPort && m_IsOpen == false; ++i)
		{
			m_ArduinoSerial = USerial::OpenComPort(m_IsOpen, i, 115200);

			if (m_IsOpen == false)
			{
				UE_LOG(LogTemp, Warning, TEXT("[SensorManager] COM Port:%d is failed open."), i);
			}
			else
			{
				// 本当にポート先がセンサーか確認する

				bool isRead = false;
				FString fStr = "";
				int tryNum = 0;

				do
				{
					// initialの 'i' を送信
					m_ArduinoSerial->Println(FString(TEXT("s")));

					// 文字を読み取る
					fStr = m_ArduinoSerial->Readln(isRead);
					tryNum++;
				} while (isRead == false && tryNum < _checkSensorNum);

				UE_LOG(LogTemp, VeryVerbose, TEXT("[SensorManager] Try Read Count: %d / %d"), tryNum, _checkSensorNum);

				UE_LOG(LogTemp, Verbose, TEXT("[SensorManager] Data = %s"), *fStr);

				// センサーからデータを読み出せた場合
				if (isRead == true)
				{
					// 成功なので処理終了
					UE_LOG(LogTemp, Display, TEXT("[SensorManager] COM Port:%d is Successfully Open."), i);
					m_SerialPort = i;
					break;
				}
				else
				{
					// 正しくないので継続
					m_IsOpen = false;
					m_ArduinoSerial = NULL;
					UE_LOG(LogTemp, Warning, TEXT("[SensorManager] COM Port:%d is open but not sensor."), i);
				}
			}
		}
	}

	return m_IsOpen;
}

// センサーとの接続を切る
void USensorManager::DisconnectToSensor(bool _isResetStandard/* = true*/, bool _isResetMaxIncline/* = true*/, bool _isResetDeadZone/* = true*/)
{
	UE_LOG(LogTemp, Warning, TEXT("[SensorManager] Sensor is close."));

	if (m_ArduinoSerial != NULL && m_IsOpen == true)
	{
		// ポートから切断
		m_ArduinoSerial->Close();
	}

	// センサーの管理を終了
	m_ArduinoSerial = NULL;
	
	// シリアルポート初期化
	m_SerialPort = -1;

	// 他の変数初期化
	ResetSensorProperty(_isResetStandard, _isResetMaxIncline, _isResetDeadZone);
}

// 基準値を設定する
void USensorManager::SetStandardAuto(int _qualityLoop/* = 100*/)
{
	m_Standard = GetSensorAverage(_qualityLoop);
}

// 最大値を設定する
float USensorManager::SetMaxInclineAuto(FString _element, int _getMaxLoop/* = 100*/, int _qualityLoop/* = 100*/)
{
	int elementNum;

	// 要素（_element）の変換
	// X軸
	if (_element == "x" || _element == "X")
	{
		elementNum = 0;
	}
	// Y軸
	else if (_element == "y" || _element == "Y")
	{
		elementNum = 1;
	}
	// Z軸
	else if (_element == "z" || _element == "Z")
	{
		elementNum = 2;
	}
	// 軸が正しくない
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SensorManager] The first argument of SensorManager::SetMaxIncline is incorrect. Enter one of \"X\", \"Y\", \"Z\""));
		return -1.0f;
	}

	// センサーデータをfloat型で保持
	float sensorArray[3] = {};

	// 最大値
	float maxArray[3] = {};

	for (int loop = 0; loop < _getMaxLoop; ++loop)
	{
		bool tmp;

		// センサーデータを取得
		FVector rowData = GetSensorDataRaw(tmp, tmp, _qualityLoop);

		// 配列に格納
		sensorArray[0] = rowData.X;
		sensorArray[1] = rowData.Y;
		sensorArray[2] = rowData.Z;

		// X, Y, Zそれぞれの最大値チェック
		for (int i = 0; i < 3; ++i)
		{
			if (sensorArray[i] > maxArray[i])
			{
				maxArray[i] = sensorArray[i];
			}
		}
	}

	// m_MaxIncline操作用ポインタ配列
	float* maxInclineAdrArray[3] = {};

	// m_MaxInclineのアドレスを追加
	maxInclineAdrArray[0] = &(m_MaxIncline.X);
	maxInclineAdrArray[1] = &(m_MaxIncline.Y);
	maxInclineAdrArray[2] = &(m_MaxIncline.Z);

	// m_MaxInclineの指定した要素に、取得したセンサーデータを代入
	if (maxInclineAdrArray[elementNum] != nullptr)
	{
		if (maxArray[elementNum] != SENSOR_ERROR_READ.X)
		{
			*maxInclineAdrArray[elementNum] = maxArray[elementNum];
		}
		else
		{
			*maxInclineAdrArray[elementNum] = 0.0f;
		}
		return *maxInclineAdrArray[elementNum];
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SensorManager] Could not get m_MaxIncline address."));
		return -1.0f;
	}
}

// デッドゾーンを設定する
void USensorManager::SetDeadZoneAuto(int _qualityLoop/* = 100*/)
{
	// センサーデータをfloat型で保持
	float sensorArray[3] = {};

	// センサーデータを取得
	FVector rowData = GetSensorAverage(_qualityLoop);

	// 配列に格納
	sensorArray[0] = rowData.X;
	sensorArray[1] = rowData.Y;
	sensorArray[2] = rowData.Z;

	// m_Deadzone操作用ポインタ配列
	float* deadZoneAdrArray[3] = {};

	// m_Deadzoneのアドレスを追加
	deadZoneAdrArray[0] = &(m_Deadzone.X);
	deadZoneAdrArray[1] = &(m_Deadzone.Y);
	deadZoneAdrArray[2] = &(m_Deadzone.Z);

	for (int i = 0; i < 3; ++i)
	{
		if (sensorArray[i] != SENSOR_ERROR_READ.X)
		{
			*deadZoneAdrArray[i] = sensorArray[i];
		}
		else
		{
			*deadZoneAdrArray[i] = 0.0f;
		}
	}
}

// センサーで使用するプロパティの初期化
void USensorManager::ResetSensorProperty(bool _isResetStandard/* = true*/, bool _isResetMaxIncline/* = true*/, bool _isResetDeadZone/* = true*/)
{
	// 基準値の初期化
	if (_isResetStandard)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[SensorManager] SensorManager::m_Standard is Reset."))
			m_Standard = FVector::ZeroVector;
	}
	// 最大値の初期化
	if (_isResetMaxIncline)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[SensorManager] SensorManager::m_MaxIncline is Reset."))
			m_MaxIncline = FVector::ZeroVector;
	}
	// デッドゾーンの初期化
	if (_isResetDeadZone)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[SensorManager] SensorManager::m_Deadzone is Reset."))
			m_Deadzone = FVector::ZeroVector;
	}
}

// センサーの平均値を取得する
FVector USensorManager::GetSensorAverage(int _qualityLoop/* = 100*/)
{
	// センサーの値を複数確保する
	TArray<FVector> sensorDataArray;
	sensorDataArray.Reset();

	// センサーから複数回データを取得する
	for (int i = 0; i < _qualityLoop; ++i)
	{
		bool bTmp;
		FVector tmp = GetSensorDataRaw(bTmp, bTmp);

		if (tmp.Equals(SENSOR_ERROR_READ) == false)
		{
			sensorDataArray.Add(tmp);
		}
	}

	// 要素があれば平均値を返す
	if (sensorDataArray.Num() > 0)
	{
		// 合計格納用FVector
		FVector sumVector = FVector::ZeroVector;

		// 加算
		for (int i = 0; i < sensorDataArray.Num(); ++i)
		{
			sumVector.X += sensorDataArray[i].X;
			sumVector.Y += sensorDataArray[i].Y;
			sumVector.Z += sensorDataArray[i].Z;
		}

		FVector result = sumVector / (float)sensorDataArray.Num();

		UE_LOG(LogTemp, Verbose, TEXT("[SensorManager] return Average SensorData. X = %f Y = %f, Z = %f"), result.X, result.Y, result.Z);
		return result;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SensorManager] Could not get Average SensorData. return SENSOR_ERROR_READ (%f, %f, %f)."), SENSOR_ERROR_READ.X, SENSOR_ERROR_READ.Y, SENSOR_ERROR_READ.Z);
		return SENSOR_ERROR_READ;
	}
}

// センサーの最大値に対する傾きの割合を取得する
FVector USensorManager::GetSensorRatio(int _divNum/* = 5*/, int _tryNum/* = 500*/, FVector _maxVector/* = FVector(60.0f, 60.0f, 90.0f)*/)
{
	FVector sensorData = GetSensorData(_tryNum);

	FVector resultData = SENSOR_ERROR_READ;

	// 最大傾きが設定されている
	if (GetMaxIncline() != FVector::ZeroVector)
	{
		// 最大値を分割したときの値
		FVector maxInclineRatio = GetMaxIncline() / _divNum;

		// 実際の値を分割したときの値
		FVector targetVector = _maxVector / _divNum;

		// センサーとの値を比較する値
		FVector comparison = maxInclineRatio;

		// 値の代入回数
		int substitutionNum = 0;

		for (int i = 1; (substitutionNum < 3) && (i < _divNum); ++i)
		{
			// X軸の比較
			// 代入していない場合
			if (resultData.X == SENSOR_ERROR_READ.X)
			{
				// Xがプラス
				if (sensorData.X > 0.0f)
				{
					if (sensorData.X <= comparison.X)
					{
						resultData.X = targetVector.X;
						substitutionNum++;
					}
				}
				// Xがマイナス
				else if (sensorData.X < 0.0f)
				{
					if (sensorData.X >= -comparison.X)
					{
						resultData.X = -targetVector.X;
						substitutionNum++;
					}
				}
				// ゼロ
				else
				{
					resultData.X = 0.0f;
					substitutionNum++;
				}
			}

			// Y軸の比較
			// 代入していない場合
			if (resultData.Y == SENSOR_ERROR_READ.Y)
			{
				// Yがプラス
				if (sensorData.Y > 0.0f)
				{
					if (sensorData.Y <= targetVector.Y)
					{
						resultData.Y = targetVector.Y;
						substitutionNum++;
					}
				}
				// Yがマイナス
				else if (sensorData.Y < 0.0f)
				{
					if (sensorData.Y >= -targetVector.Y)
					{
						resultData.Y = -targetVector.Y;
						substitutionNum++;
					}
				}
				// ゼロ
				else
				{
					resultData.Y = 0.0f;
					substitutionNum++;
				}
			}

			// Z軸の比較
			// 代入していない場合
			if (resultData.Z == SENSOR_ERROR_READ.Z)
			{
				// Zがプラス
				if (sensorData.Z > 0.0f)
				{
					if (sensorData.Z <= comparison.Z)
					{
						resultData.Z = targetVector.Z;
						substitutionNum++;
					}
				}
				// Zがマイナス
				else if (sensorData.Z < 0.0f)
				{
					if (sensorData.Z >= -comparison.Z)
					{
						resultData.Z = -targetVector.Z;
						substitutionNum++;
					}
				}
				// ゼロ
				else
				{
					resultData.Z = 0.0f;
					substitutionNum++;
				}
			}
			comparison += maxInclineRatio;
			targetVector += (_maxVector / _divNum);
		}

		if (resultData.X == SENSOR_ERROR_READ.X)
		{
			resultData.X = sensorData.X > 0.0f ? _maxVector.X : -_maxVector.X;
		}
		if (resultData.Y == SENSOR_ERROR_READ.Y)
		{
			resultData.Y = sensorData.Y > 0.0f ? _maxVector.Y : -_maxVector.Y;
		}
		if (resultData.Z == SENSOR_ERROR_READ.Z)
		{
			resultData.Z = sensorData.Z > 0.0f ? _maxVector.Z : -_maxVector.Z;
		}		
	}
	// 設定されていない
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SensorManager] Max incline value is may not set. Must be set by calling a \"SensorManager::SetMaxIncline()\" Function."));
	}

	return resultData;
}

// センサーの最大値に対する傾きの割合を取得する
UFUNCTION(BlueprintPure, Category = "Sensor Manager")
FRotator USensorManager::GetSensorRatioRotator(int _divNum/* = 5*/, int _tryNum/* = 500*/, FVector _maxVector/* = FVector(60.0f, 60.0f, 90.0f)*/)
{
	FString tmp;
	FVector tempVector = GetSensorRatio(_divNum, _tryNum, _maxVector);

	return FRotator(tempVector.Y, tempVector.Z, tempVector.X);
}

// センサーからの生のデータを取得
FVector USensorManager::GetSensorDataRaw(bool& _left, bool& _right, int _tryNum/* = 500*/)
{
	bool isRead = false;		// データを読み取れたか
	FString fStr = "";			// 読み取りデータ格納用
	int tryCnt = 0;				// 読み取ろうとした回数

	// シリアルのオブジェクトがあれば
	if (m_ArduinoSerial != NULL && m_IsOpen == true)
	{
		// データの読み取り
		// データが読み取れるか、最大読み取り回数になるまで繰り返す
		do
		{
			m_ArduinoSerial->Println(FString(TEXT("s")));

			fStr = m_ArduinoSerial->Readln(isRead);
			++tryCnt;
		} while (isRead == false && tryCnt < _tryNum);

		TArray<FString> splitTextArray;
		splitTextArray.Reset();

		UE_LOG(LogTemp, VeryVerbose, TEXT("[SensorManager] Try Read Count: %d / %d"), tryCnt, _tryNum);

		// 読み取れなかったら SENSOR_ERROR_READ を返し、終了
		// センサーに物理的な問題がある可能性が高い（接続されていない、センサーが壊れているなど）
		if (isRead == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("[SensorManager] No Data From Sensor. return SENSOR_ERROR_READ."));
			return SENSOR_ERROR_READ;
		}
		else
		{
			UE_LOG(LogTemp, Verbose, TEXT("[SensorManager] Get Data From Sensor."));
			UE_LOG(LogTemp, Verbose, TEXT("[SensorManager] All Data = %s."), *fStr);
		}

		// センサーデータをカンマ区切りでsplitTextArrayに入れる
		fStr.ParseIntoArray(splitTextArray, TEXT(":"));

		for (int i = 0; i < splitTextArray.Num(); ++i)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[SensorManager] Data[%d] = %s"), i, *splitTextArray[i]);
		}

		// それぞれをint型に変換する
		TArray<float> sensorDataArray;
		sensorDataArray.Reset();

		for (int i = 0; i < splitTextArray.Num() && i < 3; ++i)
		{
			sensorDataArray.Add(FCString::Atof(*splitTextArray[i]));
		}
		// TArrayの要素番号 2にアクセスできなければ SENSOR_ERROR_READ を返し、終了
		// センサーとのデータやり取りに問題がある可能性が高い（Arduino側のプログラムが本番用になっていない、バッファにデータが中途半端に残っているなど）
		if (sensorDataArray.IsValidIndex(2) == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("[SensorManager] Failed Add TArray<float> elements. return SENSOR_ERROR_READ."));
			return SENSOR_ERROR_READ;
		}

		// ボタンの状態を読み取りたい
		// splitTextArrayの要素番号 3にアクセスできなければ OFFにする
		if (splitTextArray.IsValidIndex(3) == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("[SensorManager] Failed Get ButtonCondition. Set to \"OFF\"."));
			_left = false;
		}
		// アクセスできた
		else
		{
			if (splitTextArray[3] == "1")
			{
				_left = true;
			}
			else
			{
				_left = false;
			}
		}
		if (splitTextArray.IsValidIndex(4) == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("[SensorManager] Failed Get ButtonCondition. Set to \"OFF\"."));
			_right = false;
		}
		// アクセスできた
		else
		{
			if (splitTextArray[4] == "1")
			{
				_right = true;
			}
			else
			{
				_right = false;
			}
		}

		UE_LOG(LogTemp, Verbose, TEXT("[SensorManager] SensorData : %f  Y:%f  Z:%f"), sensorDataArray[0], sensorDataArray[1], sensorDataArray[2]);

		if (isFlipX)
		{
			sensorDataArray[0] *= -1;
		}
		if (isFlipY)
		{
			sensorDataArray[1] *= -1;
		}
		if (isFlipZ)
		{
			sensorDataArray[2] *= -1;
		}

		return FVector(sensorDataArray[0], sensorDataArray[1], sensorDataArray[2]);
	}
	// ConnectToSensor()を呼び出していない可能性がある
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[SensorManager] SensorManager::m_ArduinoSerial is NULL. return SENSOR_ERROR_READ."));

		return SENSOR_ERROR_READ;
	}
}

// センサーのデータを取得（基準値、最大値、デッドゾーンを考慮する）
FVector USensorManager::GetSensorData(int _tryNum/* = 500*/)
{
	bool tmp;

	FVector dataTempRaw = GetSensorDataRaw(tmp, tmp, _tryNum);
	FVector	dataTemp = dataTempRaw;

	// 基準値を考慮（値から基準値を減算）
	dataTemp -= m_Standard;

	// デッドゾーンを考慮（ -デッドゾーン ～ +デッドゾーン の間であれば0にする）
	// X軸
	if (dataTemp.X < m_Deadzone.X && dataTemp.X > -m_Deadzone.X)
	{
		dataTemp.X = 0.0f;
	}
	// Y軸
	if (dataTemp.Y < m_Deadzone.Y && dataTemp.Y > -m_Deadzone.Y)
	{
		dataTemp.Y = 0.0f;
	}
	// Z軸
	if (dataTemp.Z < m_Deadzone.Z && dataTemp.Z > -m_Deadzone.Z)
	{
		dataTemp.Z = 0.0f;
	}

	// 最大回転量に補正
	static FVector prevVector = dataTemp;

	if (FMath::Abs(prevVector.X - dataTemp.X) > m_MaxVariation.X)
	{
		dataTemp.X = prevVector.X;
	}
	if (FMath::Abs(prevVector.Y - dataTemp.Y) > m_MaxVariation.Y)
	{
		dataTemp.Y = prevVector.Y;
	}
	if (FMath::Abs(prevVector.Z - dataTemp.Z) > m_MaxVariation.Z)
	{
		dataTemp.Z = prevVector.Z;
	}

	// Z軸回転中にX, Yの回転を切る
	if (FMath::Abs(prevVector.Z - dataTemp.Z) >= m_XYFreezeZAxisRotation)
	{
		dataTemp.X = 0.0f;
		dataTemp.Y = 0.0f;
	}

	// エラー値の場合は前の値を返す
	if (dataTempRaw == SENSOR_ERROR_READ)
	{
		return prevVector;
	}

	prevVector = dataTempRaw;

	return dataTemp;
}

// センサーのボタンが押されているかを取得
void USensorManager::GetSensorButton(bool& _left, bool& _right, int _tryNum/* = 500*/)
{
	GetSensorDataRaw(_left, _right, _tryNum);
}

// センサーのデータをFRotatorとして取得
FRotator USensorManager::GetSensorDataRotatorRaw(int _tryNum/* = 500*/)
{
	bool tmp;
	FVector tempVector = GetSensorDataRaw(tmp, tmp, _tryNum);

	return FRotator(tempVector.Y, tempVector.Z, tempVector.X);
}

// センサーのデータをFRotatorとして取得
FRotator USensorManager::GetSensorDataRotator(int _tryNum/* = 500*/)
{
	FVector tempVector = GetSensorData(_tryNum);

	return FRotator(tempVector.Y, tempVector.Z, tempVector.X);
}