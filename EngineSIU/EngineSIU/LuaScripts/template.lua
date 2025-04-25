function BeginPlay()
    print("Lua BeginPlay function called!")
    -- UE_LOG("Error", "actor:beginplay")
    -- print("[BeginPlay] " .. actor.UUID)
    -- actor.Location = actor.Location
    -- actor:PrintLocation()
end

function EndPlay()
    print("[EndPlay] " .. actor.UUID)
    -- obj:PrintLocation()
end

function OnOverlap(OtherActor)
    OtherActor:PrintLocation();
end

function Tick(dt)
    local currentPos = actor:GetLocation()
    currentPos.X = currentPos.X + dt * 1
    actor:SetLocation(currentPos)
    -- actor:SetLocation(newPos)
    -- actor:SetLocation(FVector(200, 0, 0))
    UE_LOG("Error", "actor:SetLocation")
    -- actor.Location = FVector(0, 0, 0)
    -- actor.Location.X = actor.Location.X + dt
end
