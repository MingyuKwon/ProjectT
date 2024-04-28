// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/XRDefensePlayerController.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectT/ProjectT.h"
#include "Components/CapsuleComponent.h"


void AXRDefensePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//InputComponent->BindAction("LeftClick", IE_Pressed, this, &AXRDefensePlayerController::LeftGrabStart);
	//InputComponent->BindAction("LeftClick", IE_Released, this, &AXRDefensePlayerController::LeftGrabEnd);

}

void AXRDefensePlayerController::LeftGrabStart()
{
	// 물체를 집을 수 있는 경우는, 마우스가 물체 위에 있고, 그 물체가 하이라이트 되어 있고, 그 물체가 보드위에 있지 않은 경우에만 가능
	if (currentLeftTarget && currentLeftTarget->GetIsHighlighted() && !currentLeftTarget->GetIsOnBoard())
	{
		CurrentLeftGrabActor = Cast<AActor>(currentLeftTarget);
	}

}

void AXRDefensePlayerController::RightGrabStart()
{
	// 물체를 집을 수 있는 경우는, 마우스가 물체 위에 있고, 그 물체가 하이라이트 되어 있고, 그 물체가 보드위에 있지 않은 경우에만 가능
	if (currentRightTarget && currentRightTarget->GetIsHighlighted() && !currentRightTarget->GetIsOnBoard())
	{
		CurrentRightGrabActor = Cast<AActor>(currentRightTarget);
	}

}

void AXRDefensePlayerController::LeftGrabEnd()
{
	CurrentLeftGrabActor = nullptr;

	if (CurrentLeftGrabActorOutLineInterface)
	{
		// 자기 아래에 보드가 있는지 없는지 확인하고 그 값을 인터페이스의 set board 함수를 사용해서 배정한다
		CurrentLeftGrabActorOutLineInterface->SetIsOnBoard(CheckBeneathIsBoard(CurrentLeftGrabActorOutLineInterface));

		CurrentLeftGrabActorOutLineInterface->SetHighLightOff();
	}
	CurrentLeftGrabActorOutLineInterface = nullptr;
}

void AXRDefensePlayerController::RightGrabEnd()
{
	CurrentRightGrabActor = nullptr;

	if (CurrentRightGrabActorOutLineInterface)
	{
		// 자기 아래에 보드가 있는지 없는지 확인하고 그 값을 인터페이스의 set board 함수를 사용해서 배정한다
		CurrentRightGrabActorOutLineInterface->SetIsOnBoard(CheckBeneathIsBoard(CurrentRightGrabActorOutLineInterface));

		CurrentRightGrabActorOutLineInterface->SetHighLightOff();
	}
	CurrentRightGrabActorOutLineInterface = nullptr;
}

void AXRDefensePlayerController::LeftGrabCheck(float DeltaTime , FVector GrabPosition, FVector& MovingPointLocation)
{
	MovingPointLocation = FVector::ZeroVector;

	if (bLeftGrabGestureAvailable)
	{
		CurrentLeftGrabActorOutLineInterface = CurrentLeftGrabActorOutLineInterface == nullptr ? Cast<IOutlineInterface>(CurrentLeftGrabActor) : CurrentLeftGrabActorOutLineInterface;

		if (CurrentLeftGrabActor && CurrentLeftGrabActorOutLineInterface)
		{
			FVector MovingPoint = GrabPosition;
			CurrentLeftGrabActorOutLineInterface->SetActorPosition(MovingPoint);
			MovingPointLocation = MovingPoint;

			CurrentLeftGrabActorOutLineInterface->SetHighLightOn();
		}

	}

}

void AXRDefensePlayerController::RightGrabCheck(float DeltaTime, FVector GrabPosition, FVector& MovingPointLocation)
{
	MovingPointLocation = FVector::ZeroVector;


	if (bRightGrabGestureAvailable)
	{
		CurrentRightGrabActorOutLineInterface = CurrentRightGrabActorOutLineInterface == nullptr ? Cast<IOutlineInterface>(CurrentRightGrabActor) : CurrentRightGrabActorOutLineInterface;

		if (CurrentRightGrabActor && CurrentRightGrabActorOutLineInterface)
		{
			FVector MovingPoint = GrabPosition;
			CurrentRightGrabActorOutLineInterface->SetActorPosition(MovingPoint);
			MovingPointLocation = MovingPoint;

			CurrentRightGrabActorOutLineInterface->SetHighLightOn();
		}

	}
}


bool AXRDefensePlayerController::CheckBeneathIsBoard(IOutlineInterface* target)
{
	AActor* targetActor = Cast<AActor>(target);
	if (targetActor == nullptr) return false;

	FHitResult LinetraceResult;
	GetWorld()->LineTraceSingleByChannel(LinetraceResult, targetActor->GetActorLocation(), targetActor->GetActorLocation() + FVector::DownVector * TRACE_LENGTH, ECollisionChannel::ECC_BoardTraceChannel);

	// 보드 판을 기준으로 라인 트레이싱을 하므로 보드판과 부딪혔다면 아래는 보드판이 맞다
	return LinetraceResult.bBlockingHit;
}

void AXRDefensePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 마우스 커서 활성화
	bShowMouseCursor = true;

	// 마우스 입력을 UI와 게임 모두에 사용하도록 설정
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	CurrentMouseCursor = EMouseCursor::GrabHand;

}

void AXRDefensePlayerController::CheckLeftOverlappingActors(UCapsuleComponent* CollisionComponent)
{
	TArray<AActor*> OverlappingActors;
	CollisionComponent->GetOverlappingActors(OverlappingActors);

	IOutlineInterface* FinalTargetOutLineInterface = nullptr;

	for (AActor* Actor : OverlappingActors)
	{
		IOutlineInterface* TargetOutLineInterface = Cast<IOutlineInterface>(Actor);

		// 만약 겹치는 물체가 interface 상속을 받는다면
		if (TargetOutLineInterface)
		{
			//하이라이트를 그 물체로 옮김
			FinalTargetOutLineInterface = TargetOutLineInterface;

			// 그리고 그 물체가 현재 하이라이트 되고 있는 물체라면
			if (TargetOutLineInterface == currentLeftTarget)
			{
				//반복문 종료
				break;
			}
		}
	}

	// 이 결과로, 만약 기존에 겹치고 있던 것과 동일한 것이 겹치게 된다면 그 물체가 우선적으로 하이라이트 된다
	// 만약 겹치는게 기존 것이 없고 새로운 것만 있다면 그 물체가 하이라이트

	pastLeftTarget = currentLeftTarget;
	currentLeftTarget = FinalTargetOutLineInterface;

	if (pastLeftTarget && (pastLeftTarget != currentLeftTarget))
	{
		pastLeftTarget->SetHighLightOff();
	}

	if (currentLeftTarget && !bLeftGrabGestureAvailable)
	{
		currentLeftTarget->SetHighLightOn();
	}
}

void AXRDefensePlayerController::CheckRightOverlappingActors(UCapsuleComponent* CollisionComponent)
{
	TArray<AActor*> OverlappingActors;
	CollisionComponent->GetOverlappingActors(OverlappingActors);

	IOutlineInterface* FinalTargetOutLineInterface = nullptr;

	for (AActor* Actor : OverlappingActors)
	{
		IOutlineInterface* TargetOutLineInterface = Cast<IOutlineInterface>(Actor);

		// 만약 겹치는 물체가 interface 상속을 받는다면
		if (TargetOutLineInterface)
		{
			//하이라이트를 그 물체로 옮김
			FinalTargetOutLineInterface = TargetOutLineInterface;

			// 그리고 그 물체가 현재 하이라이트 되고 있는 물체라면
			if (TargetOutLineInterface == currentRightTarget)
			{
				//반복문 종료
				break;
			}
		}
	}

	// 이 결과로, 만약 기존에 겹치고 있던 것과 동일한 것이 겹치게 된다면 그 물체가 우선적으로 하이라이트 된다
	// 만약 겹치는게 기존 것이 없고 새로운 것만 있다면 그 물체가 하이라이트

	pastRightTarget = currentRightTarget;
	currentRightTarget = FinalTargetOutLineInterface;

	if (pastRightTarget && (pastRightTarget != currentRightTarget))
	{
		pastRightTarget->SetHighLightOff();
	}

	if (currentRightTarget && !bRightGrabGestureAvailable)
	{
		currentRightTarget->SetHighLightOn();
	}
}


////////////////////////////////////////// Deprecated /////////////////////////////////////////////////////

////////////////////////////////////////// Deprecated /////////////////////////////////////////////////////


bool AXRDefensePlayerController::CheckOutLineInterfaceLeft(AActor* Target, bool isOverlapStart)
{
	IOutlineInterface* TargetOutLineInterface = Cast<IOutlineInterface>(Target);

	if (isOverlapStart)
	{
		pastLeftTarget = currentLeftTarget;
		currentLeftTarget = TargetOutLineInterface;

		if (pastLeftTarget)
		{
			pastLeftTarget->SetHighLightOff();
		}

		if (currentLeftTarget && !bLeftGrabGestureAvailable)
		{
			currentLeftTarget->SetHighLightOn();
			return true;
		}

	}
	else // 만약 캐릭터에서 떼지면서 호출된 거라면 
	{
		pastLeftTarget = currentLeftTarget;
		currentLeftTarget = nullptr;

		if (pastLeftTarget)
		{
			pastLeftTarget->SetHighLightOff();
		}
	}

	return false;


}

bool AXRDefensePlayerController::CheckOutLineInterfaceRight(AActor* Target, bool isOverlapStart)
{
	IOutlineInterface* TargetOutLineInterface = Cast<IOutlineInterface>(Target);

	if (isOverlapStart)
	{
		pastRightTarget = currentRightTarget;
		currentRightTarget = TargetOutLineInterface;

		if (pastRightTarget)
		{
			pastRightTarget->SetHighLightOff();
		}

		if (currentRightTarget && !bRightGrabGestureAvailable)
		{
			currentRightTarget->SetHighLightOn();
			return true;
		}

	}
	else // 만약 캐릭터에서 떼지면서 호출된 거라면 
	{
		pastRightTarget = currentRightTarget;
		currentRightTarget = nullptr;

		if (pastRightTarget)
		{
			pastRightTarget->SetHighLightOff();
		}
	}

	return false;
}


/*
void AXRDefensePlayerController::TraceUnderMouse()
{
	FHitResult UnderMouseHitResult;
	GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, UnderMouseHitResult);

	// 마우스 아래에 아무것도 hit 하지 못했다면 그냥 넘어감
	if (!UnderMouseHitResult.bBlockingHit)
	{
		if (pastTarget)
		{
			pastTarget->SetHighLightOff();
		}

		if (currentTarget)
		{
			currentTarget->SetHighLightOff();
		}

		currentTarget = nullptr;

		return;
	}

	CheckOutLineInterface(UnderMouseHitResult.GetActor(), true);

	//DrawDebugSphere(GetWorld(), UnderMouseHitResult.ImpactPoint, 20.f, 20, FColor::Red);


}
*/

/*
void AXRDefensePlayerController::LineTraceMouseToFloor(FHitResult& LinetraceResult)
{
	float ScreenX;
	float ScreenY;
	GetMousePosition(ScreenX, ScreenY);

	FVector WorldLocation;
	FVector WorldDirection;

	DeprojectScreenPositionToWorld(ScreenX, ScreenY, WorldLocation, WorldDirection);

	GetWorld()->LineTraceSingleByChannel(LinetraceResult, WorldLocation, WorldLocation + WorldDirection * TRACE_LENGTH, ECollisionChannel::ECC_FloorTraceChannel);

	if (LinetraceResult.bBlockingHit)
	{
		FromMouseToFloorTracingPoint = LinetraceResult.ImpactPoint;
	}

}
*/
