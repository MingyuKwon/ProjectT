// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/XRDefensePlayerController.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectT/ProjectT.h"


void AXRDefensePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//TraceUnderMouse();
	LeftClickCheck(DeltaTime);
}

void AXRDefensePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("LeftClick", IE_Pressed, this, &AXRDefensePlayerController::OnLeftClickPressed);
	InputComponent->BindAction("LeftClick", IE_Released, this, &AXRDefensePlayerController::OnLeftClickReleased);

}

void AXRDefensePlayerController::LeftClickCheck(float DeltaTime)
{
	if (bIsLeftButtonPressed)
	{
		FHitResult LinetraceResult;

		LineTraceMouseToFloor(LinetraceResult);

		if (LinetraceResult.bBlockingHit)
		{
			CurrentGrabActorOutLineInterface = CurrentGrabActorOutLineInterface == nullptr ? Cast<IOutlineInterface>(CurrentGrabActor) : CurrentGrabActorOutLineInterface;

			if (CurrentGrabActor && CurrentGrabActorOutLineInterface)
			{
				FVector MovingPoint = FromMouseToFloorTracingPoint + FVector::UpVector * PlaceUpwardValue;
				CurrentGrabActorOutLineInterface->SetActorPosition(MovingPoint);

				CurrentGrabActorOutLineInterface->SetHighLightOn();
			}
		}


	}
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

void AXRDefensePlayerController::OnLeftClickPressed()
{
	bIsLeftButtonPressed = true;

	// ��ü�� ���� �� �ִ� ����, ���콺�� ��ü ���� �ְ�, �� ��ü�� ���̶���Ʈ �Ǿ� �ְ�, �� ��ü�� �������� ���� ���� ��쿡�� ����
	if (currentTarget && currentTarget->GetIsHighlighted() && !currentTarget->GetIsOnBoard())
	{
		CurrentGrabActor = Cast<AActor>(currentTarget);
	}

}

void AXRDefensePlayerController::OnLeftClickReleased()
{
	bIsLeftButtonPressed = false;
	CurrentGrabActor = nullptr;

	if (CurrentGrabActorOutLineInterface)
	{
		// �ڱ� �Ʒ��� ���尡 �ִ��� ������ Ȯ���ϰ� �� ���� �������̽��� set board �Լ��� ����ؼ� �����Ѵ�
		CurrentGrabActorOutLineInterface->SetIsOnBoard(CheckBeneathIsBoard(CurrentGrabActorOutLineInterface));

		CurrentGrabActorOutLineInterface->SetHighLightOff();
	}
	CurrentGrabActorOutLineInterface = nullptr;
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

	CheckOutLineInterface(UnderMouseHitResult.GetActor());

	//DrawDebugSphere(GetWorld(), UnderMouseHitResult.ImpactPoint, 20.f, 20, FColor::Red);


}

void AXRDefensePlayerController::CheckOutLineInterface(AActor* Target)
{
	IOutlineInterface* TargetOutLineInterface = Cast<IOutlineInterface>(Target);

	pastTarget = currentTarget;
	currentTarget = TargetOutLineInterface;

	if (pastTarget)
	{
		pastTarget->SetHighLightOff();
	}

	if (currentTarget && !bIsLeftButtonPressed)
	{
		currentTarget->SetHighLightOn();
	}
}
