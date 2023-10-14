#pragma once

#define CUSTOM_DEPTH_PURPLE 250
#define CUSTOM_DEPTH_BLUE 251
#define CUSTOM_DEPTH_TAN 252

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EW_AssaultRifle UMETA(DisplayName = "Assault Rifle"),
	EW_RocketLauncher UMETA(DisplayName = "Rocket Launcher"),
	EW_Pistol UMETA(DisplayName = "Pistol"),
	EW_SMG UMETA(DisplayName = "SMG"),
	EW_ShotGun UMETA(DisplayName = "Shot Gun"),
	EW_SniperRifle UMETA(DisplayName = "Sniper Rifle"),
	EW_GrenadeLauncher UMETA(DisplayName = "Grenade Launcher"),

	EW_MAX UMETA(DisplayName = "Default Max")
};