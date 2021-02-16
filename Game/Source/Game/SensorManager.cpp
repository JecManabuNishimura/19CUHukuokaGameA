// �Z���T�[�̊Ǘ����s���N���X

// 2020/12/08 �n� ���� �쐬

#include "SensorManager.h"
#include "BlueprintUtility.h"

USerial* USensorManager::m_ArduinoSerial = NULL;
bool	 USensorManager::m_IsOpen = false;
int		 USensorManager::m_SerialPort = -1;
FVector	 USensorManager::m_Standard = FVector::ZeroVector;
FVector  USensorManager::m_MaxIncline = FVector::ZeroVector;
FVector	 USensorManager::m_Deadzone = FVector::ZeroVector;

// �Z���T�[�Ƃ̐ڑ�
bool USensorManager::ConnectToSensor(int _maxSerialPort /* = 20*/, int _checkSensorNum/* = 500*/, int _tryConnectNum /* = 1*/, bool _isResetStandard/* = true*/, bool _isResetMaxIncline/* = true*/, bool _isResetDeadZone/* = true*/)
{
	DisconnectToSensor(_isResetStandard, _isResetMaxIncline, _isResetDeadZone);

	// �J��Ԃ��ڑ������݂��
	for (int tryCnt = 0; tryCnt < _tryConnectNum; ++tryCnt)
	{
		// �V���A���|�[�g��T��
		for (int i = 0; i < _maxSerialPort && m_IsOpen == false; ++i)
		{
			m_ArduinoSerial = USerial::OpenComPort(m_IsOpen, i, 115200);

			if (m_IsOpen == false)
			{
				UE_LOG(LogTemp, Warning, TEXT("[SensorManager] COM Port:%d is failed open."), i);
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

				UE_LOG(LogTemp, VeryVerbose, TEXT("[SensorManager] Try Read Count: %d / %d"), tryNum, _checkSensorNum);

				UE_LOG(LogTemp, Verbose, TEXT("[SensorManager] Data = %s"), *fStr);

				// �Z���T�[����f�[�^��ǂݏo�����ꍇ
				if (isRead == true)
				{
					// �����Ȃ̂ŏ����I��
					UE_LOG(LogTemp, Display, TEXT("[SensorManager] COM Port:%d is Successfully Open."), i);
					m_SerialPort = i;
					break;
				}
				else
				{
					// �������Ȃ��̂Ōp��
					m_IsOpen = false;
					m_ArduinoSerial = NULL;
					UE_LOG(LogTemp, Warning, TEXT("[SensorManager] COM Port:%d is open but not sensor."), i);
				}
			}
		}
	}

	return m_IsOpen;
}

// �Z���T�[�Ƃ̐ڑ���؂�
void USensorManager::DisconnectToSensor(bool _isResetStandard/* = true*/, bool _isResetMaxIncline/* = true*/, bool _isResetDeadZone/* = true*/)
{
	UE_LOG(LogTemp, Warning, TEXT("[SensorManager] Sensor is close."));

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

	// ���̕ϐ�������
	ResetSensorProperty(_isResetStandard, _isResetMaxIncline, _isResetDeadZone);
}

// ��l��ݒ肷��
void USensorManager::SetStandardAuto(int _qualityLoop/* = 100*/)
{
	m_Standard = GetSensorAverage(_qualityLoop);
}

// �ő�l��ݒ肷��
float USensorManager::SetMaxInclineAuto(FString _element, int _getMaxLoop/* = 100*/, int _qualityLoop/* = 100*/)
{
	int elementNum;

	// �v�f�i_element�j�̕ϊ�
	// X��
	if (_element == "x" || _element == "X")
	{
		elementNum = 0;
	}
	// Y��
	else if (_element == "y" || _element == "Y")
	{
		elementNum = 1;
	}
	// Z��
	else if (_element == "z" || _element == "Z")
	{
		elementNum = 2;
	}
	// �����������Ȃ�
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SensorManager] The first argument of SensorManager::SetMaxIncline is incorrect. Enter one of \"X\", \"Y\", \"Z\""));
		return -1.0f;
	}

	// �Z���T�[�f�[�^��float�^�ŕێ�
	float sensorArray[3] = {};

	// �ő�l
	float maxArray[3] = {};

	for (int loop = 0; loop < _getMaxLoop; ++loop)
	{
		FString tmp;

		// �Z���T�[�f�[�^���擾
		FVector rowData = GetSensorDataRaw(tmp, _qualityLoop);

		// �z��Ɋi�[
		sensorArray[0] = rowData.X;
		sensorArray[1] = rowData.Y;
		sensorArray[2] = rowData.Z;

		// X, Y, Z���ꂼ��̍ő�l�`�F�b�N
		for (int i = 0; i < 3; ++i)
		{
			if (sensorArray[i] > maxArray[i])
			{
				maxArray[i] = sensorArray[i];
			}
		}
	}

	// m_MaxIncline����p�|�C���^�z��
	float* maxInclineAdrArray[3] = {};

	// m_MaxIncline�̃A�h���X��ǉ�
	maxInclineAdrArray[0] = &(m_MaxIncline.X);
	maxInclineAdrArray[1] = &(m_MaxIncline.Y);
	maxInclineAdrArray[2] = &(m_MaxIncline.Z);

	// m_MaxIncline�̎w�肵���v�f�ɁA�擾�����Z���T�[�f�[�^����
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

// �f�b�h�]�[����ݒ肷��
void USensorManager::SetDeadZoneAuto(int _qualityLoop/* = 100*/)
{
	// �Z���T�[�f�[�^��float�^�ŕێ�
	float sensorArray[3] = {};

	// �Z���T�[�f�[�^���擾
	FVector rowData = GetSensorAverage(_qualityLoop);

	// �z��Ɋi�[
	sensorArray[0] = rowData.X;
	sensorArray[1] = rowData.Y;
	sensorArray[2] = rowData.Z;

	// m_Deadzone����p�|�C���^�z��
	float* deadZoneAdrArray[3] = {};

	// m_Deadzone�̃A�h���X��ǉ�
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

// �Z���T�[�Ŏg�p����v���p�e�B�̏�����
void USensorManager::ResetSensorProperty(bool _isResetStandard/* = true*/, bool _isResetMaxIncline/* = true*/, bool _isResetDeadZone/* = true*/)
{
	// ��l�̏�����
	if (_isResetStandard)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[SensorManager] SensorManager::m_Standard is Reset."))
			m_Standard = FVector::ZeroVector;
	}
	// �ő�l�̏�����
	if (_isResetMaxIncline)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[SensorManager] SensorManager::m_MaxIncline is Reset."))
			m_MaxIncline = FVector::ZeroVector;
	}
	// �f�b�h�]�[���̏�����
	if (_isResetDeadZone)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[SensorManager] SensorManager::m_Deadzone is Reset."))
			m_Deadzone = FVector::ZeroVector;
	}
}

// �Z���T�[�̕��ϒl���擾����
FVector USensorManager::GetSensorAverage(int _qualityLoop/* = 100*/)
{
	// �Z���T�[�̒l�𕡐��m�ۂ���
	TArray<FVector> sensorDataArray;
	sensorDataArray.Reset();

	// �Z���T�[���畡����f�[�^���擾����
	for (int i = 0; i < _qualityLoop; ++i)
	{
		FString tmpStr;
		FVector tmp = GetSensorDataRaw(tmpStr);

		if (tmp.Equals(SENSOR_ERROR_READ) == false)
		{
			sensorDataArray.Add(tmp);
		}
	}

	// �v�f������Ε��ϒl��Ԃ�
	if (sensorDataArray.Num() > 0)
	{
		// ���v�i�[�pFVector
		FVector sumVector = FVector::ZeroVector;

		// ���Z
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

// �Z���T�[�̍ő�l�ɑ΂���X���̊������擾����
FVector USensorManager::GetSensorRatio(int _divNum/* = 5*/, int _tryNum/* = 500*/)
{
	FString tmp;

	FVector sensorData = GetSensorData(tmp, _tryNum);

	FVector resultData = SENSOR_ERROR_READ;

	// �ő�X�����ݒ肳��Ă���
	if (GetMaxIncline() != FVector::ZeroVector)
	{
		// �ő�l�𕪊������Ƃ��̒l
		FVector maxInclineRatio = GetMaxIncline() / _divNum;

		// �Z���T�[�Ƃ̒l���r����l
		FVector comparison = maxInclineRatio;

		// �l�̑����
		int substitutionNum = 0;

		for (int i = 1; (substitutionNum < 3) && (i <= _divNum); ++i)
		{
			// X���̔�r
			// ������Ă��Ȃ��ꍇ
			if (resultData.X == SENSOR_ERROR_READ.X)
			{
				// X���v���X
				if (sensorData.X > 0.0f)
				{
					if (sensorData.X <= comparison.X)
					{
						resultData.X = comparison.X;
						substitutionNum++;
					}
				}
				// X���}�C�i�X
				else if (sensorData.X < 0.0f)
				{
					if (sensorData.X >= -comparison.X)
					{
						resultData.X = -comparison.X;
						substitutionNum++;
					}
				}
				// �[��
				else
				{
					resultData.X = 0.0f;
					substitutionNum++;
				}
			}

			// Y���̔�r
			// ������Ă��Ȃ��ꍇ
			if (resultData.Y == SENSOR_ERROR_READ.Y)
			{
				// Y���v���X
				if (sensorData.Y > 0.0f)
				{
					if (sensorData.Y <= comparison.Y)
					{
						resultData.Y = comparison.Y;
						substitutionNum++;
					}
				}
				// Y���}�C�i�X
				else if (sensorData.Y < 0.0f)
				{
					if (sensorData.Y >= -comparison.Y)
					{
						resultData.Y = -comparison.Y;
						substitutionNum++;
					}
				}
				// �[��
				else
				{
					resultData.Y = 0.0f;
					substitutionNum++;
				}
			}

			// Z���̔�r
			// ������Ă��Ȃ��ꍇ
			if (resultData.Z == SENSOR_ERROR_READ.Z)
			{
				// Z���v���X
				if (sensorData.Z > 0.0f)
				{
					if (sensorData.Z <= comparison.Z)
					{
						resultData.Z = comparison.Z;
						substitutionNum++;
					}
				}
				// Z���}�C�i�X
				else if (sensorData.Z < 0.0f)
				{
					if (sensorData.Z >= -comparison.Z)
					{
						resultData.Z = -comparison.Z;
						substitutionNum++;
					}
				}
				// �[��
				else
				{
					resultData.Z = 0.0f;
					substitutionNum++;
				}
			}
			comparison += maxInclineRatio;
		}

		if (resultData.X == SENSOR_ERROR_READ.X)
		{
			resultData.X = sensorData.X > 0.0f ? GetMaxIncline().X : -GetMaxIncline().X;
		}
		if (resultData.Y == SENSOR_ERROR_READ.Y)
		{
			resultData.Y = sensorData.Y > 0.0f ? GetMaxIncline().Y : -GetMaxIncline().Y;
		}
		if (resultData.Z == SENSOR_ERROR_READ.Z)
		{
			resultData.Z = sensorData.Z > 0.0f ? GetMaxIncline().Z : -GetMaxIncline().Z;
		}
	}
	// �ݒ肳��Ă��Ȃ�
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SensorManager] Max incline value is may not set. Must be set by calling a \"SensorManager::SetMaxIncline()\" Function."));
	}

	return resultData;
}

// �Z���T�[����̐��̃f�[�^���擾
FVector USensorManager::GetSensorDataRaw(FString& _strAdr, int _tryNum/* = 500*/)
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

		UE_LOG(LogTemp, VeryVerbose, TEXT("[SensorManager] Try Read Count: %d / %d"), tryCnt, _tryNum);

		// �ǂݎ��Ȃ������� SENSOR_ERROR_READ ��Ԃ��A�I��
		// �Z���T�[�ɕ����I�Ȗ�肪����\���������i�ڑ�����Ă��Ȃ��A�Z���T�[�����Ă���Ȃǁj
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

		// �Z���T�[�f�[�^���J���}��؂��splitTextArray�ɓ����
		fStr.ParseIntoArray(splitTextArray, TEXT(":"));

		for (int i = 0; i < splitTextArray.Num(); ++i)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[SensorManager] Data[%d] = %s"), i, *splitTextArray[i]);
		}

		// ���ꂼ���int�^�ɕϊ�����
		TArray<float> sensorDataArray;
		sensorDataArray.Reset();

		for (int i = 0; i < splitTextArray.Num() && i < 3; ++i)
		{
			sensorDataArray.Add(FCString::Atof(*splitTextArray[i]));
		}
		// TArray�̗v�f�ԍ� 2�ɃA�N�Z�X�ł��Ȃ���� SENSOR_ERROR_READ ��Ԃ��A�I��
		// �Z���T�[�Ƃ̃f�[�^�����ɖ�肪����\���������iArduino���̃v���O�������{�ԗp�ɂȂ��Ă��Ȃ��A�o�b�t�@�Ƀf�[�^�����r���[�Ɏc���Ă���Ȃǁj
		if (sensorDataArray.IsValidIndex(2) == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("[SensorManager] Failed Add TArray<float> elements. return SENSOR_ERROR_READ."));
			return SENSOR_ERROR_READ;
		}

		// �{�^���̏�Ԃ�ǂݎ�肽��
			// splitTextArray�̗v�f�ԍ� 3�ɃA�N�Z�X�ł��Ȃ���� OFF�ɂ���
			if (splitTextArray.IsValidIndex(3) == false)
			{
				UE_LOG(LogTemp, Warning, TEXT("[SensorManager] Failed Get ButtonCondition. Set to \"OFF\"."));
				_strAdr = "OFF";
			}
			// �A�N�Z�X�ł���
			else
			{
				if (splitTextArray[3] == "ON")
				{
					_strAdr = "ON";
				}
				else
				{
					_strAdr = "OFF";
				}
			}

		UE_LOG(LogTemp, Verbose, TEXT("[SensorManager] SensorData : %f  Y:%f  Z:%f"), sensorDataArray[0], sensorDataArray[1], sensorDataArray[2]);

		return FVector(sensorDataArray[0], sensorDataArray[1], sensorDataArray[2]);
	}
	// ConnectToSensor()���Ăяo���Ă��Ȃ��\��������
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[SensorManager] SensorManager::m_ArduinoSerial is NULL. return SENSOR_ERROR_READ."));

		return SENSOR_ERROR_READ;
	}
}

// �Z���T�[�̃f�[�^���擾�i��l�A�ő�l�A�f�b�h�]�[�����l������j
FVector USensorManager::GetSensorData(FString& _strAdr, int _tryNum/* = 500*/)
{
	FVector dataTemp = GetSensorDataRaw(_strAdr, _tryNum);

	// ��l���l���i�l�����l�����Z�j
	dataTemp -= m_Standard;

	// �f�b�h�]�[�����l���i -�f�b�h�]�[�� �` +�f�b�h�]�[�� �̊Ԃł����0�ɂ���j
	// X��
	if (dataTemp.X < m_Deadzone.X && dataTemp.X > -m_Deadzone.X)
	{
		dataTemp.X = 0.0f;
	}
	// Y��
	if (dataTemp.Y < m_Deadzone.Y && dataTemp.Y > -m_Deadzone.Y)
	{
		dataTemp.Y = 0.0f;
	}
	// Z��
	if (dataTemp.Z < m_Deadzone.Z && dataTemp.Z > -m_Deadzone.Z)
	{
		dataTemp.Z = 0.0f;
	}

	return dataTemp;
}

// �Z���T�[�̃{�^����������Ă��邩���擾
bool USensorManager::GetSensorButton(int _tryNum/* = 500*/)
{
	FString result;
	GetSensorDataRaw(result, _tryNum);

	if (result == "ON")
	{
		return true;
	}
	else
	{
		return false;
	}
}

// �Z���T�[�̃f�[�^��FRotator�Ƃ��Ď擾
FRotator USensorManager::GetSensorDataRotator(int _tryNum/* = 500*/)
{
	FString tmp;
	FVector tempVector = GetSensorDataRaw(tmp, _tryNum);

	return FRotator(tempVector.Y, tempVector.Z, tempVector.X);
}