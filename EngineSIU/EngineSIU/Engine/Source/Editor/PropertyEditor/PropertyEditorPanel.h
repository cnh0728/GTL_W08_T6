#pragma once

#include "Components/ActorComponent.h"
#include "UnrealEd/EditorPanel.h"
#include "Math/Rotator.h"

class UStaticMeshComponent;

// 헬퍼 함수 예시
template<typename Getter, typename Setter>
void DrawColorProperty(const char* Label, Getter Get, Setter Set)
{
    ImGui::PushItemWidth(200.0f);
    const FLinearColor CurrentColor = Get();
    float Col[4] = { CurrentColor.R, CurrentColor.G, CurrentColor.B, CurrentColor.A };

    if (ImGui::ColorEdit4(Label, Col,
        ImGuiColorEditFlags_DisplayRGB |
        ImGuiColorEditFlags_NoSidePreview |
        ImGuiColorEditFlags_NoInputs |
        ImGuiColorEditFlags_Float))
    {
        Set(FLinearColor(Col[0], Col[1], Col[2], Col[3]));
    }
    ImGui::PopItemWidth();
}


class PropertyEditorPanel : public UEditorPanel
{
public:
    virtual void Render() override;
    virtual void OnResize(HWND hWnd) override;
    void OpenLuaScriptFile(LPCTSTR InLuaFilePath);

private:
    static void RGBToHSV(float R, float G, float B, float& H, float& S, float& V);
    static void HSVToRGB(float H, float S, float V, float& R, float& G, float& B);

    /* Static Mesh Settings */
    static void RenderForStaticMesh(UStaticMeshComponent* StaticMeshComp);
    
    /* Materials Settings */
    void RenderForMaterial(UStaticMeshComponent* StaticMeshComp);
    void RenderMaterialView(UMaterial* Material);
    void RenderCreateMaterialView();
private:
    float Width = 0, Height = 0;
    FVector Location = FVector(0, 0, 0);
    FRotator Rotation = FRotator(0, 0, 0);
    FVector Scale = FVector(0, 0, 0);
    FVector LightDirection = FVector(0, 0, 0);
    /* Material Property */
    int SelectedMaterialIndex = -1;
    int CurMaterialIndex = -1;
    UStaticMeshComponent* SelectedStaticMeshComp = nullptr;
    FObjMaterialInfo tempMaterialInfo;
    bool IsCreateMaterial;
};
