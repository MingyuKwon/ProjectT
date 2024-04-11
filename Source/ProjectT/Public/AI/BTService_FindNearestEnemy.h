// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "BTService_FindNearestEnemy.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT_API UBTService_FindNearestEnemy : public UBTService_BlueprintBase
{
	GENERATED_BODY()
	
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);

	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
	FBlackboardKeySelector TargetToFollowSelector;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
	FBlackboardKeySelector DistanceToTargeSelector;

private:
	void CheckEnemies(TArray<AActor*> ActorArray);

	APawn* OwningPawn;
	class ICombatInterface* OwningPawnCombat;
	class IOutlineInterface* OwningOutLine;
};
