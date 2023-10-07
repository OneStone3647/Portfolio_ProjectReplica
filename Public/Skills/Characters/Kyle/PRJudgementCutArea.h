// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Objects/PRPooledObject.h"
#include "PRJudgementCutArea.generated.h"

class UNiagaraComponent;
class USphereComponent;

/**
 * JudgementCut 스킬의 범위에 대미지를 주는 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRJudgementCutArea : public APRPooledObject
{
	GENERATED_BODY()

public:
	APRJudgementCutArea();

protected:
	virtual void BeginPlay() override;
	
protected:
	virtual void UpdatePooledObject_Implementation(float DeltaTime) override;
	virtual void InitializeSpawnLocation_Implementation() override;
	virtual void Activate_Implementation() override;

protected:
	/** DamageArea에 존재하는 Target들에게 대미지를 주는 함수입니다. */
	void ActivateDamageArea();

	/** DamageArea를 최신화하는 함수입니다. */
	void UpdateDamageArea(float DeltaTime);
	
	/** Target의 바닥의 위치를 반환하는 함수입니다. */
	FVector FindFloorLocation(AActor* Target) const;

	/**
	 * 두 Target 중 ObjectOwner와 가장 가까운 Target을 반환하는 함수입니다.
	 *
	 * @return 비교한 TargetA, TargetB 중 ObjectOwner와 가장 가까운 Target입니다.
	 */
	AActor* GetNearestTarget(AActor* TargetA, AActor* TargetB);

	/** Target을 DamageArea로 끌어당기는 함수입니다. */
	void PullTargetToDamageArea(AActor* Target);

	/**
	 * 입력받은 SoundAsset을 사용하는 AudioComponent를 생성하고 RootComponent에 부착하는 함수입니다.
	 *
	 * @param SoundAsset 생성한 AudioComponent에서 사용할 사운드입니다.
	 * @param GenerateIndex 생성한 AudioComponent의 Index입니다.
	 * @return 생성한 AudioComponent입니다.
	 */
	UAudioComponent* GenerateHitSound(USoundBase* SoundAsset, int32 GenerateIndex = 0);

	/** Index에 해당하는 HitSound를 반환하는 함수입니다. */
	UAudioComponent* GetHitSound(int32 Index) const;

private:
	/** 대미지를 주는 범위입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRJudgementCutArea", meta = (AllowPrivateAccess = "true"))
	USphereComponent* DamageArea;
	
	/** JudgementCut의 Effect입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRJudgementCutArea", meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* JudgementCutEffect;

	/** 대미지를 입혔을 때 재생하는 사운드입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRJudgementCutArea", meta = (AllowPrivateAccess = "true"))
	USoundBase* HitSoundSource;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRJudgementCutArea", meta = (AllowPrivateAccess = "true"))
	TArray<UAudioComponent*> HitSounds;

	/** Debug 실행여부입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRJudgementCutArea", meta = (AllowPrivateAccess = "true"))
	bool bActivateDebug;

	/** Target에게 줄 대미지입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRJudgementCutArea", meta = (AllowPrivateAccess = "true"))
	float Damage;

	/** 대미지를 주는 시간의 간격입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRJudgementCutArea", meta = (AllowPrivateAccess = "true"))
	float DamageInterval;

	/** TimeStop 상태일 때 DeltaTime의 배율입니다.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRJudgementCutArea", meta = (AllowPrivateAccess = "true"))
	float TimeStopTimeDilation;

	/** DamageArea에 존재하는 Target들에게 대미지를 줄 것인지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRJudgementCutArea", meta = (AllowPrivateAccess = "true"))
	bool bActivateDamageArea;

	/** 재사용 대기시간의 경과 시간입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRJudgementCutArea", Transient, meta = (AllowPrivateAccess = "true"))
	float CooldownElapsed;

	/** DamageArea에 존재하는 Target들에게 대미지를 입일 최대 횟수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRJudgementCutArea", meta = (AllowPrivateAccess = "true"))
	int32 MaxDamageCount;

	/** DamageArea에 존재하는 Target들에게 대미지를 입힌 횟수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PRJudgementCutArea", Transient, meta = (AllowPrivateAccess = "true"))
	int32 DamageCount;

	/** 가장 가까운 곳에 있는 Target입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRJudgementCutArea", Transient, meta = (AllowPrivateAccess = "true"))
	AActor* NearestTarget;	

	/** Spawn할 위치를 나타내는 구체의 반지름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRJudgementCutArea", meta = (AllowPrivateAccess = "true"))
	float SpawnRadius;

	/** Target이 없을 경우 Spawn할 캐릭터 정면으로부터의 거리입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRJudgementCutArea", meta = (AllowPrivateAccess = "true"))
	float DefaultSpawnDistance;	
		
	/** FindFloorLocation 함수에서 Target에서 바닥까지 LineTrace를 실행하는 거리입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRJudgementCutArea", meta = (AllowPrivateAccess = "true"))
	float FindFloorRange;

	/** Target을 끌어당기는 힘입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRJudgementCutArea", meta = (AllowPrivateAccess = "true"))
	float TargetPullForce;
};
