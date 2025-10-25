// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/LockOnComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/GameplayLog.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
ULockOnComponent::ULockOnComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

AActor* ULockOnComponent::OwnerPawn() const {
	return GetOwner();
}

// TO BE DEBUG
AController* ULockOnComponent::OwnerController() const {
	if (AActor* Owner = OwnerPawn()) {
		return Owner->GetInstigatorController();
	}
	return nullptr;
}

bool ULockOnComponent::IsValidTarget(AActor* Target) const {
	if (!Target || Target->IsPendingKillPending()) {
		return false;
	}
	const AActor* Owner = OwnerPawn();
	if (!Owner) {
		return false;
	}
	// Check distance
	if (FVector::DistSquared(Target->GetActorLocation(), OwnerPawn()->GetActorLocation()) > LockRadius * LockRadius) {
		return false;
	}
	// Check FOV
	if (AController* Controller = OwnerController()) {
		FVector DirectionToTarget = (Target->GetActorLocation() - OwnerPawn()->GetActorLocation()).GetSafeNormal();
		FVector ForwardVector = Controller->GetControlRotation().Vector();
		float CosAngle = FVector::DotProduct(DirectionToTarget, ForwardVector);
		float CosHalfFOV = FMath::Cos(FMath::DegreesToRadians(LockFOV / 2.f));
		if (CosAngle < CosHalfFOV) {
			return false;
		}
	}
	// TODO: Line of sight check can be added here

	return true;
}

// === Unified target search function ===
// bForSwitch: true when used for left/right switching
// Direction: -1 for left, +1 for right
AActor* ULockOnComponent::FindTarget(bool bForSwitch, int32 Direction) const
{
	AActor* Owner = OwnerPawn();
	if (!Owner) return nullptr;

	const FVector OwnerLoc = Owner->GetActorLocation();
	const FRotator CamRot = OwnerController() ? OwnerController()->GetControlRotation() : FRotator::ZeroRotator;
	const FVector CamForward = FRotationMatrix(CamRot).GetScaledAxis(EAxis::X);
	const FVector CamRight = FRotationMatrix(CamRot).GetScaledAxis(EAxis::Y);

	TArray<FOverlapResult> Hits;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(LockRadius);
	FCollisionQueryParams Params(TEXT("FindTarget"), false, Owner);
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(QueryChannel);

	GetWorld()->OverlapMultiByObjectType(Hits, OwnerLoc, FQuat::Identity, ObjectParams, Sphere, Params);

	AActor* Best = nullptr;
	float BestScore = bForSwitch ? -1.f : FLT_MAX;

	for (const FOverlapResult& R : Hits)
	{
		AActor* Candidate = R.GetActor();
		if (!Candidate || Candidate == Owner) continue;
		if (!IsValidTarget(Candidate)) continue;
		if (bForSwitch && Candidate == CurrentTarget.Get()) continue;

		FVector To = (Candidate->GetActorLocation() - OwnerLoc).GetSafeNormal();

		if (bForSwitch)
		{
			// 左右切换逻辑
			float SideDot = FVector::DotProduct(To, CamRight);
			if ((Direction > 0 && SideDot < 0) || (Direction < 0 && SideDot > 0))
				continue; // 不是想要切换的那一侧

			float ForwardDot = FVector::DotProduct(To, CamForward);
			float Score = ForwardDot + FMath::Abs(SideDot) * 0.5f; // 前方优先 + 横向轻微补偿
			if (Score > BestScore)
			{
				BestScore = Score;
				Best = Candidate;
			}
		}
		else
		{
			// 普通锁定逻辑
			const float Angle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(CamForward.GetSafeNormal(), To)));
			const float Dist = FVector::Dist(Candidate->GetActorLocation(), OwnerLoc);
			const float Score = Angle * 2.0f + Dist * 0.01f;
			if (Score < BestScore)
			{
				BestScore = Score;
				Best = Candidate;
			}
		}
	}

	return Best;
}

bool ULockOnComponent::ToggleLockOn() {
	if (CurrentTarget.IsValid()) {
		BreakLock();
		return false;
	}
	else {
		AActor* NewTarget = FindTarget(false, 0);
		if (NewTarget) {
			CurrentTarget = NewTarget;
			UGameplayLog::InputLog(FString::Printf(TEXT("Locked on to target: %s"), *NewTarget->GetName()));
			//DrawDebugSphere(GetWorld(), NewTarget->GetActorLocation(), 50.f, 16, FColor::Red, false, 0.1f);
			return true;
		}
		else {
			UGameplayLog::InputLog(TEXT("No valid target found."));
			return false;
		}
	}
}

void ULockOnComponent::BreakLock() {
	if (CurrentTarget.IsValid()) {
		UGameplayLog::InputLog(FString::Printf(TEXT("Lock-On broken from target: %s"), *CurrentTarget->GetName()));
	}
	CurrentTarget.Reset();
}

void ULockOnComponent::SwitchTarget(int32 Direction)
{
	if (!CurrentTarget.IsValid()) return;

	AActor* NewTarget = FindTarget(true, Direction);
	if (NewTarget && NewTarget != CurrentTarget.Get())
	{
		CurrentTarget = NewTarget;
		UGameplayLog::InputLog(FString::Printf(TEXT("Switched lock-on to target: %s"), *NewTarget->GetName()));
	}
}

bool ULockOnComponent::IsLockedOn() const
{
	return CurrentTarget.IsValid();
}