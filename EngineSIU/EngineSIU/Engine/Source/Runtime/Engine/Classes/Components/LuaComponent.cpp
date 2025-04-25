#include "LuaComponent.h"

#include "Components/SceneComponent.h"
#include "Container/CString.h"

ULuaComponent::ULuaComponent()
{
    // Lua 기본 라이브러리 로드
    Lua.open_libraries(sol::lib::base, sol::lib::math);

    // C++ 함수 노출
    Lua.set_function("log", [](const std::string& msg)
    {
       OutputDebugStringA(("[LUA] "+ msg + "\n").c_str()); 
    });

    Lua.new_usertype<FVector>("FVector",
        sol::constructors<FVector(), FVector(float, float, float)>(),
        "X", &FVector::X,
        "Y", &FVector::Y,
        "Z", &FVector::Z,
        sol::meta_function::addition, [](const FVector& a, const FVector& b){ return a + b;},
        sol::meta_function::multiplication, sol::overload(
            [](const FVector&a, float b){return a * b;},
            [](const FVector&a, const FVector& b){return a * b;}
        )
    );
    
    //USceneComponent 바인딩
    Lua.new_usertype<USceneComponent>("USceneComponent",
        "GetLocation", &USceneComponent::GetRelativeLocation,
        "SetLocation", &USceneComponent::SetRelativeLocation,
        "Forward", &USceneComponent::GetForwardVector
    );

    // Lua.new_usertype<Console>("Console",
    //     "UE_LOG", &Console::AddLog
    // );
    
    Lua["DeltaTime"] = 0.f;

    try
    {
        Lua.script_file("LuaScripts/LuaScript.lua");
    }catch (const sol::error& e)
    {
        UE_LOG(LogLevel::Error, "Failed Load LuaScript");
    }
    
}

void ULuaComponent::TickComponent(float DeltaTime)
{
    Lua["DeltaTime"] = DeltaTime;
    
    sol::optional<sol::function> LuaUpdate = Lua["Update"];
    if (LuaUpdate) {
        (*LuaUpdate)(DeltaTime);
    }
}
