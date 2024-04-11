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
	virtual void Tick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
protected:
	virtual void BeginPlay() override;


private:
	void TraceUnderMouse();

	void OnLeftClickPressed();
	void OnLeftClickReleased();

	void LeftClickCheck(float DeltaTime);

	// 현재 마우스의 위치를 바닥에 투영하면 어디에 부딪히는지를 구한다
	void LineTraceMouseToFloor(FHitResult& LinetraceResult);
	bool CheckBeneathIsBoard(IOutlineInterface* target);

	bool bIsLeftButtonPressed = false;
	FVector FromMouseToFloorTracingPoint;
	AActor* CurrentGrabActor = nullptr;
	IOutlineInterface* CurrentGrabActorOutLineInterface = nullptr;

	IOutlineInterface* pastTarget = nullptr;
	IOutlineInterface* currentTarget = nullptr;


	UPROPERTY(EditAnywhere)
	float PlaceUpwardValue = 100.f;
};
