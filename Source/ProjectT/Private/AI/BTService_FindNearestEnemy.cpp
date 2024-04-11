// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_FindNearestEnemy.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "Interaction/CombatInterface.h"
#include "Character/XRDefenseCharacter.h"
#include "Interaction/OutlineInterface.h"




void UBTService_FindNearestEnemy::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	OwningPawn = AIOwner->GetPawn();
	OwningPawnCombat = Cast<ICombatInterface>(OwningPawn);
	OwningOutLine = Cast<IOutlineInterface>(OwningPawn);

	if (OwningOutLine == nullptr) return;
	if (!OwningOutLine->GetIsOnBoard()) return;
	if (OwningPawnCombat == nullptr) return;

	OwningPawnCombat->SetCombatTarget(nullptr);

	if (OwningPawnCombat->GetObjectType() == EObjectType::EOT_NONE) return;

	TArray<AActor*> ActorArray;
	UGameplayStatics::GetAllActorsOfClass(OwningPawn, AXRDefenseCharacter::StaticClass(), ActorArray);

	CheckEnemies(ActorArray);

}

void UBTService_FindNearestEnemy::CheckEnemies(TArray<AActor*> ActorArray)
{
	OwningPawnCombat->SetCombatTarget(nullptr);

	TMap<AActor*, float> EnemyMap;

	// 우선 자기와 다른 팀들을 싹 맵에 담음
	for (AActor* detected : ActorArray)
	{
		ICombatInterface* DetectedCombat = Cast<ICombatInterface>(detected);
		IOutlineInterface* DetectedOutLine = Cast<IOutlineInterface>(detected);

		if (DetectedCombat && DetectedOutLine)
		{
			if (OwningPawnCombat->GetObjectType() != DetectedCombat->GetObjectType()
				&& DetectedCombat->GetObjectType() != EObjectType::EOT_NONE
				&& DetectedOutLine->GetIsOnBoard()
				&& !DetectedCombat->GetisDead()

				)
			{
				float dist = FVector::Dist(detected->GetActorLocation(), OwningPawn->GetActorLocation());
				EnemyMap.Emplace(detected, dist);
			}
		}
	}

	AActor* ClosestEnemy = nullptr;
	float MinDistance = FLT_MAX; // 가장 큰 float 값으로 초기화

	// EnemyMap을 순회하며 가장 가까운 적 찾기
	for (const TPair<AActor*, float>& Pair : EnemyMap)
	{
		if (Pair.Value < MinDistance)
		{
			MinDistance = Pair.Value;
			ClosestEnemy = Pair.Key;
		}
	}

	OwningPawnCombat->SetCombatTarget(ClosestEnemy);
	UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollowSelector, ClosestEnemy);
	UBTFunctionLibrary::SetBlackboardValueAsFloat(this, DistanceToTargeSelector, MinDistance);

	if (ClosestEnemy)
	{
		FString str = FString::Printf(TEXT("%s : %f"), *ClosestEnemy->GetName(), MinDistance);
		if (OwningPawnCombat->GetObjectType() == EObjectType::EOT_ATTACKER)
		{
			//GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Red , *str);
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(2, 1.f, FColor::Blue, *str);

		}
	}

}
