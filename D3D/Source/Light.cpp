#include "Light.h"
#include <imgui/imgui.h>

PointLight::PointLight(float Radius)
    : m_Buffer(), m_Mesh(Radius)
{
    Reset();
}

void PointLight::SpawnControlWindow() noexcept
{
    if (ImGui::Begin("Light"))
    {
        if (ImGui::BeginTable("LightControls", 2, ImGuiTableFlags_None, ImVec2{}, 0.0f))
        {
            const ImVec2 v = ImGui::GetContentRegionAvail();

            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 120.0f, 0);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, v.x, 1);

            ImGui::TableNextRow();
            {
                ImGui::TableSetColumnIndex(0); ImGui::Text("Position");
                ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderFloat3("##Position", &m_Buffer.Position.X, -100.0f, 100.0f, "%.2f");
            }

            ImGui::TableNextRow();
            {
                ImGui::TableSetColumnIndex(0); ImGui::Text("Ambient Color"); ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::ColorEdit3("##AmbientColor", &m_Buffer.AmbientColor.X, ImGuiColorEditFlags_None);
            }
            
            ImGui::TableNextRow();
            {
                ImGui::TableSetColumnIndex(0); ImGui::Text("Material Color"); ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::ColorEdit3("##MaterialColor", &m_Buffer.MaterialColor.X, ImGuiColorEditFlags_None);
            }

            ImGui::TableNextRow();
            {
                ImGui::TableSetColumnIndex(0); ImGui::Text("Diff. Intensity"); ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderFloat("##DiffuseIntensity", &m_Buffer.DiffuseIntensity, 0.0f, 10.0f, "%.2f");
            }
            
            ImGui::TableNextRow();
            {
                ImGui::TableSetColumnIndex(0); ImGui::Text("Diff. Color"); ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::ColorEdit3("##DiffuseColor", &m_Buffer.DiffuseColor.X, ImGuiColorEditFlags_None);
            }
            
            ImGui::TableNextRow();
            {
                ImGui::TableSetColumnIndex(0); ImGui::Text("Atten. Constant"); ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderFloat("##AttenuationConstant", &m_Buffer.AttenuationConstant, 0.0f, 1.0f, "%.3f");
            }
            
            ImGui::TableNextRow();
            {
                ImGui::TableSetColumnIndex(0); ImGui::Text("Atten. Linear"); ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderFloat("##AttenuationLinear", &m_Buffer.AttenuationLinear, 0.01f, 0.1f, "%.5f");
            }
            
            ImGui::TableNextRow();
            {
                ImGui::TableSetColumnIndex(0); ImGui::Text("Atten. Quadratic"); ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderFloat("##AttenuationQuadratic", &m_Buffer.AttenuationQuadratic, 0.001f, 0.01f, "%.5f");
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

void PointLight::Reset() noexcept
{
    m_Buffer =
    {
        Float3(),
        Float3(0.93f, 0.87f, 0.31f),
        Float3(0.07f, 0.07f, 0.07f),
        Float3(1.00f, 1.00f, 1.00f),
        2.0f,
        1.0f,
        0.045f,
        0.0075f,
    };
}

void PointLight::Draw() const noexcept
{
    m_Mesh.SetPosition(m_Buffer.Position);
    m_Mesh.Draw();
}

void PointLight::Bind() const noexcept
{
    m_CB.Update(m_Buffer);
    m_CB.Bind();
}
