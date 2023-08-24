// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "Components/TimelineComponent.h"
#include "PRAnimSystemComponent.generated.h"

/**
 * 캐릭터가 재생하는 PRAnimMontage를 관리하고 재생하는 ActorComponent입니다.
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
	 * Curve가 사용하는 Callback 함수에 MoveLerp함수를 바인드합니다.
	 * Timeline에 DefaultMoveLerpFloatCurve를 추가합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRAnimSystem|MoveLerp")
	void InitializeMoveLerpTimeline();
	
	/**
	 * 캐릭터를 선형보간 이동시키는 함수입니다.
	 * @param Value Curve에 의해 받는 값입니다.
	 */
	UFUNCTION()
	void MoveLerp(float Value);

	/** 캐릭터를 선형보간 이동을 마친후 호출하는 함수입니다. */
	UFUNCTION()
	void MoveLerpFinish();
	
protected:
	/** Curve가 사용할 Callback 함수입니다. */
	FOnTimelineFloat MoveLerpTimelineProgress;

	/** Timeline이 끝난 후 호출하는 Callback 함수입니다. */
	FOnTimelineEvent MoveLerpTimelineFinish;
	
	/** 캐릭터가 선형보간 이동할 때 사용하는 Timeline입니다. */
	FTimeline MoveLerpTimeline;

	/** 선형보간 이동에 기본으로 사용하는 Curve입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoveLerp")
	UCurveFloat* DefaultMoveLerpFloatCurve;

	/** 캐릭터의 현재 위치입니다. */
	UPROPERTY(BlueprintReadWrite, Category = "MoveLerp")
	FVector CurrentOwnerLocation;

public:
	/** MoveLerpTimeline을 반환하는 함수입니다. */
	FTimeline& GetMoveLerpTimeline();
#pragma endregion

#pragma region DataTable
private:
	/** 캐릭터가 사용하는 PRAnimMontage들을 보관한 데이터 테이블입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	UDataTable* PRAnimMontageDataTable;
#pragma endregion 

#pragma region PRAnimMontage
public:
	/**
	 * PRAnimMontage를 재생하고 있는지 판별하는 함수입니다.
	 * @return PRAnimMontage를 재생하고 있으면 true를 아닐 경우 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRAnimSystem|PRAnimMontage")
	bool IsPlayPRAnimMontage() const;
		
	/**
	 * PRAnimMontage를 재생하는 함수입니다.
	 * 
	 * @param NewPRAnimMontage 재생할 PRAnimMontage입니다.
	 * @param PlayDirectionVector 재생할 방향을 나타내는 벡터입니다. 매개변수를 넣지 않으면 캐릭터의 전방벡터를 사용합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRAnimSystem|PRAnimMontage")
	void PlayPRAnimMontage(FPRAnimMontage NewPRAnimMontage, FVector PlayDirectionVector = FVector::ZeroVector);

	/** PRAnimMontage를 정지하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PRAnimSystem|PRAnimMontage")
	void StopPRAnimMontage();

	/**
	 * 데이터 테이블에서 ID에 해당하는 PRAnimMontage를 가져오는 함수입니다.
	 *
	 * @param NewDataTable ID로 탐색할 데이터 테이블입니다.
	 * @param ID 찾고자 하는 ID입니다.
	 * @return ID에 해당하는 PRAnimMontage를 가져온 배열입니다
	 */
	UFUNCTION(BlueprintCallable, Category = "PRAnimSystem|PRAnimMontage")
	static FPRAnimMontage GetPRAnimMontageFromDataTableByID(UDataTable* NewDataTable, const int32 ID);

	/**
	 * PRAnimMontageDataTable에서 ID에 해당하는 PRAnimMontage를 가져오는 함수입니다.
	 * 
	 * @param ID 찾고자 하는 ID입니다.
	 * @return ID에 해당하는 PRAnimMontage를 가져온 배열입니다
	 */
	UFUNCTION(BlueprintCallable, Category = "PRAnimSystem|PRAnimMontage")
	FPRAnimMontage GetPRAnimMontageFromPRAnimMontageDataTableByID(const int32 ID); 
	
	/**
	 * 데이터 테이블의 정보를 배열로 가져오는 함수입니다.
	 * 
	 * @param NewDataTable 배열로 가져올 데이터 테이블입니다.
	 * @return 데이타 테이블의 정보를 가져온 배열입니다
	 */
	UFUNCTION(BlueprintCallable, Category = "PRAnimSystem|PRAnimMontage")
	static TArray<FPRAnimMontage> GetPRAnimMontageFromDataTableToArray(UDataTable* NewDataTable);
	
	/**
 	 * 데이터 테이블에서 ID 범위 내의 정보를 배열로 가져오는 함수입니다.
	 * 
	 * @param NewDataTable 배열로 가져올 데이터 테이블입니다.
	 * @param FromID 시작 ID입니다.
	 * @param ToID 끝 ID입니다.
	 * @return 데이타 테이블의 정보를 가져온 배열입니다
	 */
	UFUNCTION(BlueprintCallable, Category = "PRAnimSystem|PRAnimMotnage")
	static TArray<FPRAnimMontage> GetPRAnimMontageFromDataTableByIDRangeToArray(UDataTable* NewDataTable, const int32 FromID, const int32 ToID);

	/**
	 * PRAnimMontageDataTable에서 ID 범위 내의 정보를 배열로 가져오는 함수입니다.
	 * 
	 * @param FromID 시작 ID입니다.
	 * @param ToID 끝 ID입니다.
	 * @return 데이타 테이블의 정보를 가져온 배열입니다
	 */
	UFUNCTION(BlueprintCallable, Category = "PRAnimSystem|PRAnimMotnage")
	TArray<FPRAnimMontage> GetPRAnimMontageFromPRAnimMontageDataTableByIDRangeToArray(const int32 FromID, const int32 ToID);

	/**
	 * 데이터 테이블의 정보를 Map으로 가져오는 함수입니다.
	 * Key값은 데이터 테이블의 행이름입니다.
	 * 
	 * @param NewDataTable Map으로 가져올 데이터 테이블입니다.
	 * @return 데이타 테이블의 정보를 가져온 Map입니다
	 */
	UFUNCTION(BlueprintCallable, Category = "PRAnimSystem|PRAnimMontage")
	static TMap<FName, FPRAnimMontage> GetPRAnimMontageFromDataTableToMap(UDataTable* NewDataTable);

	/**
  * 데이터 테이블에서 ID 범위 내의 정보를 Map으로 가져오는 함수입니다.
	 * 
	 * @param NewDataTable Map으로 가져올 데이터 테이블입니다.
	 * @param FromID 시작 ID입니다.
	 * @param ToID 끝 ID입니다.
	 * @return 데이타 테이블의 정보를 가져온 Map입니다
	 */
	UFUNCTION(BlueprintCallable, Category = "PRAnimSystem|PRAnimMotnage")
	static TMap<FName, FPRAnimMontage> GetPRAnimMontageFromDataTableByIDRangeToMap(UDataTable* NewDataTable, const int32 FromID, const int32 ToID);

	/**
	 * PRAnimMontageDataTable에서 ID 범위 내의 정보를 Map으로 가져오는 함수입니다.
	 * 
	 * @param FromID 시작 ID입니다.
	 * @param ToID 끝 ID입니다.
	 * @return 데이타 테이블의 정보를 가져온 Map입니다
	 */
	UFUNCTION(BlueprintCallable, Category = "PRAnimSystem|PRAnimMotnage")
	TMap<FName, FPRAnimMontage> GetPRAnimMontageFromPRAnimMontageDataTableByIDRangeToMap(const int32 FromID, const int32 ToID);

	// /**
	//  * 데이터 테이블의 정보를 Map으로 가져오는 함수입니다.
	//  * Key값은 데이터 테이블의 행이름(PRAnimMontage의 ID 값)입니다.
	//  * 
	//  * @param NewDataTable Map으로 가져올 데이터 테이블입니다.
	//  * @return 데이타 테이블의 정보를 가져온 Map입니다
	//  */
	// UFUNCTION(BlueprintCallable, Category = "PRAnimSystem|PRAnimMontage")
	// static TMap<int32, FPRAnimMontage> GetPRAnimMontageFromDataTableToMap(UDataTable* NewDataTable);
	
protected:
	/** 현재 재생하는 PRAnimMontage입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRAnimMontage")
	FPRAnimMontage CurrentPlayPRAnimMontage;

	/** CurrentPlayPRAnimMontage를 재생할 방향입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRAnimMontage")
	FVector CurrentPlayDirectionVector;

public:
	/** CurrentPRPlayerAnimMontage를 반환하는 함수입니다. */
	FPRAnimMontage GetCurrentPlayPRAnimMontage() const;
#pragma endregion

#pragma region PRAttackAnimMontage
public:
	// /** 인자로 받은 PRAttackAnimMontage의 Index를 초기화하는 함수입니다. */	
	// UFUNCTION(BlueprintCallable, Category = "PRAnimSystem|PRAttackAnimMontage")
	// void InitializePRAttackAnimMontageIndex(FPRAttackAnimMontage& NewPRAttackAnimMontage);
	//
	// /** 인자로 받은 PRAttackAnimMontage의 Index를 1증가 시키는 함수입니다. */
	// UFUNCTION(BlueprintCallable, Category = "PRAnimSystem|PRAttackAnimMontage")
	// void IncreasePRAttackAnimMontageIndex(FPRAttackAnimMontage& NewPRAttackAnimMontage);
#pragma endregion
};
