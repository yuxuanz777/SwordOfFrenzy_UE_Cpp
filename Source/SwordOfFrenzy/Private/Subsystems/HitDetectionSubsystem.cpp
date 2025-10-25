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
	//// 初步示例：施加击退效果
	//if (TargetChar)
	//{
	//	FVector KnockDir = (TargetChar->GetActorLocation() - Attacker->GetActorLocation()).GetSafeNormal();
	//	KnockDir.Z = 0.f;
	//	TargetChar->LaunchCharacter(KnockDir * 400.f + FVector(0, 0, 200.f), true, true);
	//}
	// 应用伤害效果
	if (TargetASC && SourceASC)
	{
		FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
		Context.AddSourceObject(Attacker);

		// 如果未指定外部GE，则用默认GE_Damage
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

	// --- 播放受击反馈 ---
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
//	// TODO： 播放受击动画（动态闪红未实现）
//	ACharacter* Char = Cast<ACharacter>(Target);
//	if (!Char) return;
//
//	USkeletalMeshComponent* Mesh = Char->GetMesh();
//	if (!Mesh) return;
//
//	// 启动 CustomDepth
//	Mesh->SetRenderCustomDepth(true);
//	Mesh->SetCustomDepthStencilValue(250);
//
//	// 改变材质颜色（动态闪红）
//	static const FName ParamName(TEXT("HitFlash"));
//	for (int32 i = 0; i < Mesh->GetNumMaterials(); ++i)
//	{
//		UMaterialInstanceDynamic* MID = Mesh->CreateAndSetMaterialInstanceDynamic(i);
//		if (MID)
//			MID->SetVectorParameterValue(ParamName, FLinearColor::Red);
//	}
//
//	// 恢复默认状态
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
