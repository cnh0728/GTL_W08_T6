#pragma once
#include "Actor.h"
#include "Delegates/DelegateCombination.h"

DECLARE_MULTICAST_DELEGATE(FOnGameInit);
DECLARE_MULTICAST_DELEGATE(FOnGameStart);
DECLARE_MULTICAST_DELEGATE(FOnGameEnd);

struct FGameInfo
{
    float TotalGameTime = 0.0f;
    float ElapsedGameTime = 0.0f;
};

class AGameMode : public AActor
{
    DECLARE_CLASS(AGameMode, AActor)
public:
    AGameMode();
    virtual ~AGameMode() override;
    void InitializeComponent();
    UObject* Duplicate(UObject* InOuter) override;

    //virtual void BeginPlay() override;
    //virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


    // 게임 모드 초기화
    virtual void InitGame();

    // 게임 시작
    virtual void StartMatch();

    // 게임 종료
    virtual void EndMatch();

    virtual void Tick(float DeltaTime) override;


public:
    FOnGameInit OnGameInit;
    FOnGameStart OnGameStart;
    FOnGameEnd OnGameEnd;

    bool     bGameRunning = false; // 내부 
    bool     bGameEnded = true;
    FGameInfo GameInfo;


    float LogTimer = 0.f;
    float LogInterval = 1.f;  // 1초마다 로그
};
