#include "Camera.h"

#include <imgui/imgui.h>

Matrix4x4 SceneCamera::GetMatrix() const noexcept
{
    DirectX::XMVECTOR Position = DirectX::XMVector3Transform(
        DirectX::XMVectorSet(0.0f, 0.0f, -m_Radius, 0.0f),
        DirectX::XMMatrixRotationRollPitchYaw(m_Phi, m_Theta, 0.0f)
    );
    return DirectX::XMMatrixLookAtLH(Position, DirectX::XMVectorZero(), DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)) *
           DirectX::XMMatrixRotationRollPitchYaw(m_Pitch, -m_Yaw, m_Roll);
}

void SceneCamera::SpawnControlWindow() noexcept
{
    if (ImGui::Begin("SceneCamera"))
    {
        if (ImGui::BeginTable("SceneCameraControls", 2, ImGuiTableFlags_None, ImVec2{}, 0.0f))
        {
            const ImVec2 v = ImGui::GetContentRegionAvail();

            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 100.0f, 0);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed,    v.x, 1);

            ImGui::TableNextRow();
            {
                ImGui::TableSetColumnIndex(1);
                ImGui::TextDisabled("Position");
            }

            ImGui::TableNextRow();
            {
                ImGui::TableSetColumnIndex(0); ImGui::Text("Distance");
                ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderFloat("##Distance", &m_Radius, 0.01f, 500.0f, "%.2f");
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0); ImGui::Text("Theta");
                ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderAngle("##Theta", &m_Theta, -180.0f, 180.0f);
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0); ImGui::Text("Phi");
                ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderAngle("##Phi", &m_Phi, -180.0f, 180.0f);
            }

            ImGui::TableNextRow();
            {
                ImGui::TableSetColumnIndex(1);
                ImGui::TextDisabled("Orientation");
            }

            ImGui::TableNextRow();
            {
                ImGui::TableSetColumnIndex(0); ImGui::Text("Roll");
                ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderAngle("##Roll", &m_Roll, -180.0f, 180.0f);
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0); ImGui::Text("Pitch");
                ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderAngle("##Pitch", &m_Pitch, -180.0f, 180.0f);
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0); ImGui::Text("Yaw");
                ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderAngle("##Yaw", &m_Yaw, -180.0f, 180.0f);
            }

            ImGui::EndTable();
        }
        if (ImGui::Button("Reset"))
        {
            Reset();
        }
    }
    ImGui::End();
}

void SceneCamera::Reset() noexcept
{
    new(this) SceneCamera{};
}
