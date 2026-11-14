#include "MCPAdvanceFramesOperation.h"

#include "Interface/TimelineServiceInterface.h"

#include "../../Contexts/MCPRuntimeContext.h"

#include "Kernel/NotificationHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPAdvanceFramesOperation::MCPAdvanceFramesOperation( MCPRuntimeContext * _context, uint32_t _frames, float _timeFactor )
        : m_context( _context )
        , m_frames( _frames )
        , m_framesAdvanced( 0 )
        , m_timeFactor( _timeFactor )
        , m_previousTimeFactorBase( 1.f )
        , m_previousTimeFactorCount( 0 )
        , m_advancing( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MCPAdvanceFramesOperation::~MCPAdvanceFramesOperation()
    {
        this->end_();
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPAdvanceFramesOperation::begin()
    {
        m_advancing = true;

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_APPLICATION_BEGIN_UPDATE, &MCPAdvanceFramesOperation::notifyApplicationBeginUpdate_, MENGINE_DOCUMENT_FACTORABLE );

        m_previousTimeFactorBase = TIMELINE_SERVICE()
            ->getTimeFactorBase();

        m_previousTimeFactorCount = TIMELINE_SERVICE()
            ->getTimeFactorCount();

        TIMELINE_SERVICE()
            ->setTimeFactorCount( 0 );

        TIMELINE_SERVICE()
            ->setTimeFactorBase( m_timeFactor );

        m_context->beginAdvance();
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPAdvanceFramesOperation::update( MCPCommandResponse * const _response )
    {
        if( m_framesAdvanced < m_frames )
        {
            return EMCPCommandStatus::DEFERRED;
        }

        this->end_();

        bool paused = m_context->isPaused();

        _response->result.set( "action", "advance_frames" );
        _response->result.set( "frames", m_framesAdvanced );
        _response->result.set( "runtimePaused", paused );
        _response->result.set( "completed", true );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPAdvanceFramesOperation::cancel()
    {
        this->end_();
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPAdvanceFramesOperation::end_()
    {
        if( m_advancing == false )
        {
            return;
        }

        m_advancing = false;

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_BEGIN_UPDATE );

        TIMELINE_SERVICE()
            ->setTimeFactorBase( m_previousTimeFactorBase );

        TIMELINE_SERVICE()
            ->setTimeFactorCount( m_previousTimeFactorCount );

        m_context->endAdvance();
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPAdvanceFramesOperation::notifyApplicationBeginUpdate_()
    {
        ++m_framesAdvanced;
    }
    //////////////////////////////////////////////////////////////////////////
}
