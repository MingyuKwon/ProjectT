// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/PlacableArea.h"
#include "ProjectT/ProjectT.h"

APlacableArea::APlacableArea()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Floor Mesh"));
	FloorMesh->SetupAttachment(RootComponent);

	FloorMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FloorMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

}

void APlacableArea::BeginPlay()
{
	Super::BeginPlay();

	switch (objectType)
	{
	case EObjectType::EOT_NONE:
		if (DefaulteArea)
		{
			FloorMesh->SetStaticMesh(DefaulteArea);
		}
		FloorMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

		break;
	case EObjectType::EOT_ATTACKER:
		if (OffenceArea)
		{
			FloorMesh->SetStaticMesh(OffenceArea);
		}

		FloorMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_OffencerFieldTraceChannel, ECollisionResponse::ECR_Block);
		break;

	case EObjectType::EOT_DEFENDER:
		if (DefenceArea)
		{
			FloorMesh->SetStaticMesh(DefenceArea);
		}
		FloorMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_DeffenceFieldTraceChannel, ECollisionResponse::ECR_Block);
		break;
	}

}
