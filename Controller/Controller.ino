#include <Wire.h>

// MPU-6050のアドレス、レジスタ設定値
#define MPU6050_WHO_AM_I     0x75  // Read Only
#define MPU6050_PWR_MGMT_1   0x6B  // Read and Write
#define MPU_ADDRESS  0x68

// アナログピン
#define YAW_POTENTIOMETER_PIN	A0 // A0ピンをポテンショメーターのピンにする
#define YAW_POTENTIOMETER_READ	analogRead(YAW_POTENTIOMETER_PIN)	// A


// デバイス初期化時に実行される
void setup() {
  Wire.begin();

  // アナログピンの初期化
  pinMode(YAW_POTENTIOMETER_PIN, INPUT);

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
  accArray[2] = azRaw / 16384.0;

  Serial.print(accArray[0], 1);				Serial.print(",");
  Serial.print(accArray[1], 1);				Serial.print(",");
  Serial.print(YAW_POTENTIOMETER_READ);		Serial.println("");

  delay(1);
}
