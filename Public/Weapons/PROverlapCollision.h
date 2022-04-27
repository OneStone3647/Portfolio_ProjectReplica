// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "GameFramework/Actor.h"
#include "PROverlapCollision.generated.h"

/**
 * 일정 범위 안에 존재하는(Overlap되는) Actor를 찾아 대미지를 주는 Actor 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APROverlapCollision : public AActor
{
	GENERATED_BODY()
	
public:	
	APROverlapCollision();

protected:
	virtual void BeginPlay() override;

#pragma region Debug
public:
	/**
	 * Debug를 하기 위해 OverlapCollision들을 게임에서 그릴지 설정하는 함수입니다.
	 * @param bFlag true일 경우 OverlapCollision들을 게임에서 그립니다. false일 경우 그리지 않습니다.
	 */
	UFUNCTION()
	void DrawDebugOverlapCollision(bool bFlag);

protected:
	/** OverlapCollision의 Debug를 출력을 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PROverlapCollision|Debug")
	bool bDrawDebug;

	/** Debug를 출력할 시간입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRBaseWeapon|Debug")
	float DrawDebugTime;

	/** OverlapCollision에 닿은 곳을 표시하는 DebugPoint의 크기입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRBaseWeapon|Debug")
	float DrawDebugPointSize;

public:
	/**
	 * 입력받은 인자 값으로 bDrawDebug를 설정하는 함수입니다.
	 * @param bFlag bDrawDebug로 설정할 값입니다.
	 */
	UFUNCTION()
	void SetDrawDebug(bool bFlag);
#pragma endregion 

#pragma region PRCharacterReference
public:
	/**
	 * PROwner를 초기화하는 함수입니다.
	 * @param PRBaseCharacter PROwner로 설정하는 PRBaseCharacter 클래스입니다.
	 */
	void InitializePROwner(class APRBaseCharacter* PRBaseCharacter);
	
protected:
	/** 이 BoxComponent를 소유하는 PRBaseCharacter입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PROverlapCollision|PRCharacterReference")
	class APRBaseCharacter* PROwner;
#pragma endregion 

#pragma region OverlapCollision
public:
	/**
	 * OverlapEvent를 활성화하는 함수입니다.
	 * @param bFlag true일 경우 OverlapEvent를 활성화하고 false일 경우 OverlapEvent를 비활성화합니다.
	 */
	void ExecuteOverlap(bool bFlag);

	/**
	 * OverlapEvent를 활성화했는지 나타내는 함수입니다.
	 * @return 활성화했을 경우 true를 비활성화했을 경우 false를 반환합니다.
	 */
	bool IsExecuteOverlap() const;
	
protected:
	/** OverlapEvent에 사용하는 BoxComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PROverlapCollision|OverlapCollision")
	class UBoxComponent* OverlapCollision;
#pragma endregion 
	
#pragma region AttackOverlap
public:
	/** HitActors Map을 비우는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PROverlapCollision|Trace")
	void ClearHitActors();
	
protected:
	/** OverlapEvent에 등록하여 Overlap이 발생하였을 때 Overlap되는 대상을 공격하는 함수입니다. */
	UFUNCTION()
	void AttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
	 * 인자로 받은 액터가 피격된 액터인지 판별하는 함수입니다.
	 * @param HitedActor 값을 가져올 Key 값입니다.
	 * @return HitActors Map에 NewActor가 존재할 경우 true를, 존재하지 않을 경우 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PROverlapCollision|Overlap")
	bool IsHitActor(const AActor* HitedActor) const;

	/**
	 * 인자로 받은 액터가 대미지를 받은 액터인지 판별하는 함수입니다.
	 * @param HitedActor 값을 가져올 Key 값입니다.
	 * @return HitActors Map에 NewActor가 존재할 경우 true(값을 포인터로 가져오고)를, 존재하지 않을 경우 false(nullptr)를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PROverlapCollision|Overlap")
	bool IsTakeDamageActor(const AActor* HitedActor) const;

	/**
	 * 인자로 받은 액터에게 대미지를 주는 함수입니다.
	 * @param HitedActor 대미지를 줄 액터입니다. 
	 */
	UFUNCTION(BlueprintCallable, Category = "PROverlapCollision|Overlap")
	void ApplyDamage(AActor* HitedActor);
	
protected:
	/**
	 * AttackTrace에 걸린 액터를 Key로 가지고 Key인 액터가 대미지를 받았는의 여부를 Value로 저장되어 있는 Map입니다.
	 * @Key: AttackTrace에 걸린 액터입니다.
	 * @Value: 대미지를 받은지의 여부입니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PROverlapCollision|Overlap")
	TMap<AActor*, bool> HitActors;
#pragma endregion 
};
