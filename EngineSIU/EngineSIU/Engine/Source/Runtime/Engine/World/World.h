#pragma once
#include "Define.h"
#include "Container/Set.h"
#include "UObject/ObjectFactory.h"
#include "UObject/ObjectMacros.h"
#include "WorldType.h"
#include "Level.h"
#include "Actors/Player.h"
#include "Components/CameraComponent.h"
#include "Engine/EventManager.h"

class UPrimitiveComponent;
struct FOverlapResult;
class UCameraComponent;
class FObjectFactory;
class AActor;
class UObject;
class USceneComponent;
class FCollisionManager;
class AGameMode;

class UWorld : public UObject
{
    DECLARE_CLASS(UWorld, UObject)

public:
    UWorld() = default;

    static UWorld* CreateWorld(UObject* InOuter, const EWorldType InWorldType, const FString& InWorldName = "DefaultWorld");

    void InitializeNewWorld();
    void InitializeLightScene();
    virtual UObject* Duplicate(UObject* InOuter) override;

    void Tick(float DeltaTime);
    void BeginPlay();

    void Release();

    /**
     * World에 Actor를 Spawn합니다.
     * @param InClass Spawn할 Actor 정보
     * @return Spawn된 Actor
     */
    AActor* SpawnActor(UClass* InClass, FName InActorName = NAME_None);

    /** 
     * World에 Actor를 Spawn합니다.
     * @tparam T AActor를 상속받은 클래스
     * @return Spawn된 Actor의 포인터
     */
    template <typename T>
        requires std::derived_from<T, AActor>
    T* SpawnActor();

    /** World에 존재하는 Actor를 제거합니다. */
    bool DestroyActor(AActor* ThisActor);

    virtual UWorld* GetWorld() const override;
    ULevel* GetActiveLevel() const { return ActiveLevel; }

    template <typename T>
        requires std::derived_from<T, AActor>
    T* DuplicateActor(T* InActor);

    EWorldType WorldType = EWorldType::None;
    
    FEventManager EventManager;

    void SetMainCamera(UCameraComponent* InCamera) { MainCamera = InCamera; }
    UCameraComponent* GetMainCamera(){ return MainCamera; }
    void SetMainPlayer(APlayer* InPlayer){ MainPlayer = InPlayer; }
    APlayer* GetMainPlayer(){ return MainPlayer; }
    
    void CheckOverlap(const UPrimitiveComponent* Component, TArray<FOverlapResult>& OutOverlaps) const;

    AGameMode* GetGameMode() const { return GameMode; }
private:
    AGameMode* GameMode = nullptr;

    FString WorldName = "DefaultWorld";

    ULevel* ActiveLevel;

    /** Actor가 Spawn되었고, 아직 BeginPlay가 호출되지 않은 Actor들 */
    TArray<AActor*> PendingBeginPlayActors;

    UCameraComponent* MainCamera = nullptr;

    APlayer* MainPlayer = nullptr;

    FCollisionManager* CollisionManager = nullptr;
};


template <typename T>
    requires std::derived_from<T, AActor>
T* UWorld::SpawnActor()
{
    return Cast<T>(SpawnActor(T::StaticClass()));
}

template <typename T>
    requires std::derived_from<T, AActor>
T* UWorld::DuplicateActor(T* InActor)
{
    if (ULevel* ActiveLevel = GetActiveLevel())
    {
        T* NewActor = static_cast<T*>(InActor->Duplicate(this));
        ActiveLevel->Actors.Add(NewActor);
        PendingBeginPlayActors.Add(NewActor);
        return NewActor;
    }
    return nullptr;
}

