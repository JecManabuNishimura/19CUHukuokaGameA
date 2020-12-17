//----------------------------------------------------------
// ファイル名		：SensorTest.h
// 概要				：敵の操作を行う
// 作成者			：19CU0238 渡邊龍音
//
// 更新内容			：2020/09/13 渡邊龍音 作成
//					：2020/09/19 渡邊龍音 溜めのある敵の動きの追加
//					：			 直線移動をForwardVectorで行う
//----------------------------------------------------------
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyChara.generated.h"

UENUM(BlueprintType)
enum class EEnemyMoveType : uint8
{
	Line			UMETA(DisplayName = "Line Move"),
	Sin				UMETA(DisplayName = "Sine Wave Move"),
	BodyBlow		UMETA(DisplayName = "BodyBlow"),
	Overtake_Line	UMETA(DisplayName = "Overtake (Line)"),
	Overtake_Smooth	UMETA(DisplayName = "Overtake (Smooth)"),
	None			UMETA(DisplayName = "None Move"),
};

UCLASS()

class GAME_API AEnemyChara : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyChara();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// 正弦波に使う時間計測用
	float sinTime;

	// もともとのY軸の位置
	float originPosY;

	// もともとの回転量
	FRotator originRotate;

	// 突進の時間計測用
	float chargeTime;

	// 突進を始めるかどうか
	bool isBlowing;

public:
	// 敵の移動の種類
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Move Generic")
		EEnemyMoveType enemyMoveType;

	// 直線の移動の速さ
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Enemy Move Generic")
		float forwardSpeed;

	// 正弦波移動のときの横移動の速さ
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Enemy Move Sine")
		float sinWaveSpeed;

	// 正弦波移動のときの振れ幅
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Enemy Move Sine")
		float sinWaveFrequency;

	// （仮）プレイヤーの移動速度
	// 本来はプレイヤーから移動速度を取得できるようにしたい
	UPROPERTY(EditAnyWhere, Category = "Enemy Body Blow")
		float _TEMP_playerMoveSpeed;

	// 突進の時に止まるプレイヤーとの距離
	UPROPERTY(EditAnyWhere, Category = "Enemy Body Blow")
		float bodyBlowDistance;

	// 突進の時の向かってくる速度
	UPROPERTY(EditAnyWhere, Category = "Enemy Body Blow")
		float bodyBlowMoveSpeed;

	// 突進の時の速度
	UPROPERTY(EditAnyWhere, Category = "Enemy Body Blow")
		float bodyBlowSpeed;

	// 突進の時の溜め時間
	UPROPERTY(EditAnyWhere, Category = "Enemy Body Blow")
		float bodyBlowChargeTime;

	// 突進の時の回転が開始するまでを遅らせる
	UPROPERTY(EditAnyWhere, Category = "Enemy Body Blow")
		float bodyBlowTurnDelay;

	// 突進の時の回転が終了するまでを早める
	UPROPERTY(EditAnyWhere, Category = "Enemy Body Blow")
		float bodyBlowTurnShorten;

	// Playerを代入
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Move Overtake")
		AActor* playerActor;

	// 追い抜くときの速度
	UPROPERTY(EditAnyWhere, Category = "Enemy Move Overtake")
		float overtakeSpeed;

	// 中に入り始めるオフセット値
	UPROPERTY(EditAnyWhere, Category = "Enemy Move Overtake")
		float overtakeOffset;

	// プレイヤーの位置に来てから内側に入るまでの距離
	UPROPERTY(EditAnyWhere, Category = "Enemy Move Overtake")
		float overtakeDistance;

	// 追い抜いたあとにY方向に移動する量（内側に入る量）
	UPROPERTY(EditAnyWhere, Category = "Enemy Move Overtake")
		float overtakeYAxisMove;

	// 直線移動処理
	//
	// _speed：移動速度
	// _pos	 ：現在の位置
	void Move_Line(const float _speed, const FVector _pos);


	// 正弦波移動処理
	// 
	// _speedForward：前への移動速度
	// _speedSide	：横への移動速度
	// _frequency	：横の振れ幅
	// _deltaTime	：sinの計算に使用する。　Tick()のDeltaTimeを代入
	// _pos			：現在の位置
	void Move_Sin(const float _speedForward, const float _speedSide, const float _frequency, const float _deltaTime, const FVector _pos);


	// 追い越し・直線での移動
	// 内側に入る時に速度も徐々に補間（_overtakeSpeed -> _speed）します。
	//
	// _overtakeSpeed：追い越し時のスピード
	// _speed		 ：追い越しが終わった時の速度
	// _ePos		 ：このActorの現在の位置
	// _pPos		 ：Playerに設定したActorの現在の位置
	void Move_Overtake_Line(const float _overtakeSpeed, const float _speed, const FVector _ePos, const FVector _pPos);


	// 追い越し・なめらかな移動
	// 内側に入る時に速度も徐々に補間（_overtakeSpeed -> _speed）します。
	//
	// _overtakeSpeed：追い越し時のスピード
	// _speed		 ：追い越しが終わった時の速度
	// _ePos		 ：このActorの現在の位置
	// _pPos		 ：Playerに設定したActorの現在の位置
	void Move_Overtake_Lerp(const float _overtakeSpeed, const float _speed, const FVector _ePos, const FVector _pPos);


	// なめらかな移動の際に使う処理（3点の座標からなめらかな曲線を描く）
	//
	// _startPos		：3点のうち、開始地点に当たる座標
	// _midPos			：3点のうち、中間地点に当たる座標
	// _endPos			：3点のうち、目標地点に当たる座標
	// _interpolation	：それぞれの座標を0～1とした時の割合
	// _posX			：（任意）float型の変数のアドレスを指定すれば、そのアドレスの変数にX座標のみを返す
	// _posY			：（任意）float型の変数のアドレスを指定すれば、そのアドレスの変数にY座標のみを返す
	// _posZ			：（任意）float型の変数のアドレスを指定すれば、そのアドレスの変数にZ座標のみを返す
	FVector CalcLerpPos(const FVector _startPos, const FVector _midPos, const FVector _endPos, const float _interpolation
		, float* _posX = NULL, float* _posY = NULL, float* _posZ = NULL);

	// 敵の移動方法を変更する
	void SetEnemyMoveType(EEnemyMoveType _moveType);

	// プレイヤーのActorをGameInstanceから取得する
	UFUNCTION(BlueprintImplementableEvent)
		void GetPlayerFromGameInstance();
};
