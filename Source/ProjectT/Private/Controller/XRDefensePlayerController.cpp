// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/XRDefensePlayerController.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectT/ProjectT.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"


void AXRDefensePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

TArray<FVector> AXRDefensePlayerController::ProjectSphereCollisionPoints(USphereComponent* SphereCollision, float Interval)
{
	TArray<FVector> ProjectedPoints;

	if (!SphereCollision) return ProjectedPoints;

	FVector Origin = SphereCollision->GetComponentLocation();
	float Radius = SphereCollision->GetScaledSphereRadius();

	// 구의 평면을 일정 간격으로 분할
	for (float Theta = 0.0f; Theta <= 360.0f; Theta += Interval)
	{
		for (float Phi = 0.0f; Phi <= 360.0f; Phi += Interval)
		{
			// 구면 좌표계를 사용하여 평면 위의 점 계산
			float RadTheta = FMath::DegreesToRadians(Theta);
			float RadPhi = FMath::DegreesToRadians(Phi);

			FVector Point;
			Point.X = Origin.X + Radius * FMath::Cos(RadTheta) * FMath::Sin(RadPhi);
			Point.Y = Origin.Y + Radius * FMath::Sin(RadTheta) * FMath::Sin(RadPhi);
			Point.Z = Origin.Z; // z=0 평면으로 자름

			if (CheckBeneathIsBoard(Point))
			{
				ProjectedPoints.Add(Point);
			}
		}
	}

	return ProjectedPoints;
}

TArray<FVector> AXRDefensePlayerController::ProjectBoxCollisionPoints(UBoxComponent* BoxCollision, float Interval)
{
	TArray<FVector> ProjectedPoints;

	if (!BoxCollision) return ProjectedPoints;

	FVector Origin;
	FVector Extent;
	Extent = BoxCollision->GetScaledBoxExtent();
	Origin = BoxCollision->GetComponentLocation();
	FRotator Rotation = BoxCollision->GetComponentRotation();

	TArray<FVector> BoxPoints;
	BoxPoints.Add(FVector(-Extent.X, -Extent.Y, Extent.Z));
	BoxPoints.Add(FVector(Extent.X, -Extent.Y, Extent.Z));
	BoxPoints.Add(FVector(-Extent.X, Extent.Y, Extent.Z));
	BoxPoints.Add(FVector(Extent.X, Extent.Y, Extent.Z));


	// 로컬 좌표계를 월드 좌표계로 변환
	TArray<FVector> TransformedPoints;
	for (const FVector& Point : BoxPoints)
	{
		FVector RotatedPoint = Rotation.RotateVector(Point);
		FVector TransformedPoint = Origin + RotatedPoint;
		TransformedPoints.Add(TransformedPoint);
	}

	// 직사각형의 네 개의 꼭짓점
	FVector P1 = TransformedPoints[0];
	FVector P2 = TransformedPoints[1];
	FVector P3 = TransformedPoints[2];
	FVector P4 = TransformedPoints[3];

	// P1-P2-P3-P4 평면을 Interval 간격으로 분할
	for (float x = 0.0f; x <= 1.0f; x += Interval)
	{
		for (float y = 0.0f; y <= 1.0f; y += Interval)
		{
			// 두 변을 따라 보간
			FVector Interp1 = FMath::Lerp(P1, P2, x);
			FVector Interp2 = FMath::Lerp(P3, P4, x);
			// 보간된 두 점을 연결
			FVector Point = FMath::Lerp(Interp1, Interp2, y);
			if (CheckBeneathIsBoard(Point))
			{
				ProjectedPoints.Add(Point);
			}
		}
	}

	return ProjectedPoints;
}

bool AXRDefensePlayerController::CheckBeneathIsBoard(FVector& Point)
{
	FHitResult LinetraceResult;
	GetWorld()->LineTraceSingleByChannel(LinetraceResult, Point, Point + FVector::DownVector * TRACE_LENGTH, ECollisionChannel::ECC_BoardTraceChannel);

	if (LinetraceResult.bBlockingHit)
	{
		Point = LinetraceResult.ImpactPoint;
	}

	return LinetraceResult.bBlockingHit;
}

void AXRDefensePlayerController::LeftGrabStart()
{
	if (currentLeftTarget && currentLeftTarget->GetIsHighlighted() && !currentLeftTarget->GetIsOnBoard())
	{
		CurrentLeftGrabActor = Cast<AActor>(currentLeftTarget);
	}

}

void AXRDefensePlayerController::RightGrabStart()
{
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
		CurrentRightGrabActorOutLineInterface->SetIsOnBoard(CurrentRightGrabActorOutLineInterface->CheckBeneathIsBoard());

		CurrentRightGrabActorOutLineInterface->SetHighLightOff();
	}
	CurrentRightGrabActorOutLineInterface = nullptr;
}

bool AXRDefensePlayerController::LeftGrabCheck(float DeltaTime , FVector GrabPosition)
{
	if (bLeftGrabGestureAvailable)
	{
		CurrentLeftGrabActorOutLineInterface = CurrentLeftGrabActorOutLineInterface == nullptr ? Cast<IOutlineInterface>(CurrentLeftGrabActor) : CurrentLeftGrabActorOutLineInterface;

		if (CurrentLeftGrabActor && CurrentLeftGrabActorOutLineInterface)
		{
			FVector MovingPoint = GrabPosition;
			CurrentLeftGrabActorOutLineInterface->SetActorPosition(MovingPoint);
			CurrentLeftGrabActorOutLineInterface->SetHighLightOn();

			return true;
		}

	}

	return false;

}

bool AXRDefensePlayerController::RightGrabCheck(float DeltaTime, FVector GrabPosition)
{

	if (bRightGrabGestureAvailable)
	{
		CurrentRightGrabActorOutLineInterface = CurrentRightGrabActorOutLineInterface == nullptr ? Cast<IOutlineInterface>(CurrentRightGrabActor) : CurrentRightGrabActorOutLineInterface;

		if (CurrentRightGrabActor && CurrentRightGrabActorOutLineInterface)
		{
			FVector MovingPoint = GrabPosition;
			CurrentRightGrabActorOutLineInterface->SetActorPosition(MovingPoint);
			CurrentRightGrabActorOutLineInterface->SetHighLightOn();

			return true;

		}

	}

	return false;

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
