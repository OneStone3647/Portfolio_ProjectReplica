// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "PRWeaponSystemComponent.generated.h"

class APRBaseWeapon;

/**
 * 캐릭터의 무기를 관리하는 ActorComponent입니다. 
 */
UCLASS()
class PROJECTREPLICA_API UPRWeaponSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()

public:
	UPRWeaponSystemComponent();

public:
	/**
	 * WeaponInventory를 초기화하는 함수입니다.
	 * WeaponDataTable의 정보를 바탕으로 무기를 월드에 Spawn하여 WeaponInventory에 보관합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "WeaponSystem")
	void InitializeWeaponInventory();

	/**
	 * WeaponInventory에서 인자로 받은 Index 값에 해당하는 무기를 반환하는 함수입니다.
	 *
	 * @param NewWeaponInventoryIndex WeaponInventory에서 탐색할 무기의 Index 값입니다.
	 * @return Index 값에 해당하는 무기가 있을 경우 해당하는 무기를 반환하고 없을 경우 nullptr을 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "WeaponSystem")
	APRBaseWeapon* GetWeapon(int32 NewWeaponInventoryIndex) const;

	/**
	 * WeaponInventory에서 인자로 받은 Index 값에 해당하는 무기를 발도하는 함수입니다.
	 *
	 * @param DrawWeaponIndex 발도할 무기의 Index 값입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "WeaponSystem")
	void DrawWeapon(int32 DrawWeaponIndex = 0);

	/**
	 * WeaponInventory에서 인자로 받은 Index 값에 해당하는 무기를 납도하는 함수입니다.
	 *
	 * @param SheathWeaponIndex 납도할 무기의 Index 값입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "WeaponSystem")
	void SheathWeapon(int32 SheathWeaponIndex = 0);

	/** 현재 장비한 무기를 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "WeaponSystem")
	APRBaseWeapon* GetEquippedWeapon() const;

protected:
	/**
	 * 인자로 받은 PRBaseWeapon 클래스 레퍼런스로 월드에 무기를 Spawn하는 함수입니다.
	 * 
	 * @param NewPRWeaponClass 월드에 Spawn할 무기의 클래스 레퍼런스입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRWeaponSystem")
	APRBaseWeapon* SpawnWeaponInWorld(TSubclassOf<class APRBaseWeapon> NewPRWeaponClass);

protected:
	/** 캐릭터가 사용하는 무기를 보관한 Array입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRWeaponSystem")
	TArray<class APRBaseWeapon*> WeaponInventory;

	/** 현재 장비한 무기의 Index입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRWeaponSystem")
	int32 EquippedWeaponIndex;

	/** 스폰할 무기의 클래스 레퍼런스입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PRWeaponSystem")
	TArray<TSubclassOf<class APRBaseWeapon>> WeaponsToSpawn;

public:
	/** WeaponInventory를 반환하는 함수입니다. */
	TArray<class APRBaseWeapon*> GetWeaponInventory() const;

	/** 현재 장비한 무기의 Index입니다. */
	int32 GetEquippedWeaponIndex() const;
};
