// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayLog.generated.h"

/**
 * Log Output Utility
 */
UCLASS()
class SWORDOFFRENZY_API UGameplayLog : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	

public:
	// Input event log
	UFUNCTION(BlueprintCallable, Category = "GameplayLog")
	static void InputLog(const FString& Message);
	// GAS Attributes log
	UFUNCTION(BlueprintCallable, Category = "GameplayLog")
	static void AttributeLog(const FString& Message);
	// System log (Lock-on, Statemachine, AI...)
	UFUNCTION(BlueprintCallable, Category = "GameplayLog")
	static void SystemLog(const FString& Message);
};
