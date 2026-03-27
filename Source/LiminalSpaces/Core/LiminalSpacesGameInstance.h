#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Data/LSTypes.h"
#include "LiminalSpacesGameInstance.generated.h"

UCLASS()
class LIMINALSPACES_API ULiminalSpacesGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	ULiminalSpacesGameInstance();

	virtual void Init() override;

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Auth")
	const FLSAuthSession& GetAuthSession() const { return AuthSession; }

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Auth")
	void SetAuthSession(const FLSAuthSession& InSession);

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Auth")
	bool IsLoggedIn() const { return AuthSession.bIsValid; }

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Auth")
	void ClearAuthSession();

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Settings")
	float GetMasterVolume() const { return MasterVolume; }

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Settings")
	void SetMasterVolume(float Volume);

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Settings")
	float GetMouseSensitivity() const { return MouseSensitivity; }

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Settings")
	void SetMouseSensitivity(float Sensitivity);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Settings")
	void SaveSettings();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Settings")
	void LoadSettings();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "LiminalSpaces|Auth")
	FLSAuthSession AuthSession;

	UPROPERTY(BlueprintReadWrite, Category = "LiminalSpaces|Settings")
	float MasterVolume = 1.0f;

	UPROPERTY(BlueprintReadWrite, Category = "LiminalSpaces|Settings")
	float MouseSensitivity = 1.0f;
};
