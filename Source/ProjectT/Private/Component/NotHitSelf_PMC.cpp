// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/NotHitSelf_PMC.h"

UNotHitSelf_PMC::EHandleBlockingHitResult UNotHitSelf_PMC::HandleBlockingHit(const FHitResult& Hit, float TimeTick, const FVector& MoveDelta, float& SubTickTimeRemaining)
{
	Super::HandleBlockingHit(Hit, TimeTick, MoveDelta, SubTickTimeRemaining);
	return EHandleBlockingHitResult::AdvanceNextSubstep;
}

void UNotHitSelf_PMC::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{
	// 여기에 어떤 것도 있어선 안됨

	// 여긴 기존 projectile의 오버라이딩을 해서 그냥 hit 하면 projectile move 에서 자체적으로 멈추는 것을 막기 위한 것이다
}