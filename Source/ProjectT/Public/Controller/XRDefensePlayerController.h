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
	bool CheckOutLineInterface(AActor* Target, bool isOverlapStart);

protected:
	virtual void BeginPlay() override;


private:
	void TraceUnderMouse();

	UFUNCTION(BlueprintCallable)
	void GrabStart();

	UFUNCTION(BlueprintCallable)
	void GrabEnd();

	UFUNCTION(BlueprintCallable)
	bool GrabCheck(float DeltaTime, FVector GrabPosition);

	// ���� ���콺�� ��ġ�� �ٴڿ� �����ϸ� ��� �ε��������� ���Ѵ�
	void LineTraceMouseToFloor(FHitResult& LinetraceResult);
	bool CheckBeneathIsBoard(IOutlineInterface* target);

	bool bGrabGestureAvailable = false;
	FVector FromMouseToFloorTracingPoint;
	AActor* CurrentGrabActor = nullptr;
	IOutlineInterface* CurrentGrabActorOutLineInterface = nullptr;

	IOutlineInterface* pastTarget = nullptr;
	IOutlineInterface* currentTarget = nullptr;


	UPROPERTY(EditAnywhere)
	float PlaceUpwardValue = 100.f;
};
