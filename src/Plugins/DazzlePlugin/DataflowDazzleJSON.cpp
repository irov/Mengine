#include "DataflowDazzleJSON.h"

#include "DazzleData.h"

#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/JSONHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/MemoryStreamHelper.h"

#include "evict/evict.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DataflowDazzleJSON::DataflowDazzleJSON()
        : m_service( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DataflowDazzleJSON::~DataflowDazzleJSON()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DataflowDazzleJSON::setDazzleService( const dz_service_t * _service )
    {
        m_service = _service;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowDazzleJSON::initialize()
    {
        m_factoryDazzleData = Helper::makeFactoryPool<DazzleData, 128>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DataflowDazzleJSON::finalize()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDazzleData );

        m_factoryDazzleData = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowDazzleJSON::isThreadFlow() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    DataInterfacePtr DataflowDazzleJSON::create( const DocumentInterfacePtr & _doc )
    {
        DazzleDataPtr data = m_factoryDazzleData->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( data, "invalid create dazzle json data" );

        data->setDazzleService( m_service );

        return data;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr DataflowDazzleJSON::load( const InputStreamInterfacePtr & _stream, const DocumentInterfacePtr & _doc )
    {
        MemoryInterfacePtr memory = Helper::createMemoryStream( _stream, _doc );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowDazzleJSON::flow( const DataInterfacePtr & _data, const MemoryInterfacePtr & _memory, const DataflowContext * _context,
        const DocumentInterfacePtr & _doc )
    {
        DazzleData * data = _data.getT<DazzleData *>();

        jpp::object json = Helper::loadJSONMemory( _memory, _doc );

        if( json.invalid() == true )
        {
            LOGGER_ERROR( "invalid dazzle json '%s'", _context->filePath.c_str() );

            return false;
        }

        dz_effect_t * effect = nullptr;
        dz_result_t result = dz_evict_load( m_service, &effect, json );

        if( result != DZ_SUCCESSFUL )
        {
            LOGGER_ERROR( "invalid dazzle json '%s' result '%u'", _context->filePath.c_str(), (uint32_t)result );

            return false;
        }

        data->setDazzleEffect( effect );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
