#include "FontEffectDataflow.h"

#include "FontEffectData.h"
#include "FontEffectHelper.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/JSONHelper.h"
#include "Kernel/JSONDataHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool parseTypeName( const Char * _name, EFontEffectType * const _type )
        {
            for( uint32_t index = 0; index != __EFET_MAX__; ++index )
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
        static bool parseGradientSpaceName( const Char * _name, EFontEffectGradientSpace * const _space )
        {
            for( uint32_t index = 0; index != __EFEGS_MAX__; ++index )
            {
                const Char * name = Helper::getFontEffectGradientSpaceName( (EFontEffectGradientSpace)index );

                if( StdString::strcmp( name, _name ) != 0 )
                {
                    continue;
                }

                *_space = (EFontEffectGradientSpace)index;

                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool loadGradient( const jpp::object & _json, FontEffectGradientDesc * const _gradient )
        {
            if( _json.is_type_object() == false )
            {
                return false;
            }

            _gradient->enabled = _json.get( "Enabled", true );
            _gradient->angle = _json.get( "Angle", 90.f );

            const Char * space_name = _json.get( "Space", "Glyph" );

            if( Detail::parseGradientSpaceName( space_name, &_gradient->space ) == false )
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

            Helper::getJSONVec2f( _json, "Offset", &_effect->offset );

            _effect->blur = _json.get( "Blur", 0.f );
            _effect->spread = _json.get( "Spread", 0.f );

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
        static bool loadLayer( const jpp::object & _json, FontEffectLayerDesc * const _layer )
        {
            if( _json.is_type_object() == false )
            {
                LOGGER_ERROR( "font effect layer is not object" );

                return false;
            }

            _layer->enabled = _json.get( "Enabled", true );
            _layer->opacity = _json.get( "Opacity", 1.f );

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
        static bool loadDesc( const jpp::object & _json, FontEffectDesc * const _desc )
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
    }
    //////////////////////////////////////////////////////////////////////////
    FontEffectDataflow::FontEffectDataflow()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FontEffectDataflow::~FontEffectDataflow()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectDataflow::initialize()
    {
        m_factoryFontEffectData = Helper::makeFactoryPool<FontEffectData, 32>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectDataflow::finalize()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryFontEffectData );

        m_factoryFontEffectData = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectDataflow::isThreadFlow() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    DataInterfacePtr FontEffectDataflow::create( const DocumentInterfacePtr & _doc )
    {
        FontEffectDataPtr data = m_factoryFontEffectData->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( data, "invalid create data" );

        return data;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr FontEffectDataflow::load( const InputStreamInterfacePtr & _stream, const DocumentInterfacePtr & _doc )
    {
        MemoryInterfacePtr memory = Helper::createMemoryStream( _stream, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory" );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectDataflow::flow( const DataInterfacePtr & _data, const MemoryInterfacePtr & _memory, const DataflowContext * _context, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _context );

        FontEffectData * data = _data.getT<FontEffectData *>();

        jpp::object root = Helper::loadJSONMemory( _memory, _doc );

        if( root == jpp::detail::invalid )
        {
            LOGGER_ERROR( "font effect desc invalid parse json" );

            return false;
        }

        if( root.is_type_object() == false )
        {
            LOGGER_ERROR( "font effect desc file has no 'Effects' object" );

            return false;
        }

        jpp::object j_effects;
        if( root.exist( "Effects", &j_effects ) == false )
        {
            LOGGER_ERROR( "font effect desc file has no 'Effects' object" );

            return false;
        }

        if( j_effects.is_type_object() == false )
        {
            LOGGER_ERROR( "font effect desc file 'Effects' is not object" );

            return false;
        }

        bool successful = true;

        jpp::object_iterator it = jpp::begin( j_effects );
        jpp::object_iterator it_end = jpp::end( j_effects );

        for( ; it != it_end; ++it )
        {
            jpp::object_iterator_pair pair = *it;

            ConstString name = Helper::stringizeString( pair.key );

            FontEffectDesc desc;
            if( Detail::loadDesc( pair.value, &desc ) == false )
            {
                LOGGER_ERROR( "font effect desc invalid effect '%s'"
                    , name.c_str()
                );

                successful = false;

                continue;
            }

            uint32_t sample = pair.value.get( "Sample", 0U );

            data->addEffect( name, desc, sample );
        }

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
}
