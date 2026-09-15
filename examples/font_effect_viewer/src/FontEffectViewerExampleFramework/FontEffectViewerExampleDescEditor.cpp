#include "FontEffectViewerExampleDescEditor.h"

#include "Config/StdAssert.h"
#include "Config/StdString.h"

#include "Kernel/FilePathHelper.h"

#include "Plugins/FontEffectPlugin/FontEffectDesc.h"
#include "Plugins/FontEffectPlugin/FontEffectHelper.h"

#include "Config/StdIO.h"

#include "Mosaic/Mosaic.hpp"

namespace Mengine
{
    namespace Detail
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
        static bool colorEdit( Mosaic::Context * _ui, const Char * _label, Color * const _color, Mosaic::LabelPlacement _labelPlacement = Mosaic::LabelPlacement::Before, const Mosaic::SourceLocation & _location = Mosaic::SourceLocation::current() )
        {
            Mosaic::Color value = {_color->getR(), _color->getG(), _color->getB(), _color->getA()};

            Mosaic::ColorEditOptions options;
            options.labelPlacement = _labelPlacement;

            if( Mosaic::colorEditorRgba( _ui, _label, &value, options, _location ).changed() == false )
            {
                return false;
            }

            _color->setRGBA( value.r, value.g, value.b, value.a );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool dragSize( Mosaic::Context * _ui, const Char * _label, float * const _value, float _max, const Mosaic::SourceLocation & _location = Mosaic::SourceLocation::current() )
        {
            Mosaic::SliderOptions options;
            options.minimum = 0.0;
            options.maximum = (double)_max;
            options.dragSpeed = 0.05;
            options.precision = 2;

            bool changed = Mosaic::dragValue( _ui, _label, _value, options, _location ).changed();

            if( *_value < 0.f )
            {
                *_value = 0.f;
            }

            return changed;
        }
        //////////////////////////////////////////////////////////////////////////
        constexpr uint32_t ENUM_COMBO_MAX_NAMES = 16;
        //////////////////////////////////////////////////////////////////////////
        static bool enumCombo( Mosaic::Context * _ui, const Char * _label, uint32_t * const _value, uint32_t _count, const Char * ( *_name )( uint32_t ), const Mosaic::SourceLocation & _location = Mosaic::SourceLocation::current() )
        {
            assert( _count <= ENUM_COMBO_MAX_NAMES );

            const Char * names[ENUM_COMBO_MAX_NAMES];

            for( uint32_t index = 0; index != _count; ++index )
            {
                names[index] = _name( index );
            }

            Mosaic::StringView items[ENUM_COMBO_MAX_NAMES];

            for( uint32_t index = 0; index != _count; ++index )
            {
                items[index] = names[index];
            }

            int32_t value = (int32_t)*_value;

            if( Mosaic::comboBox( _ui, _label, &value, Mosaic::StringViewSpan( items, _count ), _location ).changed() == false )
            {
                return false;
            }

            *_value = (uint32_t)value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static const Char * spaceName( uint32_t _index )
        {
            const Char * name = Helper::getFontEffectSpaceName( (EFontEffectSpace)_index );

            return name;
        }
        //////////////////////////////////////////////////////////////////////////
        static const Char * gradientTypeName( uint32_t _index )
        {
            const Char * name = Helper::getFontEffectGradientTypeName( (EFontEffectGradientType)_index );

            return name;
        }
        //////////////////////////////////////////////////////////////////////////
        static const Char * patternTileName( uint32_t _index )
        {
            const Char * name = Helper::getFontEffectPatternTileName( (EFontEffectPatternTile)_index );

            return name;
        }
        //////////////////////////////////////////////////////////////////////////
        static const Char * blendModeName( uint32_t _index )
        {
            const Char * name = Helper::getFontEffectBlendModeName( (EFontEffectBlendMode)_index );

            return name;
        }
        //////////////////////////////////////////////////////////////////////////
        static const Char * outlinePositionName( uint32_t _index )
        {
            const Char * name = Helper::getFontEffectOutlinePositionName( (EFontEffectOutlinePosition)_index );

            return name;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool renderGradient( Mosaic::Context * _ui, FontEffectGradientDesc * const _gradient )
        {
            bool changed = false;

            changed |= Mosaic::checkbox( _ui, "Gradient", &_gradient->enabled ).changed();

            if( _gradient->enabled == false )
            {
                return changed;
            }

            changed |= enumCombo( _ui, "Type", (uint32_t *)&_gradient->type, MENGINE_FONTEFFECT_GRADIENT_TYPE_MAX, &gradientTypeName );
            changed |= [&]() { Mosaic::SliderOptions o; o.minimum = 0.f; o.maximum = 360.f; o.precision = 0; return Mosaic::slider( _ui, "Angle", &_gradient->angle, o ).changed(); }();
            changed |= [&]() { Mosaic::SliderOptions o; o.minimum = -1.f; o.maximum = 1.f; o.dragSpeed = 0.01f; o.precision = 2; return Mosaic::dragFloatVector( _ui, "Center", Mosaic::FloatSpan( &_gradient->center.x, 2 ), o ).changed(); }();
            changed |= dragSize( _ui, "Scale", &_gradient->scale, 8.f );
            changed |= Mosaic::checkbox( _ui, "Reverse", &_gradient->reverse ).changed();
            Mosaic::sameLine( _ui );
            changed |= Mosaic::checkbox( _ui, "Dither", &_gradient->dither ).changed();
            changed |= enumCombo( _ui, "Space", (uint32_t *)&_gradient->space, MENGINE_FONTEFFECT_SPACE_MAX, &spaceName );

            int removeIndex = -1;

            VectorFontEffectGradientStops & stops = _gradient->stops;

            for( VectorFontEffectGradientStops::size_type index = 0; index != stops.size(); ++index )
            {
                FontEffectGradientStop & stop = stops[index];

                Mosaic::Scope idScope = Mosaic::scope( _ui, Mosaic::Key( (int)index ) );

                Mosaic::setNextItemWidth( _ui, 80.f );
                changed |= [&]() { Mosaic::SliderOptions o; o.minimum = 0.f; o.maximum = 1.f; o.dragSpeed = 0.01f; o.precision = 2; return Mosaic::dragValue( _ui, "T", &stop.t, o ).changed(); }();

                Mosaic::sameLine( _ui );
                changed |= colorEdit( _ui, "Stop color", &stop.color, Mosaic::LabelPlacement::Hidden );

                Mosaic::sameLine( _ui );
                if( Mosaic::smallButton( _ui, "X" ).clicked() == true )
                {
                    removeIndex = (int)index;
                }

            }

            if( removeIndex >= 0 )
            {
                stops.erase( stops.begin() + removeIndex );
                changed = true;
            }

            if( Mosaic::smallButton( _ui, "+ Stop" ).clicked() == true )
            {
                FontEffectGradientStop stop;
                stop.t = stops.empty() == true ? 0.f : 1.f;
                stop.color = Color( 1.f, 1.f, 1.f, 1.f );

                stops.emplace_back( stop );
                changed = true;
            }

            return changed;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool renderEffectFields( Mosaic::Context * _ui, FontEffectStyleDesc * const _effect )
        {
            bool changed = false;

            changed |= Mosaic::checkbox( _ui, "Enabled", &_effect->enabled ).changed();
            Mosaic::sameLine( _ui );
            Mosaic::setNextItemWidth( _ui, 120.f );
            changed |= [&]() { Mosaic::SliderOptions o; o.minimum = 0.f; o.maximum = 1.f; o.precision = 2; return Mosaic::slider( _ui, "Opacity", &_effect->opacity, o ).changed(); }();
            changed |= enumCombo( _ui, "Blend", (uint32_t *)&_effect->blendMode, MENGINE_FONTEFFECT_BLEND_MODE_MAX, &blendModeName );

            switch( _effect->type )
            {
            case EFET_FILL:
                {
                    changed |= colorEdit( _ui, "Color", &_effect->color );
                    changed |= renderGradient( _ui, &_effect->gradient );
                }break;
            case EFET_OUTLINE:
                {
                    changed |= colorEdit( _ui, "Color", &_effect->color );
                    changed |= dragSize( _ui, "Width", &_effect->width, 64.f );
                    changed |= dragSize( _ui, "Sharpness", &_effect->sharpness, 16.f );
                    changed |= enumCombo( _ui, "Position", (uint32_t *)&_effect->position, MENGINE_FONTEFFECT_OUTLINE_POSITION_MAX, &outlinePositionName );
                }break;
            case EFET_SHADOW:
                {
                    changed |= colorEdit( _ui, "Color", &_effect->color );
                    changed |= [&]() { Mosaic::SliderOptions o; o.minimum = -64.f; o.maximum = 64.f; o.dragSpeed = 0.1f; o.precision = 1; return Mosaic::dragFloatVector( _ui, "Offset", Mosaic::FloatSpan( &_effect->offset.x, 2 ), o ).changed(); }();
                    changed |= dragSize( _ui, "Blur", &_effect->blur, 32.f );
                    changed |= dragSize( _ui, "Spread", &_effect->spread, 32.f );
                }break;
            case EFET_GLOW:
                {
                    changed |= colorEdit( _ui, "Color", &_effect->color );
                    changed |= dragSize( _ui, "Blur", &_effect->blur, 32.f );
                    changed |= dragSize( _ui, "Spread", &_effect->spread, 32.f );
                }break;
            case EFET_INNER_SHADOW:
                {
                    changed |= colorEdit( _ui, "Color", &_effect->color );
                    changed |= [&]() { Mosaic::SliderOptions o; o.minimum = -64.f; o.maximum = 64.f; o.dragSpeed = 0.1f; o.precision = 1; return Mosaic::dragFloatVector( _ui, "Offset", Mosaic::FloatSpan( &_effect->offset.x, 2 ), o ).changed(); }();
                    changed |= dragSize( _ui, "Blur", &_effect->blur, 32.f );
                }break;
            case EFET_INNER_GLOW:
                {
                    changed |= colorEdit( _ui, "Color", &_effect->color );
                    changed |= dragSize( _ui, "Blur", &_effect->blur, 32.f );
                }break;
            case EFET_BEVEL:
                {
                    changed |= dragSize( _ui, "Depth", &_effect->depth, 16.f );
                    changed |= dragSize( _ui, "Size", &_effect->size, 32.f );
                    changed |= dragSize( _ui, "Soften", &_effect->soften, 16.f );
                    changed |= [&]() { Mosaic::SliderOptions o; o.minimum = 0.f; o.maximum = 360.f; o.precision = 0; return Mosaic::slider( _ui, "Angle", &_effect->angle, o ).changed(); }();
                    changed |= [&]() { Mosaic::SliderOptions o; o.minimum = 0.f; o.maximum = 90.f; o.precision = 0; return Mosaic::slider( _ui, "Altitude", &_effect->altitude, o ).changed(); }();
                    changed |= colorEdit( _ui, "Highlight", &_effect->highlight );
                    changed |= colorEdit( _ui, "Shadow", &_effect->shadow );
                }break;
            case EFET_BLUR:
                {
                    changed |= dragSize( _ui, "Blur", &_effect->blur, 32.f );
                }break;
            case EFET_PATTERN:
                {
                    Mosaic::String path( _effect->pattern.filePath.c_str() );

                    if( Mosaic::property( _ui, "Path", &path ).changed() == true )
                    {
                        _effect->pattern.filePath = Helper::stringizeFilePath( path.c_str() );
                        changed = true;
                    }

                    changed |= colorEdit( _ui, "Color", &_effect->color );
                    changed |= enumCombo( _ui, "Tile", (uint32_t *)&_effect->pattern.tile, MENGINE_FONTEFFECT_PATTERN_TILE_MAX, &patternTileName );
                    changed |= dragSize( _ui, "Scale", &_effect->pattern.scale, 8.f );
                    changed |= [&]() { Mosaic::SliderOptions o; o.minimum = -64.f; o.maximum = 64.f; o.dragSpeed = 0.1f; o.precision = 1; return Mosaic::dragFloatVector( _ui, "Offset", Mosaic::FloatSpan( &_effect->pattern.offset.x, 2 ), o ).changed(); }();
                    changed |= [&]() { Mosaic::SliderOptions o; o.minimum = 0.f; o.maximum = 360.f; o.precision = 0; return Mosaic::slider( _ui, "Angle", &_effect->pattern.angle, o ).changed(); }();
                    changed |= enumCombo( _ui, "Space", (uint32_t *)&_effect->pattern.space, MENGINE_FONTEFFECT_SPACE_MAX, &spaceName );
                }break;
            case EFET_SATIN:
                {
                    changed |= colorEdit( _ui, "Color", &_effect->color );
                    changed |= [&]() { Mosaic::SliderOptions o; o.minimum = -64.f; o.maximum = 64.f; o.dragSpeed = 0.1f; o.precision = 1; return Mosaic::dragValue( _ui, "Distance", &_effect->distance, o ).changed(); }();
                    changed |= [&]() { Mosaic::SliderOptions o; o.minimum = 0.f; o.maximum = 360.f; o.precision = 0; return Mosaic::slider( _ui, "Angle", &_effect->angle, o ).changed(); }();
                    changed |= dragSize( _ui, "Blur", &_effect->blur, 32.f );
                    changed |= Mosaic::checkbox( _ui, "Invert", &_effect->invert ).changed();
                }break;
            }

            return changed;
        }
        //////////////////////////////////////////////////////////////////////////
        static EEditorAction renderOrderButtons( Mosaic::Context * _ui, bool _canUp, bool _canDown, bool _duplicate )
        {
            EEditorAction action = EEA_NONE;

            {
                Mosaic::Scope disabledScope = Mosaic::disabledScope( _ui, _canUp == false );

                if( Mosaic::smallButton( _ui, "Up" ).clicked() == true )
                {
                    action = EEA_UP;
                }
            }

            Mosaic::sameLine( _ui );

            {
                Mosaic::Scope disabledScope = Mosaic::disabledScope( _ui, _canDown == false );

                if( Mosaic::smallButton( _ui, "Down" ).clicked() == true )
                {
                    action = EEA_DOWN;
                }
            }

            if( _duplicate == true )
            {
                Mosaic::sameLine( _ui );

                if( Mosaic::smallButton( _ui, "Dup" ).clicked() == true )
                {
                    action = EEA_DUPLICATE;
                }
            }

            Mosaic::sameLine( _ui );

            if( Mosaic::smallButton( _ui, "Remove" ).clicked() == true )
            {
                action = EEA_REMOVE;
            }

            return action;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool renderLayer( Mosaic::Context * _ui, FontEffectLayerDesc * const _layer )
        {
            bool changed = false;

            changed |= Mosaic::checkbox( _ui, "Enabled", &_layer->enabled ).changed();
            Mosaic::sameLine( _ui );
            Mosaic::setNextItemWidth( _ui, 120.f );
            changed |= [&]() { Mosaic::SliderOptions o; o.minimum = 0.f; o.maximum = 1.f; o.precision = 2; return Mosaic::slider( _ui, "Opacity", &_layer->opacity, o ).changed(); }();
            changed |= enumCombo( _ui, "Blend", (uint32_t *)&_layer->blendMode, MENGINE_FONTEFFECT_BLEND_MODE_MAX, &blendModeName );
            changed |= Mosaic::checkbox( _ui, "Knockout", &_layer->knockout ).changed();
            Mosaic::sameLine( _ui );
            changed |= Mosaic::checkbox( _ui, "Merge", &_layer->merge ).changed();

            VectorFontEffectStyleDescs & effects = _layer->styles;

            int actionIndex = -1;
            EEditorAction action = EEA_NONE;

            for( VectorFontEffectStyleDescs::size_type index = 0; index != effects.size(); ++index )
            {
                FontEffectStyleDesc & effect = effects[index];

                Mosaic::Scope idScope = Mosaic::scope( _ui, Mosaic::Key( (int)index ) );

                Char header[64] = {'\0'};
                MENGINE_SNPRINTF( header, sizeof( header ), "%u. %s%s"
                    , (uint32_t)index + 1
                    , Helper::getFontEffectTypeName( effect.type )
                    , effect.enabled == true ? "" : " (off)"
                );

                Mosaic::TreeScope headerScope = Mosaic::collapsingHeader( _ui, header, true );

                if( headerScope.expanded() == true )
                {
                    EEditorAction effectAction = renderOrderButtons( _ui, index > 0, index + 1 < effects.size(), true );

                    if( effectAction != EEA_NONE )
                    {
                        actionIndex = (int)index;
                        action = effectAction;
                    }

                    changed |= renderEffectFields( _ui, &effect );
                }

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
                case EEA_NONE:
                    break;
                }

                changed = true;
            }

            static int s_newEffectType = 0;

            const Char * typeNames[MENGINE_FONTEFFECT_TYPE_MAX];

            for( uint32_t typeIndex = 0; typeIndex != MENGINE_FONTEFFECT_TYPE_MAX; ++typeIndex )
            {
                typeNames[typeIndex] = Helper::getFontEffectTypeName( (EFontEffectType)typeIndex );
            }

            Mosaic::StringView typeItems[MENGINE_FONTEFFECT_TYPE_MAX];

            for( uint32_t typeIndex = 0; typeIndex != MENGINE_FONTEFFECT_TYPE_MAX; ++typeIndex )
            {
                typeItems[typeIndex] = typeNames[typeIndex];
            }

            Mosaic::ComboOptions newEffectOptions;
            newEffectOptions.labelPlacement = Mosaic::LabelPlacement::Hidden;

            Mosaic::setNextItemWidth( _ui, 140.f );
            Mosaic::comboBox( _ui, "New effect", &s_newEffectType, Mosaic::StringViewSpan( typeItems, MENGINE_FONTEFFECT_TYPE_MAX ), newEffectOptions );

            Mosaic::sameLine( _ui );

            if( Mosaic::smallButton( _ui, "+ Effect" ).clicked() == true )
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
                case EFET_PATTERN:
                    effect.color = Color( 1.f, 1.f, 1.f, 1.f );
                    break;
                case EFET_SATIN:
                    effect.color = Color( 0.f, 0.f, 0.f, 0.5f );
                    effect.distance = 4.f;
                    effect.angle = 120.f;
                    effect.blur = 3.f;
                    break;
                case EFET_FILL:
                case EFET_BEVEL:
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
    bool FontEffectViewerExampleRenderDescEditor( Mosaic::Context * _ui, FontEffectDesc * const _desc )
    {
        bool changed = false;

        VectorFontEffectLayerDescs & layers = _desc->layers;

        int actionIndex = -1;
        Detail::EEditorAction action = Detail::EEA_NONE;

        for( VectorFontEffectLayerDescs::size_type index = 0; index != layers.size(); ++index )
        {
            FontEffectLayerDesc & layer = layers[index];

            Mosaic::Scope idScope = Mosaic::scope( _ui, Mosaic::Key( (int)index ) );

            Char header[64] = {'\0'};
            MENGINE_SNPRINTF( header, sizeof( header ), "Layer %u%s"
                , (uint32_t)index
                , layer.enabled == true ? "" : " (off)"
            );

            Mosaic::TreeScope headerScope = Mosaic::collapsingHeader( _ui, header, true );

            if( headerScope.expanded() == true )
            {
                Detail::EEditorAction layerAction = Detail::renderOrderButtons( _ui, index > 0, index + 1 < layers.size(), false );

                if( layerAction != Detail::EEA_NONE )
                {
                    actionIndex = (int)index;
                    action = layerAction;
                }

                changed |= Detail::renderLayer( _ui, &layer );
            }

        }

        if( actionIndex >= 0 )
        {
            VectorFontEffectLayerDescs::size_type index = (VectorFontEffectLayerDescs::size_type)actionIndex;

            switch( action )
            {
            case Detail::EEA_UP:
                {
                    FontEffectLayerDesc tmp = layers[index - 1];
                    layers[index - 1] = layers[index];
                    layers[index] = tmp;
                }break;
            case Detail::EEA_DOWN:
                {
                    FontEffectLayerDesc tmp = layers[index + 1];
                    layers[index + 1] = layers[index];
                    layers[index] = tmp;
                }break;
            case Detail::EEA_REMOVE:
                {
                    layers.erase( layers.begin() + index );
                }break;
            case Detail::EEA_DUPLICATE:
            case Detail::EEA_NONE:
                break;
            }

            changed = true;
        }

        {
            Mosaic::Scope disabledScope = Mosaic::disabledScope( _ui, layers.size() >= MENGINE_FONTEFFECT_MAX_LAYERS );

            if( Mosaic::button( _ui, "+ Layer" ).clicked() == true )
            {
                FontEffectLayerDesc layer;

                FontEffectStyleDesc fill;
                fill.type = EFET_FILL;

                layer.styles.emplace_back( fill );

                layers.emplace_back( layer );
                changed = true;
            }

        }

        Mosaic::sameLine( _ui );
        Char hint[96] = {'\0'};
        MENGINE_SNPRINTF( hint, sizeof( hint ) - 1, "(max %u layers, layer 0 is behind)", (uint32_t)MENGINE_FONTEFFECT_MAX_LAYERS );

        Mosaic::Scope hintScope = Mosaic::disabledScope( _ui, true );

        Mosaic::text( _ui, hint );

        return changed;
    }
    //////////////////////////////////////////////////////////////////////////
}
