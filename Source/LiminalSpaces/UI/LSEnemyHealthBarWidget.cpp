#include "UI/LSEnemyHealthBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void ULSEnemyHealthBarWidget::SetHealthPercent(float Percent)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(FMath::Clamp(Percent, 0.0f, 1.0f));

		FLinearColor Color = FLinearColor::LerpUsingHSV(FLinearColor::Red, FLinearColor(1.0f, 0.3f, 0.0f), Percent);
		HealthBar->SetFillColorAndOpacity(Color);
	}
}

void ULSEnemyHealthBarWidget::SetEnemyName(const FText& Name)
{
	if (EnemyNameText)
	{
		EnemyNameText->SetText(Name);
	}
}
