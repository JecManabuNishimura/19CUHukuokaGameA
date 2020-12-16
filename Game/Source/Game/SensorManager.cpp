// �Z���T�[�̊Ǘ����s���N���X

// 2020/12/08 �n� ���� �쐬

#include "SensorManager.h"

USerial* SensorManager::m_ArduinoSerial	 = NULL;
bool	 SensorManager::m_IsOpen = false;
int		 SensorManager::m_SerialPort = -1;
FVector	 SensorManager::m_Standard = FVector::ZeroVector;
FVector  SensorManager::m_MaxIncline = FVector::ZeroVector;
FVector	 SensorManager::m_Deadzone = FVector::ZeroVector;

// �Z���T�[�Ƃ̐ڑ�
bool SensorManager::ConnectToSensor(int _maxSerialPort /* = 20*/, int _checkSensorNum/* = 500*/, int _tryConnectNum /* = 1*/)
{
	// �J��Ԃ��ڑ������݂��
	for (int tryCnt = 0; tryCnt < _tryConnectNum; ++tryCnt)
	{
		// �V���A���|�[�g��T��
		for (int i = 0; i < _maxSerialPort && m_IsOpen == false; ++i)
		{
			m_ArduinoSerial = USerial::OpenComPort(m_IsOpen, i, 115200);

			if (m_IsOpen == false)
			{
				UE_LOG(LogTemp, Error, TEXT("COM Port:%d is failed open."), i);
			}
			else
			{
				// �{���Ƀ|�[�g�悪�Z���T�[���m�F����

				bool isRead = false;
				FString fStr = "";
				int tryNum = 0;

				do
				{
					// initial�� 'i' �𑗐M
					m_ArduinoSerial->Println(FString(TEXT("s")));

					// ������ǂݎ��
					fStr = m_ArduinoSerial->Readln(isRead);
					tryNum++;
				} while (isRead == false && tryNum < _checkSensorNum);

				UE_LOG(LogTemp, VeryVerbose, TEXT("Try Read Count: %d / %d"), tryNum, _checkSensorNum);

				UE_LOG(LogTemp, Verbose, TEXT("Data = %s"), *fStr);

				// �Z���T�[����f�[�^��ǂݏo�����ꍇ
				if (isRead == true)
				{
					// �����Ȃ̂ŏ����I��
					UE_LOG(LogTemp, Display, TEXT("COM Port:%d is Successfully Open."), i);
					m_SerialPort = i;
					break;
				}
				else
				{
					// �������Ȃ��̂Ōp��
					m_IsOpen = false;
					m_ArduinoSerial = NULL;
					UE_LOG(LogTemp, Error, TEXT("COM Port:%d is open but not sensor."), i);
				}
			}
		}
	}

	return m_IsOpen;
}

// �Z���T�[�Ƃ̐ڑ���؂�
void SensorManager::DisconnectToSensor()
{
	UE_LOG(LogTemp, Warning, TEXT("Sensor is close."));

	if (m_ArduinoSerial != NULL && m_IsOpen == true)
	{
		// �|�[�g����ؒf
		m_ArduinoSerial->Close();
	}

	// �Z���T�[�̊Ǘ����I��
	m_ArduinoSerial = NULL;

	// �|�[�g���J���Ă��Ȃ���Ԃ�
	m_IsOpen = false;

	// �V���A���|�[�g������
	m_SerialPort = -1;
}

// ��l��ݒ肷��
void SensorManager::SetStandard(int _qualityLoop/* = 100*/)
{
	m_Standard = GetSensorAverage(_qualityLoop);
}

// �ő�l��ݒ肷��
void SensorManager::SetMaxIncline(SensorElement _element, int _qualityLoop/* = 100*/)
{
	// �Z���T�[�f�[�^��float�^�ŕێ�
	float sensorArray[3] = {};

	// �Z���T�[�f�[�^���擾
	FVector rowData = GetSensorAverage(_qualityLoop);

	// �z��Ɋi�[
	sensorArray[0] = rowData.X;
	sensorArray[1] = rowData.Y;
	sensorArray[2] = rowData.Z;

	// m_MaxIncline����p�|�C���^�z��
	float* maxInclineAdrArray[3] = {};

	// m_MaxIncline�̃A�h���X��ǉ�
	maxInclineAdrArray[0] = &(m_MaxIncline.X);
	maxInclineAdrArray[1] = &(m_MaxIncline.Y);
	maxInclineAdrArray[2] = &(m_MaxIncline.Z);

	// m_MaxIncline�̎w�肵���v�f�ɁA�擾�����Z���T�[�f�[�^����
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

// �Z���T�[�Ŏg�p����v���p�e�B�̏�����
void SensorManager::ResetSensorProperty(bool _isResetStandard/* = true*/, bool _isResetMaxIncline/* = true*/, bool _isResetDeadZone/* = true*/)
{
	// ��l�̏�����
	if (_isResetStandard)
	{
		UE_LOG(LogTemp, Verbose, TEXT("SensorManager::m_Standard is Reset."))
		m_Standard = FVector::ZeroVector;
	}
	// �ő�l�̏�����
	if (_isResetMaxIncline)
	{
		UE_LOG(LogTemp, Verbose, TEXT("SensorManager::m_MaxIncline is Reset."))
		m_MaxIncline = FVector::ZeroVector;
	}
	// �f�b�h�]�[���̏�����
	if (_isResetDeadZone)
	{
		UE_LOG(LogTemp, Verbose, TEXT("SensorManager::m_Deadzone is Reset."))
		m_Deadzone = FVector::ZeroVector;
	}
}

// �Z���T�[�̕��ϒl���擾����
FVector SensorManager::GetSensorAverage(int _qualityLoop/* = 100*/)
{
	// �Z���T�[�̒l�𕡐��m�ۂ���
	TArray<FVector> sensorDataArray;
	sensorDataArray.Reset();

	// �Z���T�[���畡����f�[�^���擾����
	for (int i = 0; i < _qualityLoop; ++i)
	{
		FVector tmp = GetSensorDataRaw();

		if (tmp.Equals(SENSOR_ERROR_READ) == false)
		{
			sensorDataArray.Add(tmp);
		}
	}

	// ���v�i�[�pFVector
	FVector sumVector = FVector::ZeroVector;

	// ���Z
	for (int i = 0; i < sensorDataArray.Num(); ++i)
	{
		sumVector.X += sensorDataArray[i].X;
		sumVector.Y += sensorDataArray[i].Y;
		sumVector.Z += sensorDataArray[i].Z;
	}

	// �v�f������Ε��ϒl��Ԃ�
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

// �Z���T�[����̐��̃f�[�^���擾
FVector SensorManager::GetSensorDataRaw(int _tryNum/* = 500*/)
{
	bool isRead = false;		// �f�[�^��ǂݎ�ꂽ��
	FString fStr = "";			// �ǂݎ��f�[�^�i�[�p
	int tryCnt = 0;				// �ǂݎ�낤�Ƃ�����

	// �V���A���̃I�u�W�F�N�g�������
	if (m_ArduinoSerial != NULL && m_IsOpen == true)
	{
		// �f�[�^�̓ǂݎ��
		// �f�[�^���ǂݎ��邩�A�ő�ǂݎ��񐔂ɂȂ�܂ŌJ��Ԃ�
		do
		{
			m_ArduinoSerial->Println(FString(TEXT("s")));

			fStr = m_ArduinoSerial->Readln(isRead);
			++tryCnt;
		} while (isRead == false && tryCnt < _tryNum);

		TArray<FString> splitTextArray;
		splitTextArray.Reset();

		UE_LOG(LogTemp, VeryVerbose, TEXT("Try Read Count: %d / %d"), tryCnt, _tryNum);

		// �ǂݎ��Ȃ������� SENSOR_ERROR_READ ��Ԃ��A�I��
		// �Z���T�[�ɕ����I�Ȗ�肪����\���������i�ڑ�����Ă��Ȃ��A�Z���T�[�����Ă���Ȃǁj
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

		// �Z���T�[�f�[�^���J���}��؂��splitTextArray�ɓ����
		fStr.ParseIntoArray(splitTextArray, TEXT(","));

		for (int i = 0; i < splitTextArray.Num(); ++i)
		{
			UE_LOG(LogTemp, Verbose, TEXT("Data[%d] = %s"), i, *splitTextArray[i]);
		}

		// ���ꂼ���int�^�ɕϊ�����
		TArray<float> sensorDataArray;
		sensorDataArray.Reset();

		for (int i = 0; i < splitTextArray.Num(); ++i)
		{
			sensorDataArray.Add(FCString::Atof(*splitTextArray[i]));
		}
		// TArray�̗v�f�ԍ� 2�ɃA�N�Z�X�ł��Ȃ���� SENSOR_ERROR_READ ��Ԃ��A�I��
		// �Z���T�[�Ƃ̃f�[�^�����ɖ�肪����\���������iArduino���̃v���O�������{�ԗp�ɂȂ��Ă��Ȃ��A�o�b�t�@�Ƀf�[�^�����r���[�Ɏc���Ă���Ȃǁj
		if (sensorDataArray.IsValidIndex(2) == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed Add TArray<float> elements. return SENSOR_ERROR_READ."));
			return SENSOR_ERROR_READ;
		}

		UE_LOG(LogTemp, Verbose, TEXT("SensorData : %f  Y:%f  Z:%f"), sensorDataArray[0], sensorDataArray[1], sensorDataArray[2]);

		return FVector(sensorDataArray[0], sensorDataArray[1], sensorDataArray[2]);
	}
	// ConnectToSensor()���Ăяo���Ă��Ȃ��\��������
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SensorManager::m_ArduinoSerial is NULL.�@return SENSOR_ERROR_READ."));

		return SENSOR_ERROR_READ;
	}
}

// �Z���T�[�̃f�[�^��FRotator�Ƃ��Ď擾
FRotator SensorManager::GetSensorDataRotator(int _tryNum/* = 500*/)
{
	FVector tempVector = GetSensorDataRaw(_tryNum);

	return FRotator(tempVector.Y, tempVector.Z, tempVector.X);
}
