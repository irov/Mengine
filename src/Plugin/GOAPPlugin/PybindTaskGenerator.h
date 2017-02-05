#   pragma once

#   include "Factory/FactorablePtr.h"

#   include "GOAP/GOAP.h"

#   include "pybind/bindable.hpp"

#   include "pybind/object.hpp"
#   include "pybind/tuple.hpp"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<class PybindTask> PybindTaskPtr;
    //////////////////////////////////////////////////////////////////////////
    class PybindTaskGenerator
        : public FactorablePtr
    {
    public:
        PybindTaskGenerator();
        ~PybindTaskGenerator();

    public:
        bool initialize( const pybind::object & _type );

    public:
        PybindTaskPtr generate( const pybind::object & _obj );

    public:
        bool callEventInitialize( const pybind::object & _obj );
        void callEventFinalize( const pybind::object & _obj );
        bool callEventValidate( const pybind::object & _obj );
        bool callEventCheck( const pybind::object & _obj );
        bool callEventRun( const pybind::object & _obj );
        bool callEventSkipable( const pybind::object & _obj );
        void callEventSkipNoSkiped( const pybind::object & _obj );
        bool callEventSkipBlock( const pybind::object & _obj );
        void callEventComplete( const pybind::object & _obj );
        void callEventSkip( const pybind::object & _obj );
        void callEventCancel( const pybind::object & _obj );
        void callEventFinally( const pybind::object & _obj );
        bool callEventCheckRun( const pybind::object & _obj );
        bool callEventCheckSkip( const pybind::object & _obj );

    protected:
        uint32_t m_events;

        pybind::object m_eventInitialize;
        pybind::object m_eventFinalize;

        pybind::object m_eventValidate;
        pybind::object m_eventCheck;
        pybind::object m_eventRun;
        pybind::object m_eventSkipable;
        pybind::object m_eventSkipNoSkiped;
        pybind::object m_eventSkipBlock;
        pybind::object m_eventComplete;
        pybind::object m_eventSkip;
        pybind::object m_eventCancel;
        pybind::object m_eventFinally;
        pybind::object m_eventCheckRun;
        pybind::object m_eventCheckSkip;
    };
}