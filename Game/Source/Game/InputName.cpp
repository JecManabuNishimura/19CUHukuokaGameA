// Fill out your copyright notice in the Description page of Project Settings.


#include "InputName.h"
#include "SensorManager.h"
#include "Engine.h"

// Sets default values
AInputName::AInputName()
	: confirmIndex(1)
	, tempSelectIndex(0)
	, firstNameIndex(0)
	, middleNameIndex(0)
	, lastNameIndex(0)
	, firstNameFinish(false)
	, middleNameFinish(false)
	, lastNameFinish(false)
	, fullName("")
	, nameIsConfirm(false)
	, isSkip(false)
	, isCancel(false)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//	デフォルトプレイヤーとして設定
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	firstName = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "!", "?", "#"};
	middleName = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "!", "?", "#"};
	lastName = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "!", "?", "#"};
}

// Called when the game starts or when spawned
void AInputName::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AInputName::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (tempSelectIndex < 0)
	{
		tempSelectIndex = 28;
	}
	else if (tempSelectIndex > 28)
	{
		tempSelectIndex = 0;
	}

	if (confirmIndex == 1)
	{
		firstNameIndex = tempSelectIndex;		
	}
	else if (confirmIndex == 2)
	{
		firstNameFinish = true;
		middleNameIndex = tempSelectIndex;			
	}
	else if (confirmIndex == 3)
	{
		middleNameFinish = true;
		lastNameIndex = tempSelectIndex;
		
	}
	else if (confirmIndex == 4)
	{
		lastNameFinish = true;
		fullName = firstName[firstNameIndex] + middleName[middleNameIndex] + lastName[lastNameIndex];
	}
	else if (confirmIndex == 5)
	{
		nameIsConfirm = true;
	}

	if (USensorManager::GetIsOpen() == true)
	{
		bool left = false;
		bool right = false;
		USensorManager::GetSensorButton(left, right);

		if (left)
		{
			SpaceButton();
		}
		if (right)
		{
			EnterButton();
		}

		float sensorDataY = USensorManager::GetSensorData().Y;
		if (sensorDataY > sensorThreshold)
		{
			UpButton();
		}
		else if (sensorDataY < -sensorThreshold)
		{
			DownButton();
		}
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, nameIsConfirm ? TEXT("true") : TEXT("false"));
}

// Called to bind functionality to input
void AInputName::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("UpButton", IE_Pressed, this, &AInputName::UpButton);

	InputComponent->BindAction("DownButton", IE_Pressed, this, &AInputName::DownButton);

	InputComponent->BindAction("OK", IE_Pressed, this, &AInputName::EnterButton);

	InputComponent->BindAction("Skip", IE_Pressed, this, &AInputName::SpaceButton);
}

void AInputName::UpButton()
{
	tempSelectIndex--;
}

void AInputName::DownButton()
{
	tempSelectIndex++;
}

void AInputName::EnterButton()
{
	tempSelectIndex = 0;

	if (confirmIndex < 5)
	{
		confirmIndex++;
	}	
}

void AInputName::SpaceButton()
{
	if (confirmIndex < 4)
	{
		isSkip = true;
	}	
	else
	{
		isCancel = true;
	}
}

