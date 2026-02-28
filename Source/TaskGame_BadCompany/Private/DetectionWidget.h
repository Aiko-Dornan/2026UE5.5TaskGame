#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DetectionWidget.generated.h"

class UProgressBar;

UCLASS()
class TASKGAME_BADCOMPANY_API UDetectionWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION()
    void UpdateDetection(float Value);

protected:
    virtual bool Initialize() override;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* DetectionBar;

   

    FLinearColor GetColorByDetection(float Value) const;

};