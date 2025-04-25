#pragma once
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include "Components/ActorComponent.h"

class ULuaComponent : UActorComponent
{
public:
    ULuaComponent();
    virtual void TickComponent(float DeltaTime) override;

private:
    sol::state Lua;
    
};
