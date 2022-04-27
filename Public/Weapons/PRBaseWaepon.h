// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NiagaraComponent.h"
#include "ProjectReplica.h"
#include "Structs/Struct_PRAnimMontage.h"
#include "GameFramework/Actor.h"
#include "PRBaseWaepon.generated.h"

/**
 * 무기의 장착 타입을 나타내는 열거형입니다.
 * 한 손에 하나의 무기를 장착하는 무기일 경우 Single, 양 손에 하나씩 착용하는 무기일 경우 Dual을 나타냅니다.
 */
UENUM(BlueprintType)
enum class EPRWeaponEquipType : uint8
{
	WeaponEquipType_Single		UMETA(DisplayName = "Single"),
	WeaponEquipType_Dual		UMETA(DisplayName = "Dual")
};

/**
 * 무기를 소유한 캐릭터의 타입을 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPROwnerType : uint8
{
	OwnerType_None			UMETA(DisplayName = "None"),
	OwnerType_Player		UMETA(DisplayName = "Player"),
	OwnerType_AI			UMETA(DisplayName = "AI")
};

/**
 * 캐릭터가 사용하는 무기인 Actor 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRBaseWaepon : public AActor
{
	GENERATED_BODY()
	
public:	
	APRBaseWaepon();

protected:
	virtual void BeginPlay() override;

#pragma region PRBaseWeapon
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseWeapon", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseWeapon", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WeaponStaticMesh;
#pragma endregion

#pragma region Debug
protected:
	/**
	 * Debug를 하기 위해 TraceCollision을 게임에서 그릴지 설정하는 함수입니다.
	 * @param bFlag true일 경우 TraceCollision을 게임에서 그리고 false일 경우 TraceCollision을 게임에서 그리지 않습니다.
	 */
	void DrawDebug(bool bFlag);

protected:
	/** TraceCollision과 Trace의 Debug를 출력을 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRBaseWeapon|Debug")
	bool bDrawDebug;

	/** Debug를 출력할 시간입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRBaseWeapon|Debug")
	float DrawDebugTime;

	/** TraceCollision의 경로에 닿은 곳을 표시하는 DebugPoint의 크기입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRBaseWeapon|Debug")
	float DrawDebugTracePointSize;

public:
	/** bDrawDebug를 반환하는 함수입니다. */
	bool IsDrawDebug() const;
#pragma endregion 

#pragma region WeaponInfo
public:
	/**
	 * 무기의 활성화를 설정하는 함수입니다.
	 * @param bFlag true일 경우 무기를 게임에서 숨기지 않고 Tick을 활성화합니다.
	 *				 false일 경우 무기를 게임에서 숨기고 Tick을 비활성화합니다.
	 * @param bSpawnEffectFlag true일 경우 SpawnWeaponEffect를 실행합니다.
	 *							 false일 경우 SpawnWeaponEffect를 실행하지 않습니다.
	 */
	void SetWeaponActive(bool bFlag, bool bSpawnEffectFlag = false);

	/** 무기가 활성화 되었는지 판별하는 함수입니다. */
	bool IsWeaponActive() const;
	
	/**
	 * 입력받은 인자로 PROwner를 설정하고 이를 바탕으로 OwnerType을 설정하는 함수입니다.
	 * @param NewPROwner PROwner로 설정할 PRBaseCharacter 클래스입니다.
	 */
	void SetPROnwerAndOwnerType(class APRBaseCharacter* NewPROwner);

	/**
 	 * 입력받은 인자가 WeaponEquipType와 같은지 판별하는 함수입니다.
 	 * 같은 경우 true, 다를 경우 false를 반환합니다.
 	 * @param NewPRWeaponEquipType 비교할 PRWeaponEquipType입니다.
 	 */
	bool IsEqualWeaponEquipType(EPRWeaponEquipType NewPRWeaponEquipType) const;
	
protected:
	/** 무기의 활성화를 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseWeapon|WeaponInfo", meta = (AllowPrivateAccess = "true"))
	bool bIsActive;
	
	/** 무기를 소유한 캐릭터입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRBaseWeapon|WeaponInfo", meta = (AllowPrivateAccess = "true"))
	class APRBaseCharacter* PROwner;
	
	/** 무기를 소유한 캐릭터의 타입입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRBaseWeapon|WeaponInfo", meta = (AllowPrivateAccess = "true"))
	EPROwnerType OwnerType;
	
	/** 무기의 장착 타입입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRBaseWeapon|WeaponInfo", meta = (AllowPrivateAccess = "true"))
	EPRWeaponEquipType WeaponEquipType;

	/** 무기의 대미지입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRBaseWeapon|WeaponInfo", meta = (AllowPrivateAccess = "true"))
	float WeaponDamage;

public:
	/** 입력받은 인자로 PROwner를 설정하는 함수입니다. */
	void SetPROwner(class APRBaseCharacter* NewPROwner);
	
	/** 입력받은 인자로 OwnerType을 설정하는 함수입니다. */
	void SetOwnertype(EPROwnerType NewOwnerType);
	
	/** WeaponEquipType을 반환하는 함수입니다. */
	EPRWeaponEquipType GetWeaponEquipType() const;

	/** WeaponDamage를 반환하는 함수입니다. */
	float GetWeaponDamage() const;
#pragma endregion

#pragma region WeaponPRAnimMontage
public:
	/** WeaponPRAnimMontageIndex를 초기화하는 함수입니다. */
	void InitializeWeaponPRAnimMontageIndex();
	
	/** WeaponPRAnimMontageIndex를 증가시키는 함수입니다. */
	void IncreaseWeaponPRAnimMontageIndex();
	
protected:
	/**
	 * WeaponPRAnimMontageDataTable에서 받은 데이터로 WeaponPRAnimMontageArray를 초기화하고
	 * WeaponPRAnimMontageIndex를 초기화하는 함수입니다.
	 */
	void InitializeWeaponPRAnimMontageArray();
	
protected:
	/** 무기의 PRAnimMontage를 보관한 데이터 테이블입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRBaseWeapon|WeaponPRAnimMontage")
	UDataTable* WeaponPRAnimMontageDataTable;

	/** WeaponPRAnimMontageDataTable에서 받은 데이터를 보관한 Array입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseWeapon|WeaponPRAnimMontage")
	TArray<FPRAnimMontage> WeaponPRAnimMontageArray;

	/** WeaponPRAnimMontageArray에서 재생하는 PRAnimMontage의 Index입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseWeapon|WeaponPRAnimMontage")
	int32 WeaponPRAnimMontageIndex;
	
public:
	/** WeaponPRAnimMontageArray에서 WeaponPRAnimMontageIndex에 해당하는 PRAnimMontage를 반환하는 함수입니다. */
	FORCEINLINE FPRAnimMontage GetWeaponPRAnimMontage() const
	{
		return WeaponPRAnimMontageArray[WeaponPRAnimMontageIndex];
	}

	/** WeaponPRAnimMontageIndex를 반환하는 함수입니다. */
	int32 GetWeaponPRAnimMontageIndex() const;
#pragma endregion

#pragma region Effect
public:
	/**
	 * 공격당한 액터의 타격된 위치에 HitParticleEffect를 스폰하는 함수입니다.
	 * @param HitActor 공격당한 액터입니다.
	 * @param HitLocation 타격된 위치입니다.
	 * @param WeaponRotator 무기의 회전 값입니다. 무기를 휘두르면서 무기의 경로에 따라 이팩트르 그리기 위해 무기의 Rotation을 사용합니다.
	 */
	void ExecuteHitParticleEffect(AActor* HitActor, FVector HitLocation, FRotator WeaponRotator);

	/** 무기의 위치에 SpawnWeaponEffect를 스폰하는 함수입니다. */
	void ExecuteSpawnWeaponEffect();

protected:
	/** 타격한 위치에 스폰하는 파티클 시스템입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRBaseWeapon|Effect")
	UParticleSystem* HitParticleEffect;

	/** HitParticleEffect의 크기입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRBaseWeapon|Effect")
	FVector HitParticleEffectScale;

	/** 무기를 Spawn할 때 사용하는 NiagaraSystem입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRBaseWeapon|Effect")
	UNiagaraSystem* SpawnWeaponEffect;

	/** SpawnWeaponEffect의 색입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRBaseWeapon|Effect")
	FLinearColor SpawnWeaponEffectColor;
#pragma endregion 

#pragma region TraceCollision
public:
	/** TraceCollisionsArray의 모든 TraceCollision들의 Location을 각각의 PreviousLocation에 저장하여 최신화 하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRBaseWeapon|TraceCollision")
	void UpdateAllTraceCollisionsLocation();

protected:
	/** 모든 TraceCollision을 TraceCollisionsArray에 저장하는 함수입니다. */
	void AddAllTraceCollisionToArray();
	
protected:
	/** Base가 되는 TraceCollision입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseWeapon|TraceCollision")
	class UPRTraceCollision* BaseTraceCollision;

	/** PRTraceCollision을 모아 놓은 Array입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseWeapon|TraceCollision")
	TArray<class UPRTraceCollision*> TraceCollisionsArray;
#pragma endregion

#pragma region Trace
public:
	/**
	 * AttackTrace를 실행하는 함수입니다.
	 * TraceCollision의 위치에 따라 LineTraceChannel을 실행하여 Trace의 충돌에 검출되는 액터들에게
	 * 대미지를 주고 충돌한 위치에 HitParticleEffect를 스폰하는 함수입니다. 
	 */
	UFUNCTION(BlueprintCallable, Category = "PRWeapon|Trace")
	void ExecuteAttackTrace();

	/** HitActors Map을 비우는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRWeapon|Trace")
	void ClearHitActors();
	
protected:
	/**
	 * 인자로 받은 액터가 피격된 액터인지 판별하는 함수입니다.
	 * @param HitedActor 값을 가져올 Key 값입니다.
	 * @return HitActors Map에 NewActor가 존재할 경우 true를, 존재하지 않을 경우 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRWeapon|Trace")
	bool IsHitActor(const AActor* HitedActor) const;

	/**
	 * 인자로 받은 액터가 대미지를 받은 액터인지 판별하는 함수입니다.
	 * @param HitedActor 값을 가져올 Key 값입니다.
	 * @return HitActors Map에 NewActor가 존재할 경우 true(값을 포인터로 가져오고)를, 존재하지 않을 경우 false(nullptr)를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRWeapon|Trace")
	bool IsTakeDamageActor(const AActor* HitedActor) const;

	/**
	 * 인자로 받은 액터에게 대미지를 주는 함수입니다.
	 * @param HitedActor 대미지를 줄 액터입니다. 
	 */
	UFUNCTION(BlueprintCallable, Category = "PRWeapon|Trace")
	void ApplyDamage(AActor* HitedActor);

protected:
	/**
  	 * AttackTrace에 걸린 액터를 Key로 가지고 Key인 액터가 대미지를 받았는의 여부를 Value로 저장되어 있는 Map입니다.
  	 * @Key: AttackTrace에 걸린 액터입니다.
  	 * @Value: 대미지를 받았는지 나타냅니다.
  	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRBaseWeapon|Trace")
	TMap<AActor*, bool> HitActors;
#pragma endregion 
};
