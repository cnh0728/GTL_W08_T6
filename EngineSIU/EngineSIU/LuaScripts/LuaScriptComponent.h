#pragma once
#include "Runtime/CoreUObject/UObject/ObjectMacros.h"
#include "Components/ActorComponent.h"
#include <sol/sol.hpp>
#include <filesystem>

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLocationTenUp, const FVector);

class ULuaScriptComponent : public UActorComponent
{
    DECLARE_CLASS(ULuaScriptComponent, UActorComponent)

public:
    ULuaScriptComponent();
    ~ULuaScriptComponent();

    FString BasePath = FString(L"LuaScripts");

    TMap<FString, sol::object> ExposedProperties;

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime) override;
    virtual UObject* Duplicate(UObject* InOuter) override;
    virtual void InitializeComponent() override;

    // Lua 함수 호출 메서드
    template<typename... Arguments> void CallLuaFunction(const FString& FunctionName, Arguments... args);
    
    FString GetScriptPath() const { return ScriptPath; }
    void SetScriptPath(const FString& InScriptPath);

    FString GetDisplayName() const { return DisplayName; }
    void SetDisplayName(const FString& InDisplayName) { DisplayName = InDisplayName; }

    void OnPressSpacebar()
    {
        UE_LOG(LogLevel::Error, "Deligate Space Press");
    }

    FOnLocationTenUp FOnLocationTenUp;
    
private:
    // Lua 환경 초기화
    void InitializeLuaState();

    // Lua-Engine 바인딩
    void BindEngineAPI();
    bool CheckFileModified();
    void ReloadScript();

    FString ScriptPath;
    FString DisplayName;

    TArray<FDelegateHandle> DelegateHandles;
    
    sol::state LuaState;
    sol::environment LuaEnv;

    std::filesystem::file_time_type LastWriteTime;

    bool bScriptValid = false;
};

template <typename ... Arguments>
void ULuaScriptComponent::CallLuaFunction(const FString& FunctionName, Arguments... args)
{
    if (bScriptValid && LuaState[*FunctionName].valid())
    {
        LuaState[*FunctionName](args...);
    }
}
