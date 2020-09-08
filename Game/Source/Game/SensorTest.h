//----------------------------------------------------------
// ファイル名		：SensorTest.h
// 概要				：Arduinoからセンサーの情報を読み取り、ActorのRotatorに反映させる
// 作成者			：19CU0238 渡邊龍音
//
// 更新内容			：2020/08/31 渡邊龍音 作成
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

	// センサーの値をRotatorに変換
	FRotator SensorToRotator();

private:
	// Arduinoのシリアル通信保存用
	USerial* m_pArduinoSerial;

	// 回転量の保存（なめらかに移動するように）
	TArray<FRotator> prevRotator;
	FRotator prevDiffRot;

	// For Arduino Com Port
	UPROPERTY(EditAnywhere, Category = "Sensor")
		int serialPort;

public:
	// Is Open Com Port
	UPROPERTY(BlueprintReadOnly, Category = "Sensor")
		bool isOpen;
};