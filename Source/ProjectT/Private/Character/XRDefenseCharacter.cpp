#include "Character/XRDefenseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "HUD/HealthBarWidget.h"
#include "AI/XRDefenceAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"
#include "AI/BTTask_Attack.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Battle/Projectile.h"
#include "ProjectT/ProjectT.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "Materials/MaterialInterface.h"


AXRDefenseCharacter::AXRDefenseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 전체적인 충격 감지 캡슐은 카메라와 부딪히지 않도록 함
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);


	// 몬스터 에셋은 충돌을 아예 없앰
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_BulletTraceChannel, ECollisionResponse::ECR_Block);

	CharacterFloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Floor Mesh"));
	CharacterFloorMesh->SetupAttachment(GetMesh());

	CharacterFloorMesh->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	CharacterFloorMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CharacterFloorMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CharacterFloorMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	CharacterFloorMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	HealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(FName("Health Bar"));
	HealthWidgetComponent->SetupAttachment(RootComponent);


}

void AXRDefenseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 초기에 어떤 테두리 색이 될 지 정함
	SetDefaultStencilValue();

	switch (objectType)
	{
	case EObjectType::EOT_NONE:
		if (NoneCircle)
		{
			CharacterFloorMesh->SetStaticMesh(NoneCircle);
		}
		break;
	case EObjectType::EOT_ATTACKER:
		if (OffenceCircle)
		{
			CharacterFloorMesh->SetStaticMesh(OffenceCircle);
		}
		break;
	case EObjectType::EOT_DEFENDER:
		if (DefenseCircle)
		{
			CharacterFloorMesh->SetStaticMesh(DefenseCircle);
		}
		break;
	}

	FloorMeshFirstStartPosition = CharacterFloorMesh->GetComponentLocation();
	LastPlacablePosition = GetActorLocation();

	UpdateHealthBarWidget();

	SetHighLightShowEnable(false);


	DefaultMaterial = GetMesh()->GetMaterial(0);

}

void AXRDefenseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (BehaviorTree && NewController)
	{
		XRDefenceAIController = Cast<AXRDefenceAIController>(NewController);
		if (XRDefenceAIController)
		{
			XRDefenceAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
			XRDefenceAIController->RunBehaviorTree(BehaviorTree);
		}
	}

}

float AXRDefenseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health -= ActualDamage;
	Health = FMath::Clamp(Health, 0, MaxHealth);

	UpdateHealthBarWidget();

	GetMesh()->SetMaterial(0, DamagedMaterial);

	if (DamagedSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), DamagedSound, GetActorLocation());
	}

	DamageMaterialStartTimer(0.15f);

	if (Health <= 0)
	{
		Death();
	}

	return ActualDamage;

}

void AXRDefenseCharacter::DamageMaterialTimerExpired()
{
	GetMesh()->SetMaterial(0, DefaultMaterial);
}

void AXRDefenseCharacter::DamageMaterialStartTimer(float TimeDuration)
{
	GetWorld()->GetTimerManager().SetTimer(DamageMaterialTimerHandle, this, &AXRDefenseCharacter::DamageMaterialTimerExpired, TimeDuration, false);
}

void AXRDefenseCharacter::UpdateHealthBarWidget()
{
	if (HealthWidgetComponent == nullptr) return;

	HealthBarWidget = HealthBarWidget == nullptr ? Cast<UHealthBarWidget>(HealthWidgetComponent->GetUserWidgetObject()) : HealthBarWidget;

	if (HealthBarWidget)
	{
		HealthBarWidget->SetHealthBarPercent(Health / MaxHealth);
	}
}


void AXRDefenseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetFloorMeshPosition(DeltaTime);
}

void AXRDefenseCharacter::SetFloorMeshPosition(float DeltaTime)
{
	FHitResult LinetraceResult;
	GetWorld()->LineTraceSingleByChannel(LinetraceResult, GetActorLocation(), GetActorLocation() + FVector::DownVector * TRACE_LENGTH, ECollisionChannel::ECC_FloorTraceChannel);

	if (LinetraceResult.bBlockingHit)
	{
		CharacterFloorMesh->SetWorldLocation(LinetraceResult.ImpactPoint + FVector::UpVector * 2.f);
	}
	else
	{
		CharacterFloorMesh->SetWorldLocation(FloorMeshFirstStartPosition);
	}
}

void AXRDefenseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AXRDefenseCharacter::SetHighLightOn()
{
	SetHighLightShowEnable(true);
	SetHighlightStencilValue();

	bIsHighlighted = true;
}

void AXRDefenseCharacter::SetHighLightOff()
{
	// 만약 아래에 보드가 없고 하이라이트도 사라지면 그 때 더이상 윤곽선 못 보게 함
	if (!bIsOnBoard)
	{
		SetHighLightShowEnable(false);
	}

	SetDefaultStencilValue();

	bIsHighlighted = false;
}


void AXRDefenseCharacter::SetIsOnBoard(bool isOnBoard)
{
	bIsOnBoard = isOnBoard;

	// 새로 보드 위에 놓인 경우
	if (isOnBoard)
	{
		SetHighLightShowEnable(true);
	}
	else // 보드에서 떠난 경우
	{
		SetHighLightShowEnable(false);
	}
}

void AXRDefenseCharacter::SetActorPosition(FVector Position)
{
	FVector FinalPosition = Position;

	// 지금 놓으려는 공간이 캐릭터를 놓을 수 없는 공간이라면
	if (!CheckBeneathIsPlacableArea(FinalPosition))
	{
		// 놓을 수 있는 공간중에서 가장 가까운 곳을 찾아야 할 것이다
		FinalPosition = LastPlacablePosition;
	}

	LastPlacablePosition = FinalPosition;
	SetActorLocation(FinalPosition);

}

void AXRDefenseCharacter::Attack()
{
	if (AttackMontage)
	{
		isAttacking = true;
		PlayAnimMontage(AttackMontage);
	}

}


void AXRDefenseCharacter::AttackEnd()
{
	isAttacking = false;
}

void AXRDefenseCharacter::ApplyAttackDamage()
{
	UGameplayStatics::ApplyDamage(CombatTarget, AttackDamage, GetController(), this, UDamageType::StaticClass());
}

void AXRDefenseCharacter::FireBullet()
{
	const USkeletalMeshSocket* MuzzleSocket = GetMesh()->GetSocketByName(FName("Muzzle_Front"));
	if (MuzzleSocket)
	{
		FTransform MuzzleTransform = MuzzleSocket->GetSocketTransform(GetMesh());
		FVector ToTarget = CombatTarget->GetActorLocation() - MuzzleTransform.GetLocation();
		FRotator ToTargetRotation = ToTarget.Rotation();

		FActorSpawnParameters SpawnParam;
		SpawnParam.Instigator = GetInstigator();
		SpawnParam.Owner = this;

		if (BulletClass)
		{
			AProjectile* Bullet = GetWorld()->SpawnActor<AProjectile>(
				BulletClass,
				MuzzleTransform.GetLocation(),
				ToTargetRotation,
				SpawnParam
			);

			Bullet->SetTarget(CombatTarget);
			Bullet->SetDamage(AttackDamage);

		}
	}

}

void AXRDefenseCharacter::Death()
{
	if (isDead) return;

	GetMesh()->SetMaterial(0, DefaultMaterial);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	isDead = true;

	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}

	if (DeathSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
	}

	if (DeathParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathParticle, GetActorLocation(), GetActorRotation(), FVector(0.1f, 0.1f, 0.1f ));
	}

	SetLifeSpan(DeathTime);

}

bool AXRDefenseCharacter::CheckBeneathIsPlacableArea(FVector StartPoint)
{
	ECollisionChannel traceChannel = ECollisionChannel::ECC_Visibility;

	switch (objectType)
	{
	case EObjectType::EOT_NONE:
		traceChannel = ECollisionChannel::ECC_Visibility;
		break;
	case EObjectType::EOT_ATTACKER:
		traceChannel = ECollisionChannel::ECC_OffencerFieldTraceChannel;
		break;
	case EObjectType::EOT_DEFENDER:
		traceChannel = ECollisionChannel::ECC_DeffenceFieldTraceChannel;
		break;
	}

	FHitResult LinetraceResult;
	GetWorld()->LineTraceSingleByChannel(LinetraceResult, StartPoint, StartPoint + FVector::DownVector * TRACE_LENGTH, traceChannel);

	return LinetraceResult.bBlockingHit;
}



void AXRDefenseCharacter::SetHighlightStencilValue()
{
	GetMesh()->SetCustomDepthStencilValue(WHITE_STENCIL);
	CharacterFloorMesh->SetCustomDepthStencilValue(WHITE_STENCIL);

}

void AXRDefenseCharacter::SetDefaultStencilValue()
{
	int32 StencilValue = 0;
	switch (objectType)
	{
	case EObjectType::EOT_NONE:
		StencilValue = WHITE_STENCIL;
		break;
	case EObjectType::EOT_ATTACKER:
		StencilValue = RED_STENCIL;
		break;
	case EObjectType::EOT_DEFENDER:
		StencilValue = BLUE_STENCIL;
		break;
	}

	GetMesh()->SetCustomDepthStencilValue(StencilValue);
	CharacterFloorMesh->SetCustomDepthStencilValue(StencilValue);

}

void AXRDefenseCharacter::SetHighLightShowEnable(bool bIsEnable)
{
	GetMesh()->SetRenderCustomDepth(bIsEnable);
	CharacterFloorMesh->SetRenderCustomDepth(bIsEnable);
}



