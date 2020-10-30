//----------------------------------------------------------
// ファイル名		：StageSelect.h
// 概要				：ステージ画面で各ステージの切り替え
// 作成者			：19CU0222 鍾家同
// 更新内容			：2020/10/27
//					：
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "StageSelect.generated.h"

class UWidgetComponent;
class USceneComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class GAME_API AStageSelect : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AStageSelect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

// コンポーネントの作成
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		USceneComponent* m_MainSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		USceneComponent* m_SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UWidgetComponent* m_Stage01;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UWidgetComponent* m_Stage02;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UWidgetComponent* m_Stage03;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UWidgetComponent* m_Stage04;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* m_pSpringArm;	//スプリングアーム
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		UCameraComponent* m_pCamera;			// カメラ

private:
	UPROPERTY(EditAnywhere, Category = "Controllable Value")
		float scalingTime;
	UPROPERTY(EditAnywhere, Category = "Controllable Value")
		float scalingRate;
	UPROPERTY(EditAnywhere, Category = "Controllable Value")
		float rotatingTime;
	UPROPERTY(EditAnywhere, Category = "Controllable Value")
		float rotatingRateMax;

	// rotate duration.
	UPROPERTY(VisibleAnywhere, Category = "Checkable Value")
		float rotatingRate;

	// Current Stage you selected.
	UPROPERTY(VisibleAnywhere, Category = "Checkable Value")
		int stageNum;

// 入力バインド
private:
	void SelectRight();
	void SelectLeft();
	void SelectConfirm();

private:
	bool canEnter;
	bool anyPressed;
	bool turnRight;
	bool finishSmall;
	bool startRotating;
	bool finishRotating;
	int pressCount;
	FRotator nextRotation;
	void StageChanging(float _deltaTime);
	void CurrentStage();
};