// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EnemyCharacter.h"
#include "Abilities/AttributeSet_Sword.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/* Ability System Component */
	AbilitySystemComp = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UAttributeSet>(TEXT("AttributeSet"));

}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	AbilitySystemComp->InitStats(UAttributeSet_Sword::StaticClass(), nullptr);

}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/* Hit Reaction */
void AEnemyCharacter::PlayHitReaction(const FVector& HitDirection)
{
	if (!HitReactMontage) return;

	const FVector Forward = GetActorForwardVector();
	const float Dot = FVector::DotProduct(Forward, HitDirection.GetSafeNormal());
	const float CrossZ = FVector::CrossProduct(Forward, HitDirection.GetSafeNormal()).Z;

	FName SectionName = "Hit_Front";
	if (Dot < -0.5f)
		SectionName = "Hit_Back";
	else if (CrossZ > 0.3f)
		SectionName = "Hit_Right";
	else if (CrossZ < -0.3f)
		SectionName = "Hit_Left";

	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
	{
		Anim->Montage_Play(HitReactMontage);
		Anim->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void AEnemyCharacter::ApplyKnockback(const FVector& FromDirection, float Strength)
{
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (!MoveComp) return;

	FVector KnockDir = -FromDirection.GetSafeNormal();
	KnockDir.Z = 0.f;

	FVector Launch = KnockDir * Strength + FVector(0, 0, 150.f);
	LaunchCharacter(Launch, true, true);
}
