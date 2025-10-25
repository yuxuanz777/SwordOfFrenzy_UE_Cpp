// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "AbilitySystemInterface.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Abilities/GA_Slash.h"
#include "PlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UAbilitySystemComponent;
class UAttributeSet;

// GAS Input ID（建议集中管理）
UENUM(BlueprintType)
enum class ESOFAbilityInputID : uint8
{
	None	UMETA(DisplayName = "None"),
	Slash	UMETA(DisplayName = "Slash"),
	Dodge	UMETA(DisplayName = "Dodge"),
	LockOn	UMETA(DisplayName = "LockOn")
};


UCLASS()
class SWORDOFFRENZY_API APlayerCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	// Input Assets
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* DodgeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* LockOnAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* SwitchTargetAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* SlashAction;

protected:
	// Called when the game starts or when spawned
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	// Called when Controller changes on clients
	virtual void OnRep_Controller() override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Dodge();
	void StartJump();

	// === Setup ===
	void InitializeAttributes();

	// GAS Input
	void OnAbilityInputPressed(ESOFAbilityInputID InputID);
	void OnAbilityInputReleased(ESOFAbilityInputID InputID);

	// Update animation-related variables (selected function)
	void UpdateAnimationVariables();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Lock anim when dodging/attacking
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement Control")
	bool bCanMove = true;
	UFUNCTION(BlueprintCallable, Category = "Movement Control")
	void LockMovement();
	UFUNCTION(BlueprintCallable, Category = "Movement Control")
	void UnlockMovement();


	//// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* Camera System */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FollowCamera;

	/* Lock On */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LockOn")
	class ULockOnComponent* LockOnComp;

	UFUNCTION(BlueprintCallable, Category = "LockOn") void ToggleLockOn();
	UFUNCTION(BlueprintCallable, Category = "LockOn") void SwitchTargetLeft();
	UFUNCTION(BlueprintCallable, Category = "LockOn") void SwitchTargetRight();

	bool bLockOnActive = false;

	/* GAS */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability System")
	UAbilitySystemComponent* AbilitySystemComp;

	UPROPERTY()
	const class UAttributeSet_Sword* AttributeSet;

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void TryActivateSlash();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	class UGA_Slash* GetSlashAbilityInstance() const;


	/* Stamina Management */
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	bool TryConsumeStamina(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void RecoverStamina(float Amount);

	/* Movement */
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	/* Lock On */
	void OnLockOn(const FInputActionValue& Value);
	void OnSwitchTarget(const FInputActionValue& Value);

	/* Ability System Interface */

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return AbilitySystemComp;
	}

	/* Dodge Anim */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	class UAnimMontage* DodgeMontage;


	/* Weapons */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AWeaponBase> WeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	AWeaponBase* EquippedWeapon;
};
