#include "LuaScriptComponent.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "LuaBindingHelpers.h"

ULuaScriptComponent::ULuaScriptComponent()
{
    LuaState.open_libraries();
}

ULuaScriptComponent::~ULuaScriptComponent()
{
}

void ULuaScriptComponent::BeginPlay()
{
    InitializeLuaState();
    CallLuaFunction("BeginPlay");
    Super::BeginPlay();
    // GetOwner()->SetActorTickInPIE(true); // Lua 스크립트 대상 - PIE의 tick 호출 대상

}

void ULuaScriptComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    CallLuaFunction("EndPlay");
}

void ULuaScriptComponent::InitializeLuaState()
{
    LuaState.open_libraries();

    BindEngineAPI();

    try {
        LuaState.script_file((*ScriptPath));
        bScriptValid = true;
    }
    catch (const sol::error& err) {
        UE_LOG(LogLevel::Error, TEXT("Lua Initialization error: %s"), err.what());
    }
}

void ULuaScriptComponent::BindEngineAPI()
{
    LuaBindingHelpers::BindUE_LOG(LuaState);    // 1) UE_LOG 바인딩
    LuaBindingHelpers::BindFVector(LuaState);   // 2) FVector 바인딩

    /*bool bHasFVector = LuaState["FVector"].valid();
    bool bHasUELOG =   LuaState["UE_LOG"].valid();
    UE_LOG(LogLevel::Error, TEXT("LuaBindings – FVector valid=%s, UE_LOG valid=%s"),
        bHasFVector ? TEXT("true") : TEXT("false"),
        bHasUELOG ? TEXT("true") : TEXT("false"));*/

    // 2) AActor 바인딩 및 Location Property
    auto ActorType = LuaState.new_usertype<AActor>("Actor",
        sol::constructors<>(),
        "Location", sol::property(
            &AActor::GetActorLocation,
            &AActor::SetActorLocation
        ),
        "GetLocation", &AActor::GetActorLocation,
        "SetLocation", &AActor::SetActorLocation
    );

    // 프로퍼티 바인딩
    LuaState["actor"] = GetOwner();
    LuaState["script"] = this;
}

void ULuaScriptComponent::CallLuaFunction(const FString& FunctionName)
{
    if (!bScriptValid) return;

    sol::protected_function func = LuaState[FunctionName];
    if (func.valid()) {
        auto result = func();
        if (!result.valid()) {
            sol::error err = result;
            UE_LOG(LogLevel::Error, TEXT("Lua Function Error: %s"), err.what());
        }
    }
}

void ULuaScriptComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);

    if (bScriptValid && LuaState["Tick"].valid()) {
        LuaState["Tick"](DeltaTime);
        CallLuaFunction("Tick");
    }
}
