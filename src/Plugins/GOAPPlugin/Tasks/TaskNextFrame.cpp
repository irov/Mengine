#include "TaskNextFrame.h"

#include "Interface/TimepipeServiceInterface.h"

#include "Kernel/Logger.h"

#include "GOAP/NodeInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskNextFrame::TaskNextFrame( GOAP::Allocator * _allocator, const DocumentInterfacePtr & _doc )
        : GOAP::TaskInterface( _allocator )
#if defined(MENGINE_DOCUMENT_ENABLE)
        , m_doc( _doc )
#endif
        , m_id( INVALID_UNIQUE_ID )
    {
        MENGINE_UNUSED( _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    TaskNextFrame::~TaskNextFrame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskNextFrame::_onRun( GOAP::NodeInterface * _node )
    {
        GOAP::NodeInterfacePtr node = GOAP::NodeInterfacePtr::from( _node );

        UniqueId id = TIMEPIPE_SERVICE()
            ->addTimebeginCallback( [this, node]( const UpdateContext * )
            {
                m_id = INVALID_UNIQUE_ID;

                node->complete();
            }, MENGINE_DOCUMENT_VALUE( m_doc, nullptr ) );

        if( id == INVALID_UNIQUE_ID )
        {
            LOGGER_ERROR( "error add next frame callback" );

            return true;
        }

        m_id = id;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskNextFrame::_onFinally()
    {
        if( m_id != INVALID_UNIQUE_ID )
        {
            TIMEPIPE_SERVICE()
                ->removeTimebeginCallback( m_id );

            m_id = INVALID_UNIQUE_ID;
        }

#if defined(MENGINE_DOCUMENT_ENABLE)
        m_doc = nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}
