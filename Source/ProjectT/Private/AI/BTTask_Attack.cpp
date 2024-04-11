// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_Attack.h"
#include "AIController.h"
#include "Interaction/CombatInterface.h"

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (AIController == nullptr) return EBTNodeResult::Failed;

    APawn* ControlledPawn = AIController->GetPawn();
    if (ControlledPawn == nullptr) return EBTNodeResult::Failed;

    ICombatInterface* Combat = Cast<ICombatInterface>(ControlledPawn);
    if (Combat == nullptr) return EBTNodeResult::Failed;

    // 비동기 공격 시작. 공격 완료 시 OnAttackFinished 콜백 호출 예정
    Combat->Attack();

    // 태스크가 진행 중임을 알림. 실제 완료는 콜백에서 처리됨
    return EBTNodeResult::InProgress;
}
