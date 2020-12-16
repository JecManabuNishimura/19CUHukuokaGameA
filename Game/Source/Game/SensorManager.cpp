// センサーの管理を行うクラス

// 2020/12/08 渡邊 龍音 作成

#include "SensorManager.h"

USerial* SensorManager::m_ArduinoSerial	 = NULL;
bool	 SensorManager::m_IsOpen = false;
int		 SensorManager::m_SerialPort = -1;
FVector	 SensorManager::m_Standard = FVector::ZeroVector;
FVector  SensorManager::m_MaxIncline = FVector::ZeroVector;
FVector	 SensorManager::m_Deadzone = FVector::ZeroVector;

// センサーとの接続
bool SensorManager::ConnectToSensor(int _maxSerialPort /* = 20*/, int _checkSensorNum/* = 500*/, int _tryConnectNum /* = 1*/)
{
	// 繰り返し接続を試みる回数
	for (int tryCnt = 0; tryCnt < _tryConnectNum; ++tryCnt)
	{
		// シリアルポートを探す
		for (int i = 0; i < _maxSerialPort && m_IsOpen == false; ++i)
		{
			m_ArduinoSerial = USerial::OpenComPort(m_IsOpen, i, 115200);

			if (m_IsOpen == false)
			{
				UE_LOG(LogTemp, Error, TEXT("COM Port:%d is failed open."), i);
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

				UE_LOG(LogTemp, VeryVerbose, TEXT("Try Read Count: %d / %d"), tryNum, _checkSensorNum);

				UE_LOG(LogTemp, Verbose, TEXT("Data = %s"), *fStr);

				// センサーからデータを読み出せた場合
				if (isRead == true)
				{
					// 成功なので処理終了
					UE_LOG(LogTemp, Display, TEXT("COM Port:%d is Successfully Open."), i);
					m_SerialPort = i;
					break;
				}
				else
				{
					// 正しくないので継続
					m_IsOpen = false;
					m_ArduinoSerial = NULL;
					UE_LOG(LogTemp, Error, TEXT("COM Port:%d is open but not sensor."), i);
				}
			}
		}
	}

	return m_IsOpen;
}

// センサーとの接続を切る
void SensorManager::DisconnectToSensor()
{
	UE_LOG(LogTemp, Warning, TEXT("Sensor is close."));

	if (m_ArduinoSerial != NULL && m_IsOpen == true)
	{
		// ポートから切断
		m_ArduinoSerial->Close();
	}

	// センサーの管理を終了
	m_ArduinoSerial = NULL;

	// ポートを開いていない状態に
	m_IsOpen = false;

	// シリアルポート初期化
	m_SerialPort = -1;
}

// 基準値を設定する
void SensorManager::SetStandard(int _qualityLoop/* = 100*/)
{
	m_Standard = GetSensorAverage(_qualityLoop);
}

// 最大値を設定する
void SensorManager::SetMaxIncline(SensorElement _element, int _qualityLoop/* = 100*/)
{
	// センサーデータをfloat型で保持
	float sensorArray[3] = {};

	// センサーデータを取得
	FVector rowData = GetSensorAverage(_qualityLoop);

	// 配列に格納
	sensorArray[0] = rowData.X;
	sensorArray[1] = rowData.Y;
	sensorArray[2] = rowData.Z;

	// m_MaxIncline操作用ポインタ配列
	float* maxInclineAdrArray[3] = {};

	// m_MaxInclineのアドレスを追加
	maxInclineAdrArray[0] = &(m_MaxIncline.X);
	maxInclineAdrArray[1] = &(m_MaxIncline.Y);
	maxInclineAdrArray[2] = &(m_MaxIncline.Z);

	// m_MaxInclineの指定した要素に、取得したセンサーデータを代入
	if (maxInclineAdrArray[_element] != nullptr)
	{
		if (sensorArray[_element] != SENSOR_ERROR_READ.X)
		{
			*maxInclineAdrArray[_element] = sensorArray[_element];
		}
		else
		{
			*maxInclineAdrArray[_element] = 0.0f;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not get m_MaxIncline address."));
	}
}

// センサーで使用するプロパティの初期化
void SensorManager::ResetSensorProperty(bool _isResetStandard/* = true*/, bool _isResetMaxIncline/* = true*/, bool _isResetDeadZone/* = true*/)
{
	// 基準値の初期化
	if (_isResetStandard)
	{
		UE_LOG(LogTemp, Verbose, TEXT("SensorManager::m_Standard is Reset."))
		m_Standard = FVector::ZeroVector;
	}
	// 最大値の初期化
	if (_isResetMaxIncline)
	{
		UE_LOG(LogTemp, Verbose, TEXT("SensorManager::m_MaxIncline is Reset."))
		m_MaxIncline = FVector::ZeroVector;
	}
	// デッドゾーンの初期化
	if (_isResetDeadZone)
	{
		UE_LOG(LogTemp, Verbose, TEXT("SensorManager::m_Deadzone is Reset."))
		m_Deadzone = FVector::ZeroVector;
	}
}

// センサーの平均値を取得する
FVector SensorManager::GetSensorAverage(int _qualityLoop/* = 100*/)
{
	// センサーの値を複数確保する
	TArray<FVector> sensorDataArray;
	sensorDataArray.Reset();

	// センサーから複数回データを取得する
	for (int i = 0; i < _qualityLoop; ++i)
	{
		FVector tmp = GetSensorDataRaw();

		if (tmp.Equals(SENSOR_ERROR_READ) == false)
		{
			sensorDataArray.Add(tmp);
		}
	}

	// 合計格納用FVector
	FVector sumVector = FVector::ZeroVector;

	// 加算
	for (int i = 0; i < sensorDataArray.Num(); ++i)
	{
		sumVector.X += sensorDataArray[i].X;
		sumVector.Y += sensorDataArray[i].Y;
		sumVector.Z += sensorDataArray[i].Z;
	}

	// 要素があれば平均値を返す
	if (sensorDataArray.Num() > 0)
	{
		FVector result = sumVector / (float)sensorDataArray.Num();
		UE_LOG(LogTemp, Verbose, TEXT("return Average SensorData. X = %f Y = %f, Z = %f"), result.X, result.Y, result.Z);
		return result;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not get Average SensorData. return SENSOR_ERROR_READ (%f, %f, %f)."), SENSOR_ERROR_READ.X, SENSOR_ERROR_READ.Y, SENSOR_ERROR_READ.Z);
		return SENSOR_ERROR_READ;
	}
}

// センサーからの生のデータを取得
FVector SensorManager::GetSensorDataRaw(int _tryNum/* = 500*/)
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

		UE_LOG(LogTemp, VeryVerbose, TEXT("Try Read Count: %d / %d"), tryCnt, _tryNum);

		// 読み取れなかったら SENSOR_ERROR_READ を返し、終了
		// センサーに物理的な問題がある可能性が高い（接続されていない、センサーが壊れているなど）
		if (isRead == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("No Data From Sensor. return SENSOR_ERROR_READ."));
			return SENSOR_ERROR_READ;
		}
		else
		{
			m_ArduinoSerial->Flush();

			UE_LOG(LogTemp, Verbose, TEXT("Get Data From Sensor."));
			UE_LOG(LogTemp, Verbose, TEXT("All Data = %s."), *fStr);
		}

		// センサーデータをカンマ区切りでsplitTextArrayに入れる
		fStr.ParseIntoArray(splitTextArray, TEXT(","));

		for (int i = 0; i < splitTextArray.Num(); ++i)
		{
			UE_LOG(LogTemp, Verbose, TEXT("Data[%d] = %s"), i, *splitTextArray[i]);
		}

		// それぞれをint型に変換する
		TArray<float> sensorDataArray;
		sensorDataArray.Reset();

		for (int i = 0; i < splitTextArray.Num(); ++i)
		{
			sensorDataArray.Add(FCString::Atof(*splitTextArray[i]));
		}
		// TArrayの要素番号 2にアクセスできなければ SENSOR_ERROR_READ を返し、終了
		// センサーとのデータやり取りに問題がある可能性が高い（Arduino側のプログラムが本番用になっていない、バッファにデータが中途半端に残っているなど）
		if (sensorDataArray.IsValidIndex(2) == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed Add TArray<float> elements. return SENSOR_ERROR_READ."));
			return SENSOR_ERROR_READ;
		}

		UE_LOG(LogTemp, Verbose, TEXT("SensorData : %f  Y:%f  Z:%f"), sensorDataArray[0], sensorDataArray[1], sensorDataArray[2]);

		return FVector(sensorDataArray[0], sensorDataArray[1], sensorDataArray[2]);
	}
	// ConnectToSensor()を呼び出していない可能性がある
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SensorManager::m_ArduinoSerial is NULL.　return SENSOR_ERROR_READ."));

		return SENSOR_ERROR_READ;
	}
}

// センサーのデータをFRotatorとして取得
FRotator SensorManager::GetSensorDataRotator(int _tryNum/* = 500*/)
{
	FVector tempVector = GetSensorDataRaw(_tryNum);

	return FRotator(tempVector.Y, tempVector.Z, tempVector.X);
}
