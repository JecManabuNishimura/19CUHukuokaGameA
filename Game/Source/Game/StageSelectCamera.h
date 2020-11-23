//----------------------------------------------------------
// ファイル名		：PlayerCharaInStage.h
// 概要				：StageSelectでプレイヤーの制御
// 作成者			：19CU0222 鍾家同
// 更新内容			：2020/11/23 作成　プレイヤーのカメラの動き
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "StageSelectCamera.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class GAME_API AStageSelectCamera : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AStageSelectCamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true")) USpringArmComponent* m_pSpringArm;	// スプリングアーム
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true")) UCameraComponent* m_pCamera;			// カメラ

private:
	float cameraRoll;
	float cameraPitch;

	// ============【入力バインド】=============
	// 入力バインド】カメラ移動:左右
	void CameraHorizontal(float _axisValue);
	// 入力バインド】カメラ移動:前後
	void CameraVertical(float _axisValue);
	// =========================================

};
