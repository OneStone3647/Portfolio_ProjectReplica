// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Objects/PRBaseProjectileObject.h"
#include "PRJudgementSlashProjectile.generated.h"

class UNiagaraSystem;

/**
 * 캐릭터가 검을 휘둘러 전방으로 참격을 날리는 투사체 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRJudgementSlashProjectile : public APRBaseProjectileObject
{
	GENERATED_BODY()

public:
	APRJudgementSlashProjectile();

protected:
	/** ImpactNiagaraEffect를 Spawn하는 함수입니다. */
	void SpawnImpactNiagaraEffect(FVector Location, FRotator Rotation);

protected:
	/** 투사체가 충돌했을 때 재생하는 NiagaraEffect입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JudgementSlashProjectile")
	UNiagaraSystem* ImpactNiagaraEffect;

	/** ImpactNiagaraEffect의 회전값입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JudgementSlashProjectile")
	FRotator ImpactNiagaraEffectRotation; 	

	/** 투사체가 충돌했을 때 재생하는 사운드입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JudgementSlashProjectile")
	USoundBase* ImpactSound;

public:
	/** 오브젝트를 최신화하는 함수입니다. */
	virtual void UpdatePooledObject_Implementation(float DeltaTime) override;

	/** 오브젝트를 비활성화하는 함수입니다. */
	virtual void Deactivate_Implementation() override;

protected:
	/** 오브젝트의 Spawn 위치를 초기화하는 함수입니다. */
	virtual void InitializeSpawnLocation_Implementation() override;

#pragma region Debug
protected:
	/** 디버그 실행을 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bActivateDebug;
#pragma endregion

#pragma region HitBox
protected:
	/** HitBox의 OnComponentBeginOverlap 이벤트에 등록하는 함수입니다. */
	virtual void OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

public:
	/** HitActors를 비우는 함수입니다. */
	void ClearHitActors(TMap<AActor*, bool>& NewHitActors);

protected:
	/**
	 * 입력받은 인자가 피격된 액터인지 확인하는 함수입니다.
	 *
	 * @param NewHitActors 피격된 액터들을 보관한 Map입니다. 
	 * @param NewActor 피격됐는지 확인할 액터입니다.
	 * @return 피격된 액터일 경우 true를 아닐 경우 false를 반환합니다.
	 */
	bool IsHitActor(const TMap<AActor*, bool>& NewHitActors, const AActor& NewActor) const;
	
	/**
	 * 입력받은 인자가 대미지를 받은 액터인지 확인하는 함수입니다.
	 *
	 * @param NewHitActors 대미지를 받았는지 확인할 액터들을 보관한 Map입니다. 
	 * @param NewActor 대미지를 받았는지 확인할 액터입니다.
	 * @return 대미지를 받은 액터일 경우 true를 아닐 경우 false를 반환합니다.
	 */
	bool IsTakeDamageActor(const TMap<AActor*, bool>& NewHitActors, const AActor& NewActor) const;
	
	/**
	 * 입력받은 인자에게 대미지를 주는 함수입니다.
	 *
	 * @param NewHitActors 대미지를 준 액터를 보관할 Map입니다.
	 * @param NewHitActor 대미지를 줄 액터입니다.
	 */
	void ApplyDamage(TMap<AActor*, bool>& NewHitActors, AActor* NewHitActor);
	
protected:
	/**
	 * OnBeginOverlap에 걸린 액터를 Key로 가지고 Key인 액터가 대미지를 받았는지를 나타내는 값을 Value로 저장되어 있는 Map입니다.
	 *
	 * @Key AttackTrace에 걸린 액터입니다.
	 * @Value 대미지를 받았는지 나타내는 값입니다.
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "HitBox")
	TMap<AActor*, bool> HitActors;
#pragma endregion
};
