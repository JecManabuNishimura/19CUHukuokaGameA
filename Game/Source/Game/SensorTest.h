//----------------------------------------------------------
// ファイル名		：SensorTest.h
// 概要				：Arduinoからセンサーの情報を読み取り、ActorのRotatorに反映させる
// 作成者			：19CU0238 渡邊龍音
//
// 更新内容			：2020/08/31 渡邊龍音 作成
//					：2020/09/17 渡邊龍音 クラスとして使いやすいように・デッドゾーンの追加
//					：2020/10/07 渡邊龍音 センサーのシリアルポートを自動検出できる
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SensorTest.generated.h"

class USerial;

UCLASS()
class GAME_API ASensorTest : public AActor
{
	GENERATED_BODY()

public:
	ASensorTest();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
	virtual void Tick(float DeltaTime) override;

private:
	// Arduinoのシリアル通信保存用
	USerial* m_pArduinoSerial;

	// 回転量の保存（なめらかに移動するように）
	TArray<FRotator> prevRotator;
	FRotator prevDiffRot;

	
protected:
	// センサーの値をRotatorに変換
	FRotator SensorToRotator();

	// センサーの値をSetActorRotationに設定する
	void SetActorRotationFromSensor(AActor* _setActor);

public:
	// Is Open Com Port
	UPROPERTY(BlueprintReadOnly, Category = "Sensor")
		bool isOpen;

	// (10/07 渡邊)-------------------------------------------------------
	// Is Serialport Automatic detection
	UPROPERTY(EditAnywhere, Category = "Sensor Ports")
		bool isAutoSerialPort;
	
	// For Arduino Com Port
	UPROPERTY(EditAnywhere, Category = "Sensor Ports")
		int serialPort;
	//--------------------------------------------------------------------

	// Dead zone for sensor
	UPROPERTY(EditAnywhere, Category = "Sensor")
		float deadZone;

};