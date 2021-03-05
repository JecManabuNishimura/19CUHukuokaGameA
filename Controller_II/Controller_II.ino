
#include <Wire.h>
#include <avr/wdt.h>
#include <MadgwickAHRS.h>
Madgwick MadgwickFilter;

// MPU-6050のアドレス、レジスタ設定値
#define MPU6050_WHO_AM_I     0x75  // Read Only
#define MPU6050_PWR_MGMT_1   0x6B  // Read and Write
#define MPU_ADDRESS  0x68

// アナログピン
#define YAW_POTENTIOMETER_PIN	A0 // A0ピンをポテンショメーターのピンにする
#define YAW_POTENTIOMETER_READ	analogRead(YAW_POTENTIOMETER_PIN)	// A

// ボタン用ピン
const int leftButton = 12;
const int rightButton = 13;

// センサー正規化の値
const float MULTIPUL = 7.0f;					// X, Y軸のセンサーの値の補正値
const float POTENTIOMETER_OFFSET = 512.0f;		// ポテンショメーターの最大値（1024）と最小値（0）の中間の値 0～1024の値を -512～512に変換する
const float POTENTIOMETER_COLLECTION = 5.689f;	// 正規化したポテンショメーターの値に対して割る値 512を90°に変換する

float prevData = 0.0f;
float standard = 180.0f;

bool IsButtonPush(int _port, int _chatteringLoop = 100, float _chatteringRatio = 0.8f)
{
  bool isPushing = false;
  int onCount = 0;

  for (int i = 0; i < _chatteringLoop; ++i)
  {
    if (digitalRead(_port) == LOW)
    {
      onCount++;
    }
  }

  if (onCount >= _chatteringLoop * _chatteringRatio)
  {
    isPushing = true;
  }

  return isPushing;
}

// デバイス初期化時に実行される
void setup() {
  Wire.begin();

  // アナログピンの初期化
  pinMode(YAW_POTENTIOMETER_PIN, INPUT);

  // ボタン用ピンの初期化
  pinMode(leftButton, INPUT_PULLUP);
  pinMode(rightButton, INPUT_PULLUP);

  // PCとの通信を開始
  Serial.begin(115200); //115200bps

  // 初回の読み出し
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(MPU6050_WHO_AM_I);  //MPU6050_PWR_MGMT_1
  Wire.write(0x00);
  Wire.endTransmission();

  // 動作モードの読み出し
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(MPU6050_PWR_MGMT_1);  //MPU6050_PWR_MGMT_1レジスタの設定
  Wire.write(0x00);
  Wire.endTransmission();

  // サンプリング周波数の設定
  MadgwickFilter.begin(100); //100Hz
}


void loop()
{
  //if (Serial.available() > 0)
  {
    int input = Serial.read();

    // 入力文字が "s" でないなら処理を中断
    if (input != 's')
    {
      //return;
    }


    Wire.beginTransmission(0x68);
    // データ送信を開始するレジスタの指定（ACCEL_XOUT_H, 0x3B番地）
    Wire.write(0x3B);
    Wire.endTransmission(false);

    // ACCEL_XOUT_Hレジスタから14ビット分のデータを送信させる
    Wire.requestFrom(0x68, 14, true);

    // センサーの情報を取得する
    // 取得できなければ処理を中断
    while (Wire.available() < 14)
    {
      static int count = 0;
      ++count;

      if (count > 10000)
      {
        Serial.println("No Connect Sensor.");
        return;
      }
    }

    int16_t axRaw;	// 加速度 X軸 の生のデータ
    int16_t ayRaw;	// 加速度 Y軸 の生のデータ
    int16_t azRaw;	// 加速度 Z軸 の生のデータ
    int16_t gxRaw;	// 角速度 X軸 の生のデータ
    int16_t gyRaw;	// 角速度 X軸 の生のデータ
    int16_t gzRaw;	// 角速度 X軸 の生のデータ
    int16_t temperature;	// 温度データ

    axRaw = Wire.read() << 8 | Wire.read();
    ayRaw = Wire.read() << 8 | Wire.read();
    azRaw = Wire.read() << 8 | Wire.read();
    temperature = Wire.read() << 8 | Wire.read();
    gxRaw = Wire.read() << 8 | Wire.read();
    gyRaw = Wire.read() << 8 | Wire.read();
    gzRaw = Wire.read() << 8 | Wire.read();

    // 加速度値を分解能で割って加速度(G)に変換する
    float accArray[3] = {};

    accArray[0] = axRaw / 16384.0;  //FS_SEL_0 16,384 LSB / g
    accArray[1] = ayRaw / 16384.0;
    accArray[2] = azRaw / 16384.0;
    //accArray[2] = YAW_POTENTIOMETER_READ;

    // 角速度値を分解能で割って角速度(degrees per sec)に変換する
    float gyroArray[3] = {};

    gyroArray[0] = gxRaw / 131.0;  // (度/s)
    gyroArray[1] = gyRaw / 131.0;
    gyroArray[2] = gzRaw / 131.0;

    //Madgwickフィルター（ブレをおさえる計算）を用いて、PRY（pitch, roll, yaw）を計算
    MadgwickFilter.updateIMU(gyroArray[0], gyroArray[1], gyroArray[2], accArray[0], accArray[1], accArray[2]);

    float yawPotentiometer = YAW_POTENTIOMETER_READ;

    yawPotentiometer -= POTENTIOMETER_OFFSET;
    yawPotentiometer /= POTENTIOMETER_COLLECTION;

    float sendZData = 0.0f;

    if (abs(MadgwickFilter.getYaw() - prevData) > 0.1f)
    {
      sendZData = prevData - MadgwickFilter.getYaw();
    }
    // シリアル出力
    // センサーデータ
    Serial.print(MadgwickFilter.getPitch() * MULTIPUL, 1);		Serial.print(":");		// Roll	 	X
    Serial.print(MadgwickFilter.getRoll() * MULTIPUL, 1);		Serial.print(":");		// Pitch 	Y
    //Serial.print(yawPotentiometer, 1);						Serial.print(":");		// Yaw	 	Z
    Serial.print(sendZData, 1);					Serial.print(":");		// Yaw	 	Z
    //Serial.print(0.0f, 1);									Serial.print(":");		// Yaw	 	Z

    // ボタンデータ
    Serial.print(IsButtonPush(leftButton) ? "1" : "0");
    Serial.print(":");

    Serial.print(IsButtonPush(rightButton) ? "1" : "0");
    Serial.println();
    
    prevData = MadgwickFilter.getYaw();
  }
}
