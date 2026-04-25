#pragma once

#include "CoreMinimal.h"
#include "Combat/LSWeaponBase.h"
#include "LSMeleeWeapon.generated.h"

/**
 * Short-range melee weapon (pipe / bat). Performs a sphere sweep in front of the
 * owning character on each swing instead of a hitscan trace.
 */
UCLASS()
class LIMINALSPACES_API ALSMeleeWeapon : public ALSWeaponBase
{
	GENERATED_BODY()

public:
	ALSMeleeWeapon();

protected:
	virtual void Fire() override;

	UPROPERTY(EditDefaultsOnly, Category = "LiminalSpaces|Weapon|Melee")
	float SwingRadius = 80.0f;

	UPROPERTY(EditDefaultsOnly, Category = "LiminalSpaces|Weapon|Melee")
	float SwingRange = 220.0f;
};
