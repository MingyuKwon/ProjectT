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

	// ��ü�� ���� �� �ִ� ����, ���콺�� ��ü ���� �ְ�, �� ��ü�� ���̶���Ʈ �Ǿ� �ְ�, �� ��ü�� �������� ���� ���� ��쿡�� ����
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
		// �ڱ� �Ʒ��� ���尡 �ִ��� ������ Ȯ���ϰ� �� ���� �������̽��� set board �Լ��� ����ؼ� �����Ѵ�
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

	// ���� ���� �������� ���� Ʈ���̽��� �ϹǷ� �����ǰ� �ε����ٸ� �Ʒ��� �������� �´�
	return LinetraceResult.bBlockingHit;
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
	else // ���� ĳ���Ϳ��� �����鼭 ȣ��� �Ŷ�� 
	{
		// ���� ��� ĳ���Ͷ� ���� ���̶���Ʈ �Ǿ� �ִ� ĳ���Ͱ� ���ٸ�
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
