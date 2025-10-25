// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/GameplayLog.h"

void UGameplayLog::InputLog(const FString& Message) {
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString::Printf(TEXT("[INPUT] %s"), *Message));
}

void UGameplayLog::AttributeLog(const FString& Message) {
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("[ATTRIBUTE] %s"), *Message));
}

void UGameplayLog::SystemLog(const FString& Message) {
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, FString::Printf(TEXT("[SYSTEM] %s"), *Message));
}

