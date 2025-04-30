#pragma once
#include "CameraModifier.h"

struct FPooledCameraShakes
{
    // TArray<TObjectPtr<UCameraShakeBase>> PooledShakes;
};

class UCameraModifier_CameraShake : public UCameraModifier
{
    DECLARE_CLASS(UCameraModifier_CameraShake, UCameraModifier)

public:
    UCameraModifier_CameraShake() = default;

    virtual bool ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV) override;
};
