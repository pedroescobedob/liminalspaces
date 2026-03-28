#include "UI/LSHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void ULSHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (InteractionText)
	{
		InteractionText->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (EscapeText)
	{
		EscapeText->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (DamageOverlay)
	{
		DamageOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ULSHUDWidget::UpdateHealth(float HealthPercent)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(HealthPercent);

		// Color shift from green to red based on health
		FLinearColor HealthColor = FLinearColor::LerpUsingHSV(FLinearColor::Red, FLinearColor::Green, HealthPercent);
		HealthBar->SetFillColorAndOpacity(HealthColor);
	}
}

void ULSHUDWidget::UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (AmmoText)
	{
		AmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentAmmo, MaxAmmo)));
	}
}

void ULSHUDWidget::ShowInteractionPrompt(const FText& Prompt)
{
	if (InteractionText)
	{
		InteractionText->SetText(Prompt);
		InteractionText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void ULSHUDWidget::HideInteractionPrompt()
{
	if (InteractionText)
	{
		InteractionText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ULSHUDWidget::ShowEscapeMessage(float CompletionTime)
{
	if (EscapeText)
	{
		int32 Minutes = FMath::FloorToInt(CompletionTime / 60.0f);
		int32 Seconds = FMath::FloorToInt(FMath::Fmod(CompletionTime, 60.0f));
		FString TimeString = FString::Printf(TEXT("ESCAPED!\nTime: %02d:%02d"), Minutes, Seconds);
		EscapeText->SetText(FText::FromString(TimeString));
		EscapeText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void ULSHUDWidget::ShowDamageIndicator()
{
	if (DamageOverlay)
	{
		DamageOverlay->SetVisibility(ESlateVisibility::HitTestInvisible);
		DamageOverlay->SetRenderOpacity(0.5f);

		// Fade out via timer in Blueprint (or add UWidgetAnimation binding)
	}
}
