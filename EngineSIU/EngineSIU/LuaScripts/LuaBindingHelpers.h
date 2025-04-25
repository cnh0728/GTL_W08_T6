#pragma once
#include <sol/sol.hpp>
#include "Runtime/Core/Math/Vector.h"
#include "Runtime/Engine/UserInterface/Console.h"

namespace LuaBindingHelpers
{
    // FVector 타입 바인딩 함수
    inline void BindFVector(sol::state& Lua)
    {
        Lua.new_usertype<FVector>("FVector",
            sol::constructors<
            FVector(),               // 기본 생성자
            FVector(float, float, float)  // XYZ 지정 생성자
            >(),
            // 멤버 변수 노출
            "X", &FVector::X,
            "Y", &FVector::Y,
            "Z", &FVector::Z,

            // 덧셈
            sol::meta_function::addition,
            [](const FVector& a, const FVector& b) { return a + b; },

            // 뺄셈
            sol::meta_function::subtraction,
            [](const FVector& a, const FVector& b) { return a - b; },

            // 곱셈 (vector * scalar, scalar * vector)
            sol::meta_function::multiplication,
            sol::overload(
                [](const FVector& v, float s) { return v * s; },
                [](float s, const FVector& v) { return v * s; }
            ),

            // 나눗셈 (vector / scalar)
            sol::meta_function::division,
            [](const FVector& v, float s) { return v / s; }
        );
    }

    // UE_LOG 바인딩 함수
    inline void BindUE_LOG(sol::state& Lua)
    {
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
    }
}
