// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/BoxComponent.h"
#include "PRTraceCollision.generated.h"

/**
 * Trace에 사용하는 BoxComponent인 TraceCollision과 TraceCollision의 이전 Location을 가진 구조체입니다.
 * @Atomic: 이 구조체를 항상 하나의 단위로 직렬화되게 합니다. 이 구조체를 디테일 패널에서 사용할 수 있게 됩니다.
 *			구조체 내부 변수들의 UPROPERTY 매크로에 EditAnywhere를 작성해야 합니다.
 * @BlueprintType: 이 구조체를 블루프린트에서 사용할 수 있습니다.
 *				   구조체 내부 변수들의 UPROPERTY 매크로에 BlueprintReadWrite를 작성해야 합니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRTraceCollisionStruct
{
	GENERATED_BODY()

public:
	FPRTraceCollisionStruct()
		: TraceCollision(nullptr), PreviousLocation(FVector::ZeroVector)
	{}

	FPRTraceCollisionStruct(UBoxComponent* NewTraceCollision, FVector NewPreviousLocation)
		: TraceCollision(NewTraceCollision), PreviousLocation(NewPreviousLocation)
	{}
	
public:
	/** Trace에 사용할 BoxComponent입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TraceCollision")
	UBoxComponent* TraceCollision;

	/** TraceCollision의 이전 Location입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TraceCollision")
	FVector PreviousLocation;
};

/**
 * Trace에 사용되는 BoxComponent를 관리하는 BoxComponent 클래스입니다.
 * 하위에 존재하는 BoxComponent를 Trace에서 사용하고 이 BoxComponent는 Trace에서 사용하지 않습니다.
 * 이 BoxComponent는 하위에 존재하는 동적으로 생성된 BoxComponent의 대략적인 범위를 나타냅니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTREPLICA_API UPRTraceCollision : public UBoxComponent
{
	GENERATED_BODY()

public:
	UPRTraceCollision();
	
protected:
	virtual void BeginPlay() override;

#pragma region Debug
public:
	/**
	 * Debug를 하기 위해 TraceCollision들을 게임에서 그릴지 설정하는 함수입니다.
	 * @bFlag: true일 경우 TraceCollision들을 게임에서 그립니다. false일 경우 그리지 않습니다.
	 */
	void DrawDebugTraceCollision(bool bFlag);
#pragma endregion

#pragma region TraceCollision
public:
	/** TraceCollisionStructArray에 저장된 PRTraceCollisionStruct들의 TraceCollision의 Location을 각각의 PreviousLocation에 저장하여 최신화 하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRTraceCollsion|TraceCollision")
	void UpdateTraceCollisionsLocation();

private:
	/**
	 * Trace에 사용되는 TraceCollision들을 생성하고 Array에 저장하는 함수입니다.
	 * TraceCollisionTop을 새로운 PRTraceCollisionStruct의 TraceCollision에 저장하고 TraceCollisionStructArray애 저장합니다.
	 * TraceCollisionTop과 TraceCollisionBottom 사이에 TraceCollision을 CreateCollisionCount만큼 일정한 간격으로 생성하고
	 * TraceCollisionStructArray에 저장합니다.
	 * TraceCollisionBottom을 새로운 PRTraceCollisionStruct의 TraceCollision에 저장하고 TraceCollisionStructArray에 저장합니다.
	 */
	void GenerateTraceCollisions();

	/** TraceCollision간의 Offset(중간점)을 계산하는 함수입니다. */
	float CalculateOffsetBetweenTraceCollisions() const;

	/**
	 * TraceCollision을 생성할 때 생성되는 순서에 따라 사용할 Offset과 Transform을 계산하는 함수입니다.
	 * @param TraceCollisionIndex 생성된 TraceCollision의 순서입니다.
	 */
	FTransform CalculateTraceCollisionSpawnOffsetTransform(int32 TraceCollisionIndex) const;

private:
	/** 무기의 날 끝에 위치하는 TraceCollision입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRTraceCollsion|TraceCollision", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* TraceCollisionTop;

	/** 무기의 날 밑에 위치하는 TraceCollision입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRTraceCollsion|TraceCollision", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* TraceCollisionBottom;

	/** PRTraceCollisionStruct들을 저장한 Array입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRTraceCollsion|TraceCollision", meta = (AllowPrivateAccess = "true"))
	TArray<FPRTraceCollisionStruct> TraceCollisionStructArray;

	/** TraceCollisionTop과 TraceCollisionBottom 사이에 생성할 TraceCollision의 수입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRTraceCollsion|TraceCollision", meta = (AllowPrivateAccess = "true"))
	int32 CreateCollisionCount;
	
public:
	/** TraceCollisionStructArray를 반환하는 함수입니다. */
	FORCEINLINE TArray<FPRTraceCollisionStruct> GetTraceCollisionStructArray() const
	{
		return TraceCollisionStructArray;
	}
#pragma endregion
};
