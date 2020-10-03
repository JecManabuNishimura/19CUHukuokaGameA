// インクルードガード
#pragma once

// インクルード
#include "CoreMinimal.h"
#include "GameFramework/Character.h"	// ACharacterを継承しているため
#include "Blueprint/UserWidget.h"
#include "PlayerCharaNoSensor.generated.h"

//	前方宣言
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class GAME_API APlayerCharaNoSensor : public ACharacter
{
	// UE4のオブジェクトクラスで必ず先頭に書くマクロ
	GENERATED_BODY()

public:
	// コンストラクタ
	APlayerCharaNoSensor();

protected:
	// ゲームスタート時、または生成時に呼ばれる処理
	virtual void BeginPlay() override;

public:
	// 毎フレームの更新処理
	virtual void Tick(float DeltaTime) override;

	// 各入力関係メソッドとのバインド処理
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	//	カメラ更新処理
	void UpdateCamera(float _deltaTime);

	//	移動処理
	void UpdateMove(float _deltaTime);

	//	ジャンプ処理
	void UpdateJump(float _deltaTime);

	//	ガード処理
	void UpdateGuard(float _deltaTime);
private:
	//	【入力バインド】カメラ回転:Pitch（Y軸）
	void Cam_RotatePitch(float _axisValue);
	//	【入力バインド】カメラ回転:Yaw（Z軸）
	void Cam_RotateYaw(float _axisValue);

	//	【入力バインド】キャラ移動:前後
	void Chara_MoveForward(float _axisValue);
	//	【入力バインド】キャラ移動:左右
	void Chara_MoveRight(float _axisValue);

	//	【入力バインド】ジャンプ開始
	void JumpStart();

	//	【入力バインド】ガード開始
	void GuardStart(float _axisValue);
private:
	//	UPROPERTYにすることで、ブループリント上で変数の確認、編集などができる
	//	「BlueprintReadOnly」に指定しているため、ブループリントで見ることだけ可能で、編集はできない
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* m_pSpringArm;			//	スプリングアーム

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* m_pCamera;				//	カメラ

	FVector2D m_charaMoveInput;						//	Pawn移動入力量
	FVector2D m_cameraRotateInput;					//	カメラ回転入力量
	FVector2D m_charaRotateInput;

	UPROPERTY(EditAnywhere, Category = "Camera")
		FVector2D m_cameraPitchLimit;				//	カメラのピッチ範囲

	UPROPERTY(EditAnywhere, Category = "Move")
		float m_moveSpeed;							//	移動量

	UPROPERTY(EditAnywhere, Category = "Jump")
		float m_gravity;							//	重力

	UPROPERTY(EditAnywhere, Category = "Jump")
		float m_jumpPower;							//	ジャンプ力

	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<UUserWidget> PlayerGuardUIClass;

	UUserWidget* PlayerGuardUI;

	float m_jumpTime;								//	ジャンプ時間
	float m_nowJumpHeight;							//	現在フレームのジャンプ量
	float m_prevJumpHeight;							//	前フレームのジャンプ量

	bool m_bJumping;								//	ジャンプ中フラグ
	FVector m_posBeforeJump;						//	ジャンル開始前のキャラクター座標

	bool m_bGuarding;								//	ガード中フラグ
	bool m_bCanGuard;
	float m_GuardRechargeTime;
	float m_GuardCostTime;

	float m_bDashing;
	float m_bCanDash;
	float m_DashRechargeTime;
	float m_DashCostTime;

	bool m_bCanControl;								//	操作可能な状態か?

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float m_GuardValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float m_DashValue;
};