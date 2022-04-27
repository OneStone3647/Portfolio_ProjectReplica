// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "PRStateSystemComponent.generated.h"

/**
 * 캐릭터의 상태를 관리하는 ActorComponent 클래스입니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTREPLICA_API UPRStateSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()

public:
	UPRStateSystemComponent();

#pragma region CharacterState
public:
	/** 캐릭터가 피격 당했는지 판별하는 함수입니다. */
	bool IsHit() const;
	
	/** 캐릭터가 사망했는지 판별하는 함수입니다. */
	bool IsDead() const;
	
private:
	/** 캐릭터가 피격당했는지 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRStateSystem|CharacterState", Meta = (AllowPrivateAccess = "true"))
	bool bIsHit;
	
	/** 캐릭터가 사망했는지 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRStateSystem|CharacterState", Meta = (AllowPrivateAccess = "true"))
	bool bIsDead;

public:
	/** 입력받은 인자로 bIsHit을 설정하는 함수입니다. */
	void SetIsHit(bool bFlag);
	
	/** 입력받은 인자로 bIsDead를 설정하는 함수입니다. */
	void SetIsDead(bool bFlag);
#pragma endregion 

#pragma region Move
public:
	/** 움직일 수 있는지 판별하는 함수입니다. */
	bool IsCanMove() const;
	
private:
	/** 움직일 수 있는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRStateSystem|Move", Meta = (AllowPrivateAccess = "true"))
	bool bCanMove;

public:
	/** 입력받은 인자로 bCanMove를 설정하는 함수입니다. */
	void SetCanMove(bool bFlag);
#pragma endregion 
	
#pragma region Dodge
public:
	/** 회피할 수 있는지 판별하는 함수입니다. */
	bool IsCanDodge() const;

	/** 회피하는 중인지 판별하는 함수입니다. */
	bool IsOnDodge() const;
	
private:
	/** 회피할 수 있는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRStateSystem|Dodge", Meta = (AllowPrivateAccess = "true"))
	bool bCanDodge;

	/** 회피하는 중인지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRStateSystem|Dodge", Meta = (AllowPrivateAccess = "true"))
	bool bOnDodge;

public:
	/** 입력받은 인자로 bCanDodge를 설정하는 함수입니다. */
	void SetCanDodge(bool bFlag);
	
	/** 입력받은 인자로 bOnDodge를 설정하는 함수입니다. */
	void SetOnDodge(bool bFlag);
#pragma endregion

#pragma region Attack
public:
	/** 공격할 수 있는지 판별하는 함수입니다. */
	bool IsCanAttack() const;

	/** 공격하는 중인지 판별하는 함수입니다. */
	bool IsOnAttack() const;

private:	
	/** 공격하는 중인지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PRStateSystem|Attack", Meta = (AllowPrivateAccess = "true"))
	bool bOnAttack;

public:
	/** 입력받은 인자로 bOnAttack을 설정하는 함수입니다. */
	void SetOnAttack(bool bFlag);
#pragma endregion 
};
