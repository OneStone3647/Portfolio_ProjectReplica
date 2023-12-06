// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/PRBaseProjectileObject.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"

APRBaseProjectileObject::APRBaseProjectileObject()
{
	PrimaryActorTick.bCanEverTick = false;

	// Root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	
	// HitBox
	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	HitBox->SetupAttachment(Root);
	HitBox->OnComponentHit.AddDynamic(this, &APRBaseProjectileObject::OnHit);
	HitBox->OnComponentBeginOverlap.AddDynamic(this, &APRBaseProjectileObject::OnBeginOverlap);
	HitBox->OnComponentEndOverlap.AddDynamic(this, &APRBaseProjectileObject::OnEndOverlap);

	// Mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	// ProjectileMovement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	// ArrowComponent
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(Root);

	// ProjectileSetting
	ProjectileSpeed = 1000.0f;
	ProjectileGravityScale = 0.0f;
	ProjectileDamage = 0.0f;
}

void APRBaseProjectileObject::BeginPlay()
{
	Super::BeginPlay();
	
	ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->MaxSpeed = ProjectileSpeed;
	ProjectileMovement->ProjectileGravityScale = this->ProjectileGravityScale;
}

#pragma region HitBox
void APRBaseProjectileObject::OnHit_Implementation(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}

void APRBaseProjectileObject::OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void APRBaseProjectileObject::OnEndOverlap_Implementation(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}
#pragma endregion

#pragma region PooledObject
void APRBaseProjectileObject::Activate_Implementation()
{
	Super::Activate_Implementation();

	ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileMovement->InitialSpeed;
}

void APRBaseProjectileObject::Deactivate_Implementation()
{
	Super::Deactivate_Implementation();

	HitBox->SetCollisionProfileName("NoCollision");
	ProjectileMovement->Velocity = FVector::ZeroVector;
}

float APRBaseProjectileObject::GetProjectileDamage() const
{
	return ProjectileDamage;
}
#pragma endregion

