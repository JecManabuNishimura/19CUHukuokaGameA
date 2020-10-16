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
class AActor;

UENUM(BlueprintType)
enum class PPlayerAttackType : uint8
{
	Straight		UMETA(DisplayName = "straight bullet"),
	None			UMETA(DisplayName = "no attack"),
};

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

	//発射開始
	void Shooting(float DeltaTime);

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

	bool m_bHaveGuardEnergy;

	// 発射間隔カウントダウン
	float bulletTimeCount;

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
	// Attacking type (攻撃タイプ)
	UPROPERTY(EditAnywhere, Category = "Attack")
		PPlayerAttackType playerATKType;

	// Bullet type (弾の使用タイプ)
	UPROPERTY(EditAnywhere, Category = "Bullet")
		TSubclassOf<AActor> bulletActor;

	// Time Duration between two bullets.(発射間隔)
	UPROPERTY(EditAnywhere, Category = "Bullet")
		float bulletDuration;

	// Using the Offset to prevent the collide with enemy.(弾の生成位置補正)
	UPROPERTY(EditAnywhere, Category = "Offset")
		float bulletXOffset;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	HPのUI
		TSubclassOf<UUserWidget> Player_HP_Widget_Class;
	UUserWidget* Player_HP_Widget;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	ScoreのUI
		TSubclassOf<UUserWidget> Player_Score_Widget_Class;
	UUserWidget* Player_Score_Widget;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	DeadのUI
		TSubclassOf<UUserWidget> Player_Select_Widget_Class;
	UUserWidget* Player_Select_Widget;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	GuardのUI
		TSubclassOf<UUserWidget> Player_Guard_Widget_Class;
	UUserWidget* Player_Guard_Widget;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	DashのUI
		TSubclassOf<UUserWidget> Player_Dash_Widget_Class;
	UUserWidget* Player_Dash_Widget;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	DamageのUI
		TSubclassOf<UUserWidget> Player_Damage_Widget_Class;
	UUserWidget* Player_Damage_Widget;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	GoalのUI
		TSubclassOf<UUserWidget> Player_Goal_Widget_Class;
	UUserWidget* Player_Goal_Widget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 selectPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float guardBulletUIDownSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Guard_UIDownSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Dash_UIDownSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float playerSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float GoalTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 CoinCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 CountShootEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float GuardEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float DashEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float DamageFrame;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)			//	CoinScore
		float CoinScore;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)			//	EnemyScore
		float EnemyScore;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float PlayerScore;								//	Player獲得のScore

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

	bool m_bGuarding;
};