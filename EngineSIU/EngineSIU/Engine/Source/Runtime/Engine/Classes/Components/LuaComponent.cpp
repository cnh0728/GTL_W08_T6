#include "LuaComponent.h"

#include "Components/SceneComponent.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"

ULuaComponent::ULuaComponent()
{
    // Lua 기본 라이브러리 로드
    Lua.open_libraries();
}

void ULuaComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);

    sol::protected_function LuaUpdate = Lua["Update"];
    if (LuaUpdate.valid()) {
        auto result = LuaUpdate(DeltaTime);
        if (!result.valid()) {
            sol::error err = result;
            UE_LOG(LogLevel::Error, TEXT("Lua Function Error: %s"), *FString(err.what()));
        }
    }
}

void ULuaComponent::BeginPlay()
{
    Super::BeginPlay();
    
    Lua.new_usertype<FVector>("FVector",
        sol::constructors<FVector(), FVector(float, float, float)>(),
        "X", &FVector::X,
        "Y", &FVector::Y,
        "Z", &FVector::Z,
        sol::meta_function::addition, [](const FVector& a, const FVector& b){ return a + b;},
        sol::meta_function::multiplication,
        sol::overload(
            [](const FVector& v, float s) { return v * s; },
            [](float s, const FVector& v) { return v * s; }
        )
    );
    
    Lua.set_function("UE_LOG",
        [](const std::string& Level, const std::string& Msg)
        {
            FString Converted = FString(Msg.c_str());
            if (Level == "Error")
            {
                UE_LOG(LogLevel::Error, TEXT("%s"), *Converted);
            }
            else if (Level == "Warning")
            {
                UE_LOG(LogLevel::Warning, TEXT("%s"), *Converted);
            }
            else
            {
                UE_LOG(LogLevel::Display, TEXT("%s"), *Converted);
            }
        }
    );
    
    Lua["actor"] = GetOwner();
    Lua["script"] = this;

    auto ActorType = Lua.new_usertype<AActor>("Actor",
        sol::constructors<>(),
        "Location", sol::property(
            &AActor::GetActorLocation,
            &AActor::SetActorLocation
        ),
        "Forward", &AActor::GetActorForwardVector
    );
    
    try
    {
        Lua.script_file(*ScriptPath);
        UE_LOG(LogLevel::Error, TEXT("SUCCESS Load LuaScript"));
    }catch (const sol::error& e)
    {
        UE_LOG(LogLevel::Error, "Failed Load LuaScript");
    }
}

void ULuaComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}
