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

bool UMyBlueprintUtility::GreaterAND(FVector _a, FVector _b)
{
	return (_a.X > _b.X) && (_a.Y > _b.Y) && (_a.Z > _b.Z);
}

bool UMyBlueprintUtility::GreaterEqualAND(FVector _a, FVector _b)
{
	return (_a.X >= _b.X) && (_a.Y >= _b.Y) && (_a.Z >= _b.Z);
}

bool UMyBlueprintUtility::GreaterOR(FVector _a, FVector _b)
{
	return (_a.X > _b.X) || (_a.Y > _b.Y) || (_a.Z > _b.Z);
}

bool UMyBlueprintUtility::GreaterEqualOR(FVector _a, FVector _b)
{
	return (_a.X >= _b.X) || (_a.Y >= _b.Y) || (_a.Z >= _b.Z);
}

bool UMyBlueprintUtility::LessAND(FVector _a, FVector _b)
{
	return (_a.X < _b.X) && (_a.Y < _b.Y) && (_a.Z < _b.Z);
}

bool UMyBlueprintUtility::LessEqualAND(FVector _a, FVector _b)
{
	return (_a.X <= _b.X) && (_a.Y <= _b.Y) && (_a.Z <= _b.Z);
}

bool UMyBlueprintUtility::LessOR(FVector _a, FVector _b)
{
	return (_a.X < _b.X) || (_a.Y < _b.Y) || (_a.Z < _b.Z);
}

bool UMyBlueprintUtility::LessEqualOR(FVector _a, FVector _b)
{
	return (_a.X <= _b.X) || (_a.Y <= _b.Y) || (_a.Z <= _b.Z);
}