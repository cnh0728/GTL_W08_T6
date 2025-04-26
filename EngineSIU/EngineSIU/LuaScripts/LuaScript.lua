-- LuaScripts/LuaScript.lua

function Update(DeltaTime)
    local MoveLocation = actor.Location
    MoveLocation = MoveLocation + (actor:Forward() * DeltaTime)
    actor.Location = MoveLocation
end
 
