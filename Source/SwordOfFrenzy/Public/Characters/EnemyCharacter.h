// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "EnemyCharacter.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;

UCLASS()
class SWORDOFFRENZY_API AEnemyCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

	/* Hit Reaction Anim */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Reaction")
	UAnimMontage* HitReactMontage;

	UFUNCTION(BlueprintCallable, Category = "Hit Reaction")
	void PlayHitReaction(const FVector& HitDirection);

	UFUNCTION(BlueprintCallable, Category = "Hit Reaction")
	void ApplyKnockback(const FVector& FromDirection, float Strength);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability System")
	UAbilitySystemComponent* AbilitySystemComp;

	UPROPERTY()
	const UAttributeSet* AttributeSet;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override {
		return AbilitySystemComp;
	}
};
