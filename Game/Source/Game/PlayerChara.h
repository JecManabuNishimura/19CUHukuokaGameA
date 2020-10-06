// インクルードガード
#pragma once

// インクルード
#include "CoreMinimal.h"
#include "GameFramework/Character.h"	// ACharacterを継承しているため
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Blueprint/UserWidget.h"
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
	void UpdateSensor(float _deltaTime);

	//	移動処理
	void UpdateMove(float _deltaTime);

	//	ジャンプ処理
	void UpdateJump(float _deltaTime);

	//	ガード処理
	void UpdateGuard();

	//	加速処理
	void UpdateAccelerate();

	//	リスタート
	void RestartGame();

	//	死亡カウント
	void DeadCount();
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

	UPROPERTY(EditAnywhere, Category = "Jump")		//	ジャンプ力
		float m_jumpPower;

	float m_jumpTime;								//	ジャンプ時間
	float m_nowJumpHeight;							//	現在フレームのジャンプ量
	float m_prevJumpHeight;							//	前フレームのジャンプ量

	bool m_bCanJump;
	bool m_bJumping;								//	ジャンプ中フラグ
	FVector m_posBeforeJump;						//	ジャンル開始前のキャラクター座標

	float tempRotate;								//　元状態に戻すの回転角度
	bool m_bGuarding;								//	ガード中フラグ
	bool m_bHaveGuardEnergy;

	float tempSpeed;
	bool m_bIsDamageOver;

	bool m_bDashing;
	bool m_bHaveDashEnergy;

	float m_bTempDamageFrame;
	bool m_bCanDamage;

	bool m_bDead;									//	死亡フラグ

	bool m_bIsGoal;

	float tempRoll;
	float tempPitch;
	float tempYaw;
public:
	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	HPのUI
		TSubclassOf<UUserWidget> player_HP_Widget_Class;
	UUserWidget* Player_HP_Widget;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	DeadのUI
		TSubclassOf<UUserWidget> player_select_Widget_Class;
	UUserWidget* player_select_Widget;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	GuardのUI
		TSubclassOf<UUserWidget> player_guard_Widget_Class;
	UUserWidget* player_guard_Widget;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	DashのUI
		TSubclassOf<UUserWidget> player_dash_Widget_Class;
	UUserWidget* player_dash_Widget;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	DamageのUI
		TSubclassOf<UUserWidget> player_damage_Widget_Class;
	UUserWidget* player_damage_Widget;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	GoalのUI
		TSubclassOf<UUserWidget> player_goal_Widget_Class;
	UUserWidget* player_goal_Widget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 selectPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Guard_UIDownSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Dash_UIDownSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float playerSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float HP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float GuardEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float DashEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float DamageFrame;

	UPROPERTY(EditAnywhere)
		float Fence_FilmDmg;
	// Is Open Com Port
	UPROPERTY(BlueprintReadOnly, Category = "Sensor")
		bool isOpen;

	UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* HitComp,
			class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OverlapEnds(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};