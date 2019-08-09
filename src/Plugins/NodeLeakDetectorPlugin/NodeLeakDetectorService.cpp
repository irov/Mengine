#include "NodeLeakDetectorService.h"

#include "Interface/NotificationServiceInterface.h"

#include "Kernel/CRC32.h"
#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( NodeLeakDetectorService, Mengine::NodeLeakDetectorService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    NodeLeakDetectorService::NodeLeakDetectorService()
        : m_generation( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NodeLeakDetectorService::~NodeLeakDetectorService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeLeakDetectorService::_initializeService()
    {
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_DEBUG_CREATE_OBJECT, this, &NodeLeakDetectorService::notifyDebugCreateObject );
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_DEBUG_DESTROY_OBJECT, this, &NodeLeakDetectorService::notifyDebugDestroyObject );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeLeakDetectorService::_finalizeService()
    {
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_DEBUG_CREATE_OBJECT, this, &NodeLeakDetectorService::notifyDebugCreateObject );
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_DEBUG_DESTROY_OBJECT, this, &NodeLeakDetectorService::notifyDebugDestroyObject );

        LOGGER_STATISTIC( "==================================================================" );
        LOGGER_STATISTIC( "==================================================================" );
        LOGGER_STATISTIC( "**************************MEMORY LEAK*****************************" );
        LOGGER_STATISTIC( "==================================================================" );
        LOGGER_STATISTIC( "==================================================================" );

        this->getNodeLeak( 0, []( const Factory * _factory, const Factorable *, const Char * _doc )
        {
            LOGGER_STATISTIC( "leak '%s' doc: %s"
                , _factory->getName()
                , _doc
            );
        } );

        LOGGER_STATISTIC( "==================================================================" );
        LOGGER_STATISTIC( "==================================================================" );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeLeakDetectorService::increfGeneration()
    {
        ++m_generation;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t NodeLeakDetectorService::getGeneration() const
    {
        return m_generation;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeLeakDetectorService::getNodeLeak( uint32_t _generation, const LambdaNodeLeaks & _leaks ) const
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
    void NodeLeakDetectorService::notifyDebugCreateObject( Factory * _factory, Factorable * _factorable, const Char * _doc )
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
    void NodeLeakDetectorService::notifyDebugDestroyObject( Factory * _factory, Factorable * _factorable )
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