// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/Characters/Kyle/PRSkill_Kyle_GeneralUltimate.h"
#include "Characters/PRPlayerCharacter_Kyle.h"
#include "Components/PRStateSystemComponent.h"
#include "Components/PRTargetingSystemComponent.h"
#include "Components/PREffectSystemComponent.h"
#include "Components/PRSkillSystemComponent.h"
#include "Components/PRAnimSystemComponent.h"
#include "Components/PRWeaponSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PRPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/GameplayStatics.h"
#include "Effect/PRNiagaraEffect.h"
#include "Objects/PRPooledObject.h"
#include "Objects/PRScreenShatter.h"

UPRSkill_Kyle_GeneralUltimate::UPRSkill_Kyle_GeneralUltimate()
{
	// Debug
	bActivateDebug = false;
	
	UltimateAreaExtent = FVector(800.0f);
	UltimateSlashDamage = 10.0f;
	UltimateFinishDamage = 100.0f;
	bCastingUltimate = false;
	bActivateUltimate = false;
	ActivateUltimateCount = 0;
	bFinishUltimate = false;
	FinishUltimateCount = 0;
	UltimateActivateLocation = FVector::ZeroVector;

	// Animation
	CastingUltimatePRAnimMontage = FPRAnimMontage();
	FinishUltimatePRAnimMontage = FPRAnimMontage();

	// Sound
	UltimateCastingSound = nullptr;
	UltimateSlashSound = nullptr;
	UltimateCastingAudioComponent = nullptr;

	// Effect
	UltimateSlashMirrorObject = FPRPooledObjectInfo();
	UltimateSlashNiagaraEffect = FPRNiagaraEffectInfo();
	UltimateSlashSpawnAreaExtent = FVector(300.0f);
	UltimateSlashSpawnInterval = 5;
	UltimateSlashInterval = 0.1f;
	UltimateSlashAccumulatedInterval = 0.0f;

	// ScreenShatter
	ScreenShatter = nullptr;
	ScreenShatterWidget = nullptr;
}

void UPRSkill_Kyle_GeneralUltimate::InitializeSkill_Implementation()
{
	Super::InitializeSkill_Implementation();

	UltimateActivateLocation = FVector::ZeroVector;
	
	if(IsValid(GetSkillOwner()) == true)
	{
		// UltimateNiagaraEffect 생성
		if(UltimateSlashNiagaraEffect.NiagaraSystem != nullptr
			&& GetSkillOwner()->GetEffectSystem()->IsValidNiagaraEffectPool(UltimateSlashNiagaraEffect.NiagaraSystem) == false)
		{
			GetSkillOwner()->GetEffectSystem()->CreateNiagaraEffectPool(UltimateSlashNiagaraEffect);
		}

		// UltimateSlashMirrorObject 생성
		if(UltimateSlashMirrorObject.PooledObjectClass != nullptr
			&& GetSkillOwner()->GetObjectPoolSystem()->IsCreatePooledObject(UltimateSlashMirrorObject.PooledObjectClass) == false)
		{
			GetSkillOwner()->GetObjectPoolSystem()->CreateObjectPool(UltimateSlashMirrorObject);
		}
	}
}

bool UPRSkill_Kyle_GeneralUltimate::ActivateSkill_Implementation()
{
	APRPlayerCharacter* PRPlayerCharacter = Cast<APRPlayerCharacter>(GetSkillOwner());
	if(IsCanActivateSkill() == true && IsValid(PRPlayerCharacter) == true)
	{
		bActivateSkill = true;
		ActivateCooldown();
		
		// 캐릭터를 무적상태 설정합니다.
		PRPlayerCharacter->GetStateSystem()->SetIsInvincible(true);

		// 캐릭터의 현재 위치를 저장합니다.
		UltimateActivateLocation = PRPlayerCharacter->GetActorLocation();

		// 일반공격의 콤보를 초기화합니다.
		PRPlayerCharacter->InitializePlayNormalAttackIndex();

		// 캐릭터를 공격하는 방향으로 회전시킵니다.
		if(PRPlayerCharacter->GetTargetingSystem()->IsActivateLockOnTarget() == true)
		{
			// Target이 존재하면 Target을 향해 캐릭터를 회전합니다.
			PRPlayerCharacter->RotationAutoTargetDirection();
		}
		else
		{
			// Target이 존재하지 않으면 입력방향을 향해 캐릭터를 회전합니다.
			PRPlayerCharacter->RotationInputDirection();
		}
			
		// 카메라의 위치를 초기화하여 캐릭터의 정면을 바라봅니다.
		PRPlayerCharacter->ActivateResetCamera();

		// 캐릭터의 움직임 입력과 카메라 입력을 차단합니다.
		PRPlayerCharacter->SetLockMovementInput(true);
		PRPlayerCharacter->SetLockCamera(true);
		
		// 스킬을 실행하는 동안 다른 동작을 할 수 없도록 설정합니다.
		PRPlayerCharacter->GetStateSystem()->SetActionable(EPRAction::Action_Move, false);
		PRPlayerCharacter->GetStateSystem()->SetActionable(EPRAction::Action_Attack, false);
		PRPlayerCharacter->GetStateSystem()->SetActionable(EPRAction::Action_Dodge, false);
		PRPlayerCharacter->GetStateSystem()->SetActionable(EPRAction::Action_DodgeAttack, false);
		UPRBaseSkill* DodgeAttackSkill = PRPlayerCharacter->GetSkillSystem()->GetSkillFromCommand(EPRCommandSkill::CommandSkill_DodgeAttack);
		if(IsValid(DodgeAttackSkill) == true)
		{
			DodgeAttackSkill->SetActivateSkill(false);
		}

		// 무기를 발도하고 Ultimate를 캐스팅합니다.
		PRPlayerCharacter->GetWeaponSystem()->DrawWeapon(static_cast<int32>(EPRAttackMode::AttackMode_General));
		CastingUltimate();

		return true;
	}

	return false;
}

bool UPRSkill_Kyle_GeneralUltimate::IsCanActivateSkill_Implementation() const
{
	return Super::IsCanActivateSkill_Implementation()
			&& GetSkillOwner()->GetStateSystem()->IsActionable(EPRAction::Action_Attack);
}

bool UPRSkill_Kyle_GeneralUltimate::OnSkillNotify_Implementation()
{
	if(IsActivateSkill() == true)
	{
		APRPlayerCharacter* PRPlayerCharacter = Cast<APRPlayerCharacter>(GetSkillOwner());
		if(PRPlayerCharacter != nullptr)
		{
			if(bCastingUltimate || bActivateUltimate)
			{
				ActivateUltimate();
				return true;
			}

			if((bCastingUltimate == false && bActivateUltimate == false)
				|| bFinishUltimate)
			{
				FinishUltimate();
				return true;
			}
		}

		return false;
	}
	
	return false;
}

bool UPRSkill_Kyle_GeneralUltimate::OnSkillNotifyTick_Implementation(float DeltaTime)
{
	UltimateSlashAccumulatedInterval += DeltaTime;
	if(UltimateSlashAccumulatedInterval > UltimateSlashInterval)
	{
		for(int SpawnCount = 0; SpawnCount < UltimateSlashSpawnInterval; SpawnCount++)
		{
			ActivateUltimateSlash();
		}
		
		UltimateSlashAccumulatedInterval = 0.0f;
	}
	
	return false;
}

void UPRSkill_Kyle_GeneralUltimate::CastingUltimate()
{
	if(GetSkillOwner() != nullptr)
	{
		bCastingUltimate = true;

		APRPlayerCharacter* PRPlayerCharacter = Cast<APRPlayerCharacter>(GetSkillOwner());
		if(IsValid(PRPlayerCharacter) == true)
		{
			// 컷신 재생
			// TimeStop일 때 GlobalTimeDilation 때문에 CutScene이 재생되지 않음
			if(PRPlayerCharacter->IsActivateTimeStop() == false)
			{
				PlaySkillCutScene();
			}
		}
		
		// Casting 사운드 재생
		if(UltimateCastingSound != nullptr)
		{
			UltimateCastingAudioComponent = UGameplayStatics::SpawnSoundAttached(UltimateCastingSound, GetSkillOwner()->GetMesh());
		}
		
		GetSkillOwner()->GetAnimSystem()->PlayPRAnimMontage(CastingUltimatePRAnimMontage);
	}
}

void UPRSkill_Kyle_GeneralUltimate::ActivateUltimate()
{
	APRPlayerCharacter* PRPlayerCharacter = Cast<APRPlayerCharacter>(GetSkillOwner());
	if(IsValid(PRPlayerCharacter) == true)
	{
		bActivateUltimate = true;
		bCastingUltimate = false;
		
		FRotator CurrentRotation = PRPlayerCharacter->GetActorRotation();

		switch(ActivateUltimateCount)
		{
		case 0:
			// Casting 사운드 정지
			if(UltimateCastingSound != nullptr && UltimateCastingAudioComponent != nullptr)
			{
				UltimateCastingAudioComponent->Stop();
				// SpawnSoundAttached로 AutoDestroy가 된다.
				UltimateCastingAudioComponent = nullptr;
			}
			
			// 캐릭터의 Mesh와 무기를 숨김
			SetVisibilityPlayerCharacterAndWeapon(false);

			// TimeStop 실행
			if(PRPlayerCharacter->IsActivateTimeStop() == true)
			{
				PRPlayerCharacter->DeactivateTimeStop();
			}
			PRPlayerCharacter->ActivateTimeStop(20.0f, EPRCameraPostProcessMaterial::CameraPostProcessMaterial_BlackAndWhite);
			break;
		case 1:
			// Finish 실행
			bActivateUltimate = false;
			ActivateUltimateCount = 0;
			
			// 캐릭터가 뒤를 바라도록(카메라를 바라보도록) 회전
			PRPlayerCharacter->SetActorRotation(CurrentRotation + FRotator(0.0f, 180.0f, 0.0f));
			
			PRPlayerCharacter->GetAnimSystem()->PlayPRAnimMontage(FinishUltimatePRAnimMontage);
			return;
		default:
			return;
		}

		ActivateUltimateCount++;
	}
}

void UPRSkill_Kyle_GeneralUltimate::FinishUltimate()
{
	APRPlayerCharacter* PRPlayerCharacter = Cast<APRPlayerCharacter>(GetSkillOwner());
	if(IsValid(PRPlayerCharacter) == true)
	{
		bFinishUltimate = true;
		bActivateUltimate = false;

		switch(FinishUltimateCount)
		{
		case 0:
			// 캐릭터의 Mesh와 무기를 보이게 설정
			SetVisibilityPlayerCharacterAndWeapon(true);
			break;
		case 1:
			bFinishUltimate = false;
			FinishUltimateCount = 0;

			// ScreenShatter 실행
			ActivateScreenShatter();
			
			// GeneralUltimateSlash 비활성화
			DeactivateUltimateSlash();

			// 마무리 대미지를 줍니다.
			ApplyDamageUltimateArea(UltimateFinishDamage);
			
			// TimeStop이 진행 중일 경우 해제
			if(PRPlayerCharacter->IsActivateTimeStop() == true)
			{
				PRPlayerCharacter->DeactivateTimeStop();
			}

			// 무기를 납도합니다.
			PRPlayerCharacter->GetWeaponSystem()->SheathWeapon(static_cast<int32>(EPRAttackMode::AttackMode_General));
			
			// 캐릭터의 움직임 입력과 카메라 입력을 허용합니다.
			PRPlayerCharacter->SetLockMovementInput(false);
			PRPlayerCharacter->SetLockCamera(false);

			// 캐릭터의 무적상태를 해제합니다.
			PRPlayerCharacter->GetStateSystem()->SetIsInvincible(false);

			// 캐릭터가 다른 동작을 할 수 있도록 허용합니다.
			PRPlayerCharacter->GetStateSystem()->SetCanCancelAction(true);
			PRPlayerCharacter->GetStateSystem()->SetActionable(EPRAction::Action_Move, true);
			PRPlayerCharacter->GetStateSystem()->SetActionable(EPRAction::Action_Attack, true);
			PRPlayerCharacter->GetStateSystem()->SetActionable(EPRAction::Action_Dodge, true);
			return;
		default:
			return;
		}
	
		FinishUltimateCount++;
	}
}

void UPRSkill_Kyle_GeneralUltimate::SetVisibilityPlayerCharacterAndWeapon(bool bNewVisibility)
{
	APRPlayerCharacter* PRPlayerCharacter = Cast<APRPlayerCharacter>(GetSkillOwner());
	if(IsValid(PRPlayerCharacter) == true)
	{
		PRPlayerCharacter->GetMesh()->SetVisibility(bNewVisibility);
		APRBaseWeapon* EquippedWeapon = PRPlayerCharacter->GetWeaponSystem()->GetEquippedWeapon();
		if(EquippedWeapon != nullptr)
		{
			EquippedWeapon->SetVisibility(bNewVisibility);
		}
	}
}

void UPRSkill_Kyle_GeneralUltimate::ActivateUltimateSlash()
{
	if(IsValid(GetSkillOwner()) == true
		&& GetSkillOwner()->GetEffectSystem()->IsValidNiagaraEffectPool(UltimateSlashNiagaraEffect.NiagaraSystem) == true)
	{
		// Slash Effect를 Spawn합니다.
		FVector NiagaraEffectRandomLocation = UKismetMathLibrary::RandomPointInBoundingBox(UltimateActivateLocation, UltimateSlashSpawnAreaExtent);
		GetSkillOwner()->GetEffectSystem()->SpawnNiagaraEffectAtLocation(UltimateSlashNiagaraEffect.NiagaraSystem, NiagaraEffectRandomLocation,
																			UKismetMathLibrary::RandomRotator());
		
		// MirrorObject를 Spawn합니다.
		FVector MirrorObjectRandomLocation = UKismetMathLibrary::RandomPointInBoundingBox(UltimateActivateLocation, UltimateAreaExtent);
		APRPooledObject* MirrorObject = GetSkillOwner()->GetObjectPoolSystem()->ActivatePooledObject(UltimateSlashMirrorObject.PooledObjectClass);
		MirrorObject->SetActorLocationAndRotation(MirrorObjectRandomLocation, UKismetMathLibrary::RandomRotator());

		// Slash 사운드를 재생합니다.
		UGameplayStatics::SpawnSoundAttached(UltimateSlashSound, GetSkillOwner()->GetMesh());

		// 공격 범위 안의 액터들에게 대미지를 줍니다.
		ApplyDamageUltimateArea(UltimateSlashDamage);
	}
}

void UPRSkill_Kyle_GeneralUltimate::DeactivateUltimateSlash()
{
	if(IsValid(GetSkillOwner()) == true)
	{
		// Slash Effect 비활성화
		if(GetSkillOwner()->GetEffectSystem()->IsValidNiagaraEffectPool(UltimateSlashNiagaraEffect.NiagaraSystem) == true)
		{
			GetSkillOwner()->GetEffectSystem()->DeactivateNiagaraEffectPool(UltimateSlashNiagaraEffect.NiagaraSystem);
		}

		// MirrorObject 비활성화
		if(GetSkillOwner()->GetObjectPoolSystem()->IsCreatePooledObject(UltimateSlashMirrorObject.PooledObjectClass) == true)
		{
			GetSkillOwner()->GetObjectPoolSystem()->DeactivateObjectPool(UltimateSlashMirrorObject.PooledObjectClass);
		}
	}
}

void UPRSkill_Kyle_GeneralUltimate::ApplyDamageUltimateArea(float NewApplyDamage)
{
	if(IsValid(GetSkillOwner()) == true)
	{
		TArray<FHitResult> HitResults;
		bool bIsHit = false;

		// 오브젝트 소유자를 Trace 대상에서 제외합니다.
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Emplace(GetSkillOwner());
	
		// Debug 실행을 설정합니다.
		EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;
		if(bActivateDebug)
		{
			DebugType = EDrawDebugTrace::ForDuration;
			UKismetSystemLibrary::DrawDebugBox(GetSkillOwner()->GetWorld(), UltimateActivateLocation, UltimateAreaExtent,
												FLinearColor::Green, FRotator::ZeroRotator, 2.0f);
		}
		
		// ECC_GameTraceChannel3는 TraceChannels의 PlayerAttack을 나타냅니다.
		bIsHit = UKismetSystemLibrary::BoxTraceMulti(GetWorld(), UltimateActivateLocation, UltimateActivateLocation, UltimateAreaExtent,
														FRotator::ZeroRotator, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel3), false, ActorsToIgnore,
														DebugType, HitResults, true);

		// Hit된 액터들에게 대미지를 줍니다.
		APRBaseCharacter* PRCharacter = Cast<APRBaseCharacter>(GetSkillOwner());
		if(bIsHit && IsValid(PRCharacter) == true)
		{
			for(FHitResult Hit : HitResults)
			{
				if(Hit.Actor.IsValid() == true)
				{
					AActor* HitActor = Hit.GetActor();
					UGameplayStatics::ApplyDamage(HitActor, NewApplyDamage, PRCharacter->GetController(), PRCharacter, nullptr);
				}
			}
		}
	}
}

void UPRSkill_Kyle_GeneralUltimate::ActivateScreenShatter()
{
	APRPlayerCharacter* PRPlayerCharacter = Cast<APRPlayerCharacter>(GetSkillOwner());
	if(IsValid(PRPlayerCharacter) == true)
	{
		PRPlayerCharacter->GetSceneCapture()->CaptureScene();
		APRScreenShatter* NewScreenShatter = PRPlayerCharacter->GetWorld()->SpawnActor<APRScreenShatter>(ScreenShatter);
		if(IsValid(NewScreenShatter) == true)
		{
			// ScreenShatter 이외에 아무것도 보이지 않아야합니다.
			NewScreenShatter->SetActorLocation(FVector(0.0f, 0.0f, -4000.0f));

			// ScreenShatterWidget을 생성하고 Viewport에 추가합니다.
			APRPlayerController* PRPlayerController = Cast<APRPlayerController>(PRPlayerCharacter->GetController());
			if(PRPlayerController != nullptr)
			{
				UUserWidget* NewScreenShatterWidget = PRPlayerController->CreateWidgetFromClassReference(ScreenShatterWidget);
				if(IsValid(NewScreenShatterWidget) == true)
				{
					// Viewport의 제일 상단에 출력합니다.
					NewScreenShatterWidget->AddToViewport(1);
				}
			}
		}
	}
}

void UPRSkill_Kyle_GeneralUltimate::SetUltimateSlashInterval(float NewSlashInterval)
{
	UltimateSlashInterval = NewSlashInterval;
}

void UPRSkill_Kyle_GeneralUltimate::SetUltimateSlashAccumulatedInterval(float NewSlashAccumulatedInterval)
{
	UltimateSlashAccumulatedInterval = NewSlashAccumulatedInterval;
}
