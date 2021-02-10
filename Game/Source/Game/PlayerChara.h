//----------------------------------------------------------
// ファイル名		：PlayerChara.h
// 概要				：プレイヤーの制御
// 作成者			：19CU0220 曹飛
// 更新内容			：2020/10/02 作成　プレイヤーの各操作
//					：2020/11/12 更新　渡邊龍音　センサーを自動的に検出するようになる
//					：2020/11/16 変更　鍾家同　bulletActorをAPlayerBullet型に継承する
//----------------------------------------------------------

// インクルードガード
#pragma once

// インクルード
#include "CoreMinimal.h"
#include "GameFramework/Character.h"	// ACharacterを継承しているため
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Blueprint/UserWidget.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerChara.generated.h"

//	前方宣言
class USerial;
class USpringArmComponent;
class UCameraComponent;
class AActor;
class APlayerBullet;

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

	// 各入力関係メソッドとのバインド処理
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
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

	//	プレイヤーのZ軸獲得
	void GetPlayerPosZ(float DeltaTime);

	//	プレイヤーのエフェクト
	void PlayEffects();

	//	====================================
	//	センサーが持ってない関数

	void Shooting(float DeltaTime);

	//	【入力バインド】キャラ移動:左右
	void MoveRightWithNoSensor(float _axisValue);

	//	【入力バインド】ガード開始
	void GuardStartWithNoSensor(float _axisValue);

	//	【入力バインド】ダッシュ開始
	void DashOrJumpStartWithNoSensor(float _axisValue);

	void ShotStart(float _axisValue);
	//	====================================

private:
	// Arduinoのシリアル通信保存用
	USerial* m_pArduinoSerial;

	// 回転量の保存（なめらかに移動するように）
	TArray<FRotator> prevRotator;
	FRotator prevDiffRot;

	int serialPort;
	//----------------------------------------------------------------------------------
	bool isButtonRelerse;

	//	UPROPERTYにすることで、ブループリント上で変数の確認、編集などができる
	//	「BlueprintReadOnly」に指定しているため、ブループリントで見ることだけ可能で、編集はできない
	UPROPERTY(EditAnywhere, Category = "Jump")
		float gravity;								//	重力

	UPROPERTY(EditAnywhere, Category = "Jump")		//	ジャンプ力
		float jumpPower;

	UPROPERTY(EditAnywhere, Category = "Jump")		//	ジャンプ力
		float superJumpPower;

	float tempJumpPower;

	float jumpTime;									//	ジャンプ時間
	float nowJumpHeight;							//	現在フレームのジャンプ量
	float prevJumpHeight;							//	前フレームのジャンプ量

	bool canJump;
	FVector posBeforeJump;							//	ジャンル開始前のキャラクター座標

	float startPosZ;								//	開始時のZ座標
	float nowPosZ;									//	今の座標
	bool overStartHight;							//	始点の高度を比べるflag

	bool hadDoOnce;

	float tempRotate;								//　元状態に戻すの回転角度

	bool haveGuardEnergy;							//	ガードゲージがまだあるか

	float bulletTimeCount;							// 発射間隔カウントダウン

	float tempSpeed;								//	最大速度を保存
	bool isDamageOver;								//	ダメージを受けられるかどうか

	bool haveDashEnergy;							//	ダッシュのエネルギー

	float tempDamageFrame;							//	ダメージを受けた後の無敵時間

	//	センサーをPC側でテストするためのX,Y,Z角度プロパティ
	float tempPitch;								
	float tempYaw;
	float tempRoll;

	float fps;										//	ゲームのFPS

public:
	//	センサーが持っていますか
	UPROPERTY(EditAnywhere, Category = "Sensor")
		bool withSensor;
	//------------------------------------------------------

	// Attacking type (攻撃タイプ)
	UPROPERTY(EditAnywhere, Category = "Attack")
		PPlayerAttackType playerATKType;

	// Bullet type (弾の使用タイプ)
	UPROPERTY(EditAnywhere, Category = "Bullet")
		TSubclassOf<APlayerBullet> bulletActor;

	// Time Duration between two bullets.(発射間隔)
	UPROPERTY(EditAnywhere, Category = "Bullet")
		float bulletDuration;

	// Using the Offset to prevent the collide with enemy.(弾の生成位置補正)
	UPROPERTY(EditAnywhere, Category = "Offset")
		float bulletXOffset;

	UPROPERTY(EditAnywhere, Category = "Offset")
		float bulletYOffset;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	DeadのUI
		TSubclassOf<UUserWidget> Player_Select_Widget_Class;
	UUserWidget* Player_Select_Widget;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	DamageのUI
		TSubclassOf<UUserWidget> Player_Damage_Widget_Class;
	UUserWidget* Player_Damage_Widget;

	UPROPERTY(EditAnywhere, Category = "UI HUD")	//	GoalのUI
		TSubclassOf<UUserWidget> Player_Goal_Widget_Class;
	UUserWidget* Player_Goal_Widget;

	UPROPERTY(EditAnywhere, Category = "Effects")	//	ダッシュエフェクト
		UNiagaraSystem* DashEffect;

	//	ダッシュエフェクト生成する時の位置補正
	UPROPERTY(EditAnywhere, Category = "Effects")	
		FVector DashEffectLocationOffset;
	UPROPERTY(EditAnywhere, Category = "Effects")
		FRotator DashEffectRotationOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	プレイヤー死んだ後の選択肢
		int32 selectPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	シュートゲージの下がるスピード
		float Shot_UIDownSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	シュートゲージの回復スピード
		float Shot_UIUpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	ガード成功したらゲージの下がる量
		float guardUIDownValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	ガード状態のゲージを下がるスピード
		float Guard_UIDownSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	ガードゲージの回復スピード
		float Guard_UIUpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	ダッシュゲージを下がるスピード
		float Dash_UIDownSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	ダッシュゲージの回復スピード
		float Dash_UIUpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	プレイヤーのスピード
		float playerSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	プレイヤーの最大スピード
		float playerMaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	ダッシュする時の速度（値はプレイヤーの通常スピードと掛けたい数字）
		float DashSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	ゴール到着までの時間
		float GoalTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	プレイヤーのHP
		float HP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	獲得したコインの数
		int32 CoinCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	敵を消滅した数
		int32 CountShootEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	現状態弾のエネルギー
		float ShotEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	弾の最大エネルギー
		float ShotMaxEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	現状態ガードのエネルギー
		float GuardEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	ガードの最大エネルギー
		float GuardEnergyMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	現状態ダッシュのエネルギー
		float DashEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)		//	ダッシュエネルギーの最大値
		float DashEnergyMax;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)		//	敵を消滅或いはコインを獲得したらダッシュエネルギーの増加値
		float AddDashEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)		//	ダメージを受けたら
		float DamageFrame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	コインのScore
		float CoinScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	Player獲得の総Score
		float PlayerScore;							

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	ScoreUIのアニメーションflag
		bool ScoreAnimStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	一時スコアアニメを開始flag
		bool IsAddScoreOver;

	//	=============================================================
	//	プレイヤーの状態
	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	ゲームスタートflag
		bool isStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	ゴールが辿り着くflag
		bool isGoal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	ランキングUIが出るflag
		bool haveShowedRanking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	Dead UIが出るflag
		bool haveShowedDeadUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	プレイヤーの死亡flag
		bool isDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	初めて射撃ボタンを押すflag
		bool isFirstShoting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	射撃中かどうかのflag
		bool isShoting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	弾エネルギーがあるかflag
		bool haveShotEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		//	加速タイルに踏んでいるかflag
		bool isDashLine;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)	//	ジャンプ中かflag
		bool isJumping;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)	//	ジャンプから着地したかflag
		bool isLanding;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)	//	プレイヤーがダッシュ中ですかflag
		bool isDashing;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)	//	プレイヤーがガード中ですかflag
		bool isGuarding;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)	//	ダメージを受けるかflag
		bool canBeDamaged;

	UPROPERTY(EditAnywhere)								//	プレイヤーが受けたダメージ量
		float Damage;

	// Is Open Com Port
	UPROPERTY(BlueprintReadOnly, Category = "Sensor")
		bool isOpen;

	void GetCoin();

	//	Collision detect
	UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* HitComp,
			class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OverlapEnds(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};