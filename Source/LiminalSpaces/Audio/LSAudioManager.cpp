#include "Audio/LSAudioManager.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LiminalSpaces.h"

void ULSAudioManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogLiminalSpaces, Log, TEXT("Audio Manager initialized"));
}

void ULSAudioManager::PlayAmbientMusic(USoundBase* Music)
{
	if (!Music)
	{
		return;
	}

	StopAmbientMusic(0.5f);

	if (UWorld* World = GetWorld())
	{
		MusicComponent = UGameplayStatics::SpawnSound2D(World, Music, MusicVolume * MasterVolume, 1.0f, 0.0f, nullptr, false, false);
	}
}

void ULSAudioManager::StopAmbientMusic(float FadeOutDuration)
{
	if (MusicComponent)
	{
		MusicComponent->FadeOut(FadeOutDuration, 0.0f);
		MusicComponent = nullptr;
	}
}

void ULSAudioManager::SetMasterVolume(float Volume)
{
	MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
}

void ULSAudioManager::SetMusicVolume(float Volume)
{
	MusicVolume = FMath::Clamp(Volume, 0.0f, 1.0f);

	if (MusicComponent)
	{
		MusicComponent->SetVolumeMultiplier(MusicVolume * MasterVolume);
	}
}

void ULSAudioManager::SetSFXVolume(float Volume)
{
	SFXVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
}
