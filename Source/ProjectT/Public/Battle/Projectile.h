// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class PROJECTT_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UNotHitSelf_PMC* Projectile_Movement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* BulletMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCapsuleComponent* Capsule;

	virtual void Destroyed() override;

private:
	UPROPERTY(EditAnywhere)
	float shotSpeed;

	float BulletDamage = 0.f;

	UFUNCTION(BlueprintCallable)
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere)
	USoundBase* HitImpactSound;

	UPROPERTY(EditAnywhere)
	UParticleSystem* HitImpactParticle;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetTarget(AActor* Target);
	void SetDamage(float Damage);

};
