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

	// 値反転
	static bool isFlipX;
	static bool isFlipY;
	static bool isFlipZ;

	// X,Yを固定するZ軸の回転量
	static float m_XYFreezeZAxisRotation;

	// この値より大きく変わったら値を反映させない
	static FVector m_MaxVariation;

	// publicクラスメソッド
public:

	//--各種クラス変数取得用---------------------------------------------------------------------------------------

	// センサーの格納用
	// 取得
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static USerial* GetArduinoSerial()
	{
		return m_ArduinoSerial;
	}
	// 設定
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
	static void SetArduinoSerial(USerial* _serial)
	{
		m_ArduinoSerial = _serial;
	}

	// ポートが開いているか
	// 取得
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static bool GetIsOpen()
	{
		return m_IsOpen;
	}

	// センサーのつながっているシリアルポート
	// 取得
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static int GetSerialPort()
	{
		return m_SerialPort;
	}

	// 基準値
	// 取得
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static FVector GetStandard()
	{
		return m_Standard;
	}
	// 設定
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
	static void SetStandard(FVector _vector)
	{
		m_Standard = _vector;
	}

	// 最大傾き
	// 取得
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static FVector GetMaxIncline()
	{
		return m_MaxIncline;
	}
	// 設定
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
	static void SetMaxIncline(FVector _vector)
	{
		m_MaxIncline = _vector;
	}
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
		static void SetMaxInclineX(float _value)
	{
		m_MaxIncline.X = _value;
	}
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
		static void SetMaxInclineY(float _value)
	{
		m_MaxIncline.Y = _value;
	}
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
		static void SetMaxInclineZ(float _value)
	{
		m_MaxIncline.Z = _value;
	}

	// デッドゾーン
	// 取得
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static FVector GetDeadZone()
	{
		return m_Deadzone;
	}
	// 設定
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
	static void SetDeadzone(FVector _vector)
	{
		m_Deadzone = _vector;
	}

	// エラー値
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static FVector GetSensorError()
	{
		return SENSOR_ERROR_READ;
	}
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static bool IsSensorError(FVector _data)
	{
		return _data == SENSOR_ERROR_READ;
	}

	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
		static FRotator GetSensorErrorRotator()
	{
		return SENSOR_ERROR_ROTATOR;
	}
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
		static bool IsSensorErrorRotator(FRotator _data)
	{
		return _data == SENSOR_ERROR_ROTATOR;
	}


	// 値反転
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
		static void SetFlipAxis(bool _flipX = true, bool _flipY = true, bool _flipZ = true)
	{
		isFlipX = _flipX;
		isFlipY = _flipY;
		isFlipZ = _flipZ;
	}

	// X,Yを固定するZ軸の回転量
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
		static void SetFreezeZAxisRotation(float _value)
	{
		m_XYFreezeZAxisRotation = _value;
	}

	// この値より大きく変わったら値を反映させない
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
		static void SetMaxVariation(FVector _value)
	{
		m_MaxVariation = _value;
	}

	//-------------------------------------------------------------------------------------------------------------
	
	// センサーとの接続
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
	static bool ConnectToSensor(int _maxSerialPort = 20, int _checkSensorNum = 500, int _tryConnectNum = 1, bool _isResetStandard = true, bool _isResetMaxIncline = true, bool _isResetDeadZone = true);

	// センサーとの接続を切る
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
	static void DisconnectToSensor(bool _isResetStandard = true, bool _isResetMaxIncline = true, bool _isResetDeadZone = true);
	
	// 基準値を設定する
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
	static void SetStandardAuto(int _qualityLoop = 100);

	// 最大値を設定する
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
	static float SetMaxInclineAuto(FString _element, int _getMaxLoop = 100, int _qualityLoop = 100);
	
	// デッドゾーンを設定する
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
	static void SetDeadZoneAuto(int _qualityLoop = 100);

	// センサーで使用するプロパティの初期化
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
	static void ResetSensorProperty(bool _isResetStandard = true, bool _isResetMaxIncline = true, bool _isResetDeadZone = true);

	// センサーの平均値を取得する
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static FVector GetSensorAverage(int _qualityLoop = 100);

	// センサーの最大値に対する傾きの割合を取得する
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static FVector GetSensorRatio(int _divNum = 5, int _tryNum = 500, FVector _maxVector = FVector(60.0f, 60.0f, 90.0f));

	// センサーの最大値に対する傾きの割合を取得する
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static FRotator GetSensorRatioRotator(int _divNum = 5, int _tryNum = 500, FVector _maxVector = FVector(60.0f, 60.0f, 90.0f));

	// センサーからの生のデータを取得
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static FVector GetSensorDataRaw(bool& _left, bool& _right, int _tryNum = 500);

	// センサーのデータを取得（基準値、最大値、デッドゾーンを考慮する）
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static FVector GetSensorData(int _tryNum = 500);

	// センサーのボタンが押されているかを取得
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static void GetSensorButton(bool& _left, bool& _right, int _tryNum = 500);

	// センサーの生のデータをFRotatorとして取得
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static FRotator GetSensorDataRotatorRaw(int _tryNum = 500);

	// センサーの生のデータをFRotatorとして取得
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
		static FRotator GetSensorDataRotator(int _tryNum = 500);

	// 

};