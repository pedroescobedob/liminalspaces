#include "Combat/LSCombatComponent.h"
#include "Combat/LSWeaponBase.h"
#include "GameFramework/Character.h"
#include "LiminalSpaces.h"

ULSCombatComponent::ULSCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULSCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (DefaultWeaponClass)
	{
		EquipWeapon(DefaultWeaponClass);
	}
}

void ULSCombatComponent::EquipWeapon(TSubclassOf<ALSWeaponBase> WeaponClass)
{
	if (!WeaponClass)
	{
		return;
	}

	UnequipWeapon();

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = OwnerCharacter;

	CurrentWeapon = GetWorld()->SpawnActor<ALSWeaponBase>(WeaponClass, SpawnParams);
	if (CurrentWeapon)
	{
		CurrentWeapon->SetOwningCharacter(OwnerCharacter);
		OnWeaponChanged.Broadcast(CurrentWeapon);

		UE_LOG(LogLiminalSpaces, Log, TEXT("Equipped weapon: %s"), *CurrentWeapon->GetName());
	}
}

void ULSCombatComponent::UnequipWeapon()
{
	if (CurrentWeapon)
	{
		StopFire();
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
		OnWeaponChanged.Broadcast(nullptr);
	}
}

void ULSCombatComponent::StartFire()
{
	if (CurrentWeapon && !bIsFiring)
	{
		bIsFiring = true;
		CurrentWeapon->StartFire();
	}
}

void ULSCombatComponent::StopFire()
{
	if (CurrentWeapon && bIsFiring)
	{
		bIsFiring = false;
		CurrentWeapon->StopFire();
	}
}

void ULSCombatComponent::Reload()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Reload();
	}
}
