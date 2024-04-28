// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interaction/OutlineInterface.h"
#include "Interaction/CombatInterface.h"
#include "XRDefenseCharacter.generated.h"


class UWidgetComponent;
class UHealthBarWidget;
class UBehaviorTree;
class AXRDefenceAIController;
class AProjectile;


UCLASS()
class PROJECTT_API AXRDefenseCharacter : public ACharacter, public IOutlineInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AXRDefenseCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void SetHighLightOn();
	virtual void SetHighLightOff();


protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
	EObjectType objectType;

	UPROPERTY(VisibleAnyWhere)
	UStaticMeshComponent* CharacterFloorMesh;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	UWidgetComponent* HealthWidgetComponent;

	UPROPERTY()
	UHealthBarWidget* HealthBarWidget;

	void UpdateHealthBarWidget();

	virtual void PossessedBy(AController* NewController) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(BlueprintReadOnly)
	AActor* CombatTarget = nullptr;



private:
	UPROPERTY(EditAnyWhere)
	float Health = 100.f;

	UPROPERTY(EditAnyWhere)
	float MaxHealth = 100.f;

	bool isAttacking = false;


	UPROPERTY(EditAnyWhere)
	UStaticMesh* DefenseCircle;

	UPROPERTY(EditAnyWhere)
	UStaticMesh* OffenceCircle;

	UPROPERTY(EditAnyWhere)
	UStaticMesh* NoneCircle;

	FVector FloorMeshFirstStartPosition;

	FVector LastPlacablePosition;

	//Animation
	UPROPERTY(EditAnyWhere)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnyWhere)
	UAnimMontage* DeathMontage;


	UPROPERTY(EditAnyWhere)
	UAnimMontage* HitMontage;
	//Animation


	//Material
	// 기본 Material
	UPROPERTY(VisibleAnywhere)
	class UMaterialInterface* DefaultMaterial;

	// 데미지 받았을 때의 Material
	UPROPERTY(EditAnywhere)
	class UMaterialInterface* DamagedMaterial;
	//Material

	UPROPERTY(EditAnywhere)
	USoundBase* DamagedSound;

	FTimerHandle DamageMaterialTimerHandle;

	// 타이머가 만료될 때 호출될 함수
	void DamageMaterialTimerExpired();

	// 타이머를 시작하는 함수
	void DamageMaterialStartTimer(float TimeDuration);


	UPROPERTY(EditAnywhere)
	UParticleSystem* DeathParticle;

	UPROPERTY(EditAnywhere)
	USoundBase* DeathSound;



	UPROPERTY(VisibleAnyWhere)
	bool bIsHighlighted;

	UPROPERTY(EditAnywhere)
	bool bIsOnBoard = false;

	void SetHighlightStencilValue();
	void SetDefaultStencilValue();

	void SetHighLightShowEnable(bool bIsEnable);

	void SetFloorMeshPosition(float DeltaTime);
	bool CheckBeneathIsPlacableArea(FVector StartPoint);


	UPROPERTY(EditAnywhere)
	UBehaviorTree* BehaviorTree;

	UPROPERTY()
	AXRDefenceAIController* XRDefenceAIController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> BulletClass;

	UPROPERTY(EditAnyWhere)
	float AttackRange;

	UPROPERTY(EditAnyWhere)
	float AttackDamage;


	UPROPERTY(EditAnyWhere)
	float DeathTime;

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	virtual void ApplyAttackDamage();

	UFUNCTION(BlueprintCallable)
	virtual void FireBullet();

	bool isDead = false;

	void Death();

public:

	// OutlineINterface
	FORCEINLINE virtual bool GetIsHighlighted() override { return bIsHighlighted; }
	FORCEINLINE virtual bool GetIsOnBoard() override { return bIsOnBoard; }
	virtual void SetIsOnBoard(bool isOnBoard) override;

	virtual void SetActorPosition(FVector Position) override;

	// OutlineINterface

	FORCEINLINE float GetHealth() { return Health; }
	FORCEINLINE float GetMaxHealth() { return MaxHealth; }

	// CombatInterface

	FORCEINLINE virtual EObjectType GetObjectType() override { return objectType; }

	UFUNCTION(BlueprintCallable)
	virtual bool GetIsAttacking() override { return isAttacking; };

	FORCEINLINE virtual AActor* GetCombatTarget() { return CombatTarget; }
	FORCEINLINE virtual void SetCombatTarget(AActor* target) { CombatTarget = target; }

	UFUNCTION(BlueprintCallable)
	virtual float GetAttackRange() override { return AttackRange; }

	UFUNCTION(BlueprintCallable)
	virtual bool GetisDead() override { return isDead; }

	virtual void Attack() override;
	// CombatInterface

};
