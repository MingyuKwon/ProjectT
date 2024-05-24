// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProjectT/ProjectT.h"
#include "OutlineInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UOutlineInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTT_API IOutlineInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void SetHighLightOn() = 0;
	virtual void SetHighLightOff() = 0;
	virtual bool GetIsHighlighted() = 0;

	virtual bool GetIsOnBoard() = 0;
	virtual void SetIsOnBoard(bool isOnBoard) = 0;

	virtual void SetActorPosition(FVector Position) = 0;

	virtual bool CheckBeneathIsBoard() = 0;

};
