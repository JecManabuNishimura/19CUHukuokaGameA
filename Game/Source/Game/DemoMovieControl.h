//----------------------------------------------------------
// ファイル名		：DemoMovieControl.h
// 概要				：デモムービーを生成
// 作成者			：19CU0222 鍾家同
// 更新内容			：2020/10/09 作成 デモムービーを生成
//					：
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h"
#include "DemoMovieControl.generated.h"

UCLASS()
class GAME_API ADemoMovieControl : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADemoMovieControl();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<UUserWidget> DemoMovieUIClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
		UUserWidget* DemoMovieUI;

	UFUNCTION()
		void OnLeftClicked();

	// The time to start to play the DemoMovie
	UPROPERTY(EditAnywhere, Category = "Duration")
		float duration;

private:
	FString currentLevelName;

	void CountDown(float _deltaTime);
	
};
