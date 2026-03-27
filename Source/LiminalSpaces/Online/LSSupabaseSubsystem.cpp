#include "Online/LSSupabaseSubsystem.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"
#include "Misc/ConfigCacheIni.h"
#include "LiminalSpaces.h"

void ULSSupabaseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Load config from DefaultGame.ini
	GConfig->GetString(TEXT("/Script/LiminalSpaces.LSSupabaseSubsystem"), TEXT("SupabaseURL"), SupabaseURL, GGameIni);
	GConfig->GetString(TEXT("/Script/LiminalSpaces.LSSupabaseSubsystem"), TEXT("SupabaseAnonKey"), SupabaseAnonKey, GGameIni);

	if (SupabaseURL.IsEmpty())
	{
		UE_LOG(LogLiminalSpaces, Warning, TEXT("Supabase URL not configured. Set it in DefaultGame.ini"));
	}
	else
	{
		UE_LOG(LogLiminalSpaces, Log, TEXT("Supabase subsystem initialized with URL: %s"), *SupabaseURL);
	}
}

void ULSSupabaseSubsystem::SignUp(const FString& Email, const FString& Password, FOnAuthComplete OnComplete)
{
	FString Body = FString::Printf(TEXT("{\"email\":\"%s\",\"password\":\"%s\"}"), *Email, *Password);

	SendRequest(TEXT("/auth/v1/signup"), TEXT("POST"), Body, [this, OnComplete](bool bSuccess, const FString& Response)
	{
		if (bSuccess)
		{
			ParseAuthResponse(Response);
		}
		OnComplete.ExecuteIfBound(bSuccess && CurrentSession.bIsValid, bSuccess ? TEXT("") : Response);
	});
}

void ULSSupabaseSubsystem::SignIn(const FString& Email, const FString& Password, FOnAuthComplete OnComplete)
{
	FString Body = FString::Printf(TEXT("{\"email\":\"%s\",\"password\":\"%s\"}"), *Email, *Password);

	SendRequest(TEXT("/auth/v1/token?grant_type=password"), TEXT("POST"), Body, [this, OnComplete](bool bSuccess, const FString& Response)
	{
		if (bSuccess)
		{
			ParseAuthResponse(Response);
		}
		OnComplete.ExecuteIfBound(bSuccess && CurrentSession.bIsValid, bSuccess ? TEXT("") : Response);
	});
}

void ULSSupabaseSubsystem::SignOut()
{
	CurrentSession = FLSAuthSession();
	UE_LOG(LogLiminalSpaces, Log, TEXT("Signed out"));
}

void ULSSupabaseSubsystem::RefreshSession(FOnAuthComplete OnComplete)
{
	if (CurrentSession.RefreshToken.IsEmpty())
	{
		OnComplete.ExecuteIfBound(false, TEXT("No refresh token"));
		return;
	}

	FString Body = FString::Printf(TEXT("{\"refresh_token\":\"%s\"}"), *CurrentSession.RefreshToken);

	SendRequest(TEXT("/auth/v1/token?grant_type=refresh_token"), TEXT("POST"), Body, [this, OnComplete](bool bSuccess, const FString& Response)
	{
		if (bSuccess)
		{
			ParseAuthResponse(Response);
		}
		OnComplete.ExecuteIfBound(bSuccess && CurrentSession.bIsValid, bSuccess ? TEXT("") : Response);
	});
}

void ULSSupabaseSubsystem::SavePlayerData(const FString& TableName, const FString& JsonData, FOnDataFetched OnComplete)
{
	FString Endpoint = FString::Printf(TEXT("/rest/v1/%s"), *TableName);

	SendRequest(Endpoint, TEXT("POST"), JsonData, [OnComplete](bool bSuccess, const FString& Response)
	{
		OnComplete.ExecuteIfBound(bSuccess, Response);
	});
}

void ULSSupabaseSubsystem::FetchLeaderboard(const FString& MapID, FOnDataFetched OnComplete)
{
	FString Endpoint = FString::Printf(TEXT("/rest/v1/leaderboard?map_id=eq.%s&order=completion_time.asc&limit=100"), *MapID);

	SendRequest(Endpoint, TEXT("GET"), TEXT(""), [OnComplete](bool bSuccess, const FString& Response)
	{
		OnComplete.ExecuteIfBound(bSuccess, Response);
	});
}

void ULSSupabaseSubsystem::SendRequest(const FString& Endpoint, const FString& Method, const FString& Body, TFunction<void(bool, const FString&)> Callback)
{
	if (SupabaseURL.IsEmpty() || SupabaseAnonKey.IsEmpty())
	{
		if (Callback)
		{
			Callback(false, TEXT("Supabase not configured"));
		}
		return;
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(SupabaseURL + Endpoint);
	Request->SetVerb(Method);
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("apikey"), SupabaseAnonKey);

	if (CurrentSession.bIsValid)
	{
		Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *CurrentSession.AccessToken));
	}

	if (!Body.IsEmpty())
	{
		Request->SetContentAsString(Body);
	}

	Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Req, FHttpResponsePtr Resp, bool bConnectedSuccessfully)
	{
		if (bConnectedSuccessfully && Resp.IsValid() && EHttpResponseCodes::IsOk(Resp->GetResponseCode()))
		{
			if (Callback)
			{
				Callback(true, Resp->GetContentAsString());
			}
		}
		else
		{
			FString ErrorMsg = Resp.IsValid() ? Resp->GetContentAsString() : TEXT("Connection failed");
			if (Callback)
			{
				Callback(false, ErrorMsg);
			}
		}
	});

	Request->ProcessRequest();
}

void ULSSupabaseSubsystem::ParseAuthResponse(const FString& ResponseBody)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		CurrentSession.AccessToken = JsonObject->GetStringField(TEXT("access_token"));
		CurrentSession.RefreshToken = JsonObject->GetStringField(TEXT("refresh_token"));

		if (const TSharedPtr<FJsonObject>* UserObject = nullptr; JsonObject->TryGetObjectField(TEXT("user"), UserObject))
		{
			CurrentSession.UserID = (*UserObject)->GetStringField(TEXT("id"));
			CurrentSession.Email = (*UserObject)->GetStringField(TEXT("email"));
		}

		CurrentSession.bIsValid = !CurrentSession.AccessToken.IsEmpty();

		UE_LOG(LogLiminalSpaces, Log, TEXT("Auth successful: UserID=%s"), *CurrentSession.UserID);
	}
}
