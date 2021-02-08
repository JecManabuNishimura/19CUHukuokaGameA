// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintUtility.h"

float UMyBlueprintUtility::GetExecTimeMS()
{
	extern ENGINE_API float GAverageMS;
	return GAverageMS;
}

float UMyBlueprintUtility::GetExecTimeSec()
{
	extern ENGINE_API float GAverageMS;
	return GAverageMS / 1000.0f;
}

float UMyBlueprintUtility::GetFrameRate()
{
	extern ENGINE_API float GAverageFPS;
	return GAverageFPS;
}