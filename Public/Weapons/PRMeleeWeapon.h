// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/BoxComponent.h"
#include "Weapons/PRBaseWeapon.h"
#include "PRMeleeWeapon.generated.h"

class UBoxComponent;

/** Trace에 사용하는 Point와 이전 Location을 변수로 가진 구조체입니다. */
USTRUCT(Atomic, BlueprintType)
struct FPRTracePoint
{
	GENERATED_BODY()

public:
	FPRTracePoint()
		: Point(nullptr)
		, PreviousLocation(FVector::ZeroVector)
	{}

	FPRTracePoint(UBoxComponent* NewPoint, FVector NewPreviousLocation)
		: Point(NewPoint)
		, PreviousLocation(NewPreviousLocation)
	{}


public:
	/** Trace에 사용하는 BoxComponent입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TracePoint")
	UBoxComponent* Point;

	/** TraceCollision의 이전 Location입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TracePoint")
	FVector PreviousLocation;

public:
	/** TraceCollision의 이전 Location을 최신화하는 함수입니다. */
	void UpdatePreviousLocation()
	{
		if(IsValid(Point) == true)
		{
			PreviousLocation = Point->GetComponentLocation();
		}
	}
};

/** 나열해 놓은 TracePoint를 Array로 보관한 구조체입니다. */
USTRUCT(Atomic, BlueprintType)
struct FPRTraceLine
{
	GENERATED_BODY()
	
public:
	/** 나열한 TracePoint를 보관한 Array입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TraceLine")
	TArray<FPRTracePoint> TracePoints;
};

/** TraceLine의 생성에 필요한 정보를 보관한 구조체입니다. */
USTRUCT(Atomic, BlueprintType)
struct FPRTraceLineGenerateInfo
{
	GENERATED_BODY()
	
public:
	FPRTraceLineGenerateInfo()
		: TopTracePointSocketName(NAME_None)
		, BottomTracePointSocketName(NAME_None)
		, TracePointGenerateCount(3)
	{}

	FPRTraceLineGenerateInfo(FName NewTopTracePointSocketName, FName NewBottomTraceSocketName, int32 NewTracePointGenerateCount)
		: TopTracePointSocketName(NewTopTracePointSocketName)
		, BottomTracePointSocketName(NewBottomTraceSocketName)
		, TracePointGenerateCount(NewTracePointGenerateCount)
	{}
	
public:
	/** Trace에 사용하는 무기의 날 위 지점의 Socket의 이름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TracePointSocketName")
	FName TopTracePointSocketName;

	/** Trace에 사용하는 무기의 날 아래 지점의 Socket의 이름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TracePointSocketName")
	FName BottomTracePointSocketName;

	/** TopTracePointSocket과 BottomTracePointSocket 사이에 생성할 TracePoint의 개수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TracePointSocketName", meta = (ClampMin = "0"))
	int32 TracePointGenerateCount;
};

/**
 * 근접 전투에서 사용하는 무기 Actor 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRMeleeWeapon : public APRBaseWeapon
{
	GENERATED_BODY()

public:
	APRMeleeWeapon();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

#pragma region Debug
protected:
	/** 디버그를 최신화하는 함수입니다. */
	virtual void UpdateTraceDebug();

protected:
	/** Trace 디버그 활성화를 나타내는 변수입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRMeleeWeapon|Debug")
	bool bActivateTraceDebug;

public:
	/** 입력받은 인자로 bActivateTraceDebug를 설정하는 함수입니다. */
	void SetActivateTraceDebug(bool bNewActivateTraceDebug);
#pragma endregion

#pragma region TracePoint
protected:
	/**
	 * TraceLine을 생성하는 함수입니다.
	 *
	 * @param NewWeaponStaticMesh TraceLine을 부착할 무기의 외형입니다.
	 * @param NewTraceLineGenerateInfo 생성할 TraceLine의 정보입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRMeleeWeapon|TarcePoint")
	TArray<FPRTraceLine> GenerateTraceLines(UStaticMeshComponent* NewWeaponStaticMesh, const TArray<FPRTraceLineGenerateInfo>& NewTraceLineGenerateInfo);
	
	/**
	 * 무기의 외형에 입력받은 소켓의 위치의 사이에 설정한 개수의 TracePoint를 생성하고 Array에 보관한 후 모든 TracePoint를 보관한 Array를 반환하는 함수입니다.
	 *
	 * @param NewWeaponStaticMesh TracePoint를 부착할 무기의 외형입니다.
	 * @param NewTraceLineIndex 입력받은 소켓의 사이에 생성한 TracePoint를 구분하기 위한 Index 값입니다.
	 * @param NewTopTracePointSocketName TracePoint를 생성하기 위한 무기의 외형의 가장 윗부분의 Socket의 이름입니다.
	 * @param NewBottomTracePointSocketName	TracePoint를 생성하기 위한 무기의 외형의 가장 아랫부분의 Socket의 이름입니다.
	 * @param NewTracePointGenerateCount 입력받은 소켓의 사이에 생성할 TracePoint의 개수입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRMeleeWeapon|TarcePoint")
	TArray<FPRTracePoint> GenerateTracePoints(UStaticMeshComponent* NewWeaponStaticMesh, int32 NewTraceLineIndex, FName NewTopTracePointSocketName, FName NewBottomTracePointSocketName, int32 NewTracePointGenerateCount);

	/**
	 *  TracePoint를 생성하고 반환하는 함수입니다.
	 *
	 *	@param NewWeaponStaticMesh TracePoint를 부착할 무기의 외형입니다.
	 *  @param NewTracePointName 생성한 TracePoint의 이름입니다. 
	 *  @param NewTracePointRelativeLocation TracePoint를 생성할 위치입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "PRMeleeWeapon|TracePoint")
	FPRTracePoint GenerateTracePoint(UStaticMeshComponent* NewWeaponStaticMesh, FName NewTracePointName, FVector NewTracePointRelativeLocation);

	/**
	 * 생성된 TracePoint가 사용할 Offset을 적용한 Location을 계산하는 함수입니다.
	 * 
	 * @param TracePointFrom 첫 번째 TracePoint입니다.
	 * @param TracePointTo 두 번째 TracePoint입니다.
	 * @param NewOffsetBetweenTracePoints 두 TracePoint 사이의 Offset입니다.
	 * @return 생성된 TracePoint가 사용할 Location입니다.
	 */
	FVector CalculateTracePointSpawnOffsetLocation(FVector TracePointFrom, FVector TracePointTo, int32 NewOffsetBetweenTracePoints) const;

	/**
	 * 두 TracePoint 사이의 Offset을 계산하는 함수입니다.
	 *
	 * @param TracePointFrom 첫 번째 TracePoint입니다.
	 * @param TracePointTo 두 번째 TracePoint입니다.
	 * @param NewGenerateTracePointCount 두 TracePoint 사이에 생성하는 TracePoint의 개수입니다. 
	 * @return 두 TracePoint 사이의 Offset입니다.
	 */
	float CalculateOffsetBetweenTracePoints(FVector TracePointFrom, FVector TracePointTo, int32 NewGenerateTracePointCount) const;

protected:
	/** MainWeapon의 TraceLine의 생성에 사용하는 정보를 가진 구조체를 보관한 Array입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRMeleeWeapon|TracePoint")
	TArray<FPRTraceLineGenerateInfo> MainWeaponTraceLineGenerateInfo;	
	
	/** MainWeapon에서 생성한 TraceLine들을 보관한 Array입니다. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "PRMeleeWeapon|TracePoint")
	TArray<FPRTraceLine> MainWeaponTraceLines;

public:
	/** MainWeaponTraceLines를 반환하는 함수입니다. */
	TArray<FPRTraceLine> GetMainWeaponTraceLines() const;

	/** MainWeaponTraceLines를 레퍼런스로 반환하는 함수입니다. */
	TArray<FPRTraceLine>& GetMainWeaponTraceLinesReference();
#pragma endregion

#pragma region Trace
public:
	/**
	 * MainWeapon의 AttackTraceLine을 활성화하는 함수입니다.
	 *
	 * @param bNewActivateTraceDebug TraceDebug의 실행을 결정합니다.
	 */
	void ActivateMainWeaponAttackTraceLine(bool bNewActivateTraceDebug = false);
	
	/**
	 * 입력받은 TraceLine들에 존재하는 TracePoint들의 PreviousLocation을 최신화하는 함수입니다.
	 *
	 * @param NewTraceLines 최신화할 TraceLine들입니다.
	 */
	void UpdateTracePointsPreviousLocation(TArray<FPRTraceLine>& NewTraceLines);

	/**
	 * PROwner를 기준으로 SphereTrace를 실행하여 히트된 액터에 대미지를 주는 함수입니다.
	 *
	 * @param NewWeaponPosition 사용한 무기의 위치입니다.
	 * @param NewHitActors Hit된 액터들을 보관한 Array입니다.
	 * @param NewSphereTraceSize SphereTrace의 크기입니다.
	 * @param bNewActivateTraceDebug TraceDebug의 실행을 결정합니다.
	 */
	void ActivateSphereAttackTraceByOwner(EPRWeaponPosition NewWeaponPosition, TMap<AActor*, bool>& NewHitActors, float NewSphereTraceSize, bool bNewActivateTraceDebug = false);

	/** HitActors를 비우는 함수입니다. */
	void ClearHitActors(TMap<AActor*, bool>& NewHitActors);

protected:
	/**
	 * TracePoint를 기준으로 LineTrace를 실행하여 히트된 액터에 대미지를 주는 함수입니다.
	 *
	 * @param NewTraceLines LineTrace를 실행하는 TraceLine들을 보관한 Array입니다.
	 * @param NewHitActors Hit된 액터들을 보관한 Array입니다.
	 * @param bNewActivateTraceDebug TraceDebug의 실행을 결정합니다.
	 */
	void ActivateAttackTraceLine(TArray<FPRTraceLine> NewTraceLines, TMap<AActor*, bool>& NewHitActors, bool bNewActivateTraceDebug = false);

private:
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
	 * AttackTrace에 걸린 액터를 Key로 가지고 Key인 액터가 대미지를 받았는지를 나타내는 값을 Value로 저장되어 있는 Map입니다.
	 *
	 * @Key AttackTrace에 걸린 액터입니다.
	 * @Value 대미지를 받았는지 나타내는 값입니다.
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "PRMeleeWeapon|Trace")
	TMap<AActor*, bool> MainWeaponHitActors;

public:
	/** MainWeaponHitActors를 반환하는 함수입니다. */
	TMap<AActor*, bool>& GetMainWeaponHitActors();
#pragma endregion

#pragma region Effect
public:
	/**
	 * 사용한 무기의 종류에 따라 입력받은 위치에 HitEffect를 Spawn하는 함수입니다.
	 *
	 * @param NewWeaponPosition 사용한 무기의 위치입니다.
	 * @param NewHitLocation HitEffect를 Spawn할 위치입니다.
	 */
	virtual void SpawnHitEffectByWeaponPosition(EPRWeaponPosition NewWeaponPosition, FVector NewHitLocation);

	/**
	 * 입력받은 위치에 HitEffect를 사용한 무긔 회전값을 적용하여 Spawn하는 함수입니다.
	 *
	 * @param NewWeaponRotator 사용한 무기의 회전값입니다.
	 * @param NewHitLocation HitEffect를 Spawn할 위치입니다.
	 */
	void SpawnHitEffect(FRotator NewWeaponRotator, FVector NewHitLocation);

protected:
	/** 대미지 받은 액터를 타격한 위치에 Spawn하는 NiagaraSystem입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRMeleeWeapon|Effect")
	UNiagaraSystem* HitEffect;

	// /** 대미지를 받은 액터를 타격한 위치에 Spawn하는 HitEffect의 이름입니다.*/
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRMeleeWeapon|Effect")
	// FName HitEffectName;

	/** HitEffect의 기본 회전값입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRMeleeWeapon|Effect")
	FRotator HitEffectRotator;

	/** HitEffect의 크기입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PRMeleeWeapon|Effect")
	FVector HitEffectScale;
#pragma endregion 
};
