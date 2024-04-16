#include "Battle/Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "ProjectT/ProjectT.h"
#include "Kismet/GameplayStatics.h"
#include "Component/NotHitSelf_PMC.h"
#include "Sound/SoundBase.h"
#include "Particles/ParticleSystem.h"

// Sets default values
AProjectile::AProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    Capsule = CreateDefaultSubobject<UCapsuleComponent>(FName("Capsule collision"));
    RootComponent = Capsule;
    Capsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Capsule->SetCollisionObjectType(ECollisionChannel::ECC_BulletTraceChannel);
    Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

    BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Bullet Mesh"));
    BulletMesh->SetupAttachment(Capsule);
    BulletMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BulletMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

    Projectile_Movement = CreateDefaultSubobject<UNotHitSelf_PMC>(FName("NoProjectile Movement"));
    Projectile_Movement->bIsHomingProjectile = true;
    Projectile_Movement->HomingAccelerationMagnitude = 2000.f; // 유도 탄의 홈밍 가속도 설정

    Projectile_Movement->bRotationFollowsVelocity = true;
    Projectile_Movement->MaxSpeed = 10000.f;
    Projectile_Movement->InitialSpeed = 2000.f; // 초기 속도 설정
}

void AProjectile::SetTarget(AActor* Target)
{
    if (Projectile_Movement && Target)
    {
        // 타겟의 SkeletalMeshComponent 또는 StaticMeshComponent 등을 홈밍 타겟으로 설정
        UPrimitiveComponent* TargetComponent = Cast<UPrimitiveComponent>(Target->GetRootComponent());
        if (TargetComponent)
        {
            Projectile_Movement->HomingTargetComponent = TargetComponent;
        }
    }
}

void AProjectile::SetDamage(float Damage)
{
    BulletDamage = Damage;
}

void AProjectile::BeginPlay()
{
    Super::BeginPlay();
    BulletDamage = 50;
}

void AProjectile::Destroyed()
{
    if (HitImpactSound)
    {
        UGameplayStatics::SpawnSoundAtLocation(GetWorld(), HitImpactSound, GetActorLocation());
    }

    if (HitImpactParticle)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitImpactParticle, GetActorLocation(), GetActorRotation(), FVector(0.1f, 0.1f, 0.1f));
    }
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    AController* Controller = GetInstigator()->GetController();

    if (GetOwner() == OtherActor)
    {
        return;
    }

    UGameplayStatics::ApplyDamage(OtherActor, BulletDamage, Controller, this, UDamageType::StaticClass());
    Destroy();

}

void AProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // Tick에서의 추가적인 로직 (필요한 경우)
}
