#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LSAudioManager.generated.h"

class USoundBase;
class UAudioComponent;

UCLASS()
class LIMINALSPACES_API ULSAudioManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Audio")
	void PlayAmbientMusic(USoundBase* Music);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Audio")
	void StopAmbientMusic(float FadeOutDuration = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Audio")
	void SetMasterVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Audio")
	void SetMusicVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Audio")
	void SetSFXVolume(float Volume);

protected:
	UPROPERTY()
	TObjectPtr<UAudioComponent> MusicComponent;

	float MasterVolume = 1.0f;
	float MusicVolume = 0.7f;
	float SFXVolume = 1.0f;
};
