// �Z���T�[�̊Ǘ����s���N���X

// 2020/12/08 �n� ���� �쐬

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

	// private�N���X�ϐ�
private:
	// �Z���T�[�̊i�[�p
	static USerial* m_ArduinoSerial;

	// �|�[�g���J���Ă��邩
	static bool m_IsOpen;

	// �Z���T�[�̂Ȃ����Ă���V���A���|�[�g
	static int m_SerialPort;

	// ��l
	static FVector m_Standard;

	// �ő�X��
	static FVector m_MaxIncline;

	// �f�b�h�]�[��
	static FVector m_Deadzone;

	// public�N���X���\�b�h
public:

	//--�e��N���X�ϐ��擾�p---------------------------------------------------------------------------------------

	// �Z���T�[�̊i�[�p
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static USerial* GetArduinoSerial()
	{
		return m_ArduinoSerial;
	}

	// �|�[�g���J���Ă��邩
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static bool GetIsOpen()
	{
		return m_IsOpen;
	}

	// �Z���T�[�̂Ȃ����Ă���V���A���|�[�g
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static int GetSerialPort()
	{
		return m_SerialPort;
	}

	// ��l
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static FVector GetStandard()
	{
		return m_Standard;
	}

	// �ő�X��
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static FVector GetMaxIncline()
	{
		return m_MaxIncline;
	}

	// �f�b�h�]�[��
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static FVector GetDeadzone()
	{
		return m_Deadzone;
	}

	//-------------------------------------------------------------------------------------------------------------
	
	// �Z���T�[�Ƃ̐ڑ�
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
	static bool ConnectToSensor(int _maxSerialPort = 20, int _checkSensorNum = 500, int _tryConnectNum = 1);

	// �Z���T�[�Ƃ̐ڑ���؂�
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
	static void DisconnectToSensor();
	
	// ��l��ݒ肷��
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
	static void SetStandard(int _qualityLoop = 100);

	// �ő�l��ݒ肷��
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
	static float SetMaxIncline(FString _element, int _getMaxLoop = 100, int _qualityLoop = 100);

	// �f�b�h�]�[����ݒ肷��
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
	static void SetDeadZone(int _qualityLoop = 100);

	// �Z���T�[�Ŏg�p����v���p�e�B�̏�����
	UFUNCTION(BlueprintCallable, Category = "Sensor Manager")
	static void ResetSensorProperty(bool _isResetStandard = true, bool _isResetMaxIncline = true, bool _isResetDeadZone = true);

	// �Z���T�[�̕��ϒl���擾����
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static FVector GetSensorAverage(int _qualityLoop = 100);

	// �Z���T�[�̍ő�l�ɑ΂���X���̊������擾����
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static FVector GetSensorRatio(int _divNum = 5, int _tryNum = 500);

	// �Z���T�[����̐��̃f�[�^���擾
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static FVector GetSensorDataRaw(FString& _strAdr, int _tryNum = 500);

	// �Z���T�[�̃{�^����������Ă��邩���擾
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static bool GetSensorButton(int _tryNum = 500);

	// �Z���T�[�̃f�[�^��FRotator�Ƃ��Ď擾
	UFUNCTION(BlueprintPure, Category = "Sensor Manager")
	static FRotator GetSensorDataRotator(int _tryNum = 500);
};