// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 프로젝트에서 사용하는 열거형을 정의한 파일입니다.
 */

#include "CoreMinimal.h"
#include "PRCommonEnum.generated.h"

/**
 * Enum과 관련된 함수를 구현한 Library 클래스입니다.
 */
class PROJECTREPLICA_API PRCommonEnum
{
public:
	/**
	 * Enum 인자와 같은 이름의 Enum을 찾고 EnumValue 인자에 해당하는 값을 찾아 해당 값의 DisplayName을 String으로 변환합니다.
	 * @param Enum: 찾으려는 Enum의 이름입니다. TEXT("") 형식으로 입력합니다.
	 * @param EnumValue: 찾으려는 Enum에서 불러올 값입니다.
	 * @return String으로 변환한 Enum의 값의 DisplayName입니다. 
	 * ex)  PREnumFunctionLibrary* EnumFunctionLibrary = nullptr;
	 *		    PR_LOG_SCREEN("Gait: %s", *EnumFunctionLibrary->GetEnumToString(TEXT("EPRGait"), static_cast<uint8>(Gait)));
	 *
	 *		    or
	 *
	 *		    PR_LOG_SCREEN("Gait: %s", *PRCommonEnum::GetEnumDisplayNameToString(TEXT("EPRGait"), static_cast<uint8>(Gait)));
	 */
	static FString GetEnumDisplayNameToString(const TCHAR* Enum, int32 EnumValue = 0);
};

/**
 * 방향을 나타내는 열거형입니다. 
 */
UENUM(BlueprintType)
enum class EPRDirection : uint8
{
    Direction_None					UMETA(DisplayName = "None"),
	Direction_Forward		    	UMETA(DisplayName = "Forward"),
    Direction_Backward 		  		UMETA(DisplayName = "Backward"),
    Direction_Left		        	UMETA(DisplayName = "Left"),
    Direction_Right			    	UMETA(DisplayName = "Right"),
    Direction_ForwardLeft			UMETA(DisplayName = "ForwardLeft"),
    Direction_ForwardRight			UMETA(DisplayName = "ForwardRight"),
    Direction_BackwardLeft			UMETA(DisplayName = "BackwardLeft"),
    Direction_BackwardRight			UMETA(DisplayName = "BackwardRight")
};

/**
 * 캐릭터의 발을 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRFoot : uint8
{
    Foot_Left	   UMETA(DisplayName = "LeftFoot"),
	Foot_Right	   UMETA(DisplayName = "RightFoot")
};

/**
 * 캐릭터의 걷는 모양을 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRGait : uint8
{
    Gait_None        UMETA(DisplayName = "None"),
    Gait_Walking     UMETA(DisplayName = "Walking"),
    Gait_Running     UMETA(DisplayName = "Running"),
    Gait_Sprinting   UMETA(DisplayName = "Sprinting")
};

/**
 * 캐릭터의 성별을 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRGender : uint8
{
    Gender_None       UMETA(DisplayName = "None"),
    Gender_Male       UMETA(DisplayName = "Male"),
    Gender_Female     UMETA(DisplayName = "Female")
};

/**
 * 캐릭터의 움직임 상태를 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRMovementState : uint8
{
    MovementState_None       UMETA(DisplayName = "None"),
    MovementState_Grounded   UMETA(DisplayName = "Grounded"),
    MovementState_InAir      UMETA(DisplayName = "InAir")
};

/**
 * 무기의 발도(Draw) / 납도(Sheath)를 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRWeaponState : uint8
{
    EPRWeaponState_Draw	   		UMETA(DisplayName = "Draw"),
    EPRWeaponState_Sheath		UMETA(DisplayName = "Sehath")
};

/**
 * 무기의 위치를 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRWeaponPosition : uint8
{
    WeaponPosition_MainWeapon			UMETA(DisplayName = "MainWeapon"),
    WeaponPosition_SubWeapon			UMETA(DisplayName = "SubWeapon")
};

/**
 * 캐릭터의 상태를 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRCharacterState : uint8
{
    CharacterState_Idle				UMETA(DisplayName = "Idle"),
	CharacterState_Hit				UMETA(DisplayName = "Hit"),
	CharacterState_Dead			    UMETA(DisplayName = "Dead"),
	CharacterState_Move				UMETA(DisplayName = "Move"),
	CharacterState_Dodge			UMETA(DisplayName = "Dodge"),
    CharacterState_Attack			UMETA(DisplayName = "Attack")
};

/**
 * 캐릭터의 Able(가능 상태)를 설정하는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRAbleState : uint8
{
	AbleState_Moveable				UMETA(DisplayName = "Moveable"),
	AbleState_Dodgeable				UMETA(DisplayName = "Dodgeable"),
	AbleState_Attackable		    UMETA(DisplayName = "Attackable"),
	AbleState_DodgeAttackable		UMETA(DisplayName = "DodgeAttackbale"),
	AbleState_ExtremeDodgeable		UMETA(DisplayName = "ExtremDodgeable")

};

/**
 * 캐릭터의 액션을 나타내는 열거형입니다.
 */
UENUM(BlueprintType)
enum class EPRAction : uint8
{
	Action_None					UMETA(DisplayName = "None"),
	Action_Move					UMETA(DisplayName = "Move"),
	Action_DoubleJump			UMETA(DisplayName = "DoubleJump"),
	Action_Dodge				UMETA(DisplayName = "Dodge"),
	Action_Attack				UMETA(DisplayName = "Attack"),
	Action_NormalAttack			UMETA(DisplayName = "NormalAttack"),
	Action_ExtraAttack			UMETA(DisplayName = "ExtraAttack"),
	Action_FirstBattleSkill		UMETA(DisplayName = "FirstBattleSkill"),
	Action_SecondBattleSkill	UMETA(DisplayName = "SecondBattleSkill"),
	Action_ThirdBattleSkill		UMETA(DisplayName = "ThirdBattleSkill"),
	Action_Ultimate				UMETA(DisplayName = "Ultimate"),
	Action_CoreSkill			UMETA(DosplayName = "CoreSkill"),
	Action_Guard				UMETA(DisplayName = "Guard"),
	Action_Parry				UMETA(DisplayName = "Parry"),
	Action_DodgeAttack			UMETA(DisplayName = "DodgeAttack"),
};

