// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/OutlineInterface.h"
#include "PlacableArea.generated.h"

UCLASS()
class PROJECTT_API APlacableArea : public AActor
{
	GENERATED_BODY()
	
public:
	APlacableArea();

protected:
	virtual void BeginPlay() override;


private:
	UPROPERTY(EditAnyWhere)
	UStaticMesh* OffenceArea;

	UPROPERTY(EditAnyWhere)
	UStaticMesh* DefenceArea;

	UPROPERTY(EditAnyWhere)
	UStaticMesh* DefaulteArea;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* FloorMesh;


	UPROPERTY(EditAnyWhere)
	EObjectType objectType;

};
