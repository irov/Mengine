#include "FontEffectSerialization.h"

#include "FontEffectHelper.h"

#include "Kernel/JSONDataHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdString.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            static bool parseTypeName( const Char * _name, EFontEffectType * const _type )
            {
                for( uint32_t index = 0; index != MENGINE_FONTEFFECT_TYPE_MAX; ++index )
                {
                    const Char * name = Helper::getFontEffectTypeName( (EFontEffectType)index );

                    if( StdString::strcmp( name, _name ) != 0 )
                    {
                        continue;
                    }

                    *_type = (EFontEffectType)index;

                    return true;
                }

                return false;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool parseSpaceName( const Char * _name, EFontEffectSpace * const _space )
            {
                for( uint32_t index = 0; index != MENGINE_FONTEFFECT_SPACE_MAX; ++index )
                {
                    const Char * name = Helper::getFontEffectSpaceName( (EFontEffectSpace)index );

                    if( StdString::strcmp( name, _name ) != 0 )
                    {
                        continue;
                    }

                    *_space = (EFontEffectSpace)index;

                    return true;
                }

                return false;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool parseBlendModeName( const Char * _name, EFontEffectBlendMode * const _mode )
            {
                for( uint32_t index = 0; index != MENGINE_FONTEFFECT_BLEND_MODE_MAX; ++index )
                {
                    const Char * name = Helper::getFontEffectBlendModeName( (EFontEffectBlendMode)index );

                    if( StdString::strcmp( name, _name ) != 0 )
                    {
                        continue;
                    }

                    *_mode = (EFontEffectBlendMode)index;

                    return true;
                }

                return false;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool parseOutlinePositionName( const Char * _name, EFontEffectOutlinePosition * const _position )
            {
                for( uint32_t index = 0; index != MENGINE_FONTEFFECT_OUTLINE_POSITION_MAX; ++index )
                {
                    const Char * name = Helper::getFontEffectOutlinePositionName( (EFontEffectOutlinePosition)index );

                    if( StdString::strcmp( name, _name ) != 0 )
                    {
                        continue;
                    }

                    *_position = (EFontEffectOutlinePosition)index;

                    return true;
                }

                return false;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool parseGradientTypeName( const Char * _name, EFontEffectGradientType * const _type )
            {
                for( uint32_t index = 0; index != MENGINE_FONTEFFECT_GRADIENT_TYPE_MAX; ++index )
                {
                    const Char * name = Helper::getFontEffectGradientTypeName( (EFontEffectGradientType)index );

                    if( StdString::strcmp( name, _name ) != 0 )
                    {
                        continue;
                    }

                    *_type = (EFontEffectGradientType)index;

                    return true;
                }

                return false;
            }
            //////////////////////////////////////////////////////////////////////////
            static void dumpVec2f( const mt::vec2f & _value, jpp::object * const _json )
            {
                jpp::array j_value = jpp::make_array();

                j_value.push_back( _value.x );
                j_value.push_back( _value.y );

                *_json = j_value.to_object();
            }
            //////////////////////////////////////////////////////////////////////////
            static bool loadGradient( const jpp::object & _json, FontEffectGradientDesc * const _gradient )
            {
                if( _json.is_type_object() == false )
                {
                    LOGGER_ERROR( "font effect gradient is not object" );

                    return false;
                }

                _gradient->enabled = _json.get( "Enabled", true );
                _gradient->angle = _json.get( "Angle", 90.f );
                _gradient->scale = _json.get( "Scale", 1.f );
                _gradient->reverse = _json.get( "Reverse", false );
                _gradient->dither = _json.get( "Dither", false );

                Helper::getJSONVec2f( _json, "Center", &_gradient->center );

                const Char * type_name = _json.get( "Type", "Linear" );

                if( Detail::parseGradientTypeName( type_name, &_gradient->type ) == false )
                {
                    LOGGER_ERROR( "font effect gradient invalid type '%s'"
                        , type_name
                    );

                    return false;
                }

                const Char * space_name = _json.get( "Space", "Glyph" );

                if( Detail::parseSpaceName( space_name, &_gradient->space ) == false )
                {
                    LOGGER_ERROR( "font effect gradient invalid space '%s'"
                        , space_name
                    );

                    return false;
                }

                _gradient->stops.clear();

                jpp::object j_stops;
                if( _json.exist( "Stops", &j_stops ) == true )
                {
                    if( j_stops.is_type_array() == false )
                    {
                        LOGGER_ERROR( "font effect gradient 'Stops' is not array" );

                        return false;
                    }

                    for( const jpp::object & j_stop : jpp::array( j_stops ) )
                    {
                        FontEffectGradientStop stop;
                        stop.t = j_stop.get( "T", 0.f );

                        Helper::getJSONColor( j_stop, "Color", &stop.color );

                        _gradient->stops.emplace_back( stop );
                    }
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            static void dumpGradient( const FontEffectGradientDesc & _gradient, jpp::object * const _json )
            {
                jpp::object j_gradient = jpp::make_object();

                const Char * type_name = Helper::getFontEffectGradientTypeName( _gradient.type );
                const Char * space_name = Helper::getFontEffectSpaceName( _gradient.space );

                jpp::object j_center;
                Detail::dumpVec2f( _gradient.center, &j_center );

                j_gradient.set( "Enabled", _gradient.enabled );
                j_gradient.set( "Type", type_name );
                j_gradient.set( "Angle", _gradient.angle );
                j_gradient.set( "Center", j_center );
                j_gradient.set( "Scale", _gradient.scale );
                j_gradient.set( "Reverse", _gradient.reverse );
                j_gradient.set( "Dither", _gradient.dither );
                j_gradient.set( "Space", space_name );

                jpp::array j_stops = jpp::make_array();

                for( const FontEffectGradientStop & stop : _gradient.stops )
                {
                    jpp::object j_stop = jpp::make_object();

                    j_stop.set( "T", stop.t );
                    j_stop.set( "Color", stop.color );

                    j_stops.push_back( j_stop );
                }

                j_gradient.set( "Stops", j_stops.to_object() );

                *_json = j_gradient;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool loadEffect( const jpp::object & _json, FontEffectStyleDesc * const _effect )
            {
                if( _json.is_type_object() == false )
                {
                    LOGGER_ERROR( "font effect effect is not object" );

                    return false;
                }

                const Char * type_name = _json.get( "Type", "" );

                if( Detail::parseTypeName( type_name, &_effect->type ) == false )
                {
                    LOGGER_ERROR( "font effect invalid type '%s'"
                        , type_name
                    );

                    return false;
                }

                _effect->enabled = _json.get( "Enabled", true );
                _effect->opacity = _json.get( "Opacity", 1.f );

                const Char * blend_name = _json.get( "BlendMode", "Normal" );

                if( Detail::parseBlendModeName( blend_name, &_effect->blendMode ) == false )
                {
                    LOGGER_ERROR( "font effect invalid blend mode '%s'"
                        , blend_name
                    );

                    return false;
                }

                Helper::getJSONColor( _json, "Color", &_effect->color );

                jpp::object j_gradient;
                if( _json.exist( "Gradient", &j_gradient ) == true )
                {
                    if( Detail::loadGradient( j_gradient, &_effect->gradient ) == false )
                    {
                        LOGGER_ERROR( "font effect invalid gradient" );

                        return false;
                    }
                }

                _effect->width = _json.get( "Width", 1.f );
                _effect->sharpness = _json.get( "Sharpness", 0.f );

                const Char * position_name = _json.get( "Position", "Outside" );

                if( Detail::parseOutlinePositionName( position_name, &_effect->position ) == false )
                {
                    LOGGER_ERROR( "font effect invalid outline position '%s'"
                        , position_name
                    );

                    return false;
                }

                Helper::getJSONVec2f( _json, "Offset", &_effect->offset );

                _effect->blur = _json.get( "Blur", 0.f );
                _effect->spread = _json.get( "Spread", 0.f );

                _effect->distance = _json.get( "Distance", 0.f );
                _effect->invert = _json.get( "Invert", false );

                _effect->depth = _json.get( "Depth", 1.f );
                _effect->size = _json.get( "Size", 2.f );
                _effect->soften = _json.get( "Soften", 0.f );
                _effect->angle = _json.get( "Angle", 120.f );
                _effect->altitude = _json.get( "Altitude", 30.f );

                Helper::getJSONColor( _json, "Highlight", &_effect->highlight );
                Helper::getJSONColor( _json, "Shadow", &_effect->shadow );

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            static void dumpEffect( const FontEffectStyleDesc & _effect, jpp::object * const _json )
            {
                jpp::object j_effect = jpp::make_object();

                const Char * type_name = Helper::getFontEffectTypeName( _effect.type );

                const Char * blend_name = Helper::getFontEffectBlendModeName( _effect.blendMode );

                j_effect.set( "Type", type_name );
                j_effect.set( "Enabled", _effect.enabled );
                j_effect.set( "Opacity", _effect.opacity );
                j_effect.set( "BlendMode", blend_name );

                switch( _effect.type )
                {
                case EFET_FILL:
                    {
                        j_effect.set( "Color", _effect.color );

                        if( _effect.gradient.enabled == true || _effect.gradient.stops.empty() == false )
                        {
                            jpp::object j_gradient;
                            Detail::dumpGradient( _effect.gradient, &j_gradient );

                            j_effect.set( "Gradient", j_gradient );
                        }
                    }break;
                case EFET_OUTLINE:
                    {
                        const Char * position_name = Helper::getFontEffectOutlinePositionName( _effect.position );

                        j_effect.set( "Color", _effect.color );
                        j_effect.set( "Width", _effect.width );
                        j_effect.set( "Sharpness", _effect.sharpness );
                        j_effect.set( "Position", position_name );
                    }break;
                case EFET_SHADOW:
                case EFET_INNER_SHADOW:
                    {
                        jpp::object j_offset;
                        Detail::dumpVec2f( _effect.offset, &j_offset );

                        j_effect.set( "Color", _effect.color );
                        j_effect.set( "Offset", j_offset );
                        j_effect.set( "Blur", _effect.blur );
                        j_effect.set( "Spread", _effect.spread );
                    }break;
                case EFET_GLOW:
                case EFET_INNER_GLOW:
                    {
                        j_effect.set( "Color", _effect.color );
                        j_effect.set( "Blur", _effect.blur );
                        j_effect.set( "Spread", _effect.spread );
                    }break;
                case EFET_BEVEL:
                    {
                        j_effect.set( "Depth", _effect.depth );
                        j_effect.set( "Size", _effect.size );
                        j_effect.set( "Soften", _effect.soften );
                        j_effect.set( "Angle", _effect.angle );
                        j_effect.set( "Altitude", _effect.altitude );
                        j_effect.set( "Highlight", _effect.highlight );
                        j_effect.set( "Shadow", _effect.shadow );
                    }break;
                case EFET_BLUR:
                    {
                        j_effect.set( "Blur", _effect.blur );
                    }break;
                case EFET_SATIN:
                    {
                        j_effect.set( "Color", _effect.color );
                        j_effect.set( "Distance", _effect.distance );
                        j_effect.set( "Angle", _effect.angle );
                        j_effect.set( "Blur", _effect.blur );
                        j_effect.set( "Invert", _effect.invert );
                    }break;
                }

                *_json = j_effect;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool loadLayer( const jpp::object & _json, FontEffectLayerDesc * const _layer )
            {
                if( _json.is_type_object() == false )
                {
                    LOGGER_ERROR( "font effect layer is not object" );

                    return false;
                }

                _layer->enabled = _json.get( "Enabled", true );
                _layer->opacity = _json.get( "Opacity", 1.f );
                _layer->knockout = _json.get( "Knockout", false );
                _layer->merge = _json.get( "Merge", false );

                const Char * blend_name = _json.get( "BlendMode", "Normal" );

                if( Detail::parseBlendModeName( blend_name, &_layer->blendMode ) == false )
                {
                    LOGGER_ERROR( "font effect layer invalid blend mode '%s'"
                        , blend_name
                    );

                    return false;
                }

                _layer->styles.clear();

                jpp::object j_effects;
                if( _json.exist( "Effects", &j_effects ) == true )
                {
                    if( j_effects.is_type_array() == false )
                    {
                        LOGGER_ERROR( "font effect layer 'Effects' is not array" );

                        return false;
                    }

                    for( const jpp::object & j_effect : jpp::array( j_effects ) )
                    {
                        FontEffectStyleDesc effect;
                        if( Detail::loadEffect( j_effect, &effect ) == false )
                        {
                            return false;
                        }

                        _layer->styles.emplace_back( effect );
                    }
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            static void dumpLayer( const FontEffectLayerDesc & _layer, jpp::object * const _json )
            {
                jpp::object j_layer = jpp::make_object();

                const Char * blend_name = Helper::getFontEffectBlendModeName( _layer.blendMode );

                j_layer.set( "Enabled", _layer.enabled );
                j_layer.set( "Opacity", _layer.opacity );
                j_layer.set( "BlendMode", blend_name );
                j_layer.set( "Knockout", _layer.knockout );
                j_layer.set( "Merge", _layer.merge );

                jpp::array j_effects = jpp::make_array();

                for( const FontEffectStyleDesc & effect : _layer.styles )
                {
                    jpp::object j_effect;
                    Detail::dumpEffect( effect, &j_effect );

                    j_effects.push_back( j_effect );
                }

                j_layer.set( "Effects", j_effects.to_object() );

                *_json = j_layer;
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        bool loadFontEffectDesc( const jpp::object & _json, FontEffectDesc * const _desc )
        {
            if( _json.is_type_object() == false )
            {
                LOGGER_ERROR( "font effect desc is not object" );

                return false;
            }

            _desc->layers.clear();

            jpp::object j_layers;
            if( _json.exist( "Layers", &j_layers ) == false )
            {
                LOGGER_ERROR( "font effect desc don't have 'Layers'" );

                return false;
            }

            if( j_layers.is_type_array() == false )
            {
                LOGGER_ERROR( "font effect desc 'Layers' is not array" );

                return false;
            }

            for( const jpp::object & j_layer : jpp::array( j_layers ) )
            {
                FontEffectLayerDesc layer;
                if( Detail::loadLayer( j_layer, &layer ) == false )
                {
                    return false;
                }

                _desc->layers.emplace_back( layer );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void dumpFontEffectDesc( const FontEffectDesc & _desc, jpp::object * const _json )
        {
            jpp::object j_desc = jpp::make_object();

            jpp::array j_layers = jpp::make_array();

            for( const FontEffectLayerDesc & layer : _desc.layers )
            {
                jpp::object j_layer;
                Detail::dumpLayer( layer, &j_layer );

                j_layers.push_back( j_layer );
            }

            j_desc.set( "Layers", j_layers.to_object() );

            *_json = j_desc;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
