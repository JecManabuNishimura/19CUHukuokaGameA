// センサーの管理を行うクラス

// 2020/12/08 渡邊 龍音 作成

#pragma once

#include "CoreMinimal.h"
#include "..\..\UE4Duino\Source\UE4Duino\Public\Serial.h"
#include "SensorManager.generated.h"

#define SENSOR_ERROR_READ FVector(999.9f, 999.9f, 999.9f)
#define SENSOR_ERROR_ROTATOR FRotator(999.9f, 999.9f, 999.9f)

UCLASS(BlueprintType, Category = "Sensor Manager", meta = (Keywords = "com arduino serial"))
class GAME_API USensorManager : public UObject
{
public:
	GENERATED_BODY()

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
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static USerial* GetArduinoSerial()
	{
		return m_ArduinoSerial;
	}

	// ポートが開いているか
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static bool GetIsOpen()
	{
		return m_IsOpen;
	}

	// センサーのつながっているシリアルポート
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static int GetSerialPort()
	{
		return m_SerialPort;
	}

	// 基準値
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static FVector GetStandard()
	{
		return m_Standard;
	}

	// 最大傾き
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static FVector GetMaxIncline()
	{
		return m_MaxIncline;
	}

	// デッドゾーン
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static FVector GetDeadzone()
	{
		return m_Deadzone;
	}

	//-------------------------------------------------------------------------------------------------------------
	
	// センサーとの接続
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
	static bool ConnectToSensor(int _maxSerialPort = 20, int _checkSensorNum = 500, int _tryConnectNum = 1);

	// センサーとの接続を切る
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
	static void DisconnectToSensor();
	
	// 基準値を設定する
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
	static void SetStandard(int _qualityLoop = 100);

	// 最大値を設定する
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
	static float SetMaxIncline(FString _element, int _getMaxLoop = 100, int _qualityLoop = 100);

	// デッドゾーンを設定する
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
	static void SetDeadZone(int _qualityLoop = 100);

	// センサーで使用するプロパティの初期化
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
	static void ResetSensorProperty(bool _isResetStandard = true, bool _isResetMaxIncline = true, bool _isResetDeadZone = true);

	// センサーの平均値を取得する
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static FVector GetSensorAverage(int _qualityLoop = 100);

	// センサーの最大値に対する傾きの割合を取得する
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static FVector GetSensorRatio(int _divNum = 5, int _tryNum = 500);

	// センサーからの生のデータを取得
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static FVector GetSensorDataRaw(FString& _strAdr, int _tryNum = 500);

	// センサーのボタンが押されているかを取得
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static bool GetSensorButton(int _tryNum = 500);

	// センサーのデータをFRotatorとして取得
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static FRotator GetSensorDataRotator(int _tryNum = 500);
};