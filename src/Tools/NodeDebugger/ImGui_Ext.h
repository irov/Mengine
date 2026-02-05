#pragma once
#include "imgui_internal.h"

namespace ImGuiExt
{
    //////////////////////////////////////////////////////////////////////////
    struct ImIcon
    {
        ImTextureID image;
        ImVec2      uv0;
        ImVec2      uv1;
    };
    //////////////////////////////////////////////////////////////////////////
    static std::pair<bool, bool> TreeNodeWithIcon( const ImIcon * _icon, const char * _label, const ImGuiTreeNodeFlags _flags = ImGuiTreeNodeFlags_None, const bool _disabled = false )
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
            const ImVec4 tintImageColor = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );
            const ImVec4 borderImageColor = _disabled ? ImVec4( 0.5f, 0.5f, 0.5f, 1.0f ) : ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );
            ImGui::Image( _icon->image, ImVec2( 16.f, 16.f ), _icon->uv0, _icon->uv1, tintImageColor, borderImageColor );
        }

        return {isOpened, isClicked};
    }
}
