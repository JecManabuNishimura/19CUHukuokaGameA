//  インクルードガード
#pragma once

// インクルード
#include "CoreMinimal.h"
#include "GameFramework/Character.h"	// ACharacterを継承しているため
#include "PlayerChara.generated.h"

//	前方宣言
class USerial;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class GAME_API APlayerChara : public ACharacter
{
	// UE4のオブジェクトクラスで必ず先頭に書くマクロ
	GENERATED_BODY()

public:
	// コンストラクタ
	APlayerChara();

protected:
	// ゲームスタート時、または生成時に呼ばれる処理
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
public:
	// 毎フレームの更新処理
	virtual void Tick(float DeltaTime) override;

	// センサーの値をRotatorに変換
	FRotator SensorToRotator();

private:
	//	カメラ更新処理
	void UpdateCamera(float _deltaTime);

	//	移動処理
	void UpdateMove(float _deltaTime);

	//	ジャンプ処理
	void UpdateJump(float _deltaTime);

	//	ガード処理
	void UpdateGuard();

	//	加速処理
	void UpdateAccelerate();
private:
	//	【入力バインド】ジャンプ開始
	void JumpStart();

	//	【入力バインド】ガード開始
	void GuardStart(float _axisValue);
private:
	// Arduinoのシリアル通信保存用
	USerial* m_pArduinoSerial;

	// 回転量の保存（なめらかに移動するように）
	TArray<FRotator> prevRotator;
	FRotator prevDiffRot;

	// For Arduino Com Port
	UPROPERTY(EditAnywhere, Category = "Sensor")
		int serialPort;

	//	UPROPERTYにすることで、ブループリント上で変数の確認、編集などができる
	//	「BlueprintReadOnly」に指定しているため、ブループリントで見ることだけ可能で、編集はできない
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* m_pSpringArm;			//	スプリングアーム

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* m_pCamera;				//	カメラ

	UPROPERTY(EditAnywhere, Category = "Camera")
		FVector2D m_cameraPitchLimit;				//	カメラのピッチ範囲

	UPROPERTY(EditAnywhere, Category = "Jump")
		float m_gravity;							//	重力

	UPROPERTY(EditAnywhere, Category = "Jump")
		float m_jumpPower;							//	ジャンプ力

	float m_jumpTime;								//	ジャンプ時間
	float m_nowJumpHeight;							//	現在フレームのジャンプ量
	float m_prevJumpHeight;							//	前フレームのジャンプ量

	bool m_bJumping;								//	ジャンプ中フラグ
	FVector m_posBeforeJump;						//	ジャンル開始前のキャラクター座標

	bool m_bGuarding;								//	ガード中フラグ

	bool m_bAccelerate;

	bool m_bCanControl;								//	操作可能な状態か?
public:
	// Is Open Com Port
	UPROPERTY(BlueprintReadOnly, Category = "Sensor")
		bool isOpen;

	float tempRoll;
	float tempPitch;
	float tempYaw;
};