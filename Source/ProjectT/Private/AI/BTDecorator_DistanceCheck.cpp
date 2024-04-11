// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator_DistanceCheck.h"
#include "Interaction/CombatInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"


UBTDecorator_DistanceCheck::UBTDecorator_DistanceCheck()
{
    NodeName = "Close Enough to";

}

bool UBTDecorator_DistanceCheck::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (BlackboardComp == nullptr) return false;

    // AI 컨트롤러에 접근
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (AIController == nullptr) return false;

    // AI 컨트롤러가 제어하는 폰(캐릭터)에 접근
    APawn* ControlledPawn = AIController->GetPawn();
    if (ControlledPawn == nullptr) return false;

    ICombatInterface* Combat = Cast<ICombatInterface>(ControlledPawn);
    if (Combat == nullptr) return false;


    // 두 키의 값을 가져옵니다
    float FirstValue = Combat->GetAttackRange();
    float SecondValue = DIstanceValue;

    // 값 비교
    return FirstValue >= SecondValue;
}

