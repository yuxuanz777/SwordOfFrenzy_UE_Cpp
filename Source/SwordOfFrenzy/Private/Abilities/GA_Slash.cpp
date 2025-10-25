// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GA_Slash.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Subsystems/HitDetectionSubsystem.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Utils/GameplayLog.h"


UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Slash, "Ability.Attack.Slash")

UGA_Slash::UGA_Slash() {
	bReplicateInputDirectly = true; // Replicate input directly to server for instant response
	bServerRespectsRemoteAbilityCancellation = false; // Server does not respect remote ability cancellation
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor; // Each actor gets its own instance of this ability
	// Ability Tag
	AbilityTags.AddTag(TAG_Ability_Slash);
	
}

void UGA_Slash::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	const FGameplayEventData* TriggerEventData) 
{
	if (bHasAttack) {
		return; // Prevent re-entrance
	}
	bHasAttack = true;

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) {
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character) {
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UGameplayLog::InputLog(TEXT("Slash Ability Activated"));

	if (AttackMontage) {
		Character->PlayAnimMontage(AttackMontage);
	}

	PerformAttack(Character);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	bHasAttack = false;
}

void UGA_Slash::PerformAttack(AActor* Avatar) {
	FVector Start = Avatar->GetActorLocation()
		+ Avatar->GetActorForwardVector() * 80.f
		+ FVector(0, 0, 50.f);
	FVector End = Start + Avatar->GetActorForwardVector() * AttackRange;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(AttackRadius);
	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params(TEXT("SlashTrace"), false, Avatar);

	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		Sphere,
		Params
	);

#if WITH_EDITOR
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.f, 0, 1.f);
	DrawDebugSphere(GetWorld(), End, AttackRadius, 12, FColor::Blue, false, 1.f);
#endif

	if (bHit) {
		// Prevent hitting the same actor multiple times
		TSet<AActor*> HitActors;
		for (const FHitResult& Hit : HitResults) {
			AActor* Target = Hit.GetActor();
			if (!Target || Target == Avatar) continue;
			// 使用子系统处理
			if (UWorld* World = Avatar->GetWorld())
			{
				if (UHitDetectionSubsystem* HitSystem = World->GetSubsystem<UHitDetectionSubsystem>())
				{
					HitSystem->HandleHit(Avatar, Target, 20.f);
				}
			}
			//if (!HitActors.Contains(Target)) {
			//	HitActors.Add(Target);
			//	UGameplayLog::SystemLog(FString::Printf(TEXT("Hit target: %s"), *Target->GetName()));
			//	UGameplayStatics::ApplyDamage(Target, 20.f, Avatar->GetInstigatorController(), Avatar, nullptr);

			//	// 应用 GameplayEffect（GE_Damage）
			//	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
			//	{
			//		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
			//		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
			//		Context.AddSourceObject(this);

			//		// 从资源加载 GE_Damage（路径要与你在 Content 中的一致）
			//		UClass* GEClass = LoadClass<UGameplayEffect>(
			//			nullptr,
			//			TEXT("/Game/SwordOfFrenzy/Abilities/Effects/GE_Damage.GE_Damage_C")
			//		);
			//		UGameplayEffect* GE_DamageAsset = GEClass->GetDefaultObject<UGameplayEffect>();

			//		if (GE_DamageAsset)
			//		{
			//			FGameplayEffectSpecHandle Spec = TargetASC->MakeOutgoingSpec(
			//				GE_DamageAsset->GetClass(), 1.f, Context);
			//			if (Spec.IsValid())
			//			{
			//				TargetASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
			//			}
			//		}
			//	}
			//}
		}
	}
	else {
		UGameplayLog::SystemLog("No targets hit");
	}
}

void UGA_Slash::SetAttackMontage(UAnimMontage* NewMontage)
{
	AttackMontage = NewMontage;
}
