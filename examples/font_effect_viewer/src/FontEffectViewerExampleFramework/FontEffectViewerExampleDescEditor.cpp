#include "FontEffectViewerExampleDescEditor.h"

#include "Plugins/FontEffectPlugin/FontEffectDesc.h"
#include "Plugins/FontEffectPlugin/FontEffectHelper.h"

#include "Config/StdIO.h"

#include "imgui.h"

namespace Mengine
{
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        enum EEditorAction
        {
            EEA_NONE = 0,
            EEA_UP,
            EEA_DOWN,
            EEA_REMOVE,
            EEA_DUPLICATE
        };
        //////////////////////////////////////////////////////////////////////////
        static bool s_colorEdit( const Char * _label, Color * const _color )
        {
            float v[4] = {_color->getR(), _color->getG(), _color->getB(), _color->getA()};

            if( ImGui::ColorEdit4( _label, v, ImGuiColorEditFlags_AlphaBar ) == false )
            {
                return false;
            }

            _color->setRGBA( v[0], v[1], v[2], v[3] );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_dragSize( const Char * _label, float * const _value, float _max )
        {
            bool changed = ImGui::DragFloat( _label, _value, 0.05f, 0.f, _max, "%.2f" );

            if( *_value < 0.f )
            {
                *_value = 0.f;
            }

            return changed;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_renderGradient( FontEffectGradientDesc * const _gradient )
        {
            bool changed = false;

            changed |= ImGui::Checkbox( "Gradient", &_gradient->enabled );

            if( _gradient->enabled == false )
            {
                return changed;
            }

            ImGui::Indent();

            changed |= ImGui::SliderFloat( "Angle", &_gradient->angle, 0.f, 360.f, "%.0f" );

            int space = (int)_gradient->space;

            const Char * spaceNames[__EFEGS_MAX__];

            for( uint32_t spaceIndex = 0; spaceIndex != __EFEGS_MAX__; ++spaceIndex )
            {
                spaceNames[spaceIndex] = Helper::getFontEffectGradientSpaceName( (EFontEffectGradientSpace)spaceIndex );
            }

            if( ImGui::Combo( "Space", &space, spaceNames, __EFEGS_MAX__ ) == true )
            {
                _gradient->space = (EFontEffectGradientSpace)space;
                changed = true;
            }

            int removeIndex = -1;

            VectorFontEffectGradientStops & stops = _gradient->stops;

            for( VectorFontEffectGradientStops::size_type index = 0; index != stops.size(); ++index )
            {
                FontEffectGradientStop & stop = stops[index];

                ImGui::PushID( (int)index );

                ImGui::SetNextItemWidth( 80.f );
                changed |= ImGui::DragFloat( "T", &stop.t, 0.01f, 0.f, 1.f, "%.2f" );

                ImGui::SameLine();
                changed |= s_colorEdit( "##stopcolor", &stop.color );

                ImGui::SameLine();
                if( ImGui::SmallButton( "X" ) == true )
                {
                    removeIndex = (int)index;
                }

                ImGui::PopID();
            }

            if( removeIndex >= 0 )
            {
                stops.erase( stops.begin() + removeIndex );
                changed = true;
            }

            if( ImGui::SmallButton( "+ Stop" ) == true )
            {
                FontEffectGradientStop stop;
                stop.t = stops.empty() == true ? 0.f : 1.f;
                stop.color = Color( 1.f, 1.f, 1.f, 1.f );

                stops.emplace_back( stop );
                changed = true;
            }

            ImGui::Unindent();

            return changed;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_renderEffectFields( FontEffectStyleDesc * const _effect )
        {
            bool changed = false;

            changed |= ImGui::Checkbox( "Enabled", &_effect->enabled );
            ImGui::SameLine();
            ImGui::SetNextItemWidth( 120.f );
            changed |= ImGui::SliderFloat( "Opacity", &_effect->opacity, 0.f, 1.f, "%.2f" );

            switch( _effect->type )
            {
            case EFET_FILL:
                {
                    changed |= s_colorEdit( "Color", &_effect->color );
                    changed |= s_renderGradient( &_effect->gradient );
                }break;
            case EFET_OUTLINE:
                {
                    changed |= s_colorEdit( "Color", &_effect->color );
                    changed |= s_dragSize( "Width", &_effect->width, 64.f );
                    changed |= s_dragSize( "Sharpness", &_effect->sharpness, 16.f );
                }break;
            case EFET_SHADOW:
                {
                    changed |= s_colorEdit( "Color", &_effect->color );
                    changed |= ImGui::DragFloat2( "Offset", &_effect->offset.x, 0.1f, -64.f, 64.f, "%.1f" );
                    changed |= s_dragSize( "Blur", &_effect->blur, 32.f );
                    changed |= s_dragSize( "Spread", &_effect->spread, 32.f );
                }break;
            case EFET_GLOW:
                {
                    changed |= s_colorEdit( "Color", &_effect->color );
                    changed |= s_dragSize( "Blur", &_effect->blur, 32.f );
                    changed |= s_dragSize( "Spread", &_effect->spread, 32.f );
                }break;
            case EFET_INNER_SHADOW:
                {
                    changed |= s_colorEdit( "Color", &_effect->color );
                    changed |= ImGui::DragFloat2( "Offset", &_effect->offset.x, 0.1f, -64.f, 64.f, "%.1f" );
                    changed |= s_dragSize( "Blur", &_effect->blur, 32.f );
                }break;
            case EFET_INNER_GLOW:
                {
                    changed |= s_colorEdit( "Color", &_effect->color );
                    changed |= s_dragSize( "Blur", &_effect->blur, 32.f );
                }break;
            case EFET_BEVEL:
                {
                    changed |= s_dragSize( "Depth", &_effect->depth, 16.f );
                    changed |= s_dragSize( "Size", &_effect->size, 32.f );
                    changed |= s_dragSize( "Soften", &_effect->soften, 16.f );
                    changed |= ImGui::SliderFloat( "Angle", &_effect->angle, 0.f, 360.f, "%.0f" );
                    changed |= ImGui::SliderFloat( "Altitude", &_effect->altitude, 0.f, 90.f, "%.0f" );
                    changed |= s_colorEdit( "Highlight", &_effect->highlight );
                    changed |= s_colorEdit( "Shadow", &_effect->shadow );
                }break;
            case EFET_BLUR:
                {
                    changed |= s_dragSize( "Blur", &_effect->blur, 32.f );
                }break;
            default:
                break;
            }

            return changed;
        }
        //////////////////////////////////////////////////////////////////////////
        static EEditorAction s_renderOrderButtons( bool _canUp, bool _canDown, bool _duplicate )
        {
            EEditorAction action = EEA_NONE;

            ImGui::BeginDisabled( _canUp == false );
            if( ImGui::SmallButton( "Up" ) == true )
            {
                action = EEA_UP;
            }
            ImGui::EndDisabled();

            ImGui::SameLine();

            ImGui::BeginDisabled( _canDown == false );
            if( ImGui::SmallButton( "Down" ) == true )
            {
                action = EEA_DOWN;
            }
            ImGui::EndDisabled();

            if( _duplicate == true )
            {
                ImGui::SameLine();

                if( ImGui::SmallButton( "Dup" ) == true )
                {
                    action = EEA_DUPLICATE;
                }
            }

            ImGui::SameLine();

            if( ImGui::SmallButton( "Remove" ) == true )
            {
                action = EEA_REMOVE;
            }

            return action;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_renderLayer( FontEffectLayerDesc * const _layer )
        {
            bool changed = false;

            changed |= ImGui::Checkbox( "Enabled", &_layer->enabled );
            ImGui::SameLine();
            ImGui::SetNextItemWidth( 120.f );
            changed |= ImGui::SliderFloat( "Opacity", &_layer->opacity, 0.f, 1.f, "%.2f" );

            VectorFontEffectStyleDescs & effects = _layer->styles;

            int actionIndex = -1;
            EEditorAction action = EEA_NONE;

            for( VectorFontEffectStyleDescs::size_type index = 0; index != effects.size(); ++index )
            {
                FontEffectStyleDesc & effect = effects[index];

                ImGui::PushID( (int)index );

                Char header[64] = {'\0'};
                MENGINE_SNPRINTF( header, sizeof( header ), "%u. %s%s###effect"
                    , (uint32_t)index + 1
                    , Helper::getFontEffectTypeName( effect.type )
                    , effect.enabled == true ? "" : " (off)"
                );

                bool open = ImGui::CollapsingHeader( header, ImGuiTreeNodeFlags_DefaultOpen );

                if( open == true )
                {
                    ImGui::Indent();

                    EEditorAction effectAction = s_renderOrderButtons( index > 0, index + 1 < effects.size(), true );

                    if( effectAction != EEA_NONE )
                    {
                        actionIndex = (int)index;
                        action = effectAction;
                    }

                    changed |= s_renderEffectFields( &effect );

                    ImGui::Unindent();
                }

                ImGui::PopID();
            }

            if( actionIndex >= 0 )
            {
                VectorFontEffectStyleDescs::size_type index = (VectorFontEffectStyleDescs::size_type)actionIndex;

                switch( action )
                {
                case EEA_UP:
                    {
                        FontEffectStyleDesc tmp = effects[index - 1];
                        effects[index - 1] = effects[index];
                        effects[index] = tmp;
                    }break;
                case EEA_DOWN:
                    {
                        FontEffectStyleDesc tmp = effects[index + 1];
                        effects[index + 1] = effects[index];
                        effects[index] = tmp;
                    }break;
                case EEA_REMOVE:
                    {
                        effects.erase( effects.begin() + index );
                    }break;
                case EEA_DUPLICATE:
                    {
                        FontEffectStyleDesc copy = effects[index];
                        effects.insert( effects.begin() + index + 1, copy );
                    }break;
                default:
                    break;
                }

                changed = true;
            }

            static int s_newEffectType = 0;

            const Char * typeNames[__EFET_MAX__];

            for( uint32_t typeIndex = 0; typeIndex != __EFET_MAX__; ++typeIndex )
            {
                typeNames[typeIndex] = Helper::getFontEffectTypeName( (EFontEffectType)typeIndex );
            }

            ImGui::SetNextItemWidth( 140.f );
            ImGui::Combo( "##neweffect", &s_newEffectType, typeNames, __EFET_MAX__ );

            ImGui::SameLine();

            if( ImGui::SmallButton( "+ Effect" ) == true )
            {
                FontEffectStyleDesc effect;
                effect.type = (EFontEffectType)s_newEffectType;

                switch( effect.type )
                {
                case EFET_OUTLINE:
                    effect.color = Color( 0.f, 0.f, 0.f, 1.f );
                    effect.width = 2.f;
                    break;
                case EFET_SHADOW:
                    effect.color = Color( 0.f, 0.f, 0.f, 0.6f );
                    effect.offset = mt::vec2f( 2.f, 2.f );
                    effect.blur = 2.f;
                    break;
                case EFET_GLOW:
                    effect.color = Color( 1.f, 0.9f, 0.3f, 1.f );
                    effect.blur = 4.f;
                    break;
                case EFET_INNER_SHADOW:
                    effect.color = Color( 0.f, 0.f, 0.f, 0.5f );
                    effect.offset = mt::vec2f( 0.f, 2.f );
                    effect.blur = 2.f;
                    break;
                case EFET_INNER_GLOW:
                    effect.color = Color( 1.f, 1.f, 1.f, 0.6f );
                    effect.blur = 3.f;
                    break;
                case EFET_BLUR:
                    effect.blur = 1.f;
                    break;
                default:
                    break;
                }

                effects.emplace_back( effect );
                changed = true;
            }

            return changed;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectViewerExampleRenderDescEditor( FontEffectDesc * const _desc )
    {
        bool changed = false;

        VectorFontEffectLayerDescs & layers = _desc->layers;

        int actionIndex = -1;
        EEditorAction action = EEA_NONE;

        for( VectorFontEffectLayerDescs::size_type index = 0; index != layers.size(); ++index )
        {
            FontEffectLayerDesc & layer = layers[index];

            ImGui::PushID( 1000 + (int)index );

            Char header[64] = {'\0'};
            MENGINE_SNPRINTF( header, sizeof( header ), "Layer %u%s###layer"
                , (uint32_t)index
                , layer.enabled == true ? "" : " (off)"
            );

            bool open = ImGui::CollapsingHeader( header, ImGuiTreeNodeFlags_DefaultOpen );

            if( open == true )
            {
                EEditorAction layerAction = s_renderOrderButtons( index > 0, index + 1 < layers.size(), false );

                if( layerAction != EEA_NONE )
                {
                    actionIndex = (int)index;
                    action = layerAction;
                }

                ImGui::Indent();
                changed |= s_renderLayer( &layer );
                ImGui::Unindent();
            }

            ImGui::PopID();
        }

        if( actionIndex >= 0 )
        {
            VectorFontEffectLayerDescs::size_type index = (VectorFontEffectLayerDescs::size_type)actionIndex;

            switch( action )
            {
            case EEA_UP:
                {
                    FontEffectLayerDesc tmp = layers[index - 1];
                    layers[index - 1] = layers[index];
                    layers[index] = tmp;
                }break;
            case EEA_DOWN:
                {
                    FontEffectLayerDesc tmp = layers[index + 1];
                    layers[index + 1] = layers[index];
                    layers[index] = tmp;
                }break;
            case EEA_REMOVE:
                {
                    layers.erase( layers.begin() + index );
                }break;
            default:
                break;
            }

            changed = true;
        }

        ImGui::BeginDisabled( layers.size() >= MENGINE_FONTEFFECT_MAX_LAYERS );

        if( ImGui::Button( "+ Layer" ) == true )
        {
            FontEffectLayerDesc layer;

            FontEffectStyleDesc fill;
            fill.type = EFET_FILL;

            layer.styles.emplace_back( fill );

            layers.emplace_back( layer );
            changed = true;
        }

        ImGui::EndDisabled();

        ImGui::SameLine();
        ImGui::TextDisabled( "(max %u layers, layer 0 is behind)", (uint32_t)MENGINE_FONTEFFECT_MAX_LAYERS );

        return changed;
    }
    //////////////////////////////////////////////////////////////////////////
}
