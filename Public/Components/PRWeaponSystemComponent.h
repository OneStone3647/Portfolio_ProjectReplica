// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "Enums/Enum_PRWeaponEquipPosition.h"
#include "PRWeaponSystemComponent.generated.h"

enum class EPRWeaponEquipPosition : uint8;

/**
 * 캐릭터의 무기를 관리하는 ActorComponent 클래스입니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTREPLICA_API UPRWeaponSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()
	
public:
	UPRWeaponSystemComponent();

#pragma region WeaponInventory
public:
	/**
	 * 무기를 월드에 스폰한 후 WeaponInventory에 저장하는 함수입니다.
	 * 스폰한 무기의 WeaponEquipType이 Dual일 경우 무기를 한 번 더 월드에 스폰한 후 SubWeaponInventory에 저장합니다.
	 * @param NewWeapon: 스폰할 무기의 클래스 레퍼런스입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRWeaponSystem|WeaponInventory")
	void SpawnWeaponAndAddToInventory(TSubclassOf<class APRBaseWaepon> NewWeapon);
	
	/**
	 * 입력받은 인자를 키 값으로 가지는 무기를 각각의 WeaponInventory에서 탐색하여 존재할 경우
	 * 각각 MainWeapon과 SubWeapon으로 장착하는 함수입니다.
	 * @param NewWeaponIndex: 탐색할 키 값입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRWeaponSystem|WeaponInventory")
	void EquipWeapons(int32 NewWeaponIndex);

protected:
	/**
	 * MainWeapon으로 장착하는 무기를 저장한 Map입니다.
	 * @Key: WeaponIndex
	 * @Value: PRBaseWeapon Reference
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRWeaponSystem|WeaponInventory")
	TMap<int32, class APRBaseWaepon*> MainWeaponInventory;

	/**
 	 * SubWeapon으로 장착하는 무기를 저장한 Map입니다.
 	 * @Key: WeaponIndex
 	 * @Value: PRBaseWeapon Reference
 	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRWeaponSystem|WeaponInventory")
	TMap<int32, class APRBaseWaepon*> SubWeaponInventory;
#pragma endregion 

#pragma region PRWeaponSystem
public:
	/** MainWeapon과 SubWeapon을 발도하는 함수입니다.*/
	UFUNCTION(BlueprintCallable, Category = "PRWeaponSystem")
	void DrawWeapons();

	/**
	 * MainWeapon과 SubWeapon을 납도하는 함수입니다.
	 * @param bExecuteEffect: 납도할 때 이펙트를 실행할지 결정합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRWeaponSystem")
	void SheathWeapons(bool bExecuteEffect = false);

	/**
	 * 무기를 발도하였는지 판별하는 함수입니다.
	 * @return 무기를 발도하였다면 true를 아닐 경우 false를 반환합니다.
	 */
	bool IsDrawWeapons() const;

	/**
	 * WeaponIndex를 증가 시키고 증가시킨 WeaponIndex에 해당하는 Key 값을 가진 무기를
	 * MainWeapon과 SubWeapon으로 설정하는 함수입니다.
	 */
	void SwapWeapons();

	/**
	 * 무기를 변경할 수 있는지 판별하는 함수입니다.
	 * @return 무기를 변경할 수 있다면 true를 아닐 경우 false를 반환합니다.
	 */
	bool IsCanSwapWeapon();

	/**
	 * 입력받은 인자에 해당하는 위치에 장착한 무기를 반환하는 함수입니다. 
	 * @param WeaponEquipPosition 가져올 무기의 장착 위치입니다.
	 * @return 장착 위치에 해당하는 무기를 반환합니다.
	 */
	APRBaseWaepon* GetEquipWeapon(EPRWeaponEquipPosition WeaponEquipPosition = EPRWeaponEquipPosition::WeaponEquipPosition_Main) const;

	/**
	 * 입력받은 인자에 해당하는 위치에 무기를 장착했는지 판별하는 함수입니다.
	 * @param WeaponEquipPosition 무기를 장착했는지 판별하는 위치입니다.
	 * @return 판별하는 위치에 무기를 장착하였으면 true를 아닐 경우 false를 반환합니다.
	 */
	bool IsEquipWeapon(EPRWeaponEquipPosition WeaponEquipPosition = EPRWeaponEquipPosition::WeaponEquipPosition_Main) const;
	
protected:
	/**
	 * 입력받은 PRBaseWeapon 클래스 레퍼런스를 바탕으로 무기를 월드에 스폰하는 함수입니다.
	 * @param NewWeapon: 스폰할 무기의 클래스 레퍼런스입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRWeaponSystem")
	class APRBaseWaepon* SpawnWeaponInWorld(TSubclassOf<class APRBaseWaepon> NewWeapon);

private:
	/**
	 * 입력받은 인자를 바탕으로 무기를 소켓에 장착(발도)하는 함수입니다.
	 * @param Weapon: 장착할 무기입니다.
	 * @param EquipSocketName: 장착할 소켓의 이름입니다.
	 * @param bExecuteEffect: 장착할 때 이펙트를 실행할지 결정합니다.
	 */
	void DrawWeapon(class APRBaseWaepon* Weapon, FName EquipSocketName, bool bExecuteEffect = false);

	/**
	 * 입력받은 인자를 바탕으로 무기를 캐릭터에서 떼어내는(납도) 함수입니다.
	 * @param Weapon: 캐릭터에서 떼어낼 무기입니다.
	 * @param bExecuteEffect: 떼어낼 때 이펙트를 실행할지 결정합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRWeaponSystem")
	void SheathWeapon(class APRBaseWaepon* Weapon, bool bExecuteEffect = false);

private:
	/** WeaponInventory에서 현재 사용하는 무기의 Key값을 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRWeaponSystem", meta = (AllowPrivateAccess = "true"))
	int32 WeaponIndex;
	
	/** PROwner의 SkeletalMesh에서 MainWeaponEquipSocketName에 해당하는 소켓에 장착하는 무기입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRWeaponSystem", meta = (AllowPrivateAccess = "true"))
	class APRBaseWaepon* MainWeapon;

	/** PROwner의 SkeletalMesh에서 SubWeaponEquipSocketName에 해당하는 소켓에 장착하는 무기입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRWeaponSystem", meta = (AllowPrivateAccess = "true"))
	class APRBaseWaepon* SubWeapon;
	
	/** MainWeapon을 장착하는 소켓의 이름입니다. */ 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRWeaponSystem", meta = (AllowPrivateAccess = "true"))
	FName MainWeaponEquipSocketName;

	/** SubWeapon을 장착하는 소켓의 이름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRWeaponSystem", meta = (AllowPrivateAccess = "true"))
	FName SubWeaponEquipSocketName;

	/** 무기를 변경할 수 있는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRWeaponSystem", meta = (AllowPrivateAccess = "true"))
	bool bCanSwapWeapon;

public:
	/** 입력받은 인자로 bCanSwapWeapon을 설정하는 함수입니다. */
	void SetCanSwapWeapon(bool bFlag);
#pragma endregion 
};

