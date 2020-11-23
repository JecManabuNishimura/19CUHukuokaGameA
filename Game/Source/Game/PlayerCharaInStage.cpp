//----------------------------------------------------------
// ファイル名		：PlayerCharaInStage.cpp
// 概要				：StageSelectでプレイヤーの制御
// 作成者			：19CU0222 鍾家同
// 更新内容			：2020/11/23 作成　プレイヤーのカメラの動き
//----------------------------------------------------------


#include "PlayerCharaInStage.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APlayerCharaInStage::APlayerCharaInStage()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerCharaInStage::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerCharaInStage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharaInStage::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("CameraUp", this, &APlayerCharaInStage::CameraVertical);
	InputComponent->BindAxis("CameraRight", this, &APlayerCharaInStage::CameraHorizontal);
}

// =======================【入力バインド】=========================
// 入力バインド】カメラ移動:左右
void APlayerCharaInStage::CameraHorizontal(float _axisValue)
{
	cameraRoll = _axisValue;
}
// 入力バインド】カメラ移動:前後
void APlayerCharaInStage::CameraVertical(float _axisValue)
{
	cameraPitch = _axisValue;
}
// =================================================================

