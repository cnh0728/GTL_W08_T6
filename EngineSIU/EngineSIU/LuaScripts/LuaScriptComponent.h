#pragma once
#include "Runtime/CoreUObject/UObject/ObjectMacros.h"
#include "Components/ActorComponent.h"
#include <sol/sol.hpp>

class ULuaScriptComponent : public UActorComponent
{
    DECLARE_CLASS(ULuaScriptComponent, UActorComponent)

public:
    ULuaScriptComponent();
    ~ULuaScriptComponent();

    FString ScriptPath = FString(L"LuaScripts\\template.lua");

    TMap<FString, sol::object> ExposedProperties;

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime) override;

    // Lua 함수 호출 메서드
    void CallLuaFunction(const FString& FunctionName);

private:
    // Lua 환경 초기화
    void InitializeLuaState();

    // Lua-Engine 바인딩
    void BindEngineAPI();

    sol::state LuaState;
    bool bScriptValid = false;
};
