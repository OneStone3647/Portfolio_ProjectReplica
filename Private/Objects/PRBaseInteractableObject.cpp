// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/PRBaseInteractableObject.h"
#include "Characters/PRPlayerCharacter.h"
#include "Components/BoxComponent.h"

APRBaseInteractableObject::APRBaseInteractableObject()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractInfo = FPRInteractInfo();
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->InitBoxExtent(FVector(30.0f, 30.0f, 30.0f));
	BoxComponent->SetCollisionProfileName(TEXT("InteractalbeObject"));
	RootComponent = BoxComponent;
}

void APRBaseInteractableObject::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &APRBaseInteractableObject::OnOverlapBegin);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &APRBaseInteractableObject::OnOverlapEnd);
}

bool APRBaseInteractableObject::OnInteract_Implementation()
{
	return IInterface_PRInteract::OnInteract_Implementation();
}

FPRInteractInfo APRBaseInteractableObject::GetInteractInfo_Implementation()
{
	return InteractInfo;
}

void APRBaseInteractableObject::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APRPlayerCharacter* PRPlayerCharacter = Cast<APRPlayerCharacter>(OtherActor);
	if(IsValid(PRPlayerCharacter) == true)
	{
		InteractInfo.InteractTarget = PRPlayerCharacter;
		PRPlayerCharacter->AddToInteractableObjects(this);
	}
}

void APRBaseInteractableObject::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
	APRPlayerCharacter* PRPlayerCharacter = Cast<APRPlayerCharacter>(OtherActor);
	if(IsValid(PRPlayerCharacter) == true)
	{
		InteractInfo.InteractTarget = nullptr;
		PRPlayerCharacter->RemoveToInteractableObjects(this);
	}
}

