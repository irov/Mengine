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
        m_type = _type;

#   define CHECK_EVENTS(O, M)\
        (O = m_type.get_attr( #M ) )

        CHECK_EVENTS( m_eventInitialize, onInitialize );
        CHECK_EVENTS( m_eventFinalize, onFinalize );
        CHECK_EVENTS( m_eventValidate, onValidate );
        CHECK_EVENTS( m_eventCheck, onCheck );
        CHECK_EVENTS( m_eventRun, onRun );
        CHECK_EVENTS( m_eventSkipable, onSkipable );
        CHECK_EVENTS( m_eventSkipNoSkiped, onSkipNoSkiped );
        CHECK_EVENTS( m_eventSkipBlock, onSkipBlock );
        CHECK_EVENTS( m_eventComplete, onComplete );
        CHECK_EVENTS( m_eventSkip, onSkip );
        CHECK_EVENTS( m_eventCancel, onCancel );
        CHECK_EVENTS( m_eventFinally, onFinally );
        CHECK_EVENTS( m_eventCheckRun, onCheckRun );
        CHECK_EVENTS( m_eventCheckSkip, onCheckSkip );

#   undef CHECK_EVENTS

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    PybindTaskPtr PybindTaskGenerator::generate()
    {
        PybindTask * task = GOAP_NEW PybindTask();

        task->setGenerator( this );

        pybind::object obj = m_type.call();
        task->setScriptObject( obj );

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
