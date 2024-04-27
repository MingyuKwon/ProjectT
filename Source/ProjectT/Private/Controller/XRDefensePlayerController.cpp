// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/XRDefensePlayerController.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectT/ProjectT.h"


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

	if (bGrabGestureAvailable)
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


	if (bGrabGestureAvailable)
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

		if (currentLeftTarget && !bGrabGestureAvailable)
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

		// 만약 뗴는 캐릭터랑 현재 하이라이트 되어 있는 캐릭터가 같다면
		if (currentLeftTarget == TargetOutLineInterface)
		{
			

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

		if (currentRightTarget && !bGrabGestureAvailable)
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

		// 만약 뗴는 캐릭터랑 현재 하이라이트 되어 있는 캐릭터가 같다면
		if (currentRightTarget == TargetOutLineInterface)
		{
			

		}
	}

	return false;
}

////////////////////////////////////////// Deprecated /////////////////////////////////////////////////////

////////////////////////////////////////// Deprecated /////////////////////////////////////////////////////

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
