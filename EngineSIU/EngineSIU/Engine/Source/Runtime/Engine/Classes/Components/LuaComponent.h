#pragma once
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include "Components/ActorComponent.h"

class ULuaComponent : public UActorComponent
{
    DECLARE_CLASS(ULuaComponent, UActorComponent)

public:
    ULuaComponent();
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime) override;

    FString ScriptPath = FString(L"LuaScripts\\LuaScript.lua");
    
private:
    sol::state Lua;
    
};
