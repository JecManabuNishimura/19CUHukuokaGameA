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

	// FVector‚Ì”äŠr
	UFUNCTION(BlueprintPure, Category = "My Utility", meta = (DisplayName = "Vector > Vector (AND)", CompactNodeTitle = "> (&)"))
		static bool GreaterAND(FVector _a, FVector _b);

	UFUNCTION(BlueprintPure, Category = "My Utility", meta = (DisplayName = "Vector >= Vector (AND)", CompactNodeTitle = ">= (&)"))
		static bool GreaterEqualAND(FVector _a, FVector _b);

	UFUNCTION(BlueprintPure, Category = "My Utility", meta = (DisplayName = "Vector > Vector (OR)", CompactNodeTitle = "> (|)"))
		static bool GreaterOR(FVector _a, FVector _b);

	UFUNCTION(BlueprintPure, Category = "My Utility", meta = (DisplayName = "Vector >= Vector (OR)", CompactNodeTitle = ">= (|)"))
		static bool GreaterEqualOR(FVector _a, FVector _b);

	UFUNCTION(BlueprintPure, Category = "My Utility", meta = (DisplayName = "Vector < Vector (AND)", CompactNodeTitle = "< (&)"))
		static bool LessAND(FVector _a, FVector _b);

	UFUNCTION(BlueprintPure, Category = "My Utility", meta = (DisplayName = "Vector <= Vector (AND)", CompactNodeTitle = "<= (&)"))
		static bool LessEqualAND(FVector _a, FVector _b);

	UFUNCTION(BlueprintPure, Category = "My Utility", meta = (DisplayName = "Vector < Vector (OR)", CompactNodeTitle = "< (|)"))
		static bool LessOR(FVector _a, FVector _b);

	UFUNCTION(BlueprintPure, Category = "My Utility", meta = (DisplayName = "Vector <= Vector (OR)", CompactNodeTitle = "<= (|)"))
		static bool LessEqualOR(FVector _a, FVector _b);
};
