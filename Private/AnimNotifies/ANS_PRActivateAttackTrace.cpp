// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_PRActivateAttackTrace.h"
#include "Characters/PRBaseCharacter.h"
#include "Weapons/PRDualMeleeWeapons.h"
#include "Weapons/PRMeleeWeapon.h"

UANS_PRActivateAttackTrace::UANS_PRActivateAttackTrace()
	: Super()
{
	bIsNativeBranchingPoint = true;

	WeaponPosition = EPRWeaponPosition::WeaponPosition_MainWeapon;
	TraceType = EPRTraceType::TraceType_LineTrace;
	SphereTraceSize = 100.0f;
	bActivateAttackTraceDebug = false;
}

void UANS_PRActivateAttackTrace::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyBegin(BranchingPointPayload);
	
	USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent;
	if(MeshComp != nullptr)
	{
		APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		APRMeleeWeapon* EquippedWeapon = GetEquippedWeapon(PROwner);
		// 장비한 무기를 발도하지 않았다면 발도하고 TracePoint의 PreviousLocation을 최신화합니다.
		if(EquippedWeapon != nullptr)
		{
			if(EquippedWeapon->IsDraw() == false)
			{
				EquippedWeapon->Draw();
			}

			// 장비한 무기가 이도류일 경우
			if(EquippedWeapon->IsA(APRDualMeleeWeapons::StaticClass()) == true)
			{
				APRDualMeleeWeapons* EquippedDualMeleeWeapons = Cast<APRDualMeleeWeapons>(EquippedWeapon);
				if(IsValid(EquippedDualMeleeWeapons) == true)
				{
					// 무기의 종류가 MainWeapon일 경우
					if(WeaponPosition == EPRWeaponPosition::WeaponPosition_MainWeapon)
					{
						EquippedDualMeleeWeapons->UpdateTracePointsPreviousLocation(EquippedDualMeleeWeapons->GetMainWeaponTraceLinesReference());
					}
					// 무기의 종류가 SubWeapon일 경우
					else
					{
						EquippedDualMeleeWeapons->UpdateTracePointsPreviousLocation(EquippedDualMeleeWeapons->GetSubWeaponTraceLinesReference());
					}
				}
			}
			else
			{
				EquippedWeapon->UpdateTracePointsPreviousLocation(EquippedWeapon->GetMainWeaponTraceLinesReference());
			}
	
			if(bActivateAttackTraceDebug == true)
			{
				EquippedWeapon->SetActivateTraceDebug(bActivateAttackTraceDebug);
			}
		}
	}
}

void UANS_PRActivateAttackTrace::BranchingPointNotifyTick(FBranchingPointNotifyPayload& BranchingPointPayload, float FrameDeltaTime)
{
	Super::BranchingPointNotifyTick(BranchingPointPayload, FrameDeltaTime);

	USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent;
	if(MeshComp != nullptr)
	{
		APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		APRMeleeWeapon* EquippedWeapon = GetEquippedWeapon(PROwner);
		// 장비한 무기를 발도하지 않았다면 발도하고 장착한 무기의 AttackTrace를 실행합니다.
		if(EquippedWeapon != nullptr)
		{
			if(EquippedWeapon->IsDraw() == false)
			{
				EquippedWeapon->Draw();
			}

			// 장비한 무기가 이도류일 경우
			if(EquippedWeapon->IsA(APRDualMeleeWeapons::StaticClass()) == true)
			{
				APRDualMeleeWeapons* EquippedDualMeleeWeapons = Cast<APRDualMeleeWeapons>(EquippedWeapon);
				if(IsValid(EquippedDualMeleeWeapons) == true)
				{
					// AttackTrace 종류가 LineTrace일 경우
					if(TraceType == EPRTraceType::TraceType_LineTrace)
					{
						// 무기의 종류가 MainWeapon일 경우
						if(WeaponPosition == EPRWeaponPosition::WeaponPosition_MainWeapon)
						{
							EquippedDualMeleeWeapons->ActivateMainWeaponAttackTraceLine(bActivateAttackTraceDebug);
						}
						// 무기의 종류가 SubWeapon일 경우
						else
						{
							EquippedDualMeleeWeapons->ActivateSubWeaponAttackTraceLine(bActivateAttackTraceDebug);
						}
					}
					// AttackTrace 종류가 SphereTrace일 경우
					else
					{
						// 무기의 종류가 MainWeapon일 경우
						if(WeaponPosition == EPRWeaponPosition::WeaponPosition_MainWeapon)
						{
							EquippedDualMeleeWeapons->ActivateSphereAttackTraceByOwner(WeaponPosition, EquippedDualMeleeWeapons->GetMainWeaponHitActors(), SphereTraceSize, bActivateAttackTraceDebug);
						}
						// 무기의 종류가 SubWeapon일 경우
						else
						{
							EquippedDualMeleeWeapons->ActivateSphereAttackTraceByOwner(WeaponPosition, EquippedDualMeleeWeapons->GetSubWeaponHitActors(), SphereTraceSize, bActivateAttackTraceDebug);
						}
					}
				}
			}
			else
			{
				// AttackTrace 종류가 LineTrace일 경우
				if(TraceType == EPRTraceType::TraceType_LineTrace)
				{
					EquippedWeapon->ActivateMainWeaponAttackTraceLine(bActivateAttackTraceDebug);
				}
				// AttackTrace 종류가 SphereTrace일 경우
				else
				{
					EquippedWeapon->ActivateSphereAttackTraceByOwner(WeaponPosition, EquippedWeapon->GetMainWeaponHitActors(), SphereTraceSize, bActivateAttackTraceDebug);
				}
			}
		}
	}
}

void UANS_PRActivateAttackTrace::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyEnd(BranchingPointPayload);

	USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent;
	if(MeshComp != nullptr)
	{
		APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		APRMeleeWeapon* EquippedWeapon = GetEquippedWeapon(PROwner);
		// 장비한 무기의 HitActors를 초기화합니다.
		if(EquippedWeapon != nullptr)
		{
			// 장비한 무기가 이도류일 경우
			if(EquippedWeapon->IsA(APRDualMeleeWeapons::StaticClass()) == true)
			{
				APRDualMeleeWeapons* EquippedDualMeleeWeapons = Cast<APRDualMeleeWeapons>(EquippedWeapon);
				if(IsValid(EquippedDualMeleeWeapons) == true)
				{
					// 무기의 종류가 MainWeapon일 경우
					if(WeaponPosition == EPRWeaponPosition::WeaponPosition_MainWeapon)
					{
						EquippedDualMeleeWeapons->ClearHitActors(EquippedDualMeleeWeapons->GetMainWeaponHitActors());
					}
					// 무기의 종류가 SubWeapon일 경우
					else
					{
						EquippedDualMeleeWeapons->ClearHitActors(EquippedDualMeleeWeapons->GetSubWeaponHitActors());
					}
				}
			}
			else
			{
				EquippedWeapon->ClearHitActors(EquippedWeapon->GetMainWeaponHitActors());
			}
			
			if(bActivateAttackTraceDebug == true)
			{
				EquippedWeapon->SetActivateTraceDebug(!bActivateAttackTraceDebug);
			}
		}
	}
}

FString UANS_PRActivateAttackTrace::GetNotifyName_Implementation() const
{
	FString NewNotifyName;
	
	switch(WeaponPosition)
	{
	case EPRWeaponPosition::WeaponPosition_MainWeapon:
		NewNotifyName.Append(TEXT("Activate MainWeapon"));
		break;
	case EPRWeaponPosition::WeaponPosition_SubWeapon:
		NewNotifyName.Append(TEXT("Activate SubWeapon"));
		break;
	default:
		NewNotifyName = Super::GetNotifyName_Implementation();
		break;
	}

	switch(TraceType)
	{
	case EPRTraceType::TraceType_LineTrace:
		NewNotifyName.Append(TEXT(" LineTrace"));
		break;
	case EPRTraceType::TraceType_SphereTraceByOwner:
		NewNotifyName.Append(TEXT(" SphereTraceByOwner"));
		break;
	default:
		break;
	}
	
	return NewNotifyName;
}

APRMeleeWeapon* UANS_PRActivateAttackTrace::GetEquippedWeapon(APRBaseCharacter* PROwner)
{
	if(IsValid(PROwner) == true)
	{
		APRMeleeWeapon* EquippedWeapon = Cast<APRMeleeWeapon>(PROwner->GetWeaponSystem()->GetEquippedWeapon());
		if(IsValid(EquippedWeapon) == true)
		{
			return EquippedWeapon;
		}
	}

	return nullptr;
}
