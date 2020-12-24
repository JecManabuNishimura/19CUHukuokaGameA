// センサーの管理を行うクラス

// 2020/12/08 渡邊 龍音 作成

#pragma once

#include "CoreMinimal.h"
#include "..\..\UE4Duino\Source\UE4Duino\Public\Serial.h"

#define SENSOR_ERROR_READ FVector(999.9f, 999.9f, 999.9f)
#define SENSOR_ERROR_ROTATOR FRotator(999.9f, 999.9f, 999.9f)

class GAME_API SensorManager
{
public:
	enum SensorElement
	{
		X = 0,
		Y = 1,
		Z = 2,
	};

	// privateクラス変数
private:
	// センサーの格納用
	static USerial* m_ArduinoSerial;

	// ポートが開いているか
	static bool m_IsOpen;

	// センサーのつながっているシリアルポート
	static int m_SerialPort;

	// 基準値
	static FVector m_Standard;

	// 最大傾き
	static FVector m_MaxIncline;

	// デッドゾーン
	static FVector m_Deadzone;

	// publicクラスメソッド
public:

	//--各種クラス変数取得用---------------------------------------------------------------------------------------

	// センサーの格納用
	static USerial* GetArduinoSerial()
	{
		return m_ArduinoSerial;
	}

	// ポートが開いているか
	static bool GetIsOpen()
	{
		return m_IsOpen;
	}

	// センサーのつながっているシリアルポート
	static int GetSerialPort()
	{
		return m_SerialPort;
	}

	// 基準値
	static FVector GetStandard()
	{
		return m_Standard;
	}

	// 最大傾き
	static FVector GetMaxIncline()
	{
		return m_MaxIncline;
	}

	// デッドゾーン
	static FVector GetDeadzone()
	{
		return m_Deadzone;
	}

	//-------------------------------------------------------------------------------------------------------------

	// センサーとの接続
	static bool ConnectToSensor(int _maxSerialPort = 20, int _checkSensorNum = 500, int _tryConnectNum = 1);

	// センサーとの接続を切る
	static void DisconnectToSensor();
	
	// 基準値を設定する
	static void SetStandard(int _qualityLoop = 100);

	// 最大値を設定する
	static void SetMaxIncline(SensorElement _element, int _qualityLoop = 100);

	// デッドゾーンを設定する
	static void SetDeadZone(int _qualityLoop = 100);

	// センサーで使用するプロパティの初期化
	static void ResetSensorProperty(bool _isResetStandard = true, bool _isResetMaxIncline = true, bool _isResetDeadZone = true);

	// センサーの平均値を取得する
	static FVector GetSensorAverage(int _qualityLoop = 100);

	// センサーからの生のデータを取得
	static FVector GetSensorDataRaw( FString* _strAdr = nullptr, int _tryNum = 500);

	// センサーのボタンが押されているかを取得
	static bool GetSensorButton(int _tryNum = 500);

	// センサーのデータをFRotatorとして取得
	static FRotator GetSensorDataRotator(int _tryNum = 500);
};