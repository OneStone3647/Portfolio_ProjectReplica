// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "Components/TimelineComponent.h"
#include "Structs/Struct_PRAnimMontage.h"
#include "PRAnimSystemComponent.generated.h"

/**
 * 캐릭터의 애니메이션을 관리하는 ActorComponent 클래스입니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTREPLICA_API UPRAnimSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()

public:
	UPRAnimSystemComponent();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region MoveLerp
protected:
	/**
	 * MoveLerpTimeline을 초기화하는 함수입니다.
	 * MoveLerpTimeline가 사용하는 Callback 함수에 사용할 함수르 바인드합니다.
	 * MoveLerpTimeline에 Curve를 추가합니다.
	 */
	void InitializeMoveLerpTimeline();

	/**
	 * 캐릭터를 선형보간 이동시키는 함수입니다.
	 * @Value: Curve에 의해 받는 값입니다.
	 */
	UFUNCTION()
	void MoveLerp(float Value);

protected:
	/** PRAnimMontage를 재생하며 캐릭터가 선형보간 이동할 때 사용하는 Timeline입니다. */
	FTimeline MoveLerpTimeline;

	/** 선형보간 이동에 기본으로 사용하는 CurveFloat입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRAnimSystem|MoveLerp")
	UCurveFloat* DefaultMoveLerpCurveFloat;

	/** PROwner의 현재 위치입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "PRAnimSystem|MoveLerp")
	FVector OwnerCurrentLocation;
#pragma endregion
	
#pragma region PRAnimMontage
public:	
	/**
	 * PRAnimMontage를 재생하는 함수입니다.
	 * @NewPRAnimMontage: 재생할 PRAnimMontage입니다.
	 * @PlayDirectionVector: 재생할 방향을 나타내는 벡터입니다. 인자를 넣지 않으면 캐릭터의 전방벡터를 사용합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRAnimSystem|PRAnimMontage")
	void PlayPRAnimMontage(FPRAnimMontage NewPRAnimMontage, FVector PlayDirectionVector = FVector::ZeroVector);

	/** 현재 재생중인 PRAnimMontage를 중지하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRAnimSystem|PRAnimMontage")
	void StopPRAnimMontage();
	
protected:
	/** 현재 재상하는ㄴ PRAnimMontage입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRAnimsystem|PRAnimMontage")
	FPRAnimMontage CurrentPRAnimMontage;

	/** PRAnimMontage를 재생할 방향입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "PRAnimmSystem|PRAnimMontage")
	FVector DirectionVector;
#pragma endregion

#pragma region AnimMontage
protected:
	/** 피격 AnimMontage입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRAnimSystem|AnimMontage")
	UAnimMontage* HitAnimMontage;

	/** 사망 AnimMontage입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRAnimSystem|AnimMontage")
	UAnimMontage* DeadAnimMontage;

public:
	UAnimMontage* GetHitAnimMontage() const;
#pragma endregion 
};
