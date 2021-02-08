// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BlueprintUtility.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API UMyBlueprintUtility : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "My Utility", meta = (DisplayName = "Get Execution Time (ms)"))
		static float GetExecTimeMS();

	UFUNCTION(BlueprintPure, Category = "My Utility", meta = (DisplayName = "Get Execution Time (sec)"))
		static float GetExecTimeSec();

	UFUNCTION(BlueprintPure, Category = "My Utility", meta = (DisplayName = "Get FrameRate"))
		static float GetFrameRate();
};
