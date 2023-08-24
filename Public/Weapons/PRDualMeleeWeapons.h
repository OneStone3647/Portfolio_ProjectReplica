// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Weapons/PRMeleeWeapon.h"
#include "PRDualMeleeWeapons.generated.h"

/**
 * 근접 전투에서 양손에 한 자루씩 착용하여 사용하는 무기 Actor 클래스입니다.  
 */
UCLASS()
class PROJECTREPLICA_API APRDualMeleeWeapons : public APRMeleeWeapon
{
	GENERATED_BODY()

public:
	APRDualMeleeWeapons();

protected:
	virtual void BeginPlay() override;

public:
	/** 무기를 발도하여 DrawSocketName에 해당하는 Socket에 부착하는 함수입니다. */
	virtual void Draw() override;

	/** 무기를 납도하여 SheathSocketName에 해당하는 Socket에 부착하는 함수입니다. */
	virtual void Sheath() override;

#pragma region SubWeapon
private:
	/** 서브 무기입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponInfo|SubWeapon", meta = (AllowPrivateAccess = "true"))
	USceneComponent* SubWeapon;
	
	/** 서브 무기의 외형입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponInfo|SubWeapon", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* SubWeaponMesh;

	/** 서브 무기의 투사체가 Spawn되는 위치를 나타내는 ArrowComponent 클래스입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponInfo|SubWeapon", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* SubWeaponProjectileSpawnLocation;
	
	/** 서브 무기를 발도하여 장착하는 Socket의 이름입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponInfo|SubWeapon", meta = (AllowPrivateAccess = "true"))
	FName SubWeaponDrawSocketName;

	/** 서브 무기를 납도하여 장착하는 Socket의 이름입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponInfo|SubWeapon", meta = (AllowPrivateAccess = "true"))
	FName SubWeaponSheathSocketName;

public:
	/** SubWeapon을 반환하는 함수입니다. */
	virtual UStaticMeshComponent* GetSubWeaponMesh() const override;

	/** SubWeaponProjectileSpawnLocation을 반환하는 함수입니다. */
	virtual UArrowComponent* GetSubWeaponProjectileSpawnLocation() const override;
#pragma endregion

#pragma region Debug
private:
	/** 디버그를 최신화하는 함수입니다. */
	virtual void UpdateTraceDebug() override;
#pragma endregion 

#pragma region TracePoint
private:
	/** SubWeapon의 TraceLine의 생성에 사용하는 정보를 가진 구조체를 보관한 Array입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRMeleeWeapon|TracePoint", meta = (AllowPrivateAccess = "true"))
	TArray<FPRTraceLineGenerateInfo> SubWeaponTraceLineGenerateInfo;	
	
	/** SubWeapon에서 생성한 TraceLine들을 보관한 Array입니다. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "PRMeleeWeapon|TracePoint", meta = (AllowPrivateAccess = "true"))
	TArray<FPRTraceLine> SubWeaponTraceLines;

public:
	/** SubWeaponTraceLines를 반환하는 함수입니다. */
	TArray<FPRTraceLine> GetSubWeaponTraceLines() const;

	/** SubWeaponTraceLines을 레퍼런스로 반환하는 함수입니다. */
	TArray<FPRTraceLine>& GetSubWeaponTraceLinesReference();
#pragma endregion

#pragma region Trace
public:
	/**
	 * SubWeapon의 AttackTraceLine을 활성화하는 함수입니다.
	 *
	 * @param bNewActivateTraceDebug TraceDebug의 실행을 결정합니다.
	 */
	void ActivateSubWeaponAttackTraceLine(bool bNewActivateTraceDebug = false);
	
private:
	/**
	 * AttackTrace에 걸린 액터를 Key로 가지고 Key인 액터가 대미지를 받았는지를 나타내는 값을 Value로 저장되어 있는 Map입니다.
	 *
	 * @Key AttackTrace에 걸린 액터입니다.
	 * @Value 대미지를 받았는지 나타내는 값입니다.
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "PRMeleeWeapon|Trace", meta = (AllowPrivateAccess = "true"))
	TMap<AActor*, bool> SubWeaponHitActors;

public:
	/** SubWeaponHitActors를 반환하는 함수입니다. */
	TMap<AActor*, bool>& GetSubWeaponHitActors();
#pragma endregion

#pragma region Effect
public:
	/**
	 * 사용한 무기의 종류에 따라 입력받은 위치에 HitEffect를 Spawn하는 함수입니다.
	 *
	 * @param NewWeaponPosition 사용한 무기의 종류입니다.
	 * @param NewHitLocation HitEffect를 Spawn할 위치입니다.
	 */
	virtual void SpawnHitEffectByWeaponPosition(EPRWeaponPosition NewWeaponPosition, FVector NewHitLocation) override;
#pragma endregion 
};
