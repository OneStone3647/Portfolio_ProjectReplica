// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRSetCollisionPreset.generated.h"

/**
 * CollisionPreset을 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRCollisionPreset : uint8
{
	CollisionPreset_Pawn					UMETA(DisplayName = "Pawn"),
	CollisionPreset_PlayerCharacter			UMETA(DisplayName = "PlayerCharacter"),
	CollisionPreset_GhostCharacter			UMETA(DisplayName = "GhostCharacter")
};

/**
 * 캐릭터의 CollisionPreset을 설정하는 AnimNotify 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRSetCollisionPreset : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_PRSetCollisionPreset();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRCollisionPreset", Meta = (AllowPrivateAccess = "true"))
	EPRCollisionPreset CollisionPreset;
};
