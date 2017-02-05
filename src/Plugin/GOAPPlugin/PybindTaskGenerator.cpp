#   include "PybindTaskGenerator.h"
#   include "PybindTask.h"

#   include "Kernel/Eventable.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    PybindTaskGenerator::PybindTaskGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PybindTaskGenerator::~PybindTaskGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PybindTaskGenerator::initialize( const pybind::object & _type )
    {
#   define CHECK_EVENTS(O, M, E)\
        m_events |= (O = _type.get_attr( #M )).is_valid() == true ? GOAP::##E : 0

        CHECK_EVENTS( m_eventInitialize, onInitialize, TASK_EVENT_INITIALIZE );
        CHECK_EVENTS( m_eventFinalize, onFinalize, TASK_EVENT_FINALIZE );
        CHECK_EVENTS( m_eventValidate, onValidate, TASK_EVENT_VALIDATE );
        CHECK_EVENTS( m_eventCheck, onCheck, TASK_EVENT_CHECK );
        CHECK_EVENTS( m_eventRun, onRun, TASK_EVENT_RUN );
        CHECK_EVENTS( m_eventSkipable, onSkipable, TASK_EVENT_SKIPABLE );
        CHECK_EVENTS( m_eventSkipNoSkiped, onSkipNoSkiped, TASK_EVENT_SKIP_NO_SKIPED );
        CHECK_EVENTS( m_eventSkipBlock, onSkipBlock, TASK_EVENT_SKIP_BLOCK );
        CHECK_EVENTS( m_eventComplete, onComplete, TASK_EVENT_COMPLETE );
        CHECK_EVENTS( m_eventSkip, onSkip, TASK_EVENT_SKIP );
        CHECK_EVENTS( m_eventCancel, onCancel, TASK_EVENT_CANCEL );
        CHECK_EVENTS( m_eventFinally, onFinally, TASK_EVENT_FINALLY );
        CHECK_EVENTS( m_eventCheckRun, onCheckRun, TASK_EVENT_CHECK_RUN );
        CHECK_EVENTS( m_eventCheckSkip, onCheckSkip, TASK_EVENT_CHECK_SKIP );

#   undef CHECK_EVENTS

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    PybindTaskPtr PybindTaskGenerator::generate( const pybind::object & _obj )
    {
        PybindTask * task = new PybindTask();

        task->setEvents( m_events );
        task->setGenerator( this );
        task->setScriptObject( _obj );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PybindTaskGenerator::callEventInitialize( const pybind::object & _obj )
    {
        return m_eventInitialize.call( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    void PybindTaskGenerator::callEventFinalize( const pybind::object & _obj )
    {
        m_eventFinalize.call( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PybindTaskGenerator::callEventValidate( const pybind::object & _obj )
    {
        return m_eventValidate.call( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PybindTaskGenerator::callEventCheck( const pybind::object & _obj )
    {
        return m_eventCheck.call( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PybindTaskGenerator::callEventRun( const pybind::object & _obj )
    {
        return m_eventRun.call( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PybindTaskGenerator::callEventSkipable( const pybind::object & _obj )
    {
        return m_eventSkipable.call( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    void PybindTaskGenerator::callEventSkipNoSkiped( const pybind::object & _obj )
    {
        m_eventSkipNoSkiped.call( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PybindTaskGenerator::callEventSkipBlock( const pybind::object & _obj )
    {
        return m_eventSkipBlock.call( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    void PybindTaskGenerator::callEventComplete( const pybind::object & _obj )
    {
        m_eventComplete.call( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    void PybindTaskGenerator::callEventSkip( const pybind::object & _obj )
    {
        m_eventSkip.call( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    void PybindTaskGenerator::callEventCancel( const pybind::object & _obj )
    {
        m_eventCancel.call( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    void PybindTaskGenerator::callEventFinally( const pybind::object & _obj )
    {
        m_eventFinally.call( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PybindTaskGenerator::callEventCheckRun( const pybind::object & _obj )
    {
        return m_eventCheckRun.call( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PybindTaskGenerator::callEventCheckSkip( const pybind::object & _obj )
    {
        return m_eventCheckSkip.call( _obj );
    }
}