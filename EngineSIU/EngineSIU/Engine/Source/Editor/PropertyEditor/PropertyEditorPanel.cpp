#include "PropertyEditorPanel.h"

#include <algorithm>
#include <string>
#include <windows.h>
#include <tchar.h>
//#include <shlwapi.h>
#include <shellapi.h>

#include "World/World.h"
#include "Actors/Player.h"
#include "Components/Light/LightComponent.h"
#include "Components/Light/PointLightComponent.h"
#include "Components/Light/SpotLightComponent.h"
#include "Components/Light/DirectionalLightComponent.h"
#include "Components/Light/AmbientLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextComponent.h"
#include "Engine/EditorEngine.h"
#include "Engine/FLoaderOBJ.h"
#include "UnrealEd/ImGuiWidget.h"
#include "UObject/Casts.h"
#include "UObject/ObjectFactory.h"
#include "Engine/Engine.h"
#include "Components/HeightFogComponent.h"
#include "Components/ProjectileMovementComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/AssetManager.h"
#include "LevelEditor/SLevelEditor.h"
#include "Math/JungleMath.h"
#include "Renderer/ShadowManager.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UObject/UObjectIterator.h"
#include "LuaScripts/LuaScriptComponent.h"

void PropertyEditorPanel::Render()
{
    /* Pre Setup */
    float PanelWidth = (Width) * 0.2f - 6.0f;
    float PanelHeight = (Height) * 0.65f;

    float PanelPosX = (Width) * 0.8f + 5.0f;
    float PanelPosY = (Height) * 0.3f + 15.0f;

    ImVec2 MinSize(140, 370);
    ImVec2 MaxSize(FLT_MAX, 900);

    /* Min, Max Size */
    ImGui::SetNextWindowSizeConstraints(MinSize, MaxSize);

    /* Panel Position */
    ImGui::SetNextWindowPos(ImVec2(PanelPosX, PanelPosY), ImGuiCond_Always);

    /* Panel Size */
    ImGui::SetNextWindowSize(ImVec2(PanelWidth, PanelHeight), ImGuiCond_Always);

    /* Panel Flags */
    ImGuiWindowFlags PanelFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

    /* Render Start */
    ImGui::Begin("Detail", nullptr, PanelFlags);

    UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
    if (!Engine)
    {
        return;
    }

    AEditorPlayer* Player = Engine->GetEditorPlayer();
    AActor* PickedActor = Engine->GetSelectedActor();

    if (PickedActor)
    {
        ImGui::SetItemDefaultFocus();
        // TreeNode 배경색을 변경 (기본 상태)
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
        if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
        {
            Location = PickedActor->GetActorLocation();
            Rotation = PickedActor->GetActorRotation();
            Scale = PickedActor->GetActorScale();

            FImGuiWidget::DrawVec3Control("Location", Location, 0, 85);
            ImGui::Spacing();

            FImGuiWidget::DrawRot3Control("Rotation", Rotation, 0, 85);
            ImGui::Spacing();

            FImGuiWidget::DrawVec3Control("Scale", Scale, 0, 85);
            ImGui::Spacing();

            PickedActor->SetActorLocation(Location);
            PickedActor->SetActorRotation(Rotation);
            PickedActor->SetActorScale(Scale);

            std::string coordiButtonLabel;
            if (Player->GetCoordMode() == ECoordMode::CDM_WORLD)
            {
                coordiButtonLabel = "World";
            }
            else if (Player->GetCoordMode() == ECoordMode::CDM_LOCAL)
            {
                coordiButtonLabel = "Local";
            }

            if (ImGui::Button(coordiButtonLabel.c_str(), ImVec2(ImGui::GetWindowContentRegionMax().x * 0.9f, 32)))
            {
                Player->AddCoordiMode();
            }

            ImGui::TreePop(); // 트리 닫기
        }

        ImGui::PopStyleColor();
    }

    if (PickedActor)
    {
        if (ImGui::Button("Duplicate"))
        {
            AActor* NewActor = Engine->ActiveWorld->DuplicateActor(Engine->GetSelectedActor());
            Engine->SelectActor(NewActor);
        }

        // Add Lua Script
        if (ImGui::Button("Create Script"))
        {
            // Lua Script Component 생성 및 추가
            ULuaScriptComponent* NewScript = PickedActor->AddComponent<ULuaScriptComponent>();
            // 기본 스크립트 경로 설정
            NewScript->ScriptPath = TEXT("LuaScripts\\template.lua");
        }

        if (ImGui::Button("Edit Script"))
        {
            // 예: PickedActor에서 스크립트 경로를 받아옴
            if (auto* ScriptComp = PickedActor->GetComponentByClass<ULuaScriptComponent>())
            {
                std::wstring luaFilePath = ScriptComp->ScriptPath.ToWideString();
                LPCTSTR filePath = luaFilePath.c_str();
                OpenLuaScriptFile(filePath);
            }
        }
    }

    //// TODO: 추후에 RTTI를 이용해서 프로퍼티 출력하기
    //if (PickedActor)
    //    if (ULightComponent* lightObj = PickedActor->GetComponentByClass<ULightComponent>())
    //    {
    //        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

    //        if (ImGui::TreeNodeEx("Light Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
    //        {
    //            /*  DrawColorProperty("Ambient Color",
    //                  [&]() { return lightObj->GetAmbientColor(); },
    //                  [&](FVector4 c) { lightObj->SetAmbientColor(c); });
    //              */
    //            DrawColorProperty("Base Color",
    //                [&]() { return lightObj->GetDiffuseColor(); },
    //                [&](FLinearColor c) { lightObj->SetDiffuseColor(c); });

    //            float Intensity = lightObj->GetIntensity();
    //            if (ImGui::SliderFloat("Intensity", &Intensity, 0.0f, 100.0f, "%1.f"))
    //                lightObj->SetIntensity(Intensity);

    //             /*  
    //            float falloff = lightObj->GetFalloff();
    //            if (ImGui::SliderFloat("Falloff", &falloff, 0.1f, 10.0f, "%.2f")) {
    //                lightObj->SetFalloff(falloff);
    //            }

    //            TODO : For SpotLight
    //            */

    //            float attenuation = lightObj->GetAttenuation();
    //            if (ImGui::SliderFloat("Attenuation", &attenuation, 0.01f, 10000.f, "%.1f")) {
    //                lightObj->SetAttenuation(attenuation);
    //            }

    //            float AttenuationRadius = lightObj->GetAttenuationRadius();
    //            if (ImGui::SliderFloat("Attenuation Radius", &AttenuationRadius, 0.01f, 10000.f, "%.1f")) {
    //                lightObj->SetAttenuationRadius(AttenuationRadius);
    //            }

    //            ImGui::TreePop();
    //        }

    //        ImGui::PopStyleColor();
    //    }

    if (PickedActor)
    {
        if (UPointLightComponent* PointlightComponent = PickedActor->GetComponentByClass<UPointLightComponent>())
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

            if (ImGui::TreeNodeEx("PointLight Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
            {
                DrawColorProperty("Light Color",
                    [&]() { return PointlightComponent->GetLightColor(); },
                    [&](FLinearColor c) { PointlightComponent->SetLightColor(c); });

                float Intensity = PointlightComponent->GetIntensity();
                if (ImGui::SliderFloat("Intensity", &Intensity, 0.0f, 160.0f, "%.1f"))
                {
                    PointlightComponent->SetIntensity(Intensity);
                }

                float Radius = PointlightComponent->GetRadius();
                if (ImGui::SliderFloat("Radius", &Radius, 0.01f, 200.f, "%.1f"))
                {
                    PointlightComponent->SetRadius(Radius);
                }
                // --- Cast Shadows 체크박스 추가 ---
                bool bCastShadows = PointlightComponent->GetCastShadows(); // 현재 상태 가져오기
                if (ImGui::Checkbox("Cast Shadows", &bCastShadows)) // 체크박스 UI 생성 및 상호작용 처리
                {
                    PointlightComponent->SetCastShadows(bCastShadows); // 변경된 상태 설정
                    // 필요하다면, 상태 변경에 따른 즉각적인 렌더링 업데이트 요청 로직 추가
                    // 예: PointlightComponent->MarkRenderStateDirty();
                }

                ImGui::Text("ShadowMap");

                FShadowCubeMapArrayRHI* pointRHI = FEngineLoop::Renderer.ShadowManager->GetPointShadowCubeMapRHI();
                const char* faceNames[] = { "+X", "-X", "+Y", "-Y", "+Z", "-Z" };
                float imageSize = 128.0f;
                int index =  PointlightComponent->GetPointLightInfo().ShadowMapArrayIndex;
                // CubeMap이므로 6개의 ShadowMap을 그립니다.
                for (int i = 0; i < 6; ++i)
                {
                    ID3D11ShaderResourceView* faceSRV = pointRHI->ShadowFaceSRVs[index][i];
                    if (faceSRV)
                    {
                        ImGui::Image(reinterpret_cast<ImTextureID>(faceSRV), ImVec2(imageSize, imageSize));
                        ImGui::SameLine(); 
                        ImGui::Text("%s", faceNames[i]);
                    }
                }

                ImGui::TreePop();
            }

            ImGui::PopStyleColor();
        }
    }

    if (PickedActor)
    {
        if (USpotLightComponent* SpotLightComponent = PickedActor->GetComponentByClass<USpotLightComponent>())
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

            if (ImGui::TreeNodeEx("SpotLight Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
            {
                DrawColorProperty("Light Color",
                    [&]() { return SpotLightComponent->GetLightColor(); },
                    [&](FLinearColor c) { SpotLightComponent->SetLightColor(c); });

                float Intensity = SpotLightComponent->GetIntensity();
                if (ImGui::SliderFloat("Intensity", &Intensity, 0.0f, 160.0f, "%.1f"))
                {
                    SpotLightComponent->SetIntensity(Intensity);
                }

                float Radius = SpotLightComponent->GetRadius();
                if (ImGui::SliderFloat("Radius", &Radius, 0.01f, 200.f, "%.1f"))
                {
                    SpotLightComponent->SetRadius(Radius);
                }

                LightDirection = SpotLightComponent->GetDirection();
                FImGuiWidget::DrawVec3Control("Direction", LightDirection, 0, 85);

                float InnerConeAngle = SpotLightComponent->GetInnerDegree();
                float OuterConeAngle = SpotLightComponent->GetOuterDegree();
                if (ImGui::DragFloat("InnerConeAngle", &InnerConeAngle, 0.5f, 0.0f, 80.0f))
                {
                    OuterConeAngle = std::max(InnerConeAngle, OuterConeAngle);

                    SpotLightComponent->SetInnerDegree(InnerConeAngle);
                    SpotLightComponent->SetOuterDegree(OuterConeAngle);
                }

                if (ImGui::DragFloat("OuterConeAngle", &OuterConeAngle, 0.5f, 0.0f, 80.0f))
                {
                    InnerConeAngle = std::min(OuterConeAngle, InnerConeAngle);

                    SpotLightComponent->SetOuterDegree(OuterConeAngle);
                    SpotLightComponent->SetInnerDegree(InnerConeAngle);
                }

                // --- Cast Shadows 체크박스 추가 ---
                bool bCastShadows = SpotLightComponent->GetCastShadows(); // 현재 상태 가져오기
                if (ImGui::Checkbox("Cast Shadows", &bCastShadows)) // 체크박스 UI 생성 및 상호작용 처리
                {
                    SpotLightComponent->SetCastShadows(bCastShadows); // 변경된 상태 설정
                    // 필요하다면, 상태 변경에 따른 즉각적인 렌더링 업데이트 요청 로직 추가
                    // 예: PointlightComponent->MarkRenderStateDirty();
                }

                ImGui::Text("ShadowMap");
                ImGui::Image(reinterpret_cast<ImTextureID>(FEngineLoop::Renderer.ShadowManager->GetSpotShadowDepthRHI()->ShadowSRVs[0]), ImVec2(200, 200));

                ImGui::TreePop();
            }

            ImGui::PopStyleColor();
        }
    }

    if (PickedActor)
    {
        if (UDirectionalLightComponent* DirectionalLightComponent = PickedActor->GetComponentByClass<UDirectionalLightComponent>())
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

            if (ImGui::TreeNodeEx("DirectionalLight Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
            {
                DrawColorProperty("Light Color",
                    [&]() { return DirectionalLightComponent->GetLightColor(); },
                    [&](FLinearColor c) { DirectionalLightComponent->SetLightColor(c); });

                float Intensity = DirectionalLightComponent->GetIntensity();
                if (ImGui::SliderFloat("Intensity", &Intensity, 0.0f, 150.0f, "%.1f"))
                {
                    DirectionalLightComponent->SetIntensity(Intensity);
                }

                LightDirection = DirectionalLightComponent->GetDirection();
                FImGuiWidget::DrawVec3Control("Direction", LightDirection, 0, 85);


                // --- Cast Shadows 체크박스 추가 ---
                bool bCastShadows = DirectionalLightComponent->GetCastShadows(); // 현재 상태 가져오기
                if (ImGui::Checkbox("Cast Shadows", &bCastShadows)) // 체크박스 UI 생성 및 상호작용 처리
                {
                    DirectionalLightComponent->SetCastShadows(bCastShadows); // 변경된 상태 설정
                    // 필요하다면, 상태 변경에 따른 즉각적인 렌더링 업데이트 요청 로직 추가
                    // 예: PointlightComponent->MarkRenderStateDirty();
                }
                ImGui::Text("ShadowMap");

                // 분할된 개수만큼 CSM 해당 SRV 출력
                const uint32& NumCascades = FEngineLoop::Renderer.ShadowManager->GetNumCasCades();
                for (uint32 i = 0; i < NumCascades; ++i)
                {
                    ImGui::Image(reinterpret_cast<ImTextureID>(FEngineLoop::Renderer.ShadowManager->GetDirectionalShadowCascadeDepthRHI()->ShadowSRVs[i]), ImVec2(200, 200));
                    //ImGui::SameLine();
                }
                ImGui::TreePop();
            }

            ImGui::PopStyleColor();
        }
    }

    if (PickedActor)
    {
        if (UAmbientLightComponent* AmbientLightComponent = PickedActor->GetComponentByClass<UAmbientLightComponent>())
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

            if (ImGui::TreeNodeEx("AmbientLight Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
            {
                DrawColorProperty("Light Color",
                    [&]() { return AmbientLightComponent->GetLightColor(); },
                    [&](FLinearColor c) { AmbientLightComponent->SetLightColor(c); });
                ImGui::TreePop();
            }

            ImGui::PopStyleColor();
        }
    }
    if (PickedActor)
    {
        if (ULightComponentBase* LightComponent = PickedActor->GetComponentByClass<ULightComponentBase>())
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

            // --- "Override Camera" 버튼 추가 ---
            if (ImGui::Button("Override Camera with Light's Perspective"))
            {
                // 1. 라이트의 월드 위치 및 회전 가져오기
                FVector LightLocation = LightComponent->GetWorldLocation();

                FVector Forward = FVector(1.f, 0.f, 0.0f);
                Forward = JungleMath::FVectorRotate(Forward, LightLocation);
                FVector LightForward = Forward;
                FRotator LightRotation = LightComponent->GetWorldRotation();
                FVector LightRotationVecter;
                LightRotationVecter.X = LightRotation.Roll;
                LightRotationVecter.Y = -LightRotation.Pitch;
                LightRotationVecter.Z = LightRotation.Yaw;

                // 2. 활성 에디터 뷰포트 클라이언트 가져오기 (!!! 엔진별 구현 필요 !!!)
                std::shared_ptr<FEditorViewportClient> ViewportClient = GEngineLoop.GetLevelEditor()->GetActiveViewportClient(); // 위에 정의된 헬퍼 함수 사용 (또는 직접 구현)

                // 3. 뷰포트 클라이언트가 유효하면 카메라 설정
                if (ViewportClient)
                {
                    ViewportClient->PerspectiveCamera.SetLocation(LightLocation + LightForward); // 카메라 위치 설정 함수 호출
                    ViewportClient->PerspectiveCamera.SetRotation(LightRotationVecter); // 카메라 회전 설정 함수 호출

                    // 필요시 뷰포트 강제 업데이트/다시 그리기 호출
                    // ViewportClient->Invalidate();
                }
                else
                {
                    // 뷰포트 클라이언트를 찾을 수 없음 (오류 로그 등)
                    // UE_LOG(LogTemp, Warning, TEXT("Active Viewport Client not found."));
                }
            }
            ImGui::PopStyleColor();
        }
    }

    if (PickedActor)
    {
        if (UProjectileMovementComponent* ProjectileComp = (PickedActor->GetComponentByClass<UProjectileMovementComponent>()))
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

            if (ImGui::TreeNodeEx("Projectile Movement Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
            {
                float InitialSpeed = ProjectileComp->GetInitialSpeed();
                if (ImGui::InputFloat("InitialSpeed", &InitialSpeed, 0.f, 10000.0f, "%.1f"))
                {
                    ProjectileComp->SetInitialSpeed(InitialSpeed);
                }

                float MaxSpeed = ProjectileComp->GetMaxSpeed();
                if (ImGui::InputFloat("MaxSpeed", &MaxSpeed, 0.f, 10000.0f, "%.1f"))
                {
                    ProjectileComp->SetMaxSpeed(MaxSpeed);
                }

                float Gravity = ProjectileComp->GetGravity();
                if (ImGui::InputFloat("Gravity", &Gravity, 0.f, 10000.f, "%.1f"))
                {
                    ProjectileComp->SetGravity(Gravity);
                }

                float ProjectileLifetime = ProjectileComp->GetLifetime();
                if (ImGui::InputFloat("Lifetime", &ProjectileLifetime, 0.f, 10000.f, "%.1f"))
                {
                    ProjectileComp->SetLifetime(ProjectileLifetime);
                }

                FVector CurrentVelocity = ProjectileComp->GetVelocity();

                float Velocity[3] = { CurrentVelocity.X, CurrentVelocity.Y, CurrentVelocity.Z };

                if (ImGui::InputFloat3("Velocity", Velocity, "%.1f"))
                {
                    ProjectileComp->SetVelocity(FVector(Velocity[0], Velocity[1], Velocity[2]));
                }

                ImGui::TreePop();
            }

            ImGui::PopStyleColor();
        }
    }
    // TODO: 추후에 RTTI를 이용해서 프로퍼티 출력하기
    if (PickedActor)
    {
        if (UTextComponent* TextComponent = Cast<UTextComponent>(PickedActor->GetRootComponent()))
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
            if (ImGui::TreeNodeEx("Text Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
            {
                if (TextComponent)
                {
                    TextComponent->SetTexture(L"Assets/Texture/font.png");
                    TextComponent->SetRowColumnCount(106, 106);
                    FWString wText = TextComponent->GetText();
                    int Len = WideCharToMultiByte(CP_UTF8, 0, wText.c_str(), -1, nullptr, 0, nullptr, nullptr);
                    std::string u8Text(Len, '\0');
                    WideCharToMultiByte(CP_UTF8, 0, wText.c_str(), -1, u8Text.data(), Len, nullptr, nullptr);

                    static char Buf[256];
                    strcpy_s(Buf, u8Text.c_str());

                    ImGui::Text("Text: ", Buf);
                    ImGui::SameLine();
                    ImGui::PushItemFlag(ImGuiItemFlags_NoNavDefaultFocus, true);
                    if (ImGui::InputText("##Text", Buf, 256, ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        TextComponent->ClearText();
                        int wLen = MultiByteToWideChar(CP_UTF8, 0, Buf, -1, nullptr, 0);
                        FWString wNewText(wLen, L'\0');
                        MultiByteToWideChar(CP_UTF8, 0, Buf, -1, wNewText.data(), wLen);
                        TextComponent->SetText(wNewText.c_str());
                    }
                    ImGui::PopItemFlag();
                }
                ImGui::TreePop();
            }
            ImGui::PopStyleColor();
        }
    }

    // TODO: 추후에 RTTI를 이용해서 프로퍼티 출력하기
    if (PickedActor)
    {
        if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(PickedActor->GetRootComponent()))
        {
            RenderForStaticMesh(StaticMeshComponent);
            RenderForMaterial(StaticMeshComponent);
        }
    }

    if (PickedActor)
    {
        if (UHeightFogComponent* FogComponent = Cast<UHeightFogComponent>(PickedActor->GetRootComponent()))
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
            if (ImGui::TreeNodeEx("Exponential Height Fog", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
            {
                FLinearColor CurrColor = FogComponent->GetFogColor();

                float R = CurrColor.R;
                float G = CurrColor.G;
                float B = CurrColor.B;
                float A = CurrColor.A;
                float H, S, V;
                float LightColor[4] = { R, G, B, A };

                // Fog Color
                if (ImGui::ColorPicker4("##Fog Color", LightColor,
                    ImGuiColorEditFlags_DisplayRGB |
                    ImGuiColorEditFlags_NoSidePreview |
                    ImGuiColorEditFlags_NoInputs |
                    ImGuiColorEditFlags_Float))
                {

                    R = LightColor[0];
                    G = LightColor[1];
                    B = LightColor[2];
                    A = LightColor[3];
                    FogComponent->SetFogColor(FLinearColor(R, G, B, A));
                }
                RGBToHSV(R, G, B, H, S, V);
                // RGB/HSV
                bool ChangedRGB = false;
                bool ChangedHSV = false;

                // RGB
                ImGui::PushItemWidth(50.0f);
                if (ImGui::DragFloat("R##R", &R, 0.001f, 0.f, 1.f)) ChangedRGB = true;
                ImGui::SameLine();
                if (ImGui::DragFloat("G##G", &G, 0.001f, 0.f, 1.f)) ChangedRGB = true;
                ImGui::SameLine();
                if (ImGui::DragFloat("B##B", &B, 0.001f, 0.f, 1.f)) ChangedRGB = true;
                ImGui::Spacing();

                // HSV
                if (ImGui::DragFloat("H##H", &H, 0.1f, 0.f, 360)) ChangedHSV = true;
                ImGui::SameLine();
                if (ImGui::DragFloat("S##S", &S, 0.001f, 0.f, 1)) ChangedHSV = true;
                ImGui::SameLine();
                if (ImGui::DragFloat("V##V", &V, 0.001f, 0.f, 1)) ChangedHSV = true;
                ImGui::PopItemWidth();
                ImGui::Spacing();

                if (ChangedRGB && !ChangedHSV)
                {
                    // RGB -> HSV
                    RGBToHSV(R, G, B, H, S, V);
                    FogComponent->SetFogColor(FLinearColor(R, G, B, A));
                }
                else if (ChangedHSV && !ChangedRGB)
                {
                    // HSV -> RGB
                    HSVToRGB(H, S, V, R, G, B);
                    FogComponent->SetFogColor(FLinearColor(R, G, B, A));
                }

                float FogDensity = FogComponent->GetFogDensity();
                if (ImGui::SliderFloat("Density", &FogDensity, 0.00f, 3.0f))
                {
                    FogComponent->SetFogDensity(FogDensity);
                }

                float FogDistanceWeight = FogComponent->GetFogDistanceWeight();
                if (ImGui::SliderFloat("Distance Weight", &FogDistanceWeight, 0.00f, 3.0f))
                {
                    FogComponent->SetFogDistanceWeight(FogDistanceWeight);
                }

                float FogHeightFallOff = FogComponent->GetFogHeightFalloff();
                if (ImGui::SliderFloat("Height Fall Off", &FogHeightFallOff, 0.001f, 0.15f))
                {
                    FogComponent->SetFogHeightFalloff(FogHeightFallOff);
                }

                float FogStartDistance = FogComponent->GetStartDistance();
                if (ImGui::SliderFloat("Start Distance", &FogStartDistance, 0.00f, 50.0f))
                {
                    FogComponent->SetStartDistance(FogStartDistance);
                }

                float FogEndtDistance = FogComponent->GetEndDistance();
                if (ImGui::SliderFloat("End Distance", &FogEndtDistance, 0.00f, 50.0f))
                {
                    FogComponent->SetEndDistance(FogEndtDistance);
                }

                ImGui::TreePop();
            }
            ImGui::PopStyleColor();
        }
    }
    ImGui::End();
}

void PropertyEditorPanel::RGBToHSV(const float R, const float G, const float B, float& H, float& S, float& V)
{
    const float MX = FMath::Max(R, FMath::Max(G, B));
    const float MN = FMath::Min(R, FMath::Min(G, B));
    const float Delta = MX - MN;

    V = MX;

    if (MX == 0.0f) {
        S = 0.0f;
        H = 0.0f;
        return;
    }
    else {
        S = Delta / MX;
    }

    if (Delta < 1e-6) {
        H = 0.0f;
    }
    else {
        if (R >= MX) {
            H = (G - B) / Delta;
        }
        else if (G >= MX) {
            H = 2.0f + (B - R) / Delta;
        }
        else {
            H = 4.0f + (R - G) / Delta;
        }
        H *= 60.0f;
        if (H < 0.0f) {
            H += 360.0f;
        }
    }
}

void PropertyEditorPanel::HSVToRGB(const float H, const float S, const float V, float& R, float& G, float& B)
{
    // h: 0~360, s:0~1, v:0~1
    const float C = V * S;
    const float Hp = H / 60.0f;             // 0~6 구간
    const float X = C * (1.0f - fabsf(fmodf(Hp, 2.0f) - 1.0f));
    const float M = V - C;

    if (Hp < 1.0f) { R = C;  G = X;  B = 0.0f; }
    else if (Hp < 2.0f) { R = X;  G = C;  B = 0.0f; }
    else if (Hp < 3.0f) { R = 0.0f; G = C;  B = X; }
    else if (Hp < 4.0f) { R = 0.0f; G = X;  B = C; }
    else if (Hp < 5.0f) { R = X;  G = 0.0f; B = C; }
    else { R = C;  G = 0.0f; B = X; }

    R += M;  G += M;  B += M;
}

void PropertyEditorPanel::RenderForStaticMesh(UStaticMeshComponent* StaticMeshComp)
{
    if (StaticMeshComp->GetStaticMesh() == nullptr)
    {
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    if (ImGui::TreeNodeEx("Static Mesh", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
    {
        ImGui::Text("StaticMesh");
        ImGui::SameLine();

        FString PreviewName = StaticMeshComp->GetStaticMesh()->GetRenderData()->DisplayName;
        const TMap<FName, FAssetInfo> Assets = UAssetManager::Get().GetAssetRegistry();

        if (ImGui::BeginCombo("##StaticMesh", GetData(PreviewName), ImGuiComboFlags_None))
        {
            for (const auto& Asset : Assets)
            {
                if (ImGui::Selectable(GetData(Asset.Value.AssetName.ToString()), false))
                {
                    FString MeshName = Asset.Value.PackagePath.ToString() + "/" + Asset.Value.AssetName.ToString();
                    UStaticMesh* StaticMesh = FManagerOBJ::GetStaticMesh(MeshName.ToWideString());
                    if (StaticMesh)
                    {
                        StaticMeshComp->SetStaticMesh(StaticMesh);
                    }
                }
            }
            ImGui::EndCombo();
        }

        ImGui::TreePop();
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    if (ImGui::TreeNodeEx("Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
    {
        ImGui::Text("Add");
        ImGui::SameLine();

        TArray<UClass*> CompClasses;
        GetChildOfClass(UActorComponent::StaticClass(), CompClasses);

        if (ImGui::BeginCombo("##AddComponent", "Components", ImGuiComboFlags_None))
        {
            for (UClass* Class : CompClasses)
            {
                if (ImGui::Selectable(GetData(Class->GetName()), false))
                {
                    USceneComponent* NewComp = Cast<USceneComponent>(StaticMeshComp->GetOwner()->AddComponent(Class));
                    if (NewComp)
                    {
                        NewComp->SetupAttachment(StaticMeshComp);
                    }
                    // 추후 Engine으로부터 SelectedComponent 받아서 선택된 Comp 아래로 붙일 수있으면 붙이기.
                }
            }
            ImGui::EndCombo();
        }

        ImGui::TreePop();
    }
    ImGui::PopStyleColor();
}


void PropertyEditorPanel::RenderForMaterial(UStaticMeshComponent* StaticMeshComp)
{
    if (StaticMeshComp->GetStaticMesh() == nullptr)
    {
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    if (ImGui::TreeNodeEx("Materials", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
    {
        for (uint32 i = 0; i < StaticMeshComp->GetNumMaterials(); ++i)
        {
            if (ImGui::Selectable(GetData(StaticMeshComp->GetMaterialSlotNames()[i].ToString()), false, ImGuiSelectableFlags_AllowDoubleClick))
            {
                if (ImGui::IsMouseDoubleClicked(0))
                {
                    std::cout << GetData(StaticMeshComp->GetMaterialSlotNames()[i].ToString()) << std::endl;
                    SelectedMaterialIndex = i;
                    SelectedStaticMeshComp = StaticMeshComp;
                }
            }
        }

        if (ImGui::Button("    +    ")) {
            IsCreateMaterial = true;
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("SubMeshes", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
    {
        const auto Subsets = StaticMeshComp->GetStaticMesh()->GetRenderData()->MaterialSubsets;
        for (uint32 i = 0; i < Subsets.Num(); ++i)
        {
            std::string temp = "subset " + std::to_string(i);
            if (ImGui::Selectable(temp.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
            {
                if (ImGui::IsMouseDoubleClicked(0))
                {
                    StaticMeshComp->SetselectedSubMeshIndex(i);
                    SelectedStaticMeshComp = StaticMeshComp;
                }
            }
        }
        const std::string Temp = "clear subset";
        if (ImGui::Selectable(Temp.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
        {
            if (ImGui::IsMouseDoubleClicked(0))
                StaticMeshComp->SetselectedSubMeshIndex(-1);
        }

        ImGui::TreePop();
    }

    ImGui::PopStyleColor();

    if (SelectedMaterialIndex != -1)
    {
        RenderMaterialView(SelectedStaticMeshComp->GetMaterial(SelectedMaterialIndex));
    }
    if (IsCreateMaterial) {
        RenderCreateMaterialView();
    }
}

void PropertyEditorPanel::RenderMaterialView(UMaterial* Material)
{
    ImGui::SetNextWindowSize(ImVec2(380, 400), ImGuiCond_Once);
    ImGui::Begin("Material Viewer", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav);

    static ImGuiSelectableFlags BaseFlag = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_None | ImGuiColorEditFlags_NoAlpha;

    const FVector MatDiffuseColor = Material->GetMaterialInfo().Diffuse;
    const FVector MatSpecularColor = Material->GetMaterialInfo().Specular;
    const FVector MatAmbientColor = Material->GetMaterialInfo().Ambient;
    const FVector MatEmissiveColor = Material->GetMaterialInfo().Emissive;

    const float DiffuseR = MatDiffuseColor.X;
    const float DiffuseG = MatDiffuseColor.Y;
    const float DiffuseB = MatDiffuseColor.Z;
    constexpr float DiffuseA = 1.0f;
    float DiffuseColorPick[4] = { DiffuseR, DiffuseG, DiffuseB, DiffuseA };

    ImGui::Text("Material Name |");
    ImGui::SameLine();
    ImGui::Text(*Material->GetMaterialInfo().MaterialName);
    ImGui::Separator();

    ImGui::Text("  Diffuse Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Diffuse##Color", reinterpret_cast<float*>(&DiffuseColorPick), BaseFlag))
    {
        const FVector NewColor = { DiffuseColorPick[0], DiffuseColorPick[1], DiffuseColorPick[2] };
        Material->SetDiffuse(NewColor);
    }

    const float SpecularR = MatSpecularColor.X;
    const float SpecularG = MatSpecularColor.Y;
    const float SpecularB = MatSpecularColor.Z;
    constexpr float SpecularA = 1.0f;
    float SpecularColorPick[4] = { SpecularR, SpecularG, SpecularB, SpecularA };

    ImGui::Text("Specular Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Specular##Color", reinterpret_cast<float*>(&SpecularColorPick), BaseFlag))
    {
        const FVector NewColor = { SpecularColorPick[0], SpecularColorPick[1], SpecularColorPick[2] };
        Material->SetSpecular(NewColor);
    }

    const float AmbientR = MatAmbientColor.X;
    const float AmbientG = MatAmbientColor.Y;
    const float AmbientB = MatAmbientColor.Z;
    constexpr float AmbientA = 1.0f;
    float AmbientColorPick[4] = { AmbientR, AmbientG, AmbientB, AmbientA };

    ImGui::Text("Ambient Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Ambient##Color", reinterpret_cast<float*>(&AmbientColorPick), BaseFlag))
    {
        const FVector NewColor = { AmbientColorPick[0], AmbientColorPick[1], AmbientColorPick[2] };
        Material->SetAmbient(NewColor);
    }

    const float EmissiveR = MatEmissiveColor.X;
    const float EmissiveG = MatEmissiveColor.Y;
    const float EmissiveB = MatEmissiveColor.Z;
    constexpr float EmissiveA = 1.0f;
    float EmissiveColorPick[4] = { EmissiveR, EmissiveG, EmissiveB, EmissiveA };

    ImGui::Text("Emissive Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Emissive##Color", reinterpret_cast<float*>(&EmissiveColorPick), BaseFlag))
    {
        const FVector NewColor = { EmissiveColorPick[0], EmissiveColorPick[1], EmissiveColorPick[2] };
        Material->SetEmissive(NewColor);
    }

    ImGui::Spacing();
    ImGui::Separator();

    ImGui::Text("Choose Material");
    ImGui::Spacing();

    ImGui::Text("Material Slot Name |");
    ImGui::SameLine();
    ImGui::Text(GetData(SelectedStaticMeshComp->GetMaterialSlotNames()[SelectedMaterialIndex].ToString()));

    ImGui::Text("Override Material |");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(160);
    // 메테리얼 이름 목록을 const char* 배열로 변환
    std::vector<const char*> MaterialChars;
    for (const auto& Material : FManagerOBJ::GetMaterials()) {
        MaterialChars.push_back(*Material.Value->GetMaterialInfo().MaterialName);
    }

    //// 드롭다운 표시 (currentMaterialIndex가 범위를 벗어나지 않도록 확인)
    //if (currentMaterialIndex >= FManagerOBJ::GetMaterialNum())
    //    currentMaterialIndex = 0;

    if (ImGui::Combo("##MaterialDropdown", &CurMaterialIndex, MaterialChars.data(), FManagerOBJ::GetMaterialNum())) {
        UMaterial* Material = FManagerOBJ::GetMaterial(MaterialChars[CurMaterialIndex]);
        SelectedStaticMeshComp->SetMaterial(SelectedMaterialIndex, Material);
    }

    if (ImGui::Button("Close"))
    {
        SelectedMaterialIndex = -1;
        SelectedStaticMeshComp = nullptr;
    }

    ImGui::End();
}

void PropertyEditorPanel::RenderCreateMaterialView()
{
    ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_Once);
    ImGui::Begin("Create Material Viewer", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav);

    static ImGuiSelectableFlags BaseFlag = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_None | ImGuiColorEditFlags_NoAlpha;

    ImGui::Text("New Name");
    ImGui::SameLine();
    static char MaterialName[256] = "New Material";
    // 기본 텍스트 입력 필드
    ImGui::SetNextItemWidth(128);
    if (ImGui::InputText("##NewName", MaterialName, IM_ARRAYSIZE(MaterialName))) {
        tempMaterialInfo.MaterialName = MaterialName;
    }

    const FVector MatDiffuseColor = tempMaterialInfo.Diffuse;
    const FVector MatSpecularColor = tempMaterialInfo.Specular;
    const FVector MatAmbientColor = tempMaterialInfo.Ambient;
    const FVector MatEmissiveColor = tempMaterialInfo.Emissive;

    const float DiffuseR = MatDiffuseColor.X;
    const float DiffuseG = MatDiffuseColor.Y;
    const float DiffuseB = MatDiffuseColor.Z;
    constexpr float DiffuseA = 1.0f;
    float DiffuseColorPick[4] = { DiffuseR, DiffuseG, DiffuseB, DiffuseA };

    ImGui::Text("Set Property");
    ImGui::Indent();

    ImGui::Text("  Diffuse Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Diffuse##Color", reinterpret_cast<float*>(&DiffuseColorPick), BaseFlag))
    {
        const FVector NewColor = { DiffuseColorPick[0], DiffuseColorPick[1], DiffuseColorPick[2] };
        tempMaterialInfo.Diffuse = NewColor;
    }

    const float SpecularR = MatSpecularColor.X;
    const float SpecularG = MatSpecularColor.Y;
    const float SpecularB = MatSpecularColor.Z;
    constexpr float SpecularA = 1.0f;
    float SpecularColorPick[4] = { SpecularR, SpecularG, SpecularB, SpecularA };

    ImGui::Text("Specular Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Specular##Color", reinterpret_cast<float*>(&SpecularColorPick), BaseFlag))
    {
        const FVector NewColor = { SpecularColorPick[0], SpecularColorPick[1], SpecularColorPick[2] };
        tempMaterialInfo.Specular = NewColor;
    }

    const float AmbientR = MatAmbientColor.X;
    const float AmbientG = MatAmbientColor.Y;
    const float AmbientB = MatAmbientColor.Z;
    constexpr float AmbientA = 1.0f;
    float AmbientColorPick[4] = { AmbientR, AmbientG, AmbientB, AmbientA };

    ImGui::Text("Ambient Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Ambient##Color", reinterpret_cast<float*>(&AmbientColorPick), BaseFlag))
    {
        const FVector NewColor = { AmbientColorPick[0], AmbientColorPick[1], AmbientColorPick[2] };
        tempMaterialInfo.Ambient = NewColor;
    }

    const float EmissiveR = MatEmissiveColor.X;
    const float EmissiveG = MatEmissiveColor.Y;
    const float EmissiveB = MatEmissiveColor.Z;
    constexpr float EmissiveA = 1.0f;
    float EmissiveColorPick[4] = { EmissiveR, EmissiveG, EmissiveB, EmissiveA };

    ImGui::Text("Emissive Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Emissive##Color", reinterpret_cast<float*>(&EmissiveColorPick), BaseFlag))
    {
        const FVector NewColor = { EmissiveColorPick[0], EmissiveColorPick[1], EmissiveColorPick[2] };
        tempMaterialInfo.Emissive = NewColor;
    }
    ImGui::Unindent();

    ImGui::NewLine();
    if (ImGui::Button("Create Material")) {
        FManagerOBJ::CreateMaterial(tempMaterialInfo);
    }

    ImGui::NewLine();
    if (ImGui::Button("Close"))
    {
        IsCreateMaterial = false;
    }

    ImGui::End();
}

void PropertyEditorPanel::OnResize(HWND hWnd)
{
    RECT ClientRect;
    GetClientRect(hWnd, &ClientRect);
    Width = ClientRect.right - ClientRect.left;
    Height = ClientRect.bottom - ClientRect.top;
}

void PropertyEditorPanel::OpenLuaScriptFile(LPCTSTR InLuaFilePath)
{
    HINSTANCE hInst = ShellExecute(
        NULL,
        _T("open"),
        InLuaFilePath,
        NULL,
        NULL,
        SW_SHOWNORMAL
    );

    if ((INT_PTR)hInst <= 32) {
        MessageBox(NULL, _T("파일 열기에 실패했습니다."), _T("Error"), MB_OK | MB_ICONERROR);
    }
}
