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
	// ��ü�� ���� �� �ִ� ����, ���콺�� ��ü ���� �ְ�, �� ��ü�� ���̶���Ʈ �Ǿ� �ְ�, �� ��ü�� �������� ���� ���� ��쿡�� ����
	if (currentLeftTarget && currentLeftTarget->GetIsHighlighted() && !currentLeftTarget->GetIsOnBoard())
	{
		CurrentLeftGrabActor = Cast<AActor>(currentLeftTarget);
	}

}

void AXRDefensePlayerController::RightGrabStart()
{
	// ��ü�� ���� �� �ִ� ����, ���콺�� ��ü ���� �ְ�, �� ��ü�� ���̶���Ʈ �Ǿ� �ְ�, �� ��ü�� �������� ���� ���� ��쿡�� ����
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
		// �ڱ� �Ʒ��� ���尡 �ִ��� ������ Ȯ���ϰ� �� ���� �������̽��� set board �Լ��� ����ؼ� �����Ѵ�
		CurrentLeftGrabActorOutLineInterface->SetIsOnBoard(CurrentLeftGrabActorOutLineInterface->CheckBeneathIsBoard());

		CurrentLeftGrabActorOutLineInterface->SetHighLightOff();
	}
	CurrentLeftGrabActorOutLineInterface = nullptr;
}

void AXRDefensePlayerController::RightGrabEnd()
{
	CurrentRightGrabActor = nullptr;

	if (CurrentRightGrabActorOutLineInterface)
	{
		// �ڱ� �Ʒ��� ���尡 �ִ��� ������ Ȯ���ϰ� �� ���� �������̽��� set board �Լ��� ����ؼ� �����Ѵ�
		CurrentRightGrabActorOutLineInterface->SetIsOnBoard(CurrentRightGrabActorOutLineInterface->CheckBeneathIsBoard());

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


void AXRDefensePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// ���콺 Ŀ�� Ȱ��ȭ
	bShowMouseCursor = true;

	// ���콺 �Է��� UI�� ���� ��ο� ����ϵ��� ����
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

		// ���� ��ġ�� ��ü�� interface ����� �޴´ٸ�
		if (TargetOutLineInterface)
		{
			//���̶���Ʈ�� �� ��ü�� �ű�
			FinalTargetOutLineInterface = TargetOutLineInterface;

			// �׸��� �� ��ü�� ���� ���̶���Ʈ �ǰ� �ִ� ��ü���
			if (TargetOutLineInterface == currentLeftTarget)
			{
				//�ݺ��� ����
				break;
			}
		}
	}

	// �� �����, ���� ������ ��ġ�� �ִ� �Ͱ� ������ ���� ��ġ�� �ȴٸ� �� ��ü�� �켱������ ���̶���Ʈ �ȴ�
	// ���� ��ġ�°� ���� ���� ���� ���ο� �͸� �ִٸ� �� ��ü�� ���̶���Ʈ

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

		// ���� ��ġ�� ��ü�� interface ����� �޴´ٸ�
		if (TargetOutLineInterface)
		{
			//���̶���Ʈ�� �� ��ü�� �ű�
			FinalTargetOutLineInterface = TargetOutLineInterface;

			// �׸��� �� ��ü�� ���� ���̶���Ʈ �ǰ� �ִ� ��ü���
			if (TargetOutLineInterface == currentRightTarget)
			{
				//�ݺ��� ����
				break;
			}
		}
	}

	// �� �����, ���� ������ ��ġ�� �ִ� �Ͱ� ������ ���� ��ġ�� �ȴٸ� �� ��ü�� �켱������ ���̶���Ʈ �ȴ�
	// ���� ��ġ�°� ���� ���� ���� ���ο� �͸� �ִٸ� �� ��ü�� ���̶���Ʈ

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
	else // ���� ĳ���Ϳ��� �����鼭 ȣ��� �Ŷ�� 
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
	else // ���� ĳ���Ϳ��� �����鼭 ȣ��� �Ŷ�� 
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

	// ���콺 �Ʒ��� �ƹ��͵� hit ���� ���ߴٸ� �׳� �Ѿ
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
