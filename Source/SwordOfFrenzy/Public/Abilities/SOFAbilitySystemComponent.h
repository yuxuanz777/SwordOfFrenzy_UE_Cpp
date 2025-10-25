// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SOFAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class SWORDOFFRENZY_API USOFAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	USOFAbilitySystemComponent();
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override
	{
		Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor); // 必须调用父类
	}
};
