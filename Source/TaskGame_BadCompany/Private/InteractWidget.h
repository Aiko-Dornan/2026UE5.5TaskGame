#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractWidget.generated.h"

class UTextBlock;

UCLASS()
class TASKGAME_BADCOMPANY_API UInteractWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // テキスト更新関数
    UFUNCTION(BlueprintCallable)
    void SetInteractText(const FString& NewText);

protected:
    // BP側で Bind する
    UPROPERTY(meta = (BindWidget))
    UTextBlock* InteractText;
};