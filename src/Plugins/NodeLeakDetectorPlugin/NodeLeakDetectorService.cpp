#include "NodeLeakDetectorService.h"

#include "Interface/NotificationServiceInterface.h"

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
        for( const NodeLeakDesc & desc : m_nodeLeakDescs )
        {
            if( desc.generation != _generation )
            {
                continue;
            }

            _leaks( desc.factory, desc.factorable, desc.doc.c_str() );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeLeakDetectorService::notifyDebugCreateObject( Factory * _factory, Factorable * _factorable, const Char * _doc )
    {
        NodeLeakDesc desc;
        desc.generation = m_generation;
        desc.factory = _factory;
        desc.factorable = _factorable;
        desc.doc = _doc;

        m_nodeLeakDescs.push_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeLeakDetectorService::notifyDebugDestroyObject( Factory * _factory, Factorable * _factorable )
    {
        for( VectorNodeLeakDesc::iterator
            it = m_nodeLeakDescs.begin(),
            it_end = m_nodeLeakDescs.end();
            it != it_end;
            ++it )
        {
            const NodeLeakDesc & desc = *it;

            if( desc.factory != _factory || desc.factorable != _factorable )
            {
                continue;
            }

            *it = m_nodeLeakDescs.back();
            m_nodeLeakDescs.pop_back();

            break;
        }
    }
}