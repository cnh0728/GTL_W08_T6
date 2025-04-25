-- LuaScripts/LuaScript.lua
local Component = USceneComponent.new()

function Init()
    log("Game Initialized!")
    Component:SetLocation(0,0,0)
end

function Update(DeltaTime)
    local MoveLocation = Component:GetLocation() + (Component:Forward() * DeltaTime)
    Component:SetLocation(MoveLocation)
end
 
