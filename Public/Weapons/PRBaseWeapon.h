// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Common/PRCommonStruct.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "PRBaseWeapon.generated.h"

class APRBaseCharacter;
class UNiagaraSystem;
class UNiagaraComponent;
class UArrowComponent;

/** 이 무기를 소유하고 있는 PROwner의 종류를 나타내는 열거형입니다. */
UENUM(BlueprintType)
enum class EPROwnerType : uint8
{
	OwnerType_None			UMETA(DisplayName = "None"),
	OwnerType_Player		UMETA(DisplayName = "Player"),
	OwnerType_Enemy			UMETA(DisplayName = "Enemy")
};

/**
 * 캐릭터가 사용하는 무기의 기본 Actor 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRBaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	APRBaseWeapon();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

public:
	/**
	 * 이 무기를 초기화하는 함수입니다.
	 *
	 * @param NewPROwner 이 무기를 사용하는 캐릭터입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRBaseWeapon")
	void InitializeWeapon(class APRBaseCharacter* NewPROwner);

	/** 무기를 발도하여 DrawSocketName에 해당하는 Socket에 부착하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseWeapon")
	virtual void Draw();

	/** 무기를 납도하여 SheathSocketName에 해당하는 Socket에 부착하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseWeapon")
	virtual void Sheath();
	
protected:
	/** 이 Actor의 RootComponent입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PRBaseWeapon")
	USceneComponent* Root;

#pragma region WeaponInfo
public:
	/** 무기의 활성화를 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "WeaponInfo")
	bool IsActivate() const;

	/** 인자 값에 따라 무기의 활성화를 설정하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "WeaponInfo")
	void SetActivate(bool bNewActivate);

	/** 이 무기를 발도 했는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "WeaponInfo")
	bool IsDraw() const;

	/** 이 무기를 납도 했는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "WeaponInfo")
	bool IsSheath() const;
	
protected:
	/** 무기의 활성화를 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "WeaponInfo")
	bool bActivate;

	/** 무기의 공격력입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponInfo")
	float WeaponAttackPoint;
	
private:
	/** 이 무기를 사용하는 캐릭터입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WeaponInfo", meta = (AllowPrivateAccess = "true"))
	APRBaseCharacter* PROwner;

	/** PROwner의 종류입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "WeaponInfo", meta = (AllowPrivateAccess = "true"))
	EPROwnerType PROwnerType;

	/** 이 무기를 발도 했는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WeaponInfo", meta = (AllowPrivateAccess = "true"))
	bool bIsDraw;

	/** 이 무기를 납도 했는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WeaponInfo", meta = (AllowPrivateAccess = "true"))
	bool bIsSheath;

public:
	/** WeaponAttackPoint를 반환하는 함수입니다. */
	float GetWeaponAttackPoint() const;
	
	/** PROwner를 반환하는 함수입니다. */
	class APRBaseCharacter* GetPROwner() const;

	/** 입력받은 인자로 PROwner를 설정하는 함수입니다. */
	void SetPROwner(class APRBaseCharacter* NewPROwner);

	/** PROwnerType을 반환하는 함수입니다. */
	EPROwnerType GetPROwnerType() const;
#pragma endregion

#pragma region MainWeapon
protected:
	/** 메인 무기입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponInfo|MainWeapon")
	USceneComponent* MainWeapon;
	
	/** 메인 무기의 외형입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponInfo|MainWeapon")
	UStaticMeshComponent* MainWeaponMesh;

	/** 메인 무기의 투사체가 Spawn되는 위치를 나타내는 ArrowComponent 클래스입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponInfo|MainWeapon")
	UArrowComponent* MainWeaponProjectileSpawnLocation;
	
	/** 메인 무기를 발도하여 장착하는 Socket의 이름입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponInfo|MainWeapon")
	FName MainWeaponDrawSocketName;

	/** 메인 무기를 납도하여 장착하는 Socket의 이름입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponInfo|MainWeapon")
	FName MainWeaponSheathSocketName;

public:
	/** MainWeaponMesh을 반환하는 함수입니다. */
	UStaticMeshComponent* GetMainWeaponMesh() const;

	/** MainWeaponDrawSocketName을 반환하는 함수입니다. */
	FName GetMainWeaponDrawSocketName() const;

	/** MainWeaponSheathSocketName을 반환하는 함수입니다. */
	FName GetMainWeaponSheathSocketName() const;

	/** MainWeaponProjectileSpawnLocation을 반환하는 함수입니다. */
	UArrowComponent* GetMainWeaponProjectileSpawnLocation() const;
#pragma endregion

#pragma region SubWeapon
public:
	/** SubWeaponMesh를 반환하는 함수입니다. */
	virtual UStaticMeshComponent* GetSubWeaponMesh() const;

	/** SubWeaponProjectileSpawnLocation을 반환하는 함수입니다. */
	virtual UArrowComponent* GetSubWeaponProjectileSpawnLocation() const;
#pragma endregion
};
