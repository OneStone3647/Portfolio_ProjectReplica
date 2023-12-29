// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PREffectSystemComponent.h"
#include "Effect/PREffect.h"
#include "Effect/PRNiagaraEffect.h"
#include "Effect/PRParticleEffect.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/PRAICharacter.h"
#include "Components/PRTimeStopSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

UPREffectSystemComponent::UPREffectSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// 초기에는 TimeStop에 영향을 받지 않습니다.
	bTimeStopActive = false;

	// 동적으로 생성한 오브젝트를 제거하는 딜레이 시간을 초기화합니다.
	DynamicDestroyDelay = 60.0f;

	// NiagaraEffect
	NiagaraEffectSettingsDataTable = nullptr;
	NiagaraEffectPool.Empty();
	ActivateNiagaraEffectIndexList.Empty();
	UsedNiagaraEffectIndexList.Empty();
	DynamicDestroyNiagaraEffectList.Empty();
	
	// ParticleEffect
	ParticleEffectSettingsDataTable = nullptr;
	ParticleEffectPool.Empty();
	ActivateParticleEffectIndexList.Empty();
	UsedParticleEffectIndexList.Empty();
	DynamicDestroyParticleEffectList.Empty();
}

void UPREffectSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// TimeStop의 영향을 받았을 때 NiagaraEffect의 TimeDilation을 최신화합니다.
	if(bTimeStopActive)
	{
		// UpdateEffectTimeDilation(GetPROwner()->CustomTimeDilation);
		UpdateNiagaraEffectTimeDilation(GetPROwner()->CustomTimeDilation);
	}
}

void UPREffectSystemComponent::DestroyComponent(bool bPromoteChildren)
{
	// 모든 EffectPool을 제거합니다.
	DestroyAllEffectPool();
	
	Super::DestroyComponent(bPromoteChildren);
}

void UPREffectSystemComponent::InitializeEffectPool()
{
	// NiagaraEffectPool 초기화
	NiagaraEffectPool.Empty();
	ActivateNiagaraEffectIndexList.Empty();
	UsedNiagaraEffectIndexList.Empty();
	DynamicDestroyNiagaraEffectList.Empty();

	// 데이터 테이블을 기반으로 NiagaraEffectPool을 생성합니다.
	if(NiagaraEffectSettingsDataTable != nullptr)
	{
		TArray<FName> RowNames = NiagaraEffectSettingsDataTable->GetRowNames();
		for(const FName& RowName : RowNames)
		{
			FPRNiagaraEffectSettings* NiagaraEffectSettings = NiagaraEffectSettingsDataTable->FindRow<FPRNiagaraEffectSettings>(RowName, FString(""));
			if(NiagaraEffectSettings != nullptr)
			{
				CreateNiagaraEffectPool(*NiagaraEffectSettings);
			}
		}
	}

	// ParticleEffectPool 초기화
	ParticleEffectPool.Empty();
	ActivateParticleEffectIndexList.Empty();
	UsedParticleEffectIndexList.Empty();
	DynamicDestroyParticleEffectList.Empty();
	
	// 데이터 테이블을 기반으로 ParticleEffectPool을 생성합니다.
	if(ParticleEffectSettingsDataTable != nullptr)
	{
		TArray<FName> RowNames = ParticleEffectSettingsDataTable->GetRowNames();
		for(const FName& RowName : RowNames)
		{
			FPRParticleEffectSettings* ParticleEffectSettings = ParticleEffectSettingsDataTable->FindRow<FPRParticleEffectSettings>(RowName, FString(""));
			if(ParticleEffectSettings != nullptr)
			{
				CreateParticleEffectPool(*ParticleEffectSettings);
			}
		}
	}
}

void UPREffectSystemComponent::DestroyAllEffectPool()
{
	// NiagaraEffectPool을 제거합니다.
	if(NiagaraEffectPool.Num() > 0)
	{
		for(auto& Pair : NiagaraEffectPool)
		{
			for(auto& NiagaraEffect: Pair.Value.Effects)
			{
				NiagaraEffect->Destroy();
				Pair.Value.Effects.Remove(NiagaraEffect);
			}
		}
	}

	// NiagaraEffectPool 초기화
	NiagaraEffectPool.Empty();
	ActivateNiagaraEffectIndexList.Empty();
	UsedNiagaraEffectIndexList.Empty();
	DynamicDestroyNiagaraEffectList.Empty();

	// ParticleEffectPool을 제거합니다.
	if(ParticleEffectPool.Num() > 0)
	{
		for(auto& Pair : ParticleEffectPool)
		{
			for(auto& ParticleEffect: Pair.Value.Effects)
			{
				ParticleEffect->Destroy();
				Pair.Value.Effects.Remove(ParticleEffect);
			}
		}

	}

	// ParticleEffectPool 초기화
	ParticleEffectPool.Empty();
	ActivateParticleEffectIndexList.Empty();
	UsedParticleEffectIndexList.Empty();
	DynamicDestroyParticleEffectList.Empty();
}

bool UPREffectSystemComponent::IsTimeStopActive() const
{
	return bTimeStopActive;
}

void UPREffectSystemComponent::TimeStopActive()
{
	bTimeStopActive = true;

	// ParticleEffect의 CustomTimeDilation을 0.0f로 설정합니다.
	// NiagaraEffect의 경우 CustomTimeDilation이 없으므로 Tick함수에서 AdvanceSimulation 함수를 사용합니다.
	for(const auto& Pair : ParticleEffectPool)
	{
		for(const auto& ParticleEffect : Pair.Value.Effects)
		{
			ParticleEffect->SetParticleEffectCustomTimeDilation(0.0f);
		}
	}
}

void UPREffectSystemComponent::TimeStopDeactive()
{
	bTimeStopActive = false;
	
	// ParticleEffect의 CustomTimeDilation을 1.0f로 설정합니다.
	// NiagaraEffect의 경우 CustomTimeDilation이 없으므로 Tick함수에서 AdvanceSimulation 함수를 사용합니다.
	for(const auto& Pair : ParticleEffectPool)
	{
		for(const auto& ParticleEffect : Pair.Value.Effects)
		{
			ParticleEffect->SetParticleEffectCustomTimeDilation(1.0f);
		}
	}
}

int32 UPREffectSystemComponent::FindAvailableIndex(const TSet<int32>& UsedIndexes)
{
	int32 NewIndex = 0;
	while(UsedIndexes.Contains(NewIndex))
	{
		NewIndex++;
	}

	return NewIndex;
}

#pragma region NiagaraEffect
UPRNiagaraEffect* UPREffectSystemComponent::SpawnNiagaraEffectAtLocation(UNiagaraSystem* SpawnEffect, FVector Location,	FRotator Rotation, FVector Scale, bool bEffectAutoActivate)
{
	UPRNiagaraEffect* ActivateableNiagaraEffect = GetActivateableNiagaraEffect(SpawnEffect);
	if(ActivateableNiagaraEffect != nullptr)
	{
		// NiagaraEffect를 활성화하고 Spawn할 위치와 회전값을 적용합니다.
		ActivateableNiagaraEffect->SpawnEffectAtLocation(Location, Rotation, Scale, bEffectAutoActivate);

		// 해당 NiagaraEffect로 처음 활성화하는 경우 ActivateNiagaraEffectIndexList를 생성합니다.
		if(!IsCreateActivateNiagaraEffectIndexList(SpawnEffect))
		{
			CreateActivateNiagaraEffectIndexList(SpawnEffect);
		}

		// 활성화된 NiagaraEffect의 Index를 ActivateNiagaraEffectIndexList에 추가합니다.
		ActivateNiagaraEffectIndexList.Find(SpawnEffect)->Indexes.AddUnique(ActivateableNiagaraEffect->GetPoolIndex());
	}

	return ActivateableNiagaraEffect;
}

UPRNiagaraEffect* UPREffectSystemComponent::SpawnNiagaraEffectAttached(UNiagaraSystem* SpawnEffect,	USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, FVector Scale, EAttachLocation::Type LocationType, bool bEffectAutoActivate)
{
	UPRNiagaraEffect* ActivateableNiagaraEffect = GetActivateableNiagaraEffect(SpawnEffect);
	if(ActivateableNiagaraEffect != nullptr)
	{
		// NiagaraEffect를 활성화하고 Spawn할 위치와 회전값을 적용합니다.
		ActivateableNiagaraEffect->SpawnEffectAttached(AttachToComponent, AttachPointName, Location, Rotation, Scale, LocationType, bEffectAutoActivate);

		// 해당 NiagaraEffect로 처음 활성화하는 경우 ActivateNiagaraEffectIndexList를 생성합니다.
		if(!IsCreateActivateNiagaraEffectIndexList(SpawnEffect))
		{
			CreateActivateNiagaraEffectIndexList(SpawnEffect);
		}

		// 활성화된 NiagaraEffect의 Index를 ActivateNiagaraEffectIndexList에 추가합니다.
		ActivateNiagaraEffectIndexList.Find(SpawnEffect)->Indexes.AddUnique(ActivateableNiagaraEffect->GetPoolIndex());
	}

	return ActivateableNiagaraEffect;
}

void UPREffectSystemComponent::DeactivateNiagaraEffectPool(UNiagaraSystem* NiagaraEffect)
{
	if(IsCreateNiagaraEffectPool(NiagaraEffect) == true)
	{
		// NiagaraEffect에 해당하는 NiagaraEffectPool의 모든 NiagaraEffect들을 비활성화합니다.
		for(auto& NewNiagaraEffect : NiagaraEffectPool.Find(NiagaraEffect)->Effects)
		{
			NewNiagaraEffect->Deactivate();
		}
	}
}

bool UPREffectSystemComponent::IsActivateNiagaraEffect(UPRNiagaraEffect* NiagaraEffect) const
{
	if(IsValid(NiagaraEffect) && NiagaraEffect->GetNiagaraEffect() != nullptr)
	{
		// ActivateIndex에서 NiagaraEffect의 Index가 있는지 탐색합니다.
		for(const auto& ActivateIndexList : ActivateNiagaraEffectIndexList)
		{
			if(NiagaraEffect->GetNiagaraEffectAsset() == ActivateIndexList.Key)
			{
				for(const auto& Index : ActivateIndexList.Value.Indexes)
				{
					if(NiagaraEffect->GetPoolIndex() == Index)
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

bool UPREffectSystemComponent::IsCreateNiagaraEffectPool(UNiagaraSystem* NiagaraSystem) const
{
	return NiagaraEffectPool.Contains(NiagaraSystem);
}

bool UPREffectSystemComponent::IsCreateActivateNiagaraEffectIndexList(UNiagaraSystem* NiagaraSystem) const
{
	return ActivateNiagaraEffectIndexList.Contains(NiagaraSystem);
}

bool UPREffectSystemComponent::IsCreateUsedNiagaraEffectIndexList(UNiagaraSystem* NiagaraSystem) const
{
	return UsedNiagaraEffectIndexList.Contains(NiagaraSystem);
}

UPRNiagaraEffect* UPREffectSystemComponent::GetActivateableNiagaraEffect(UNiagaraSystem* NiagaraSystem)
{
	// 해당 NiagaraEffect에 해당하는 Pool이 생성되었는지 확인하고, 없으면 생성합니다.
	if(!IsCreateNiagaraEffectPool(NiagaraSystem))
	{
		FPRNiagaraEffectSettings NiagaraEffectPoolSettings = FPRNiagaraEffectSettings(NiagaraSystem, 1);
		CreateNiagaraEffectPool(NiagaraEffectPoolSettings);
	}

	// NiagaraEffectPool에서 해당 NiagaraEffect의 Pool을 얻습니다.
	FPRNiagaraEffectPool* Pair = NiagaraEffectPool.Find(NiagaraSystem);
	if(Pair == nullptr)
	{
		// 지정된 NiagaraEffect가 없습니다.
		return nullptr;
	}

	UPRNiagaraEffect* ActivateableNiagaraEffect = nullptr;

	// Pair에서 활성화 되지 않은 NiagaraEffect를 찾아 활성화합니다.
	for(const auto& NiagaraEffect : Pair->Effects)
	{
		if(IsValid(NiagaraEffect) && !IsActivateNiagaraEffect(NiagaraEffect))
		{
			ActivateableNiagaraEffect = NiagaraEffect;
			break;
		}
	}

	// Pair의 모든 NiagaraEffect가 활성화 되었을 경우 새로운 NiagaraEffect를 생성합니다.
	if(ActivateableNiagaraEffect == nullptr)
	{
		// Critical Section 시작
		FCriticalSection CriticalSection;
		CriticalSection.Lock();

		// 해당 NiagaraEffect의 UsedEffectIndexList가 생성되었는지 확인하고, 없으면 생성합니다.
		if(!IsCreateUsedNiagaraEffectIndexList(NiagaraSystem))
		{
			FPRUsedIndexList UsedEffectIndexList;
			for(const auto& NiagaraEffect : Pair->Effects)
			{
				UsedEffectIndexList.Indexes.Add(NiagaraEffect->GetPoolIndex());
			}

			UsedNiagaraEffectIndexList.Emplace(NiagaraSystem, UsedEffectIndexList);
		}

		// UsedNiagaraEffectIndexList에서 해당 NiagaraEffect가 사용중인 ObjectIndex 목록을 얻습니다.
		FPRUsedIndexList* UsedEffectIndexList = UsedNiagaraEffectIndexList.Find(NiagaraSystem);
		if(UsedEffectIndexList == nullptr)
		{
			// 지정된 NiagaraEffect가 없습니다.
			return nullptr;
		}

		// 사용 가능한 Index를 구합니다.
		const int32 NewIndex = FindAvailableIndex(UsedEffectIndexList->Indexes);

		// 사용 가능한 Index를 UsedEffectIndexList에 추가합니다.
		UsedEffectIndexList->Indexes.Add(NewIndex);

		// Critical Section 끝
		CriticalSection.Unlock();

		// 새로운 NiagaraEffect를 생성하고 초기화하며 사용 가능한 Index를 설정합니다.
		ActivateableNiagaraEffect = SpawnAndInitializeNiagaraEffect(NiagaraSystem, NewIndex);
		ActivateableNiagaraEffect->OnNiagaraEffectDeactivate.AddDynamic(this, &UPREffectSystemComponent::OnDynamicNiagaraEffectDeactivate);

		// 새로 생성한 NiagaraEffect를 Pair에 추가합니다.
		Pair->Effects.Emplace(ActivateableNiagaraEffect);
	}

	// DynamicNiagaraEffectTimer를 정지합니다.
	FPRDynamicDestroyNiagaraEffectList* DynamicDestroyPair = DynamicDestroyNiagaraEffectList.Find(ActivateableNiagaraEffect->GetNiagaraEffectAsset());
	if(DynamicDestroyPair != nullptr)
	{
		FTimerHandle* DynamicEffectDestroyTimer = DynamicDestroyPair->Effects.Find(ActivateableNiagaraEffect);
		if(DynamicEffectDestroyTimer != nullptr)
		{
			GetWorld()->GetTimerManager().ClearTimer(*DynamicEffectDestroyTimer);
		}
	}

	return ActivateableNiagaraEffect;
}

void UPREffectSystemComponent::UpdateNiagaraEffectTimeDilation(float TimeDilation)
{
	// NiagaraEffect의 TimeDilation 최신화
	for(const auto& NiagaraEffects : NiagaraEffectPool)
	{
		for(const auto& NiagaraEffect : NiagaraEffects.Value.Effects)
		{
			if(IsValid(NiagaraEffect) == true)
			{
				NiagaraEffect->UpdateTimeDilation(TimeDilation);
			}
		}
	}
}

UPRNiagaraEffect* UPREffectSystemComponent::SpawnNiagaraEffectInWorld(UNiagaraSystem* NiagaraSystem)
{
	if(GetWorld() != nullptr && NiagaraSystem != nullptr && IsValid(GetPROwner()))
	{
		UPRNiagaraEffect* NiagaraEffect = NewObject<UPRNiagaraEffect>();

		// SpawnSystemLocation 함수를 사용하게 되면, 후에 AttachToComponent 함수를 사용할 경우 Effect가 부착은 되지만 부착한 Component를 따라가지 않습니다.
		UNiagaraComponent* SpawnNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem, GetPROwner()->GetMesh(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator,
																								EAttachLocation::KeepRelativeOffset, false, false);
		if(SpawnNiagaraComponent != nullptr)
		{
			/** 다른 곳에 사용하기 위해서 Detach 합니다.*/
			SpawnNiagaraComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);

			NiagaraEffect->Initialize();
			NiagaraEffect->SetNiagaraEffect(SpawnNiagaraComponent);
			NiagaraEffect->SetEffectOwner(GetOwner());
			NiagaraEffect->OnNiagaraEffectDeactivate.AddDynamic(this, &UPREffectSystemComponent::OnNiagaraEffectDeactivate);

			return NiagaraEffect;
		}
	}
	
	return nullptr;
}

UPRNiagaraEffect* UPREffectSystemComponent::SpawnAndInitializeNiagaraEffect(UNiagaraSystem* NiagaraSystem, int32 Index)
{
	UPRNiagaraEffect* SpawnNiagaraEffect = SpawnNiagaraEffectInWorld(NiagaraSystem);
	if(SpawnNiagaraEffect != nullptr)
	{
		// 생성한 NiagaraEffect를 초기화하고 OnNiagaraEffectDeactivate를 바인딩합니다.
		SpawnNiagaraEffect->Initialize();
		SpawnNiagaraEffect->SetEffectOwner(GetOwner());
		SpawnNiagaraEffect->SetPoolIndex(Index);
		SpawnNiagaraEffect->OnNiagaraEffectDeactivate.AddDynamic(this, &UPREffectSystemComponent::UPREffectSystemComponent::OnNiagaraEffectDeactivate);
	}
	
	return SpawnNiagaraEffect;
}

void UPREffectSystemComponent::CreateNiagaraEffectPool(FPRNiagaraEffectSettings NiagaraEffectSettings)
{
	if(GetWorld() != nullptr && NiagaraEffectSettings.NiagaraSystem != nullptr)
	{
		// NiagaraEffectPool에 추가할 Pair를 초기화하고 NiagaraEffect를 생성하여 추가합니다.
		FPRNiagaraEffectPool Pair;
		for(int32 Index = 0; Index < NiagaraEffectSettings.PoolSize; Index++)
		{
			UPRNiagaraEffect* SpawnNiagaraEffect = SpawnAndInitializeNiagaraEffect(NiagaraEffectSettings.NiagaraSystem, Index);
			if(SpawnNiagaraEffect != nullptr)
			{
				// 초기화된 SpawnNiagaraEffect를 Pair에 추가합니다.
				Pair.Effects.Emplace(SpawnNiagaraEffect);
			}
		}

		// 초기화된 Pair를 NiagaraEffectPool에 추가하고 ActivateNiagaraEffectIndexList를 생성합니다.
		NiagaraEffectPool.Emplace(NiagaraEffectSettings.NiagaraSystem, Pair);
		IsCreateActivateNiagaraEffectIndexList(NiagaraEffectSettings.NiagaraSystem);
	}
}

void UPREffectSystemComponent::CreateActivateNiagaraEffectIndexList(UNiagaraSystem* NiagaraSystem)
{
	// NiagaraSystem의 ActivateNiagaraEffectIndexList를 생성합니다.
	ActivateNiagaraEffectIndexList.Emplace(NiagaraSystem);
}

void UPREffectSystemComponent::OnNiagaraEffectDeactivate(UPRNiagaraEffect* NiagaraEffect)
{
	if(IsActivateNiagaraEffect(NiagaraEffect))
	{
		// 비활성화된 NiagaraEffect의 Index를 ActivateIndex에서 제거합니다.
		ActivateNiagaraEffectIndexList.Find(NiagaraEffect->GetNiagaraEffectAsset())->Indexes.Remove(NiagaraEffect->GetPoolIndex());
	}
}

void UPREffectSystemComponent::OnDynamicNiagaraEffectDeactivate(UPRNiagaraEffect* NiagaraEffect)
{
	
	if(DynamicDestroyDelay > 0)
	{
		// 타이머로 딜레이 만큼 시간이지난 후 OnDynamicNiagaraEffectDestroy 함수를 실행합니다.
		FTimerHandle DynamicDestroyDelayTimerHandle;
		FTimerDelegate DynamicDestroyDelayDelegate = FTimerDelegate::CreateUObject(this, &UPREffectSystemComponent::OnDynamicNiagaraEffectDestroy, NiagaraEffect);
		GetWorld()->GetTimerManager().SetTimer(DynamicDestroyDelayTimerHandle, DynamicDestroyDelayDelegate, DynamicDestroyDelay, false);

		// 타이머를 DynamicDestroyNiagaraEffectList에 추가합니다.
		FPRDynamicDestroyNiagaraEffectList* DynamicDestroyPair = DynamicDestroyNiagaraEffectList.Find(NiagaraEffect->GetNiagaraEffectAsset());
		if(DynamicDestroyPair != nullptr)
		{
			DynamicDestroyPair->Effects.Emplace(NiagaraEffect, DynamicDestroyDelayTimerHandle);
		}
		else
		{
			FPRDynamicDestroyNiagaraEffectList EffectList;
			EffectList.Effects.Emplace(NiagaraEffect, DynamicDestroyDelayTimerHandle);
			DynamicDestroyNiagaraEffectList.Emplace(NiagaraEffect->GetNiagaraEffectAsset(), EffectList);
		}
	}
	else
	{
		// 딜레이가 없을 경우 타이머를 실행하지 않고 바로 OnDynamicNiagaraEffectDestroy 함수를 실행합니다.
		OnDynamicNiagaraEffectDestroy(NiagaraEffect);
	}
}

void UPREffectSystemComponent::OnDynamicNiagaraEffectDestroy(UPRNiagaraEffect* NiagaraEffect)
{
	// 사용 중인 NiagaraEffectIndexList를 가지고 있는지 확인합니다.
	if(IsCreateUsedNiagaraEffectIndexList(NiagaraEffect->GetNiagaraEffectAsset()))
	{
		// DynamicNiagaraEffectDestroyTimer를 제거합니다.
		FPRDynamicDestroyNiagaraEffectList* DynamicDestroyPair = DynamicDestroyNiagaraEffectList.Find(NiagaraEffect->GetNiagaraEffectAsset());
		if(DynamicDestroyPair != nullptr)
		{
			DynamicDestroyPair->Effects.Remove(NiagaraEffect);
		}

		// 해당 NiagaraEffect의 UsedIndexList를 얻습니다.
		FPRUsedIndexList* UsedIndexList = UsedNiagaraEffectIndexList.Find(NiagaraEffect->GetNiagaraEffectAsset());
		if(UsedIndexList != nullptr)
		{
			// 사용중인 Index를 제거합니다
			UsedIndexList->Indexes.Remove(NiagaraEffect->GetPoolIndex());
		}

		// 해당 NiagaraEffect의 Pool이 생성되었는지 확인합니다.
		if(IsCreateNiagaraEffectPool(NiagaraEffect->GetNiagaraEffectAsset()))
		{
			// NiagaraEffectPool에서 해당 NiagaraEffect를 제거하고 파괴합니다.
			NiagaraEffectPool.Find(NiagaraEffect->GetNiagaraEffectAsset())->Effects.Remove(NiagaraEffect);
			NiagaraEffect->Destroy();
		}
	}
}
#pragma endregion

#pragma region ParticleEffect
UPRParticleEffect* UPREffectSystemComponent::SpawnParticleEffectAtLocation(UParticleSystem* SpawnEffect, FVector Location, FRotator Rotation, FVector Scale, bool bEffectAutoActivate)
{
	UPRParticleEffect* ActivateableParticleEffect = GetActivateableParticleEffect(SpawnEffect);
	if(ActivateableParticleEffect != nullptr)
	{
		// ParticleEffect를 활성화하고 Spawn할 위치와 회전값을 적용합니다.
		ActivateableParticleEffect->SpawnEffectAtLocation(Location, Rotation, Scale, bEffectAutoActivate);

		// 해당 ParticleEffect로 처음 활성화하는 경우 ActivateParticleEffectIndexList를 생성합니다.
		if(!IsCreateActivateParticleEffectIndexList(SpawnEffect))
		{
			CreateActivateParticleEffectIndexList(SpawnEffect);
		}

		// 활성화된 ParticleEffect의 Index를 ActivateParticleEffectIndexList에 추가합니다.
		ActivateParticleEffectIndexList.Find(SpawnEffect)->Indexes.AddUnique(ActivateableParticleEffect->GetPoolIndex());
	}

	return ActivateableParticleEffect;
}

UPRParticleEffect* UPREffectSystemComponent::SpawnParticleEffectAttached(UParticleSystem* SpawnEffect, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, FVector Scale, EAttachLocation::Type LocationType, bool bEffectAutoActivate)
{
	UPRParticleEffect* ActivateableParticleEffect = GetActivateableParticleEffect(SpawnEffect);
	if(ActivateableParticleEffect != nullptr)
	{
		// ParticleEffect를 활성화하고 Spawn할 위치와 회전값을 적용합니다.
		ActivateableParticleEffect->SpawnEffectAttached(AttachToComponent, AttachPointName, Location, Rotation, Scale, LocationType, bEffectAutoActivate);

		// 해당 ParticleEffect로 처음 활성화하는 경우 ActivateParticleEffectIndexList를 생성합니다.
		if(!IsCreateActivateParticleEffectIndexList(SpawnEffect))
		{
			CreateActivateParticleEffectIndexList(SpawnEffect);
		}

		// 활성화된 ParticleEffect의 Index를 ActivateParticleEffectIndexList에 추가합니다.
		ActivateParticleEffectIndexList.Find(SpawnEffect)->Indexes.AddUnique(ActivateableParticleEffect->GetPoolIndex());
	}

	return ActivateableParticleEffect;
}

void UPREffectSystemComponent::DeactivateParticleEffectPool(UParticleSystem* ParticleEffect)
{
	if(IsCreateParticleEffectPool(ParticleEffect) == true)
	{
		// ParticleEffect에 해당하는 모든 ParticleEffect들을 비활성화합니다.
		for(auto& NewParticleEffect : ParticleEffectPool.Find(ParticleEffect)->Effects)
		{
			NewParticleEffect->Deactivate();
		}
	}
}

bool UPREffectSystemComponent::IsActivateParticleEffect(UPRParticleEffect* ParticleEffect) const
{
	if(IsValid(ParticleEffect) && ParticleEffect->GetParticleEffect() != nullptr)
	{
		// ActivateIndex에서 ParticleEffect의 Index가 있는지 탐색합니다.
		for(const auto& Pair : ActivateParticleEffectIndexList)
		{
			if(ParticleEffect->GetParticleEffectAsset() == Pair.Key)
			{
				for(const auto& Index : Pair.Value.Indexes)
				{
					if(ParticleEffect->GetPoolIndex() == Index)
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

bool UPREffectSystemComponent::IsCreateParticleEffectPool(UParticleSystem* ParticleSystem) const
{
	return ParticleEffectPool.Contains(ParticleSystem);
}

bool UPREffectSystemComponent::IsCreateActivateParticleEffectIndexList(UParticleSystem* ParticleSystem) const
{
	return ActivateParticleEffectIndexList.Contains(ParticleSystem);
}

bool UPREffectSystemComponent::IsCreateUsedParticleEffectIndexList(UParticleSystem* ParticleSystem) const
{
	return UsedParticleEffectIndexList.Contains(ParticleSystem);
}

UPRParticleEffect* UPREffectSystemComponent::GetActivateableParticleEffect(UParticleSystem* ParticleSystem)
{
	// 해당 ParticleEffect에 해당하는 Pool이 생성되었는지 확인하고, 없으면 생성합니다.
	if(!IsCreateParticleEffectPool(ParticleSystem))
	{
		FPRParticleEffectSettings ParticleEffectPoolSettings = FPRParticleEffectSettings(ParticleSystem, 1);
		CreateParticleEffectPool(ParticleEffectPoolSettings);
	}

	// ParticleEffectPool에서 해당 ParticleEffect의 Pool을 얻습니다.
	FPRParticleEffectPool* Pair = ParticleEffectPool.Find(ParticleSystem);
	if(Pair == nullptr)
	{
		// 지정된 ParticleEffect가 없습니다.
		return nullptr;
	}

	UPRParticleEffect* ActivateableParticleEffect = nullptr;

	// Pair에서 활성화 되지 않은 ParticleEffect를 찾아 활성화합니다.
	for(const auto& ParticleEffect : Pair->Effects)
	{
		if(IsValid(ParticleEffect) && !IsActivateParticleEffect(ParticleEffect))
		{
			ActivateableParticleEffect = ParticleEffect;
			break;
		}
	}

	// Pair의 모든 ParticleEffect가 활성화 되었을 경우 새로운 ParticleEffect를 생성합니다.
	if(ActivateableParticleEffect == nullptr)
	{
		// Critical Section 시작
		FCriticalSection CriticalSection;
		CriticalSection.Lock();

		// 해당 ParticleEffect의 UsedEffectIndexList가 생성되었는지 확인하고, 없으면 생성합니다.
		if(!IsCreateUsedParticleEffectIndexList(ParticleSystem))
		{
			FPRUsedIndexList UsedEffectIndexList;
			for(const auto& ParticleEffect : Pair->Effects)
			{
				UsedEffectIndexList.Indexes.Add(ParticleEffect->GetPoolIndex());
			}

			UsedParticleEffectIndexList.Emplace(ParticleSystem, UsedEffectIndexList);
		}

		// UsedParticleEffectIndexList에서 해당 ParticleEffect가 사용중인 ObjectIndex 목록을 얻습니다.
		FPRUsedIndexList* UsedEffectIndexList = UsedParticleEffectIndexList.Find(ParticleSystem);
		if(UsedEffectIndexList == nullptr)
		{
			// 지정된 ParticleEffect가 없습니다.
			return nullptr;
		}

		// 사용 가능한 Index를 구합니다.
		const int32 NewIndex = FindAvailableIndex(UsedEffectIndexList->Indexes);

		// 사용 가능한 Index를 UsedEffectIndexList에 추가합니다.
		UsedEffectIndexList->Indexes.Add(NewIndex);

		// Critical Section 끝
		CriticalSection.Unlock();

		// 새로운 ParticleEffect를 생성하고 초기화하며 사용 가능한 Index를 설정합니다.
		ActivateableParticleEffect = SpawnAndInitializeParticleEffect(ParticleSystem, NewIndex);
		ActivateableParticleEffect->OnParticleEffectDeactivate.AddDynamic(this, &UPREffectSystemComponent::OnDynamicParticleEffectDeactivate);

		// 새로 생성한 ParticleEffect를 Pair에 추가합니다.
		Pair->Effects.Emplace(ActivateableParticleEffect);
	}

	// DynamicParticleEffectTimer를 정지합니다.
	FPRDynamicDestroyParticleEffectList* DynamicDestroyPair = DynamicDestroyParticleEffectList.Find(ActivateableParticleEffect->GetParticleEffectAsset());
	if(DynamicDestroyPair != nullptr)
	{
		FTimerHandle* DynamicEffectDestroyTimer = DynamicDestroyPair->Effects.Find(ActivateableParticleEffect);
		if(DynamicEffectDestroyTimer != nullptr)
		{
			GetWorld()->GetTimerManager().ClearTimer(*DynamicEffectDestroyTimer);
		}
	}

	return ActivateableParticleEffect;
}

UPRParticleEffect* UPREffectSystemComponent::SpawnParticleEffectInWorld(UParticleSystem* ParticleSystem)
{
	if(GetWorld() != nullptr && ParticleSystem != nullptr && IsValid(GetPROwner()))
	{
		UPRParticleEffect* ParticleEffect = NewObject<UPRParticleEffect>();

		// SpawnEmitterAtLocation 함수를 사용하게 되면, 후에 AttachToComponent 함수를 사용할 경우 Effect가 부착은 되지만 부착한 Component를 따라가지 않습니다.
		UParticleSystemComponent* SpawnParticleSystemComponent = UGameplayStatics::SpawnEmitterAttached(ParticleSystem, GetPROwner()->GetMesh(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator,
																										FVector(1), EAttachLocation::KeepRelativeOffset, false, EPSCPoolMethod::None, false);
		if(SpawnParticleSystemComponent != nullptr)
		{
			/** 다른 곳에 사용하기 위해서 Detach 합니다.*/
			SpawnParticleSystemComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);

			ParticleEffect->Initialize();
			ParticleEffect->SetParticleEffect(SpawnParticleSystemComponent);
			ParticleEffect->SetEffectOwner(GetOwner());
			ParticleEffect->OnParticleEffectDeactivate.AddDynamic(this, &UPREffectSystemComponent::OnParticleEffectDeactivate);

			return ParticleEffect;
		}
	}
	
	return nullptr;
}

UPRParticleEffect* UPREffectSystemComponent::SpawnAndInitializeParticleEffect(UParticleSystem* ParticleSystem, int32 Index)
{
	UPRParticleEffect* SpawnParticleEffect = SpawnParticleEffectInWorld(ParticleSystem);
	if(SpawnParticleEffect != nullptr)
	{
		// 생성한 ParticleEffect를 초기화하고 OnParticleEffectDeactivate를 바인딩합니다.
		SpawnParticleEffect->Initialize();
		SpawnParticleEffect->SetEffectOwner(GetOwner());
		SpawnParticleEffect->SetPoolIndex(Index);
		SpawnParticleEffect->OnParticleEffectDeactivate.AddDynamic(this, &UPREffectSystemComponent::UPREffectSystemComponent::OnParticleEffectDeactivate);
	}
	
	return SpawnParticleEffect;
}

void UPREffectSystemComponent::CreateParticleEffectPool(FPRParticleEffectSettings ParticleEffectSettings)
{
	if(GetWorld() != nullptr && ParticleEffectSettings.ParticleSystem != nullptr)
	{
		// ParticleEffectPool에 추가할 Pair를 초기화하고 ParticleEffect를 생성하여 추가합니다.
		FPRParticleEffectPool Pair;
		for(int32 Index = 0; Index < ParticleEffectSettings.PoolSize; Index++)
		{
			UPRParticleEffect* SpawnParticleEffect = SpawnAndInitializeParticleEffect(ParticleEffectSettings.ParticleSystem, Index);
			if(SpawnParticleEffect != nullptr)
			{
				// 초기화된 ParticleEffect를 Pair에 추가합니다.
				Pair.Effects.Emplace(SpawnParticleEffect);
			}
		}

		// 초기화된 Pair를 ParticleEffectPool에 추가하고 ActivateParticleEffectIndexList를 생성합니다.
		ParticleEffectPool.Emplace(ParticleEffectSettings.ParticleSystem, Pair);
		IsCreateActivateParticleEffectIndexList(ParticleEffectSettings.ParticleSystem);
	}
}

void UPREffectSystemComponent::CreateActivateParticleEffectIndexList(UParticleSystem* ParticleSystem)
{
	// ParticleSystem의 ActivateParticleEffectIndexList를 생성합니다.
	ActivateParticleEffectIndexList.Emplace(ParticleSystem);
}

void UPREffectSystemComponent::OnParticleEffectDeactivate(UPRParticleEffect* ParticleEffect)
{
	if(IsActivateParticleEffect(ParticleEffect))
	{
		// 비활성화된 ParticleEffect의 Index를 ActivateIndex에서 제거합니다.
		ActivateParticleEffectIndexList.Find(ParticleEffect->GetParticleEffectAsset())->Indexes.Remove(ParticleEffect->GetPoolIndex());
	}
}

void UPREffectSystemComponent::OnDynamicParticleEffectDeactivate(UPRParticleEffect* ParticleEffect)
{
	if(DynamicDestroyDelay > 0)
	{
		// 타이머로 딜레이 만큼 시간이지난 후 OnDynamicParticleEffectDestroy 함수를 실행합니다.
		FTimerHandle DynamicDestroyDelayTimerHandle;
		FTimerDelegate DynamicDestroyDelayDelegate = FTimerDelegate::CreateUObject(this, &UPREffectSystemComponent::OnDynamicParticleEffectDestroy, ParticleEffect);
		GetWorld()->GetTimerManager().SetTimer(DynamicDestroyDelayTimerHandle, DynamicDestroyDelayDelegate, DynamicDestroyDelay, false);

		// 타이머를 DynamicDestroyParticleEffectList에 추가합니다.
		FPRDynamicDestroyParticleEffectList* DynamicDestroyPair = DynamicDestroyParticleEffectList.Find(ParticleEffect->GetParticleEffectAsset());
		if(DynamicDestroyPair != nullptr)
		{
			DynamicDestroyPair->Effects.Emplace(ParticleEffect, DynamicDestroyDelayTimerHandle);
		}
		else
		{
			FPRDynamicDestroyParticleEffectList EffectList;
			EffectList.Effects.Emplace(ParticleEffect, DynamicDestroyDelayTimerHandle);
			DynamicDestroyParticleEffectList.Emplace(ParticleEffect->GetParticleEffectAsset(), EffectList);
		}
	}
	else
	{
		// 딜레이가 없을 경우 타이머를 실행하지 않고 바로 OnDynamicParticleEffectDestroy 함수를 실행합니다.
		OnDynamicParticleEffectDestroy(ParticleEffect);
	}
}

void UPREffectSystemComponent::OnDynamicParticleEffectDestroy(UPRParticleEffect* ParticleEffect)
{
	// 사용 중인 ParticleEffectIndexList를 가지고 있는지 확인합니다.
	if(IsCreateUsedParticleEffectIndexList(ParticleEffect->GetParticleEffectAsset()))
	{
		// DynamicParticleEffectDestroyTimer를 제거합니다.
		FPRDynamicDestroyParticleEffectList* DynamicDestroyPair = DynamicDestroyParticleEffectList.Find(ParticleEffect->GetParticleEffectAsset());
		if(DynamicDestroyPair != nullptr)
		{
			DynamicDestroyPair->Effects.Remove(ParticleEffect);
		}

		// 해당 ParticleEffect의 UsedIndexList를 얻습니다.
		FPRUsedIndexList* UsedIndexList = UsedParticleEffectIndexList.Find(ParticleEffect->GetParticleEffectAsset());
		if(UsedIndexList != nullptr)
		{
			// 사용중인 Index를 제거합니다
			UsedIndexList->Indexes.Remove(ParticleEffect->GetPoolIndex());
		}

		// 해당 ParticleEffect의 Pool이 생성되었는지 확인합니다.
		if(IsCreateParticleEffectPool(ParticleEffect->GetParticleEffectAsset()))
		{
			// ParticleEffectPool에서 해당 ParticleEffect를 제거하고 파괴합니다.
			ParticleEffectPool.Find(ParticleEffect->GetParticleEffectAsset())->Effects.Remove(ParticleEffect);
			ParticleEffect->Destroy();
		}
	}
}
#pragma endregion 
