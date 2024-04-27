// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/XRPawn.h"

// Sets default values
AXRPawn::AXRPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AXRPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AXRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AXRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

