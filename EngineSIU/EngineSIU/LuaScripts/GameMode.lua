function BeginPlay(self)
    OnGameInit(function()
        print("Lua: Game Initialized")
    end)

    -- bind space to start
    BindKeyDown(KEY_SPACE, function()
        if not GameMode:IsRunning() and GameMode:IsEnded() then
            GameMode:StartMatch()
        end
    end)
    -- bind ctrl to end
    BindKeyDown(KEY_CTRL, function()
        if GameMode:IsRunning() and not GameMode:IsEnded() then
            GameMode:EndMatch()
        end
    end)
end

function Tick(dt)
    UE_LOG("Error", "Tick Game Manager")

    if GameMode:IsRunning() and not GameMode:IsEnded() then
        local et = GameMode:GetElapsedTime() + dt * 0.5
        GameMode:SetElapsedTime(et)
        -- log every second
        self._log_timer = (self._log_timer or 0) + dt * 0.5
        if self._log_timer >= 1.0 then
            print(string.format("Game Time: %.2f", et))
            self._log_timer = self._log_timer - 1.0
        end
    end
end