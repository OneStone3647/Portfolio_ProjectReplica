// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PREffectSystemComponent.h"
#include "Effect/PREffect.h"
#include "Effect/PRNiagaraEffect.h"
#include "Effect/PRParticleEffect.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/PRAICharacter.h"
#include "Characters/PRAICharacter.h"
#include "Components/PRTimeStopSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

UPREffectSystemComponent::UPREffectSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// EffectPool.Empty();
	// ActivatePoolIndexes.Empty();
	NiagaraEffectPool.Empty();
	ActivateNiagaraEffectIndexes.Empty();
	ParticleEffectPool.Empty();
	ActivateParticleEffectIndexes.Empty();
	// bIgnoreTimeStop = false;
	bTimeStopActive = false;
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

	// TimeStop을 무시할 때 Effect의 TimeDilation을 최신화합니다.
	// if(bIgnoreTimeStop)
	// {
	// 	if(bActivateTimeStop)
	// 	{
	// 		UpdateEffectTimeDilation(DeltaTime);
	// 	}
	// 	else
	// 	{
	// 		UpdateEffectTimeDilation(GetPROwner()->GetWorld()->DeltaTimeSeconds * 0.1f);
	// 	}
	// }

	// TimeStop의 영향을 받았을 때 Effect의 TimeDilation을 최신화합니다.
	if(bTimeStopActive)
	{
		UpdateEffectTimeDilation(GetPROwner()->CustomTimeDilation);
	}
}

void UPREffectSystemComponent::DestroyComponent(bool bPromoteChildren)
{
	Initialize();
	
	Super::DestroyComponent(bPromoteChildren);
}

void UPREffectSystemComponent::Initialize()
{
	// NiagaraEffectPool의 모든 Effect들을 제거합니다.
	for(const auto& NewEffects : NiagaraEffectPool)
	{
		for(const auto& Effect : NewEffects.Value.Effects)
		{
			if(IsValid(Effect) == true)
			{
				Effect->Destroy();
			}
		}
	}

	// NiagaraEffectPool과 ActivateNiagaraEffectIndexes를 초기화합니다.
	NiagaraEffectPool.Empty();
	ActivateNiagaraEffectIndexes.Empty();

	// ParticleEffectPool의 모든 Effect들을 제거합니다.
	for(const auto& NewEffects : ParticleEffectPool)
	{
		for(const auto& Effect : NewEffects.Value.Effects)
		{
			if(IsValid(Effect) == true)
			{
				Effect->Destroy();
			}
		}
	}

	// ParticleEffectPool과 ActivateParticleEffectIndexes를 초기화합니다.
	ParticleEffectPool.Empty();
	ActivateParticleEffectIndexes.Empty();

	// // EffectPool의 모든 Effect들을 제거합니다.
	// for(const auto& NewEffects : EffectPool)
	// {
	// 	for(const auto& Effect : NewEffects.Value.Effects)
	// 	{
	// 		if(IsValid(Effect) == true)
	// 		{
	// 			Effect->Destroy();
	// 		}
	// 	}
	// }
	//
	// // EffectPool과 ActivatePoolIndexes를 초기화합니다.
	// EffectPool.Empty();
	// ActivatePoolIndexes.Empty();
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
				// FPRNiagaraEffectPool NewNiagaraEffectPool = CreateNiagaraEffectPool(*DataTableNiagaraEffectInfo);
				// // EffectPool.Emplace(DataTableNiagaraEffectInfo->NiagaraSystem, NewNiagaraEffectPool);
				// NiagaraEffectPool.Emplace(DataTableNiagaraEffectInfo->NiagaraSystem, NewNiagaraEffectPool);
				//
				// FPRActivateIndex NewActivateIndexes;
				// // ActivatePoolIndexes.Emplace(DataTableNiagaraEffectInfo->NiagaraSystem, NewActivateIndexes);
				// ActivateNiagaraEffectIndexes.Emplace(DataTableNiagaraEffectInfo->NiagaraSystem, NewActivateIndexes);

				CreateNiagaraEffectPool(*DataTableNiagaraEffectInfo);
			}
		}
	}

	// ParticleEffectPool 생성
	if(ParticleEffectDataTable != nullptr)
	{
		TArray<FName> RowNames = ParticleEffectDataTable->GetRowNames();
		for(const auto& RowName : RowNames)
		{
			FPRParticleEffectInfo* DataTableParticleEffectInfo = ParticleEffectDataTable->FindRow<FPRParticleEffectInfo>(RowName, FString(""));
			if(DataTableParticleEffectInfo != nullptr && DataTableParticleEffectInfo->ParticleSystem != nullptr)
			{
				// FPRParticleEffectPool NewParticleEffectPool = CreateParticleEffectPool(*DataTableParticleEffectInfo);
				// // EffectPool.Emplace(DataTableParticleEffectInfo->ParticleSystem, NewParticleEffectPool);
				// ParticleEffectPool.Emplace(DataTableParticleEffectInfo->ParticleSystem, NewParticleEffectPool);
				//
				// FPRActivateIndex NewActivateIndexes;
				// // ActivatePoolIndexes.Emplace(DataTableParticleEffectInfo->ParticleSystem, NewActivateIndexes);
				// ActivateParticleEffectIndexes.Emplace(DataTableParticleEffectInfo->ParticleSystem, NewActivateIndexes);

				CreateParticleEffectPool(*DataTableParticleEffectInfo);
			}
		}
	}
}

UPRNiagaraEffect* UPREffectSystemComponent::SpawnNiagaraEffectAtLocation(UNiagaraSystem* SpawnEffect, FVector Location,	FRotator Rotation, FVector Scale, bool bEffectAutoActivate)
{
	// NiagaraEffectPool에서 실행할 Effect를 탐색합니다.
	for(auto& NewEffects : NiagaraEffectPool)
	{
		if(NewEffects.Key == SpawnEffect)
		{
			for(auto& ActivateableEffect : NewEffects.Value.Effects)
			{
				// 활성화되지 않은 Effect일 경우 지정한 위치에 Effect를 Spawn하고 활성화하고 반환합니다.
				if(IsActivateEffect(ActivateableEffect) == false)
				{
					ActivateableEffect->SpawnEffectAtLocation(Location, Rotation, Scale, bEffectAutoActivate);
					ActivateNiagaraEffectIndexes.Find(SpawnEffect)->ActivateIndexes.Emplace(ActivateableEffect->GetPoolIndex());
					
					return ActivateableEffect;
				}
			}

			// 풀의 모든 Effect가 활성화되었을 경우 새로운 Effect를 생성하여 풀에 넣고 활성화하고 반환합니다.
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
					ActivateNiagaraEffectIndexes.Find(SpawnEffect)->ActivateIndexes.Emplace(NewNiagaraEffect->GetPoolIndex());

					return NewNiagaraEffect;
				}
			}
		}
	}

	return nullptr;
}

UPRNiagaraEffect* UPREffectSystemComponent::SpawnNiagaraEffectAttached(UNiagaraSystem* SpawnEffect,	USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, FVector Scale, EAttachLocation::Type LocationType, bool bEffectAutoActivate)
{
	// NiagaraEffectPool에서 실행할 Effect를 탐색합니다.
	for(auto& NewEffects : NiagaraEffectPool)
	{
		if(NewEffects.Key == SpawnEffect)
		{
			for(auto& ActivateableEffect : NewEffects.Value.Effects)
			{
				// 활성화되지 않은 Effect일 경우 지정한 위치에 Effect를 Spawn하고 활성화하고 반환합니다.
				if(IsActivateEffect(ActivateableEffect) == false)
				{
					ActivateableEffect->SpawnEffectAttached(AttachToComponent, AttachPointName, Location, Rotation, Scale, LocationType, bEffectAutoActivate);
					ActivateNiagaraEffectIndexes.Find(SpawnEffect)->ActivateIndexes.Emplace(ActivateableEffect->GetPoolIndex());

					return ActivateableEffect;
				}
			}

			// 풀의 모든 Effect가 활성화되었을 경우 새로운 Effect를 생성하여 풀에 넣고 활성화하고 반환합니다.
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
					ActivateNiagaraEffectIndexes.Find(SpawnEffect)->ActivateIndexes.Emplace(NewNiagaraEffect->GetPoolIndex());

					return NewNiagaraEffect;
				}
			}
		}
	}

	return nullptr;
}

UPRParticleEffect* UPREffectSystemComponent::SpawnParticleEffectAtLocation(UParticleSystem* SpawnEffect, FVector Location, FRotator Rotation, FVector Scale, bool bEffectAutoActivate)
{
	// ParticleEffectPool에서 실행할 Effect를 탐색합니다.
	for(auto& NewEffects : ParticleEffectPool)
	{
		if(NewEffects.Key == SpawnEffect)
		{
			for(auto& ActivateableEffect : NewEffects.Value.Effects)
			{
				// 활성화되지 않은 Effect일 경우 지정한 위치에 Effect를 Spawn하고 활성화하고 반환합니다.
				if(IsActivateEffect(ActivateableEffect) == false)
				{
					ActivateableEffect->SpawnEffectAtLocation(Location, Rotation, Scale, bEffectAutoActivate);
					ActivateParticleEffectIndexes.Find(SpawnEffect)->ActivateIndexes.Emplace(ActivateableEffect->GetPoolIndex());
					
					return ActivateableEffect;
				}
			}

			// 풀의 모든 Effect가 활성화되었을 경우 새로운 Effect를 생성하여 풀에 넣고 활성화하고 반환합니다.
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
					ActivateParticleEffectIndexes.Find(SpawnEffect)->ActivateIndexes.Emplace(NewParticleEffect->GetPoolIndex());

					return NewParticleEffect;
				}
			}
		}
	}
	
	return nullptr;
}

UPRParticleEffect* UPREffectSystemComponent::SpawnParticleEffectAttached(UParticleSystem* SpawnEffect, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, FVector Scale, EAttachLocation::Type LocationType, bool bEffectAutoActivate)
{
	// ParticleEffectPool에서 실행할 Effect를 탐색합니다.
	for(auto& NewEffects : ParticleEffectPool)
	{
		if(NewEffects.Key == SpawnEffect)
		{
			for(auto& ActivateableEffect : NewEffects.Value.Effects)
			{
				// 활성화되지 않은 Effect일 경우 지정한 위치에 Effect를 Spawn하고 활성화하고 반환합니다.
				if(IsActivateEffect(ActivateableEffect) == false)
				{
					ActivateableEffect->SpawnEffectAttached(AttachToComponent, AttachPointName, Location, Rotation, Scale, LocationType, bEffectAutoActivate);
					ActivateParticleEffectIndexes.Find(SpawnEffect)->ActivateIndexes.Emplace(ActivateableEffect->GetPoolIndex());
					
					return ActivateableEffect;
				}
			}

			// 풀의 모든 Effect가 활성화되었을 경우 새로운 Effect를 생성하여 풀에 넣고 활성화하고 반환합니다.
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
					ActivateParticleEffectIndexes.Find(SpawnEffect)->ActivateIndexes.Emplace(NewParticleEffect->GetPoolIndex());

					return NewParticleEffect;
				}
			}
		}
	}
	
	return nullptr;
}

bool UPREffectSystemComponent::IsActivateEffect(UPREffect* Effect) const
{
	if(Effect != nullptr)
	{
		// Effect가 NiagaraEffect인지 ParticleEffect인지 구분하여 탐색합니다.
		
		// Effect가 NiagaraEffect인 경우
		const UPRNiagaraEffect* NiagaraEffect = Cast<UPRNiagaraEffect>(Effect);
		if(NiagaraEffect != nullptr)
		{
			for(const auto& ActivatePoolIndex : ActivateNiagaraEffectIndexes)
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

		// Effect가 ParticleEffect인 경우
		const UPRParticleEffect* ParticleEffect = Cast<UPRParticleEffect>(Effect);
		if(ParticleEffect != nullptr)
		{
			for(const auto& ActivatePoolIndex : ActivateParticleEffectIndexes)
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
	}

	return false;
}

bool UPREffectSystemComponent::IsValidEffectPool(UFXSystemAsset* FXSystemAsset) const
{
	if(FXSystemAsset != nullptr)
	{
		// Find함수가 nullptr을 반환하지 않는다면 EffectPool에 FXSystemAsset을 Key로 가진 Pool이 존재하는 것입니다.
		// FXSystemAsset이 NiagaraSystem인지 ParticleSystem인지 구분하여 실행합니다.

		// FXSystemAsset이 NiagaraSystem인 경우
		UNiagaraSystem* NewNiagaraSystem = Cast<UNiagaraSystem>(FXSystemAsset);
		bool bIsValidNiagaraEffectPool = IsValidNiagaraEffectPool(NewNiagaraSystem);

		// FXSystemAsset이 ParticleSystem인 경우
		UParticleSystem* NewParticleSystem = Cast<UParticleSystem>(FXSystemAsset);
		bool bIsValidParticleEffectPool = IsValidParticleEffectPool(NewParticleSystem);

		return bIsValidNiagaraEffectPool || bIsValidParticleEffectPool;
	}

	return false;
}

bool UPREffectSystemComponent::IsValidNiagaraEffectPool(UNiagaraSystem* NewNiagaraSystem) const
{
	if(NewNiagaraSystem != nullptr)
	{
		if(NiagaraEffectPool.Find(NewNiagaraSystem) != nullptr)
		{
			return true;
		}
	}
	
	return false;
}

bool UPREffectSystemComponent::IsValidParticleEffectPool(UParticleSystem* NewParticleSystem) const
{
	if(NewParticleSystem != nullptr)
	{
		if(ParticleEffectPool.Find(NewParticleSystem) != nullptr)
		{
			return true;
		}
	}
	
	return false;
}

UPRNiagaraEffect* UPREffectSystemComponent::CreateNiagaraEffect(UNiagaraSystem* NiagaraSystem)
{
	if(NiagaraSystem != nullptr && IsValid(GetPROwner()) == true && GetPROwner()->GetMesh() != nullptr)
	{
		UPRNiagaraEffect* NiagaraEffect = NewObject<UPRNiagaraEffect>();
		// SpawnSystemLocation을 하게 되면 후에 AttachToComponent를 할 경우 Effect가 부착은 되지만 부착한 Component를 따라가지 않습니다.
		// UNiagaraComponent* SpawnNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystem, FVector::ZeroVector, FRotator::ZeroRotator, FVector(1), false, false);
		UNiagaraComponent* SpawnNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem, GetPROwner()->GetMesh(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, false);
		if(NiagaraEffect != nullptr && SpawnNiagaraComponent != nullptr)
		{
			SpawnNiagaraComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
			// 이펙트를 사용하는 캐릭터의 CustomDepth 값을 가져와 설정합니다.
			// 이펙트는 반투명 물질이라 지금 사용하는 Post Process에서 RenderCustomDepth가 적용되지 않음
			// if(GetPROwner()->GetMesh()->bRenderCustomDepth == true)
			// {
			// 	SpawnNiagaraComponent->SetRenderCustomDepth(true);
			// 	SpawnNiagaraComponent->SetCustomDepthStencilValue(GetPROwner()->GetMesh()->CustomDepthStencilValue);
			// }
			NiagaraEffect->Initialize();
			NiagaraEffect->SetNiagaraEffect(SpawnNiagaraComponent);
			NiagaraEffect->SetEffectSourceAsset(NiagaraSystem);
			NiagaraEffect->SetEffectOwner(GetOwner());
			NiagaraEffect->OnEffectDeactivate.AddDynamic(this, &UPREffectSystemComponent::OnEffectDeactivate);

			return NiagaraEffect;
		}
	}

	return nullptr;
}

UPRParticleEffect* UPREffectSystemComponent::CreateParticleEffect(UParticleSystem* ParticleSystem)
{
	if(ParticleSystem != nullptr && IsValid(GetPROwner()) == true && GetPROwner()->GetMesh() != nullptr)
	{
		UPRParticleEffect* ParticleEffect = NewObject<UPRParticleEffect>();
		// SpawnEmitterAtLocation을 하게 되면 후에 AttachToComponent를 할 경우 Effect가 부착은 되지만 부착한 Component를 따라가지 않습니다.
		// UParticleSystemComponent* SpawnParticleSystemComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystem, FVector::ZeroVector, FRotator::ZeroRotator, FVector(1), false,  EPSCPoolMethod::None, false);
		UParticleSystemComponent* SpawnParticleSystemComponent = UGameplayStatics::SpawnEmitterAttached(ParticleSystem, GetPROwner()->GetMesh(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, FVector(1), EAttachLocation::KeepRelativeOffset, false, EPSCPoolMethod::None, false);
		if(ParticleEffect != nullptr && SpawnParticleSystemComponent != nullptr)
		{
			SpawnParticleSystemComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
			// 이펙트를 사용하는 캐릭터의 CustomDepth 값을 가져와 설정합니다.
			// 이펙트는 반투명 물질이라 지금 사용하는 Post Process에서 RenderCustomDepth가 적용되지 않음
			// if(GetPROwner()->GetMesh()->bRenderCustomDepth == true)
			// {
			// 	SpawnParticleSystemComponent->SetRenderCustomDepth(true);
			// 	SpawnParticleSystemComponent->SetCustomDepthStencilValue(GetPROwner()->GetMesh()->CustomDepthStencilValue);
			// }
			ParticleEffect->Initialize();
			ParticleEffect->SetParticleEffect(SpawnParticleSystemComponent);
			ParticleEffect->SetEffectSourceAsset(ParticleSystem);
			ParticleEffect->SetEffectOwner(GetOwner());
			ParticleEffect->OnEffectDeactivate.AddDynamic(this, &UPREffectSystemComponent::OnEffectDeactivate);

			return ParticleEffect;
		}
	}
	
	return nullptr;
}

FPRNiagaraEffectPool UPREffectSystemComponent::CreateNiagaraEffectPool(FPRNiagaraEffectInfo NiagaraEffectInfo)
{
	// 새로운 NiagaraEffectPool 생성
	FPRNiagaraEffectPool NewNiagaraEffectPool;
	for(int32 Index = 0; Index < NiagaraEffectInfo.PoolSize; Index++)
	{
		UPRNiagaraEffect* NewNiagaraEffect = CreateNiagaraEffect(NiagaraEffectInfo.NiagaraSystem);
		if(NewNiagaraEffect != nullptr && NewNiagaraEffect->GetNiagaraEffect() != nullptr)
		{
			NewNiagaraEffect->SetLifespan(NiagaraEffectInfo.Lifespan);
			NewNiagaraEffect->SetPoolIndex(Index);
			NewNiagaraEffectPool.Effects.Emplace(NewNiagaraEffect);
		}
	}

	// NiagaraEffectPool에 추가
	NiagaraEffectPool.Emplace(NiagaraEffectInfo.NiagaraSystem, NewNiagaraEffectPool);
	
	// ActivateNiagaraEffectIndexes 생성
	FPRActivateIndex NewActivateIndexes;
	ActivateNiagaraEffectIndexes.Emplace(NiagaraEffectInfo.NiagaraSystem, NewActivateIndexes);

	return NewNiagaraEffectPool;
}

FPRParticleEffectPool UPREffectSystemComponent::CreateParticleEffectPool(FPRParticleEffectInfo ParticleEffectInfo)
{
	// 새로운 ParticleEffectPool 생성
	FPRParticleEffectPool NewParticleEffectPool;
	for(int32 Index = 0; Index < ParticleEffectInfo.PoolSize; Index++)
	{
		UPRParticleEffect* NewParticleEffect = CreateParticleEffect(ParticleEffectInfo.ParticleSystem);
		if(NewParticleEffect != nullptr && NewParticleEffect->GetParticleEffect() != nullptr)
		{
			NewParticleEffect->SetLifespan(ParticleEffectInfo.Lifespan);
			NewParticleEffect->SetPoolIndex(Index);
			NewParticleEffectPool.Effects.Emplace(NewParticleEffect);
		}
	}

	// ParticleEffectPool 추가
	ParticleEffectPool.Emplace(ParticleEffectInfo.ParticleSystem, NewParticleEffectPool);

	// ActivateParticleEffectIndexes 생성
	FPRActivateIndex NewActivateIndexes;
	ActivateParticleEffectIndexes.Emplace(ParticleEffectInfo.ParticleSystem, NewActivateIndexes);
	
	return NewParticleEffectPool;
}

void UPREffectSystemComponent::OnEffectDeactivate(UPREffect* DeactivateEffect)
{
	// DeactivateEffect가 UPRNiagaraEffect인지 UPRParticleEffect인지 구분하여 실행합니다.

	// DeactivateEffect가 UPRNiagaraEffect인 경우
	UPRNiagaraEffect* NiagaraEffect = Cast<UPRNiagaraEffect>(DeactivateEffect);
	if(NiagaraEffect != nullptr)
	{
		for(auto& ActivatePoolIndex : ActivateNiagaraEffectIndexes)
		{
			if(ActivatePoolIndex.Key == DeactivateEffect->GetEffectSourceAsset())
			{
				// 비활성화하는 Effect의 TimeDilation을 초기화합니다.
				if(IsValid(GetPROwner()) == true)
				{
					DeactivateEffect->UpdateEffect(GetPROwner()->GetWorld()->GetDeltaSeconds() * 0.1f);
				}
			
				ActivatePoolIndex.Value.ActivateIndexes.Remove(DeactivateEffect->GetPoolIndex());
			
				return;
			}
		}
	}

	// DeactivateEffect가 UPRParticleEffect인 경우
	UPRParticleEffect* ParticleEffect = Cast<UPRParticleEffect>(DeactivateEffect);
	if(ParticleEffect != nullptr)
	{
		for(auto& ActivatePoolIndex : ActivateParticleEffectIndexes)
		{
			if(ActivatePoolIndex.Key == DeactivateEffect->GetEffectSourceAsset())
			{
				// 비활성화하는 Effect의 TimeDilation을 초기화합니다.
				if(IsValid(GetPROwner()) == true)
				{
					DeactivateEffect->UpdateEffect(GetPROwner()->GetWorld()->GetDeltaSeconds() * 0.1f);
				}
			
				ActivatePoolIndex.Value.ActivateIndexes.Remove(DeactivateEffect->GetPoolIndex());
			
				return;
			}
		}	
	}
}

void UPREffectSystemComponent::UpdateEffectTimeDilation(float DeltaTime)
{
	// NiagaraEffect 최신화
	for(const auto& NiagaraEffects : NiagaraEffectPool)
	{
		for(const auto& NiagaraEffect : NiagaraEffects.Value.Effects)
		{
			if(IsValid(NiagaraEffect) == true)
			{
				NiagaraEffect->UpdateEffect(DeltaTime);
			}
		}
	}

	// ParticleEffect 최신화
	for(const auto& ParticleEffects : ParticleEffectPool)
	{
		for(const auto& ParticleEffect : ParticleEffects.Value.Effects)
		{
			if(IsValid(ParticleEffect) == true)
			{
				ParticleEffect->UpdateEffect(DeltaTime);
			}
		}
	}
	
	// for(const auto& Effects : EffectPool)
	// {
	// 	for(const auto& Effect : Effects.Value.Effects)
	// 	{
	// 		if(IsValid(Effect) == true)
	// 		{
	// 			Effect->UpdateEffect(DeltaTime);
	// 		}
	// 	}
	// }
}

void UPREffectSystemComponent::SetEffectsCustomDepth(bool bNewRenderCustomDepth, int32 NewCustomDepthStencilValue)
{
	if(IsValid(GetPROwner()) == true && GetPROwner()->GetMesh() != nullptr)
	{
		// NiagaraEffect CustomDepth 설정
		for(const auto& NiagaraEffects : NiagaraEffectPool)
		{
			for(const auto& NiagaraEffect : NiagaraEffects.Value.Effects)
			{
				NiagaraEffect->GetNiagaraEffect()->SetRenderCustomDepth(bNewRenderCustomDepth);
				NiagaraEffect->GetNiagaraEffect()->SetCustomDepthStencilValue(NewCustomDepthStencilValue);
			}
		}

		// ParticleEffect CustomDepth 설정
		for(const auto& ParticleEffects : ParticleEffectPool)
		{
			for(const auto& ParticleEffect : ParticleEffects.Value.Effects)
			{
				ParticleEffect->GetParticleEffect()->SetRenderCustomDepth(bNewRenderCustomDepth);
				ParticleEffect->GetParticleEffect()->SetCustomDepthStencilValue(NewCustomDepthStencilValue);
			}
		}
	}
}

void UPREffectSystemComponent::DeactivateEffectPool(UFXSystemAsset* NewDeactivateEffect)
{
	// NiagaraEffect일 경우
	UNiagaraSystem* NewDeactivateNiagaraEffect = Cast<UNiagaraSystem>(NewDeactivateEffect);
	if(NewDeactivateNiagaraEffect != nullptr)
	{
		DeactivateNiagaraEffectPool(NewDeactivateNiagaraEffect);
		
		return;
	}

	// ParticleEffect일 경우
	UParticleSystem* NewDeactivateParticleEffect = Cast<UParticleSystem>(NewDeactivateEffect);
	if(NewDeactivateParticleEffect != nullptr)
	{
		DeactivateParticleEffectPool(NewDeactivateParticleEffect);

		return;
	}
}

void UPREffectSystemComponent::DeactivateNiagaraEffectPool(UNiagaraSystem* NewDeactivateNiagaraEffect)
{
	if(IsValidNiagaraEffectPool(NewDeactivateNiagaraEffect) == true)
	{
		FPRNiagaraEffectPool* NewDeactivateNiagaraEffectPool = NiagaraEffectPool.Find(NewDeactivateNiagaraEffect);
		for(auto& NewNiagaraEffect : NewDeactivateNiagaraEffectPool->Effects)
		{
			NewNiagaraEffect->Deactivate();
		}
	}
}

void UPREffectSystemComponent::DeactivateParticleEffectPool(UParticleSystem* NewDeactivateParticleEffect)
{
	if(IsValidParticleEffectPool(NewDeactivateParticleEffect) == true)
	{
		FPRParticleEffectPool* NewDeactivateParticleEffectPool = ParticleEffectPool.Find(NewDeactivateParticleEffect);
		for(auto& NewParticleEffect : NewDeactivateParticleEffectPool->Effects)
		{
			NewParticleEffect->Deactivate();
		}
	}
}

bool UPREffectSystemComponent::IsTimeStopActive() const
{
	return bTimeStopActive;
}

void UPREffectSystemComponent::TimeStopActive()
{
	bTimeStopActive = true;
}

void UPREffectSystemComponent::TimeStopDeactive()
{
	bTimeStopActive = false;
}

// void UPREffectSystemComponent::SetIgnoreTimeStop(bool bNewIgnoreTimeStop)
// {
// 	bIgnoreTimeStop = bNewIgnoreTimeStop;
// }

// void UPREffectSystemComponent::SetActivateTimeStop(bool bNewActivateTimeStop)
// {
// 	bActivateTimeStop = bNewActivateTimeStop;
// }
