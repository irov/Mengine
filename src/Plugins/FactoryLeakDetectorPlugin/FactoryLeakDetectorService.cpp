#include "FactoryLeakDetectorService.h"

#include "Interface/NotificationServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Kernel/CRC32.h"
#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FactoryLeakDetectorService, Mengine::FactoryLeakDetectorService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FactoryLeakDetectorService::FactoryLeakDetectorService()
        : m_generation( 0 )
        , m_fatalAssertion( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FactoryLeakDetectorService::~FactoryLeakDetectorService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FactoryLeakDetectorService::_initializeService()
    {
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_DEBUG_FACTORY_CREATE_OBJECT, this, &FactoryLeakDetectorService::notifyDebugCreateObject );
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_DEBUG_FACTORY_DESTROY_OBJECT, this, &FactoryLeakDetectorService::notifyDebugDestroyObject );

        m_fatalAssertion = CONFIG_VALUE( "FactoryLeakDetector", "Fatal", false );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryLeakDetectorService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_DEBUG_FACTORY_CREATE_OBJECT );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_DEBUG_FACTORY_DESTROY_OBJECT );

        LOGGER_STATISTIC( "==================================================================" );
        LOGGER_STATISTIC( "==================================================================" );
        LOGGER_STATISTIC( "**************************MEMORY LEAK*****************************" );
        LOGGER_STATISTIC( "==================================================================" );
        LOGGER_STATISTIC( "==================================================================" );

        uint32_t leakcount = 0;

        this->getNodeLeak( 0, [&leakcount]( const Factory * _factory, const Factorable *, const Char * _doc )
        {
            MENGINE_UNUSED( _factory );
            MENGINE_UNUSED( _doc );

            LOGGER_STATISTIC( "leak '%s' doc: %s"
                , _factory->getName()
                , _doc
            );

            ++leakcount;
        } );

        LOGGER_STATISTIC( "==================================================================" );
        LOGGER_STATISTIC( "==================================================================" );

        if( m_fatalAssertion == true )
        {
            MENGINE_ASSERTION_FATAL( leakcount == 0, "detect factory leak [%d]"
                , leakcount
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryLeakDetectorService::increfGeneration()
    {
        ++m_generation;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t FactoryLeakDetectorService::getGeneration() const
    {
        return m_generation;
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryLeakDetectorService::getNodeLeak( uint32_t _generation, const LambdaNodeLeaks & _leaks ) const
    {
        for( uint32_t index = 0; index != MENGINE_NODELEAKDETECTOR_HASHSIZE; ++index )
        {
            const VectorNodeLeakDesc & nodeLeakDescs = m_nodeLeakDescs[index];

            for( const NodeLeakDesc & desc : nodeLeakDescs )
            {
                if( desc.generation != _generation )
                {
                    continue;
                }

                _leaks( desc.factory, desc.factorable, desc.doc.c_str() );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryLeakDetectorService::notifyDebugCreateObject( Factory * _factory, Factorable * _factorable, const Char * _doc )
    {
        uint32_t hash = Helper::make_crc32_mod_pod( _factorable, MENGINE_NODELEAKDETECTOR_HASHSIZE );

        NodeLeakDesc desc;
        desc.generation = m_generation;
        desc.factory = _factory;
        desc.factorable = _factorable;
        desc.doc = _doc;

        m_nodeLeakDescs[hash].push_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryLeakDetectorService::notifyDebugDestroyObject( Factory * _factory, Factorable * _factorable )
    {
        uint32_t hash = Helper::make_crc32_mod_pod( _factorable, MENGINE_NODELEAKDETECTOR_HASHSIZE );

        VectorNodeLeakDesc & nodeLeakDescs = m_nodeLeakDescs[hash];

        for( VectorNodeLeakDesc::iterator
            it = nodeLeakDescs.begin(),
            it_end = nodeLeakDescs.end();
            it != it_end;
            ++it )
        {
            const NodeLeakDesc & desc = *it;

            if( desc.factory != _factory || desc.factorable != _factorable )
            {
                continue;
            }

            *it = nodeLeakDescs.back();
            nodeLeakDescs.pop_back();

            break;
        }
    }
}