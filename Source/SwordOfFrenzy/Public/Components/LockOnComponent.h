// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LockOnComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SWORDOFFRENZY_API ULockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULockOnComponent();

	/* Parameters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LockOn")
	float LockRadius = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LockOn")
	float LockFOV = 75.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LockOn")
	TEnumAsByte<ECollisionChannel> QueryChannel = ECC_Pawn; // Channel to detect potential targets

	/* Current Status */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LockOn")
	TWeakObjectPtr<AActor> CurrentTarget; // Currently locked-on target

	UFUNCTION(BlueprintCallable, Category = "LockOn")
	bool IsLockedOn() const; // Check if currently locked on

	/* Operation Methods */
	UFUNCTION(BlueprintCallable, Category = "LockOn")
	bool ToggleLockOn();

	UFUNCTION(BlueprintCallable, Category = "LockOn")
	void BreakLock();

	UFUNCTION(BlueprintCallable, Category = "LockOn")
	void SwitchTarget(int32 Direction); // +1: right target, -1: left target

	/* Target Logic */
	bool IsValidTarget(AActor* Target) const;

private:
	AActor* OwnerPawn() const; // Owner of this component
	AController* OwnerController() const; // Controller of the owner

	// Unified target search logic
	// bForSwitch: true when used for left/right switching
	// Direction: -1 (left), +1 (right), 0 (default)
	AActor* FindTarget(bool bForSwitch, int32 Direction) const;
};
