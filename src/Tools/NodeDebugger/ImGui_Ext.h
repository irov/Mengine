#pragma once
#include "imgui_internal.h"

namespace ImGuiExt
{
    //////////////////////////////////////////////////////////////////////////
    static void SetBrightStyle( ImGuiStyle * _dst = nullptr )
    {
        ImGuiStyle & style = _dst ? *_dst : ImGui::GetStyle();

        style.Colors[ImGuiCol_Text] = ImVec4( 0.00f, 0.00f, 0.00f, 1.00f );
        style.Colors[ImGuiCol_TextDisabled] = ImVec4( 0.60f, 0.60f, 0.60f, 1.00f );
        style.Colors[ImGuiCol_WindowBg] = ImVec4( 0.94f, 0.94f, 0.94f, 1.00f );
        style.Colors[ImGuiCol_ChildWindowBg] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
        style.Colors[ImGuiCol_Border] = ImVec4( 0.00f, 0.00f, 0.00f, 0.39f );
        style.Colors[ImGuiCol_BorderShadow] = ImVec4( 1.00f, 1.00f, 1.00f, 0.10f );
        style.Colors[ImGuiCol_FrameBg] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4( 0.26f, 0.59f, 0.98f, 0.40f );
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4( 0.26f, 0.59f, 0.98f, 0.67f );
        style.Colors[ImGuiCol_TitleBg] = ImVec4( 0.96f, 0.96f, 0.96f, 1.00f );
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4( 1.00f, 1.00f, 1.00f, 0.51f );
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4( 0.82f, 0.82f, 0.82f, 1.00f );
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4( 0.86f, 0.86f, 0.86f, 1.00f );
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4( 0.98f, 0.98f, 0.98f, 0.53f );
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4( 0.69f, 0.69f, 0.69f, 0.80f );
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4( 0.49f, 0.49f, 0.49f, 0.80f );
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4( 0.49f, 0.49f, 0.49f, 1.00f );
        style.Colors[ImGuiCol_CheckMark] = ImVec4( 0.26f, 0.59f, 0.98f, 1.00f );
        style.Colors[ImGuiCol_SliderGrab] = ImVec4( 0.26f, 0.59f, 0.98f, 0.78f );
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4( 0.26f, 0.59f, 0.98f, 1.00f );
        style.Colors[ImGuiCol_Button] = ImVec4( 0.26f, 0.59f, 0.98f, 0.40f );
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4( 0.26f, 0.59f, 0.98f, 1.00f );
        style.Colors[ImGuiCol_ButtonActive] = ImVec4( 0.06f, 0.53f, 0.98f, 1.00f );
        style.Colors[ImGuiCol_Header] = ImVec4( 0.26f, 0.59f, 0.98f, 0.31f );
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4( 0.26f, 0.59f, 0.98f, 0.80f );
        style.Colors[ImGuiCol_HeaderActive] = ImVec4( 0.26f, 0.59f, 0.98f, 1.00f );
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4( 1.00f, 1.00f, 1.00f, 0.00f );
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4( 0.26f, 0.59f, 0.98f, 0.67f );
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4( 0.26f, 0.59f, 0.98f, 0.95f );
        style.Colors[ImGuiCol_PlotLines] = ImVec4( 0.39f, 0.39f, 0.39f, 1.00f );
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4( 1.00f, 0.43f, 0.35f, 1.00f );
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4( 1.00f, 0.60f, 0.00f, 1.00f );
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4( 0.26f, 0.59f, 0.98f, 0.35f );
        style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4( 0.20f, 0.20f, 0.20f, 0.35f );
    }
    //////////////////////////////////////////////////////////////////////////
    struct ImIcon
    {
        ImTextureID image;
        ImVec2      uv0;
        ImVec2      uv1;
    };
    //////////////////////////////////////////////////////////////////////////
    static std::pair<bool, bool> TreeNodeWithIcon( const ImIcon * _icon, const char * _label, const ImGuiTreeNodeFlags _flags = 0, const bool _disabled = false )
    {
        std::string paddedLabel = std::string( "   " ) + _label;

        ImVec2 textSize = ImGui::CalcTextSize( paddedLabel.c_str(), nullptr, true );

        if( _disabled )
        {
            ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.33f, 0.33f, 0.33f, 1.0f ) );
        }

        const bool isOpened = ImGui::TreeNodeEx( paddedLabel.c_str(), _flags );

        if( _disabled )
        {
            ImGui::PopStyleColor();
        }

        const bool isClicked = ImGui::IsItemClicked();

        if( _icon != nullptr )
        {
            ImGui::SameLine();

            ImGui::SetCursorPosX( ImGui::GetCursorPosX() - textSize.x - 8.f );
            const ImVec4 imageColor = _disabled ? ImVec4( 1.0f, 1.0f, 1.0f, 0.5f ) : ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );
            ImGui::Image( _icon->image, ImVec2( 16.f, 16.f ), _icon->uv0, _icon->uv1, imageColor );
        }

        return {isOpened, isClicked};
    }
}
