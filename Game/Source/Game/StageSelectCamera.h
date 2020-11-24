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
	// スプリングアーム
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* m_pSpringArm;
	// カメラ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		UCameraComponent* m_pCamera;
	// カメラのピッチ範囲(上下)
	UPROPERTY(EditAnywhere, Category = "Camera")
		FVector2D m_cameraPitchLimit;
	// カメラのヨー範囲(左右)
	UPROPERTY(EditAnywhere, Category = "Camera")
		FVector2D m_cameraYawLimit;
	UPROPERTY(EditAnywhere, Category = "LineTrace")
		float length;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action")
		bool isLookAtStageSelect;
	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "Action")
		bool isPlayingAction;
	UPROPERTY(EditAnywhere, Category = "Action")
		FVector FinalLocation;


private:
	FVector2D m_cameraRotateInput;

	void CameraMovement(float _deltaTime);

	void ObjectSelect();

	// 選択されたアクターのリアクション
	void ObjectEnlarge(float _deltaTime);

	AActor* SelectedActor;

	// ============【入力バインド】=============
	// 入力バインド】カメラ移動:左右
	void CameraHorizontal(float _axisValue);
	// 入力バインド】カメラ移動:前後
	void CameraVertical(float _axisValue);
	// =========================================

};
