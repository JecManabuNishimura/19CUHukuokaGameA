// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputName.generated.h"

UCLASS()
class GAME_API AInputName : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AInputName();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	void UpButton();

	void DownButton();

	void EnterButton();

	void SpaceButton();
public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		int confirmIndex;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		int tempSelectIndex;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		int firstNameIndex;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		int middleNameIndex;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		int lastNameIndex;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		bool firstNameFinish;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		bool middleNameFinish;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		bool lastNameFinish;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		bool nameIsConfirm;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		bool isSkip;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		bool isCancel;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		TArray<FString> firstName;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		TArray<FString> middleName;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		TArray<FString> lastName;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		FString fullName;
};
