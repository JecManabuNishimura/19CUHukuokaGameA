//----------------------------------------------------------
// ファイル名		：StageSelect.h
// 概要				：ステージ画面で各ステージの切り替え
// 作成者			：19CU0222 鍾家同
// 更新内容			：2020/10/27
//					：2020/12/12 更新　画面の移動をStageSelectCameraで制御
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "StageSelect.generated.h"

class UWidgetComponent;
class USceneComponent;
class USpringArmComponent;
class UCameraComponent;
class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class GAME_API AStageSelect : public APawn
{
	GENERATED_BODY()

public:
	// コンストラクタ
	AStageSelect();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// オブジェクト中心
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		USceneComponent* m_MainSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		USceneComponent* m_SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* m_BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UWidgetComponent* m_Stage01;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UWidgetComponent* m_Stage02;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UWidgetComponent* m_Stage03;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UWidgetComponent* m_Stage04;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UWidgetComponent* m_Stage05;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UWidgetComponent* m_Stage06;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UWidgetComponent* m_Stage07;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UWidgetComponent* m_Stage08;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UWidgetComponent* m_Stage09;

	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* m_pSpringArm;	//スプリングアーム
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		UCameraComponent* m_pCamera;			// カメラ

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* m_Stage01;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* m_Stage02;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* m_Stage03;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* m_Stage04;*/

public:
	UPROPERTY(EditAnywhere, Category = "Controllable Value")
		float scalingTime;
	// 縮小率
	UPROPERTY(EditAnywhere, Category = "Controllable Value")
		float scalingRate;
	UPROPERTY(EditAnywhere, Category = "Controllable Value")
		float rotatingTime;
	UPROPERTY(EditAnywhere, Category = "Controllable Value")
		float rotatingRateMax;
	// ステージの数
	UPROPERTY(EditAnywhere, Category = "Controllable Value")
		int stageAmount;
	// rotate duration.
	UPROPERTY(VisibleAnywhere, Category = "Checkable Value")
		float rotatingRate;
	// Current Stage you selected.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Checkable Value")
		int currentStage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkable Value")
		bool anyPressed;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
		bool isEnterButtonPressed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button")
		bool isCancelButtonPressed;*/

		// 入力バインド
public:
	void SelectRight();
	void SelectLeft();
	void SelectConfirm();
	// ボタン制御：確定
	//void ButtonEnter();
	// ボタン制御：削除
	//void ButtonCancel();

private:
	bool canEnter;

	bool turnRight;
	bool finishSmall;
	bool startRotating;
	bool finishRotating;

	int pressCount;
	FRotator nextRotation;
	void StageChanging(float _deltaTime);
	void CurrentStage();

public:
	bool ButtonRightPressed;
	bool ButtonLeftPressed;
	void SelectRightFromSSCamera();
	void SelectLeftFromSSCamera();
};