// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "Skills/PRBaseSkill.h"
#include "PRSkillSystemComponent.generated.h"

class UPRBaseSkill;
struct FPRSkillInfo;

/**
 * 월드에 Spawn할 스킬의 정보를 가진 구조체입니다.
 */
USTRUCT(Atomic, BlueprintType)
struct FPRConstructSkillInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	FPRConstructSkillInfo()
		: SkillInfo()
		, ConstructSkillClass()
	{}

	FPRConstructSkillInfo(FPRSkillInfo NewSkillInfo, TSubclassOf<class UPRBaseSkill> NewConstructSkillClass)
		: SkillInfo(NewSkillInfo)
		, ConstructSkillClass(NewConstructSkillClass)
	{}

public:
	/** Spawn한 스킬의 정보입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConstructSkillInfo")
	FPRSkillInfo SkillInfo;

	/** Spawn할 스킬의 클래스 레퍼런스입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConstructSkillInfo")
	TSubclassOf<class UPRBaseSkill> ConstructSkillClass;
};

/**
 * 캐릭터의 스킬을 관리하는 ActorComponent입니다. 
 */
UCLASS()
class PROJECTREPLICA_API UPRSkillSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()

public:
	UPRSkillSystemComponent();

protected:
	virtual void BeginPlay() override;

public:
	/** SkillInventory를 초기화하는 함수입니다. */
	void InitializeSkillInventory();

	/**
	 * 인자로 받은 스킬을 SkillInventory에 보관하는 함수입니다.
	 *
	 * @param NewSkill SkillInventory에 보관할 스킬입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "SkillSystem")
	void AddToSkillInventory(class UPRBaseSkill* NewSkill);

	/**
	 * 인자 값에 해당하는 CommandSkill의 스킬을 SkillInventory에서 찾아 반환하는 함수입니다.
	 *
	 * @param NewCommandSkill SkillInventory에서 찾아 반환할 스킬의 CommandSkill입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "SkillSystem", meta = (AutoCreateRefTerm = "NewCommandSkillType"))
	class UPRBaseSkill* GetSkillFromCommand(EPRCommandSkill NewCommandSkill) const;

	/**
	 * 인자 값에 해당하는 Name의 스킬을 SkillInventory에서 찾아 반환하는 함수입니다.
	 *
	 * @param NewSkillName SkillInventory에서 찾아 반환할 스킬의 Name입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "SkillSystem")
	class UPRBaseSkill* GetSkillFromName(const FText& NewSkillName) const;
	
protected:
	/**
	 * 인자로 받은 PRBaseSkill 클래스 레퍼런스로 스킬을 생성하는 함수입니다.
	 *
	 * @param NewPRSkillClass 월드에 Spawn할 스킬의 클래스 레퍼런스입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "SkillSystem")
	UPRBaseSkill* CreateSkill(TSubclassOf<class UPRBaseSkill> NewPRSkillClass);

	/**
	 * 데이터 테이블을 바탕으로 월드에 스킬을 Spawn하고 Spawn한 스킬들을 Array로 반환하는 함수입니다.
	 *
	 * @param NewSkillDataTable 월드에 Spawn할 스킬의 정보를 가진 데이터 테이블입니다.
	 * @return 월드에 Spawn한 스킬들입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Skillsystem")
	TArray<class UPRBaseSkill*> CreateSkillFromDataTable(UDataTable* NewSkillDataTable);

private:
	// /** 월드에 Spawn할 스킬을 보관한 Array입니다. */
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkillSystem", meta = (AllowPrivateAccess = "true"))
	// TArray<TSubclassOf<class UPRBaseSkill>> ConstructSkillInventory;
	
	/** 월드에 Spawn하여 사용할 수 있는 스킬을 보관한 Array입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkillSystem", meta = (AllowPrivateAccess = "true"))
	TArray<class UPRBaseSkill*> SkillInventory;

public:
	/** SkillInventory를 반환하는 함수입니다. */
	TArray<class UPRBaseSkill*> GetSkillInventory() const { return SkillInventory; }

#pragma region DataTable
private:
	/** Spawn할 스킬의 정보를 보관한 데이터 테이블입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	UDataTable* ConstructSkillInfoDataTable;
#pragma endregion 
};
