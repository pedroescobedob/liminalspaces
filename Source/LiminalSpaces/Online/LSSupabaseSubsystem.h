#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/LSTypes.h"
#include "LSSupabaseSubsystem.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnAuthComplete, bool, bSuccess, const FString&, ErrorMessage);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnDataFetched, bool, bSuccess, const FString&, JsonResponse);

UCLASS()
class LIMINALSPACES_API ULSSupabaseSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Auth")
	void SignUp(const FString& Email, const FString& Password, FOnAuthComplete OnComplete);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Auth")
	void SignIn(const FString& Email, const FString& Password, FOnAuthComplete OnComplete);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Auth")
	void SignOut();

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Auth")
	void RefreshSession(FOnAuthComplete OnComplete);

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Auth")
	const FLSAuthSession& GetSession() const { return CurrentSession; }

	UFUNCTION(BlueprintPure, Category = "LiminalSpaces|Auth")
	bool IsAuthenticated() const { return CurrentSession.bIsValid; }

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Data")
	void SavePlayerData(const FString& TableName, const FString& JsonData, FOnDataFetched OnComplete);

	UFUNCTION(BlueprintCallable, Category = "LiminalSpaces|Data")
	void FetchLeaderboard(const FString& MapID, FOnDataFetched OnComplete);

protected:
	UPROPERTY()
	FString SupabaseURL;

	UPROPERTY()
	FString SupabaseAnonKey;

	FLSAuthSession CurrentSession;

private:
	void SendRequest(const FString& Endpoint, const FString& Method, const FString& Body, TFunction<void(bool, const FString&)> Callback);
	void ParseAuthResponse(const FString& ResponseBody);
};
