// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Character/XRDefenseCharacter.h"
#include "XRDefensePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT_API AXRDefensePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable)
	void CheckLeftOverlappingActors(class UCapsuleComponent* CollisionComponent);

	UFUNCTION(BlueprintCallable)
	void CheckRightOverlappingActors(class UCapsuleComponent* CollisionComponent);


	UPROPERTY(BlueprintReadWrite)
	bool bLeftGrabGestureAvailable = false;

	UPROPERTY(BlueprintReadWrite)
	bool bRightGrabGestureAvailable = false;

	UFUNCTION(BlueprintCallable)
	TArray<FVector> ProjectBoxCollisionPoints(class UBoxComponent* BoxCollision, float Interval);

	UFUNCTION(BlueprintCallable)
	TArray<FVector> ProjectSphereCollisionPoints(USphereComponent* SphereCollision, float Interval);


	bool CheckBeneathIsBoard(FVector& Point);

	
	
	///////////////////////////////Depricated///////////////////////////////////
	UFUNCTION(BlueprintCallable)
	bool CheckOutLineInterfaceLeft(AActor* Target, bool isOverlapStart);

	UFUNCTION(BlueprintCallable)
	bool CheckOutLineInterfaceRight(AActor* Target, bool isOverlapStart);

protected:
	virtual void BeginPlay() override;


private:
	//void TraceUnderMouse();

	UFUNCTION(BlueprintCallable)
	void LeftGrabStart();
	UFUNCTION(BlueprintCallable)
	void LeftGrabEnd();
	UFUNCTION(BlueprintCallable)
	bool LeftGrabCheck(float DeltaTime, FVector GrabPosition);

	AActor* CurrentLeftGrabActor = nullptr;
	IOutlineInterface* CurrentLeftGrabActorOutLineInterface = nullptr;

	IOutlineInterface* pastLeftTarget = nullptr;
	IOutlineInterface* currentLeftTarget = nullptr;



	UFUNCTION(BlueprintCallable)
	void RightGrabStart();
	UFUNCTION(BlueprintCallable)
	void RightGrabEnd();
	UFUNCTION(BlueprintCallable)
	bool RightGrabCheck(float DeltaTime, FVector GrabPosition);

	AActor* CurrentRightGrabActor = nullptr;
	IOutlineInterface* CurrentRightGrabActorOutLineInterface = nullptr;

	IOutlineInterface* pastRightTarget = nullptr;
	IOutlineInterface* currentRightTarget = nullptr;


	// 현재 마우스의 위치를 바닥에 투영하면 어디에 부딪히는지를 구한다
	//void LineTraceMouseToFloor(FHitResult& LinetraceResult);


	//FVector FromMouseToFloorTracingPoint;




	UPROPERTY(EditAnywhere)
	float PlaceUpwardValue = 100.f;
};
