// �Z���T�[�̊Ǘ����s���N���X

// 2020/12/08 �n� ���� �쐬

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
	static USerial* GetArduinoSerial()
	{
		return m_ArduinoSerial;
	}

	// �|�[�g���J���Ă��邩
	static bool GetIsOpen()
	{
		return m_IsOpen;
	}

	// �Z���T�[�̂Ȃ����Ă���V���A���|�[�g
	static int GetSerialPort()
	{
		return m_SerialPort;
	}

	// ��l
	static FVector GetStandard()
	{
		return m_Standard;
	}

	// �ő�X��
	static FVector GetMaxIncline()
	{
		return m_MaxIncline;
	}

	// �f�b�h�]�[��
	static FVector GetDeadzone()
	{
		return m_Deadzone;
	}

	//-------------------------------------------------------------------------------------------------------------

	// �Z���T�[�Ƃ̐ڑ�
	static bool ConnectToSensor(int _maxSerialPort = 20, int _checkSensorNum = 500, int _tryConnectNum = 1);

	// �Z���T�[�Ƃ̐ڑ���؂�
	static void DisconnectToSensor();
	
	// ��l��ݒ肷��
	static void SetStandard(int _qualityLoop = 100);

	// �ő�l��ݒ肷��
	static void SetMaxIncline(SensorElement _element, int _qualityLoop = 100);

	// �f�b�h�]�[����ݒ肷��
	static void SetDeadZone(int _qualityLoop = 100);

	// �Z���T�[�Ŏg�p����v���p�e�B�̏�����
	static void ResetSensorProperty(bool _isResetStandard = true, bool _isResetMaxIncline = true, bool _isResetDeadZone = true);

	// �Z���T�[�̕��ϒl���擾����
	static FVector GetSensorAverage(int _qualityLoop = 100);

	// �Z���T�[����̐��̃f�[�^���擾
	static FVector GetSensorDataRaw( FString* _strAdr = nullptr, int _tryNum = 500);

	// �Z���T�[�̃{�^����������Ă��邩���擾
	static bool GetSensorButton(int _tryNum = 500);

	// �Z���T�[�̃f�[�^��FRotator�Ƃ��Ď擾
	static FRotator GetSensorDataRotator(int _tryNum = 500);
};