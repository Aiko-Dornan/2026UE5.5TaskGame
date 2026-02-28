#include "DetectionWidget.h"
#include "Components/ProgressBar.h"

bool UDetectionWidget::Initialize()
{
    return Super::Initialize();
}

void UDetectionWidget::UpdateDetection(float Value)
{
    if (!DetectionBar) return;

    Value = FMath::Clamp(Value, 0.f, 1.f);

    DetectionBar->SetPercent(Value);

    FLinearColor NewColor = GetColorByDetection(Value);
    DetectionBar->SetFillColorAndOpacity(NewColor);
}

FLinearColor UDetectionWidget::GetColorByDetection(float Value) const
{
    if (Value < 0.4f)
    {
        return FLinearColor::Green;
    }
    else if (Value < 0.8f)
    {
        return FLinearColor::Yellow;
    }
    else
    {
        return FLinearColor::Red;
    }
}