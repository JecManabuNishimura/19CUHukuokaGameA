//----------------------------------------------------------
// ファイル名		：StageSelectCamera.h
// 概要				：StageSelectでプレイヤーの制御
// 作成者			：19CU0222 鍾家同
// 更新内容			：2020/11/23 作成　プレイヤーのカメラの動き
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "StageSelect.h"
#include "StageSelectCamera.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AActor;
class AStageSelect;

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
	// スプリングアーム
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* m_pSpringArm;
	// カメラ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		UCameraComponent* m_pCamera;*/
		// カメラのピッチ範囲(上下)
	UPROPERTY(EditAnywhere, Category = "Camera")
		FVector2D m_cameraPitchLimit;
	// カメラのヨー範囲(左右)
	UPROPERTY(EditAnywhere, Category = "Camera")
		FVector2D m_cameraYawLimit;
	UPROPERTY(EditAnywhere, Category = "LineTrace")
		float length;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FindObject")
		bool isLookAtStageSelect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FindObject")
		bool isPlayingAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FindObject")
		bool canBPPlayAction;
	UPROPERTY(EditAnywhere, Category = "FindObject")
		FVector FinalLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Found StageSelectBP")
		bool canSpawnAttachedActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Found StageSelectBP")
		bool isStageSelectAttached;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Found StartBP")
		bool canDisplayUI;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Found StartBP")
		bool isStartAttached;
	// Returning flag(after pressing "enter" key)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
		bool isReturning;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
		bool isEnterButtonPressed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
		bool isCancelButtonPressed;
	UPROPERTY(EditAnywhere, Category = "Object")
		TSubclassOf<AStageSelect> StageSelectActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
		FRotator SSCurrentRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
		int CurrentStageNum;
	UPROPERTY(EditAnywhere, Category = "Offset")
		FVector SpawnMainActorLocation;

private:
	FVector2D m_cameraRotateInput;

	void CameraMovement(float _deltaTime);

	void SelectObject();
	// 指定されたオブジェクトをカメラにアタッチ
	void AttachObject();
	// オブジェクトを元の位置に戻す
	void ReturnObject();

private:
	bool isHit;



	// values for Timer()
	float currentTime;

	// 選択されたアクターのリアクション
	void ObjectEnlarge(float _deltaTime);

	void Timer(float _deltaTime);

	AActor* SelectedActor;
	AStageSelect* pStageSelect;

	// ============【入力バインド】=============
	// カメラ移動：左右
	void CameraHorizontal(float _axisValue);
	// カメラ移動：前後
	void CameraVertical(float _axisValue);
	// ボタン制御：確定
	void ButtonEnter();
	// ボタン制御：削除
	void ButtonCancel();
	// ボタン制御：項目の切り替え
	void ButtonRight();
	// ボタン制御：項目の切り替え
	void ButtonLeft();
	// =========================================

};
