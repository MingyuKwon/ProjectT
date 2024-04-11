// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlueprintBase.h"
#include "BTDecorator_DistanceCheck.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT_API UBTDecorator_DistanceCheck : public UBTDecorator_BlueprintBase
{
	GENERATED_BODY()

public:
    UBTDecorator_DistanceCheck();

    // 블랙보드 키의 값을 비교할 두 번째 키
    UPROPERTY(EditAnywhere, Category = "Blackboard", BlueprintReadOnly)
    FBlackboardKeySelector Distance;


protected:
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DIstanceValue = 0;
};
