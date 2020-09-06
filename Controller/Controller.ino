#include <Wire.h>

// MPU-6050のアドレス、レジスタ設定値
#define MPU6050_WHO_AM_I     0x75  // Read Only
#define MPU6050_PWR_MGMT_1   0x6B  // Read and Write
#define MPU_ADDRESS  0x68

// アナログピン
#define YAW_POTENTIOMETER_PIN	A0 // A0ピンをポテンショメーターのピンにする
#define YAW_POTENTIOMETER_READ	analogRead(YAW_POTENTIOMETER_PIN)	// A

// センサー正規化の値
const float ROTATION_COLLECTION = 90.0f;		// X, Y軸のセンサーの値（0.0～1.0）に対して掛ける値
const float POTENTIOMETER_OFFSET = 512.0f;		// ポテンショメーターの最大値（1024）と最小値（0）の中間の値 0～1024の値を -512～512に変換する
const float POTENTIOMETER_COLLECTION = 5.689f;	// 正規化したポテンショメーターの値に対して割る値 512を90°に変換する

// デバイス初期化時に実行される
void setup() {
  Wire.begin();

  // アナログピンの初期化
  pinMode(YAW_POTENTIOMETER_PIN, INPUT);
  pinMode(13, OUTPUT);

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

}


void loop() {
  if (Serial.available() > 0)
  {
  	int input = Serial.read();

  	if (input != 's')
  	{
  		return;
  	}
  	
    Wire.beginTransmission(0x68);
    // データ送信を開始するレジスタの指定（ACCEL_XOUT_H, 0x3B番地）
    Wire.write(0x3B);
    Wire.endTransmission(false);

    // ACCEL_XOUT_Hレジスタから6ビット分のデータを送信させる
    Wire.requestFrom(0x68, 6, true);

    while (Wire.available() < 6);

    int16_t axRaw;	// 加速度 X軸 の生のデータ
    int16_t ayRaw;	// 加速度 Y軸 の生のデータ
    int16_t azRaw;	// 加速度 Z軸 の生のデータ

    axRaw = Wire.read() << 8 | Wire.read();
    ayRaw = Wire.read() << 8 | Wire.read();
    azRaw = Wire.read() << 8 | Wire.read();

    // 加速度値を分解能で割って加速度(G)に変換する
    float accArray[3] = {};

    accArray[0] = axRaw / 16384.0;  //FS_SEL_0 16,384 LSB / g
    accArray[1] = ayRaw / 16384.0;
    accArray[2] = YAW_POTENTIOMETER_READ;

    // センサーのデータを度数法の値に変換する
    for (int i = 0; i < 2; ++i)
    {
      accArray[i] *= ROTATION_COLLECTION;
    }

    accArray[2] -= POTENTIOMETER_OFFSET;
    accArray[2] /= POTENTIOMETER_COLLECTION;
    
    // シリアル出力
    Serial.print(accArray[0], 1);		Serial.print(",");		// Roll	 	X
    Serial.print(accArray[1], 1);		Serial.print(",");		// Pitch 	Y
    Serial.print(accArray[2], 1);		Serial.println("");		// Yaw	 	Z    

    digitalWrite(13, HIGH);
  }
  else
  {
  	digitalWrite(13, LOW);
  }
}
