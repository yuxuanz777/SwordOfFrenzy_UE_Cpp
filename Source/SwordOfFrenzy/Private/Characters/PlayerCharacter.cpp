// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/PlayerCharacter.h"
#include "Weapons/WeaponBase.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/LockOnComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Abilities/AttributeSet_Sword.h"
#include "Abilities/SOFAbilitySystemComponent.h"
//#include "AbilitySystemComponent.h"
#include "Utils/GameplayLog.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/* Move towards camera direction */
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

	/* Camera Boom */
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	// Camera Boom Parameters
	CameraBoom->TargetArmLength = 400.f; // Distance to character
	CameraBoom->bEnableCameraLag = true; // Enable camera lag
	CameraBoom->CameraLagSpeed = 5.f; // Camera lag speed
	CameraBoom->CameraLagMaxDistance = 70.f; // Max distance for camera lag
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	/* Follow Camera */
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	/* Lock On Component */ 
	LockOnComp = CreateDefaultSubobject<ULockOnComponent>(TEXT("LockOnComponent"));

	/* Ability System Component */
	AbilitySystemComp = CreateDefaultSubobject<USOFAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UAttributeSet_Sword>(TEXT("AttributeSet"));

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Attach Weapons
	//if (WeaponClass)
	//{
	//	FActorSpawnParameters SpawnParams;
	//	SpawnParams.Owner = this;
	//	SpawnParams.Instigator = this;

	//	EquippedWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, SpawnParams);
	//	if (EquippedWeapon)
	//	{
	//		EquippedWeapon->AttachToCharacter(this);
	//	}
	//}

	// Initialize GAS
	AbilitySystemComp->InitStats(UAttributeSet_Sword::StaticClass(), nullptr);
	if (DefaultMappingContext)
	{
		UE_LOG(LogTemp, Warning, TEXT("MappingContext found"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DefaultMappingContext is NULL!"));
	}

	if(APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	if (AbilitySystemComp)
	{
		AbilitySystemComp->InitAbilityActorInfo(this, this);

		// 临时添加 Buff 测试
		UClass* GEClass = LoadClass<UGameplayEffect>(
			nullptr,
			TEXT("/Game/SwordOfFrenzy/Abilities/Effects/GE_Damage.GE_Damage_C")
		);
		UGameplayEffect* GE_Buff = GEClass->GetDefaultObject<UGameplayEffect>();
		if (GE_Buff)
		{
			FGameplayEffectContextHandle Context = AbilitySystemComp->MakeEffectContext();
			FGameplayEffectSpecHandle Spec = AbilitySystemComp->MakeOutgoingSpec(GE_Buff->GetClass(), 1.f, Context);
			if (Spec.IsValid())
			{
				AbilitySystemComp->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
				UE_LOG(LogTemp, Log, TEXT("[Buff] Attack Power increased by 10 for 5s."));
			}
		}

		// Optional safety: ensure granted on server in case PossessedBy path differs
		if (HasAuthority())
		{
			if (!AbilitySystemComp->FindAbilitySpecFromClass(UGA_Slash::StaticClass()))
			{
				AbilitySystemComp->GiveAbility(FGameplayAbilitySpec(UGA_Slash::StaticClass(), 1, /*InputID*/1));
				UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Granted Slash in BeginPlay (authority)"));
			}
		}
	}
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComp)
	{
		AbilitySystemComp->InitAbilityActorInfo(this, this);

		if (HasAuthority())
		{
			if (!AbilitySystemComp->FindAbilitySpecFromClass(UGA_Slash::StaticClass()))
			{
				AbilitySystemComp->GiveAbility(FGameplayAbilitySpec(UGA_Slash::StaticClass(), 1, /*InputID*/1));
			}
			UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Granted Slash in PossessedBy"));
		}
	}
}

void APlayerCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	if (AbilitySystemComp)
	{
		AbilitySystemComp->InitAbilityActorInfo(this, this);
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/* Running Stamina Consumption */
	//if (GetVelocity().Size() > 200.f)
	//{
	//	TryConsumeStamina(5.0f * DeltaTime); // Example consumption rate
	//}

	/* Stamina Recovery */
	RecoverStamina(DeltaTime);

	/* Lock-On Camera Adjustment */ 
	if (bLockOnActive && LockOnComp && LockOnComp->CurrentTarget.IsValid()) {
		// Target disabled conditions
		if (!LockOnComp->IsValidTarget(LockOnComp->CurrentTarget.Get())) {
			ToggleLockOn();
			return;
		}

		// Rotate character to face target
		const FVector TargetLocation = LockOnComp->CurrentTarget->GetActorLocation();
		const FVector DirectionToTarget = (TargetLocation - GetActorLocation());
		const FRotator DesiredYaw(0.f, DirectionToTarget.Rotation().Yaw, 0.f);
		const FRotator NewRot = FMath::RInterpTo(GetActorRotation(), DesiredYaw, DeltaTime, 10.f);
		SetActorRotation(NewRot);

		// Rotate camera to look at target
		if (Controller) {
			const FRotator CamRot = UKismetMathLibrary::FindLookAtRotation(FollowCamera->GetComponentLocation(), 
				TargetLocation + FVector(0, 0, 20)); // Target offset (Head)
			const FRotator NewCtrl = FMath::RInterpTo(Controller->GetControlRotation(), CamRot, DeltaTime, 12.f);
			Controller->SetControlRotation(NewCtrl);
		}
	}
}

// === GAS Utility ===
void APlayerCharacter::InitializeAttributes()
{
	// 可在此应用初始属性（Attribute GE），或留空
}

/* Player Input Management */
void APlayerCharacter::OnAbilityInputPressed(ESOFAbilityInputID InputID)
{
	if (AbilitySystemComp)
		AbilitySystemComp->AbilityLocalInputPressed((int32)InputID); // 1 = Slash InputID
}

void APlayerCharacter::OnAbilityInputReleased(ESOFAbilityInputID InputID)
{
	if (AbilitySystemComp)
		AbilitySystemComp->AbilityLocalInputReleased((int32)InputID);
}
// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::StartJump);
		// Dodging
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &APlayerCharacter::Dodge);
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		// Lock On
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Started, this, &APlayerCharacter::OnLockOn);
		// Switch Target
		EnhancedInputComponent->BindAction(SwitchTargetAction, ETriggerEvent::Started, this, &APlayerCharacter::OnSwitchTarget);
		// Slash Ability
		//EnhancedInputComponent->BindAction(SlashAction, ETriggerEvent::Started, this, &APlayerCharacter::TryActivateSlash);
		EnhancedInputComponent->BindAction(SlashAction, ETriggerEvent::Started, this, &APlayerCharacter::OnAbilityInputPressed, ESOFAbilityInputID::Slash);
		EnhancedInputComponent->BindAction(SlashAction, ETriggerEvent::Completed, this, &APlayerCharacter::OnAbilityInputReleased, ESOFAbilityInputID::Slash);
	}
}

/* Player Move Management */

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (!bCanMove) return;
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr && (MovementVector.Size() > 0.f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, MovementVector.X);
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, MovementVector.Y);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr && (LookAxisVector.Size() > 0.f))
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerCharacter::StartJump()
{
	float CurrentSpeed = GetVelocity().Size();
	// Example stamina cost for jumping
	if (TryConsumeStamina(20.f)/* && CurrentSpeed > 300.f*/)
	{
		UGameplayLog::InputLog(TEXT("Jump Triggered."));
		Jump();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Too slow to jump!")); // 调试信息
	}
}

void APlayerCharacter::Dodge()
{
	// Example stamina cost for dodging
	if (TryConsumeStamina(30.f)) {
		UGameplayLog::InputLog(TEXT("Dodge Triggered."));

		// Play dodge animation here
		if (DodgeMontage)
		{
			PlayAnimMontage(DodgeMontage);
		}


		// Determine movement-based launch direction
		FVector MoveDir = GetLastMovementInputVector();
		MoveDir.Z = 0.f;
		// Fallback to current horizontal velocity if no input
		if (MoveDir.IsNearlyZero())
		{
			MoveDir = GetVelocity();
			MoveDir.Z = 0.f;
		}
		// Final fallback to actor forward if still zero
		if (MoveDir.IsNearlyZero())
		{
			MoveDir = GetActorForwardVector();
		}
		MoveDir = MoveDir.GetSafeNormal();
		LaunchCharacter(MoveDir * 600.f + FVector(0, 0, 0), true, true);
	}
	else {
		UGameplayLog::InputLog(TEXT("Not enough stamina to dodge."));
	}
}

void APlayerCharacter::OnLockOn(const FInputActionValue&) 
{ 
	ToggleLockOn();
}
void APlayerCharacter::OnSwitchTarget(const FInputActionValue& V)
{
	const float Axis = V.Get<float>();
	if (Axis > 0.5f)       SwitchTargetRight();
	else if (Axis < -0.5f) SwitchTargetLeft();
}

/* Stamina Management */
bool APlayerCharacter::TryConsumeStamina(float Amount) {
	if (!AttributeSet) {
		UE_LOG(LogTemp, Error, TEXT("AttributeSet is NULL!"));
		return false;
	}
	float CurrentStamina = AttributeSet->GetStamina();
	if(CurrentStamina >= Amount) {
		// Consume Stamina
		const_cast<UAttributeSet_Sword*>(AttributeSet)->ModifyStamina(-Amount);
		return true;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Not enough stamina to consume. Current: %f, Required: %f"), CurrentStamina, Amount);
		return false;
	}
}

void APlayerCharacter::RecoverStamina(float DeltaTime) {
	if (!AttributeSet) {
		UE_LOG(LogTemp, Error, TEXT("AttributeSet is NULL!"));
		return;
	}
	const float RecoveryRate = 5.f; // Stamina points recovered per second
	//const_cast<UAttributeSet_Sword*>(AttributeSet)->ModifyStamina(RecoveryRate *  DeltaTime);

}

/* Lock-On Control */
void APlayerCharacter::ToggleLockOn()
{
	if (!LockOnComp) return;

	if (LockOnComp->ToggleLockOn())
	{
		bLockOnActive = true;
		// 锁定时：角色用控制器朝向，不再用移动方向自动面向
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		// 相机继续用控制器旋转
		CameraBoom->bUsePawnControlRotation = true;
	}
	else
	{
		bLockOnActive = false;
		// 解绑：恢复自由状态
		LockOnComp->BreakLock();
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		CameraBoom->bUsePawnControlRotation = true;
	}
}

void APlayerCharacter::SwitchTargetLeft() { 
	if (bLockOnActive && LockOnComp) LockOnComp->SwitchTarget(-1);
}
void APlayerCharacter::SwitchTargetRight() { 
	if (bLockOnActive && LockOnComp) LockOnComp->SwitchTarget(+1); 
}

/* Abilities */
void APlayerCharacter::TryActivateSlash()
{
	if (!AbilitySystemComp)
	{
		UGameplayLog::SystemLog(TEXT("ASC is null."));
		return;
	}

	// Check if ability spec exists locally
	TArray<FGameplayAbilitySpec*> MatchingSpecs;
	constexpr bool bTagRequirementsMustMatch = false;
	AbilitySystemComp->GetActivatableGameplayAbilitySpecsByAllMatchingTags(
		FGameplayTagContainer(TAG_Ability_Slash), MatchingSpecs, bTagRequirementsMustMatch);

	if (MatchingSpecs.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Slash spec NOT present locally. Waiting for replication or granting on server if authoritative."));

		// Optional: Ensure given on server
		if (HasAuthority() && !AbilitySystemComp->FindAbilitySpecFromClass(UGA_Slash::StaticClass()))
		{
			AbilitySystemComp->GiveAbility(FGameplayAbilitySpec(UGA_Slash::StaticClass(), 1, 0));
			UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Granted Slash on server on-demand."));
		}
	}

	const bool bSuccess = AbilitySystemComp->TryActivateAbilitiesByTag(FGameplayTagContainer(TAG_Ability_Slash));
	UGameplayLog::SystemLog(bSuccess ? TEXT("Slash ability activation succeeded.") : TEXT("Slash ability activation failed."));
}

UGA_Slash* APlayerCharacter::GetSlashAbilityInstance() const
{
	if (!AbilitySystemComp) return nullptr;

	// 找到这条能力的 Spec
	FGameplayAbilitySpec* Spec = AbilitySystemComp->FindAbilitySpecFromClass(UGA_Slash::StaticClass());
	if (!Spec) return nullptr;

	// 对于 InstancedPerActor，拿“主实例”
	if (UGameplayAbility* Inst = Spec->GetPrimaryInstance())
	{
		return Cast<UGA_Slash>(Inst);
	}

	// 万一还没实例化（极少见），可以保底返回 CDO，但一般我们只用实例
	return nullptr;
}

/* 锁定移动用于动画 */
void APlayerCharacter::LockMovement()
{
	bCanMove = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	//bUseControllerRotationYaw = true;
}

void APlayerCharacter::UnlockMovement()
{
	bCanMove = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	//bUseControllerRotationYaw = false;
}

