// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/AttributeSet_Sword.h"
#include "Utils/GameplayLog.h"
#include "GameplayEffectExtension.h"

UAttributeSet_Sword::UAttributeSet_Sword()
{
	Health = 100.f;
	Stamina = 10000.f;
	AttackPower = 10.f;
	Defense = 5.f;
}

void UAttributeSet_Sword::ModifyStamina(float Delta) {
	float NewStamina = FMath::Clamp(Stamina.GetCurrentValue() + Delta, 0.f, 10000.f);
	Stamina.SetCurrentValue(NewStamina);
	UGameplayLog::AttributeLog(FString::Printf(TEXT("Stamina modified by %.1f"), NewStamina));
	
}

void UAttributeSet_Sword::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) {
	Super::PostGameplayEffectExecute(Data);

	/* Prevent Overflow */
	if (Data.EvaluatedData.Attribute == GetHealthAttribute()) {
		Health.SetCurrentValue(FMath::Clamp(Health.GetCurrentValue(), 0.f, 100.f));
		UGameplayLog::AttributeLog(FString::Printf(TEXT("Health changed to %.1f"), Health.GetCurrentValue()));
	}
}
