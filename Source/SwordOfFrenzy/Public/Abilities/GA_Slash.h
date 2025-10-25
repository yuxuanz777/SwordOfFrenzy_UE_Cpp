// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "NativeGameplayTags.h"
#include "GA_Slash.generated.h"

/**
 * 
 */
UCLASS()
class SWORDOFFRENZY_API UGA_Slash : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_Slash();

	// Ability Input ID (for ASC to know which key triggers this ability)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	int32 AbilityInputID = 1; // you can define constants later

	/* Attack Anim */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slash")
	TObjectPtr<UAnimMontage> AttackMontage;

	UFUNCTION(BlueprintCallable, Category = "Slash")
	void SetAttackMontage(class UAnimMontage* NewMontage);


	/* Attack Range */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slash")
	float AttackRange = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slash")
	float AttackRadius = 50.f;

	/* Ability Activation */
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData) override;

protected:
	void PerformAttack(AActor* Avatar);

private:
	bool bHasAttack = false;
};

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Slash);