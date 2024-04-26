// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/XRDefensePlayerController.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectT/ProjectT.h"


void AXRDefensePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("LeftClick", IE_Pressed, this, &AXRDefensePlayerController::GrabStart);
	InputComponent->BindAction("LeftClick", IE_Released, this, &AXRDefensePlayerController::GrabEnd);

}


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

void AXRDefensePlayerController::GrabStart()
{
	bGrabGestureAvailable = true;

	// 물체를 집을 수 있는 경우는, 마우스가 물체 위에 있고, 그 물체가 하이라이트 되어 있고, 그 물체가 보드위에 있지 않은 경우에만 가능
	if (currentTarget && currentTarget->GetIsHighlighted() && !currentTarget->GetIsOnBoard())
	{
		CurrentGrabActor = Cast<AActor>(currentTarget);
	}

}

void AXRDefensePlayerController::GrabEnd()
{
	bGrabGestureAvailable = false;
	CurrentGrabActor = nullptr;

	if (CurrentGrabActorOutLineInterface)
	{
		// 자기 아래에 보드가 있는지 없는지 확인하고 그 값을 인터페이스의 set board 함수를 사용해서 배정한다
		CurrentGrabActorOutLineInterface->SetIsOnBoard(CheckBeneathIsBoard(CurrentGrabActorOutLineInterface));

		CurrentGrabActorOutLineInterface->SetHighLightOff();
	}
	CurrentGrabActorOutLineInterface = nullptr;
}

bool AXRDefensePlayerController::GrabCheck(float DeltaTime , FVector GrabPosition)
{
	if (bGrabGestureAvailable)
	{
		CurrentGrabActorOutLineInterface = CurrentGrabActorOutLineInterface == nullptr ? Cast<IOutlineInterface>(CurrentGrabActor) : CurrentGrabActorOutLineInterface;

		if (CurrentGrabActor && CurrentGrabActorOutLineInterface)
		{
			FVector MovingPoint = GrabPosition + FVector::ForwardVector * PlaceUpwardValue;
			CurrentGrabActorOutLineInterface->SetActorPosition(MovingPoint);

			CurrentGrabActorOutLineInterface->SetHighLightOn();
			return true;

		}

	}

	return false;

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

bool AXRDefensePlayerController::CheckOutLineInterface(AActor* Target, bool isOverlapStart)
{
	IOutlineInterface* TargetOutLineInterface = Cast<IOutlineInterface>(Target);

	if (isOverlapStart)
	{
		pastTarget = currentTarget;
		currentTarget = TargetOutLineInterface;

		if (pastTarget)
		{
			pastTarget->SetHighLightOff();
		}

		if (currentTarget && !bGrabGestureAvailable)
		{
			currentTarget->SetHighLightOn();
			return true;
		}

	}
	else // 만약 캐릭터에서 떼지면서 호출된 거라면 
	{
		// 만약 뗴는 캐릭터랑 현재 하이라이트 되어 있는 캐릭터가 같다면
		if (currentTarget == TargetOutLineInterface)
		{
			pastTarget = currentTarget;
			currentTarget = nullptr;

			if (pastTarget)
			{
				pastTarget->SetHighLightOff();
			}

		}
	}

	return false;


}
