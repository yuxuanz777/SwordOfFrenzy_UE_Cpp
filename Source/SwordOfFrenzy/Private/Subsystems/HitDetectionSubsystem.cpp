// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/HitDetectionSubsystem.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/GameplayLog.h"

#include "Characters/EnemyCharacter.h"

void UHitDetectionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UGameplayLog::SystemLog(TEXT("[Subsystem] HitDetectionSubsystem Initialized"));
}

void UHitDetectionSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UHitDetectionSubsystem::HandleHit(AActor* Attacker, AActor* Target, float Damage, TSubclassOf<UGameplayEffect> DamageEffect)
{
	if (!Attacker || !Target)
		return;

	UGameplayLog::SystemLog(FString::Printf(TEXT("[HitSubsystem] %s hit %s"), *Attacker->GetName(), *Target->GetName()));

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Attacker);
	ACharacter* TargetChar = Cast<ACharacter>(Target);
	//// ����ʾ����ʩ�ӻ���Ч��
	//if (TargetChar)
	//{
	//	FVector KnockDir = (TargetChar->GetActorLocation() - Attacker->GetActorLocation()).GetSafeNormal();
	//	KnockDir.Z = 0.f;
	//	TargetChar->LaunchCharacter(KnockDir * 400.f + FVector(0, 0, 200.f), true, true);
	//}
	// Ӧ���˺�Ч��
	if (TargetASC && SourceASC)
	{
		FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
		Context.AddSourceObject(Attacker);

		// ���δָ���ⲿGE������Ĭ��GE_Damage
		if (!DamageEffect)
		{
			/*static ConstructorHelpers::FClassFinder<UGameplayEffect> DefaultGE(
				TEXT("/Game/SwordOfFrenzy/Abilities/Effects/GE_Damage"));

			if (DefaultGE.Succeeded())
				DamageEffect = DefaultGE.Class;*/
			UClass* GEClass = LoadClass<UGameplayEffect>(
				nullptr,
				TEXT("/Game/SwordOfFrenzy/Abilities/Effects/GE_Damage.GE_Damage_C")
			);
			if (GEClass)
			{
				DamageEffect = GEClass;
			}
		}

		if (DamageEffect)
		{
			FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(DamageEffect, 1.f, Context);
			if (Spec.IsValid())
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
			}
		}
	}

	// --- �����ܻ����� ---
	const FVector HitDir = Attacker->GetActorLocation() - Target->GetActorLocation();
	if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Target))
	{
		Enemy->PlayHitReaction(HitDir);
		Enemy->ApplyKnockback(HitDir, 600.f);
	}
	else if (ACharacter* Generic = Cast<ACharacter>(Target))
	{
		Generic->LaunchCharacter(-HitDir.GetSafeNormal() * 400.f, true, true);
	}

	//SpawnHitEffects(Target);
}

//void UHitDetectionSubsystem::PlayHitReaction(AActor* Target)
//{
//	// TODO�� �����ܻ���������̬����δʵ�֣�
//	ACharacter* Char = Cast<ACharacter>(Target);
//	if (!Char) return;
//
//	USkeletalMeshComponent* Mesh = Char->GetMesh();
//	if (!Mesh) return;
//
//	// ���� CustomDepth
//	Mesh->SetRenderCustomDepth(true);
//	Mesh->SetCustomDepthStencilValue(250);
//
//	// �ı������ɫ����̬���죩
//	static const FName ParamName(TEXT("HitFlash"));
//	for (int32 i = 0; i < Mesh->GetNumMaterials(); ++i)
//	{
//		UMaterialInstanceDynamic* MID = Mesh->CreateAndSetMaterialInstanceDynamic(i);
//		if (MID)
//			MID->SetVectorParameterValue(ParamName, FLinearColor::Red);
//	}
//
//	// �ָ�Ĭ��״̬
//	FTimerHandle Timer;
//	Char->GetWorldTimerManager().SetTimer(Timer, [Mesh]()
//		{
//			Mesh->SetRenderCustomDepth(false);
//
//			for (int32 i = 0; i < Mesh->GetNumMaterials(); ++i)
//			{
//				UMaterialInstanceDynamic* MID = Mesh->CreateAndSetMaterialInstanceDynamic(i);
//				if (MID)
//					MID->SetVectorParameterValue(TEXT("HitFlash"), FLinearColor::White);
//			}
//		}, 0.15f, false);
//}
//
//void UHitDetectionSubsystem::SpawnHitEffects(AActor* Target)
//{
//	if (!Target) return;
//	UGameplayLog::SystemLog(TEXT("[HitSubsystem] HitEffect Triggered"));
//}
