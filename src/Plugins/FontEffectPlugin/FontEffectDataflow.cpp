#include "FontEffectDataflow.h"

#include "FontEffectData.h"
#include "FontEffectSerialization.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/JSONHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
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
            if( Helper::loadFontEffectDesc( pair.value, &desc ) == false )
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
