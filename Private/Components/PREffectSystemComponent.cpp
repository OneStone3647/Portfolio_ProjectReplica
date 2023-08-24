// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PREffectSystemComponent.h"
#include "Effect/PREffect.h"
#include "Effect/PRNiagaraEffect.h"
#include "Effect/PRParticleEffect.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/PRBaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

UPREffectSystemComponent::UPREffectSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	EffectPool.Empty();
	ActivatePoolIndexes.Empty();
}

void UPREffectSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	Initialize();
	CreateEffectPool();
}

void UPREffectSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPREffectSystemComponent::Initialize()
{
	// EffectPool의 모든 Effect들을 제거합니다.
	for(const auto& NewEffects : EffectPool)
	{
		for(const auto& Effect : NewEffects.Value.Effects)
		{
			if(IsValid(Effect) == true)
			{
				Effect->Destroy();
			}
		}
	}

	// EffectPool과 ActivatePoolIndexes를 초기화합니다.
	EffectPool.Empty();
	ActivatePoolIndexes.Empty();
}

void UPREffectSystemComponent::CreateEffectPool()
{
	// NiagaraEffectPool 생성
	if(NiagaraEffectDataTable != nullptr)
	{
		TArray<FName> RowNames = NiagaraEffectDataTable->GetRowNames();
		for(const auto& RowName : RowNames)
		{
			FPRNiagaraEffectInfo* DataTableNiagaraEffectInfo = NiagaraEffectDataTable->FindRow<FPRNiagaraEffectInfo>(RowName, FString(""));
			if(DataTableNiagaraEffectInfo != nullptr && DataTableNiagaraEffectInfo->NiagaraSystem != nullptr)
			{
				FPREffectPool NewNiagaraEffectPool = CreateNiagaraEffectPool(*DataTableNiagaraEffectInfo);
				EffectPool.Emplace(DataTableNiagaraEffectInfo->NiagaraSystem, NewNiagaraEffectPool);

				FPRActivateIndex NewActivateIndexes;
				ActivatePoolIndexes.Emplace(DataTableNiagaraEffectInfo->NiagaraSystem, NewActivateIndexes);
			}
		}
	}

	// ParticleEffectPool 생성
	if(ParticleEffectDataTable != nullptr)
	{
		TArray<FName> RowNames = ParticleEffectDataTable->GetRowNames();
		for(const auto& RowName : RowNames)
		{
			FPRParticleEffectInfo* DataTableParticleEffectInfo = NiagaraEffectDataTable->FindRow<FPRParticleEffectInfo>(RowName, FString(""));
			if(DataTableParticleEffectInfo != nullptr && DataTableParticleEffectInfo->ParticleSystem != nullptr)
			{
				FPREffectPool NewParticleEffectPool = CreateParticleEffectPool(*DataTableParticleEffectInfo);
				// EffectPool.Emplace(DataTableParticleEffectInfo->EffectName, NewParticleEffectPool);
				EffectPool.Emplace(DataTableParticleEffectInfo->ParticleSystem, NewParticleEffectPool);

				FPRActivateIndex NewActivateIndexes;
				ActivatePoolIndexes.Emplace(DataTableParticleEffectInfo->ParticleSystem, NewActivateIndexes);
			}
		}
	}
}

UPREffect* UPREffectSystemComponent::SpawnEffectAtLocation(UFXSystemAsset* SpawnEffect, FVector Location, FRotator Rotation, FVector Scale, bool bEffectAutoActivate)
{
	// EffectPool에서 실행할 Effect를 탐색합니다.
	for(auto& NewEffects : EffectPool)
	{
		if(NewEffects.Key == SpawnEffect)
		{
			for(auto& ActivateableEffect : NewEffects.Value.Effects)
			{
				// 활성화되지 않은 Effect일 경우 지정한 위치에 Effect를 Spawn하고 활성화하고 반환합니다.
				if(IsActivateEffect(ActivateableEffect) == false)
				{
					ActivateableEffect->SpawnEffectAtLocation(Location, Rotation, Scale, bEffectAutoActivate);
					ActivatePoolIndexes.Find(SpawnEffect)->ActivateIndexes.Emplace(ActivateableEffect->GetPoolIndex());
					
					return ActivateableEffect;
				}
			}

			// 풀의 모든 Effect가 활성화되었을 경우 새로운 Effect를 생성하여 풀에 넣고 활성화하고 반환합니다.
			// Effect가 Niagara인지 Particle인지 판별하여 Effect를 생성합니다.
			if(SpawnEffect->IsA(UNiagaraSystem::StaticClass()))
			{
				UNiagaraSystem* NewSpawnEffect = Cast<UNiagaraSystem>(SpawnEffect);
				if(NewSpawnEffect != nullptr)
				{
					UPRNiagaraEffect* NewNiagaraEffect = CreateNiagaraEffect(NewSpawnEffect);
					if(NewNiagaraEffect != nullptr)
					{
						// Lifespan과 PoolIndex를 설정합니다.
						NewNiagaraEffect->SetLifespan(NewEffects.Value.Effects[0]->GetLifeSpan());
						NewNiagaraEffect->SetPoolIndex(NewEffects.Value.Effects.Num());
						
						// 새로 생성한 Effect를 지정한 위치에 Spawn하고 활성화합니다.
						NewNiagaraEffect->SpawnEffectAtLocation(Location, Rotation, Scale, bEffectAutoActivate);
						// 새로 생성한 Effect를 Pool에 넣습니다.
						NewEffects.Value.Effects.Emplace(NewNiagaraEffect);
						ActivatePoolIndexes.Find(SpawnEffect)->ActivateIndexes.Emplace(NewNiagaraEffect->GetPoolIndex());

						return NewNiagaraEffect;
					}
				}
			}
			else if(SpawnEffect->IsA(UParticleSystem::StaticClass()))
			{
				UParticleSystem* NewSpawnEffect = Cast<UParticleSystem>(SpawnEffect);
				if(NewSpawnEffect != nullptr)
				{
					UPRParticleEffect* NewParticleEffect = CreateParticleEffect(NewSpawnEffect);
					if(NewParticleEffect != nullptr)
					{
						// Lifespan과 PoolIndex를 설정합니다.
						NewParticleEffect->SetLifespan(NewEffects.Value.Effects[0]->GetLifeSpan());
						NewParticleEffect->SetPoolIndex(NewEffects.Value.Effects.Num());
						
						// 새로 생성한 Effect를 지정한 위치에 Spawn하고 활성화합니다.
						NewParticleEffect->SpawnEffectAtLocation(Location, Rotation, Scale, bEffectAutoActivate);
						// 새로 생성한 Effect를 Pool에 넣습니다.
						NewEffects.Value.Effects.Emplace(NewParticleEffect);
						ActivatePoolIndexes.Find(SpawnEffect)->ActivateIndexes.Emplace(NewParticleEffect->GetPoolIndex());

						return NewParticleEffect;
					}
				}
			}
		}
	}

	return nullptr;
}

UPREffect* UPREffectSystemComponent::SpawnEffectAttached(UFXSystemAsset* SpawnEffect, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, FVector Scale, EAttachLocation::Type LocationType, bool bEffectAutoActivate)
{
	// EffectPool에서 실행할 Effect를 탐색합니다.
	for(auto& NewEffects : EffectPool)
	{
		if(NewEffects.Key == SpawnEffect)
		{
			for(auto& ActivateableEffect : NewEffects.Value.Effects)
			{
				// 활성화되지 않은 Effect일 경우 지정한 위치에 Effect를 Spawn하고 활성화하고 반환합니다.
				if(IsActivateEffect(ActivateableEffect) == false)
				{
					ActivateableEffect->SpawnEffectAttached(AttachToComponent, AttachPointName, Location, Rotation, Scale, LocationType, bEffectAutoActivate);
					ActivatePoolIndexes.Find(SpawnEffect)->ActivateIndexes.Emplace(ActivateableEffect->GetPoolIndex());
					
					return ActivateableEffect;
				}
			}

			// 풀의 모든 Effect가 활성화되었을 경우 새로운 Effect를 생성하여 풀에 넣고 활성화하고 반환합니다.
			// Effect가 Niagara인지 Particle인지 판별하여 Effect를 생성합니다.
			if(SpawnEffect->IsA(UNiagaraSystem::StaticClass()))
			{
				UNiagaraSystem* NewSpawnEffect = Cast<UNiagaraSystem>(SpawnEffect);
				if(NewSpawnEffect != nullptr)
				{
					UPRNiagaraEffect* NewNiagaraEffect = CreateNiagaraEffect(NewSpawnEffect);
					if(NewNiagaraEffect != nullptr)
					{
						// Lifespan과 PoolIndex를 설정합니다.
						NewNiagaraEffect->SetLifespan(NewEffects.Value.Effects[0]->GetLifeSpan());
						NewNiagaraEffect->SetPoolIndex(NewEffects.Value.Effects.Num());
						
						// 새로 생성한 Effect를 지정한 위치에 Spawn하고 활성화합니다.
						NewNiagaraEffect->SpawnEffectAttached(AttachToComponent, AttachPointName, Location, Rotation, Scale, LocationType, bEffectAutoActivate);
						// 새로 생성한 Effect를 Pool에 넣습니다.
						NewEffects.Value.Effects.Emplace(NewNiagaraEffect);
						ActivatePoolIndexes.Find(SpawnEffect)->ActivateIndexes.Emplace(NewNiagaraEffect->GetPoolIndex());

						return NewNiagaraEffect;
					}
				}
			}
			else if(SpawnEffect->IsA(UParticleSystem::StaticClass()))
			{
				UParticleSystem* NewSpawnEffect = Cast<UParticleSystem>(SpawnEffect);
				if(NewSpawnEffect != nullptr)
				{
					UPRParticleEffect* NewParticleEffect = CreateParticleEffect(NewSpawnEffect);
					if(NewParticleEffect != nullptr)
					{
						// Lifespan과 PoolIndex를 설정합니다.
						NewParticleEffect->SetLifespan(NewEffects.Value.Effects[0]->GetLifeSpan());
						NewParticleEffect->SetPoolIndex(NewEffects.Value.Effects.Num());
						
						// 새로 생성한 Effect를 지정한 위치에 Spawn하고 활성화합니다.
						NewParticleEffect->SpawnEffectAttached(AttachToComponent, AttachPointName, Location, Rotation, Scale, LocationType, bEffectAutoActivate);
						// 새로 생성한 Effect를 Pool에 넣습니다.
						NewEffects.Value.Effects.Emplace(NewParticleEffect);
						ActivatePoolIndexes.Find(SpawnEffect)->ActivateIndexes.Emplace(NewParticleEffect->GetPoolIndex());

						return NewParticleEffect;
					}
				}
			}
		}
	}

	return nullptr;
}

bool UPREffectSystemComponent::IsActivateEffect(UPREffect* Effect)
{
	if(Effect != nullptr)
	{
		for(const auto& ActivatePoolIndex : ActivatePoolIndexes)
		{
			if(Effect->GetEffectSourceAsset() == ActivatePoolIndex.Key)
			{
				for(const auto& PoolIndex : ActivatePoolIndex.Value.ActivateIndexes)
				{
					if(Effect->GetPoolIndex() == PoolIndex)
					{
						return true;
					}
				}

				return false;
			}
		}
	}

	return false;
}

UPRNiagaraEffect* UPREffectSystemComponent::CreateNiagaraEffect(UNiagaraSystem* NiagaraSystem)
{
	UPRNiagaraEffect* NiagaraEffect = NewObject<UPRNiagaraEffect>();
	UNiagaraComponent* SpawnNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystem, FVector::ZeroVector, FRotator::ZeroRotator, FVector(1), false, false);
	if(NiagaraEffect != nullptr && SpawnNiagaraComponent != nullptr)
	{
		NiagaraEffect->Initialize();
		NiagaraEffect->SetNiagaraEffect(SpawnNiagaraComponent);
		NiagaraEffect->SetEffectSourceAsset(NiagaraSystem);
		NiagaraEffect->SetEffectOwner(GetOwner());
		NiagaraEffect->OnEffectDeactivate.AddDynamic(this, &UPREffectSystemComponent::OnEffectDeactivate);

		return NiagaraEffect;
	}

	return nullptr;
}

UPRParticleEffect* UPREffectSystemComponent::CreateParticleEffect(UParticleSystem* ParticleSystem)
{
	UPRParticleEffect* ParticleEffect = NewObject<UPRParticleEffect>();
	UParticleSystemComponent* SpawnParticleSystemComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystem, FVector::ZeroVector, FRotator::ZeroRotator, FVector(1), false,  EPSCPoolMethod::None, false);
	if(ParticleEffect != nullptr && SpawnParticleSystemComponent != nullptr)
	{
		ParticleEffect->Initialize();
		ParticleEffect->SetParticleEffect(SpawnParticleSystemComponent);
		ParticleEffect->SetEffectSourceAsset(ParticleSystem);
		ParticleEffect->SetEffectOwner(GetOwner());
		ParticleEffect->OnEffectDeactivate.AddDynamic(this, &UPREffectSystemComponent::OnEffectDeactivate);

		return ParticleEffect;
	}
	
	return nullptr;
}

FPREffectPool UPREffectSystemComponent::CreateNiagaraEffectPool(FPRNiagaraEffectInfo NiagaraEffectInfo)
{
	FPREffectPool NiagaraEffectPool;
	for(int32 Index = 0; Index < NiagaraEffectInfo.PoolSize; Index++)
	{
		UPRNiagaraEffect* NewNiagaraEffect = CreateNiagaraEffect(NiagaraEffectInfo.NiagaraSystem);
		if(NewNiagaraEffect != nullptr && NewNiagaraEffect->GetNiagaraEffect() != nullptr)
		{
			NewNiagaraEffect->SetLifespan(NiagaraEffectInfo.Lifespan);
			NewNiagaraEffect->SetPoolIndex(Index);
			NiagaraEffectPool.Effects.Emplace(NewNiagaraEffect);
		}
	}

	return NiagaraEffectPool;
}

FPREffectPool UPREffectSystemComponent::CreateParticleEffectPool(FPRParticleEffectInfo ParticleEffectInfo)
{
	FPREffectPool ParticleEffectPool;
	for(int32 Index = 0; Index < ParticleEffectInfo.PoolSize; Index++)
	{
		UPRParticleEffect* NewParticleEffect = CreateParticleEffect(ParticleEffectInfo.ParticleSystem);
		if(NewParticleEffect != nullptr && NewParticleEffect->GetParticleEffect() != nullptr)
		{
			NewParticleEffect->SetLifespan(ParticleEffectInfo.Lifespan);
			NewParticleEffect->SetPoolIndex(Index);
			ParticleEffectPool.Effects.Emplace(NewParticleEffect);
		}
	}
	
	return ParticleEffectPool;
}

void UPREffectSystemComponent::OnEffectDeactivate(UPREffect* DeactivateEffect)
{
	for(auto& ActivatePoolIndex : ActivatePoolIndexes)
	{
		if(ActivatePoolIndex.Key == DeactivateEffect->GetEffectSourceAsset())
		{
			ActivatePoolIndex.Value.ActivateIndexes.Remove(DeactivateEffect->GetPoolIndex());
			return;
		}
	}
}
