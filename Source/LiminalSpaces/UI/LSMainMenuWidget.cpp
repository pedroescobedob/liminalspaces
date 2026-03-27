#include "UI/LSMainMenuWidget.h"
#include "MediaPlayer.h"
#include "MediaSource.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LiminalSpaces.h"

void ULSMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button delegates
	if (PlayButton)
	{
		PlayButton->OnClicked.AddDynamic(this, &ULSMainMenuWidget::OnPlayClicked);
	}
	if (SettingsButton)
	{
		SettingsButton->OnClicked.AddDynamic(this, &ULSMainMenuWidget::OnSettingsClicked);
	}
	if (LoginButton)
	{
		LoginButton->OnClicked.AddDynamic(this, &ULSMainMenuWidget::OnLoginClicked);
	}
	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &ULSMainMenuWidget::OnQuitClicked);
	}

	StartVideoBackground();
}

void ULSMainMenuWidget::NativeDestruct()
{
	StopVideoBackground();
	Super::NativeDestruct();
}

void ULSMainMenuWidget::StartVideoBackground()
{
	if (BackgroundMediaPlayer && BackgroundMediaSource)
	{
		BackgroundMediaPlayer->SetLooping(true);
		BackgroundMediaPlayer->OpenSource(BackgroundMediaSource);
		BackgroundMediaPlayer->Play();

		UE_LOG(LogLiminalSpaces, Log, TEXT("Main menu video background started"));
	}
}

void ULSMainMenuWidget::StopVideoBackground()
{
	if (BackgroundMediaPlayer)
	{
		BackgroundMediaPlayer->Close();
	}
}

void ULSMainMenuWidget::OnPlayClicked()
{
	UE_LOG(LogLiminalSpaces, Log, TEXT("Play clicked - opening map select"));
	// Override in Blueprint to show map select widget
}

void ULSMainMenuWidget::OnSettingsClicked()
{
	UE_LOG(LogLiminalSpaces, Log, TEXT("Settings clicked"));
	// Override in Blueprint to show settings widget
}

void ULSMainMenuWidget::OnLoginClicked()
{
	UE_LOG(LogLiminalSpaces, Log, TEXT("Login clicked"));
	// Override in Blueprint to show login widget
}

void ULSMainMenuWidget::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}
