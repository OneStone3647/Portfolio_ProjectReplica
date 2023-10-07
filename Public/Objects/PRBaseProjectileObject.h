// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "PRPooledObject.h"
#include "PRBaseProjectileObject.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProjectileImpactDelegate, AActor*, OtherActor, FHitResult, HitResult);

/**
 * 투사체의 기본 클래스입니다.  
 */
UCLASS()
class PROJECTREPLICA_API APRBaseProjectileObject : public APRPooledObject
{
	GENERATED_BODY()
	
public:	
	APRBaseProjectileObject();

protected:
	virtual void BeginPlay() override;

private:
	/** 이 Actor의 RootComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProjectileObject", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;
	
	/** 투사체의 범위를 나타내는 BoxComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProjectileObject", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* HitBox;

	/** 투사체의 Mesh입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProjectileObject", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	/** 투사체의 움직임을 관리하는 ProjectileMovementComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProjectileObject", meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;
	
	/** 투사체의 이동 방향을 가리치는 ArrowComponent입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProjectileObject", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* ArrowComponent;

public:
	/** HitBox를 반환하는 함수입니다. */
	FORCEINLINE class UBoxComponent* GetHitBox() const { return HitBox; }

	/** Mesh를 반환하는 함수입니다. */
	FORCEINLINE class UStaticMeshComponent* GetMesh() const { return Mesh; }

	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
	
	/** ArrowComponent를 반환하는 함수입니다. */
	FORCEINLINE class UArrowComponent* GetArrowComponent() const { return ArrowComponent; }

public:
	/** 투사체가 충돌했을 때 호출하는 델리게이트입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable, Category = "ProjectileObject")
	FOnProjectileImpactDelegate OnProjectileImpactDelegate;

#pragma region HitBox
protected:
	/** HitBox의 OnComponentHit 이벤트에 등록하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "HitBox")
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	virtual void OnHit_Implementation(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	/** HitBox의 OnComponentBeginOverlap 이벤트에 등록하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "HitBox")
	void OnBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	virtual void OnBeginOverlap_Implementation(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** HitBox의 OnComponentEndOverlap 이벤트에 등록하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "HitBox")
	void OnEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	virtual void OnEndOverlap_Implementation(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
#pragma endregion 

#pragma region PooledObject
public:
	/** 오브젝트를 활성화하는 함수입니다. */
	virtual void Activate_Implementation() override;
	
	/** 오브젝트를 비활성화하는 함수입니다. */
	virtual void Deactivate_Implementation() override;
#pragma endregion 

#pragma region ProjectileSetting
protected:
	/** 투사체의 속도입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjecttileSetting", meta = (AllowPrivateAccess = "true"))
	float ProjectileSpeed;

	/** 투사체의 중력의 크기입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjecttileSetting", meta = (AllowPrivateAccess = "true"))
	float ProjectileGravityScale;

	/** 투사체의 대미지입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjecttileSetting", meta = (AllowPrivateAccess = "true"))
	float ProjectileDamage;

public:
	/** ProjectileDamage를 반환하는 함수입니다. */
	float GetProjectileDamage() const;
#pragma endregion
};
