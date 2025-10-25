// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "HitDetectionSubsystem.generated.h"

class AActor;
class UGameplayEffect;

/**
 * 
 */
UCLASS()
class SWORDOFFRENZY_API UHitDetectionSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/* Handle one hit */
	UFUNCTION(BlueprintCallable, Category = "Hit")
	void HandleHit(AActor* Attacker, AActor* Target, float Damage = 20.f, TSubclassOf<UGameplayEffect> DamageEffect = nullptr);

protected:
	/* Play Hit Animation */
	void PlayHitReaction(AActor* Target);

	/* TODO: Play Effects */
	void SpawnHitEffects(AActor* Target);
};
