#pragma once
#include <sol/sol.hpp>

class FCoroutineTask
{
public:
    virtual ~FCoroutineTask() {}
    virtual bool IsComplete(float DeltaTime) = 0;
};

class WaitTask : public FCoroutineTask
{
public:
    WaitTask(float InDuration) : Duration(InDuration), ElapsedTime(0.0f) {}

    virtual bool IsComplete(float DeltaTime) override
    {
        ElapsedTime += DeltaTime;
        return ElapsedTime >= Duration;
    }

private:
    float Duration;
    float ElapsedTime;
};
