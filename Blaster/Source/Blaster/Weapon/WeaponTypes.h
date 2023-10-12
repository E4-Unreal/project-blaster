#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EW_AssaultRifle UMETA(DisplayName = "Assault Rifle"),
	EW_RocketLauncher UMETA(DisplayName = "Rocket Launcher"),

	EW_MAX UMETA(DisplayName = "Default Max")
};