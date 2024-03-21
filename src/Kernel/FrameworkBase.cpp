#include "FrameworkBase.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionObservable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FrameworkBase::FrameworkBase()
        : m_available( false )
        , m_initialize( false )
        , m_running( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FrameworkBase::~FrameworkBase()
    {
        MENGINE_ASSERTION_OBSERVABLE( this, "framework '%s'"
            , this->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void FrameworkBase::setName( const ConstString & _name )
    {
        m_name = _name;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & FrameworkBase::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FrameworkBase::isAvailableFramework() const
    {
        return m_available;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FrameworkBase::_availableFramework() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FrameworkBase::initializeFramework()
    {
        m_available = this->_availableFramework();

        if( m_available == false )
        {
            return false;
        }

        MENGINE_ASSERTION_FATAL( m_initialize == false, "framework '%s' already initialized"
            , this->getName().c_str()
        );

        MENGINE_ASSERTION_FATAL( m_running == false, "framework '%s' already running"
            , this->getName().c_str()
        );

        m_initialize = this->_initializeFramework();

        return m_initialize;
    }
    //////////////////////////////////////////////////////////////////////////
    void FrameworkBase::finalizeFramework()
    {
        if( m_available == false )
        {
            return;
        }

        MENGINE_ASSERTION_FATAL( m_initialize == true, "framework '%s' not initialized"
            , this->getName().c_str()
        );

        MENGINE_ASSERTION_FATAL( m_running == false, "framework '%s' is still running"
            , this->getName().c_str()
        );

        this->_finalizeFramework();
    }
    //////////////////////////////////////////////////////////////////////////
    bool FrameworkBase::runFramework()
    {
        MENGINE_ASSERTION_FATAL( m_initialize == true, "framework '%s' not initialized"
            , this->getName().c_str()
        );

        MENGINE_ASSERTION_FATAL( m_running == false, "framework '%s' already running"
            , this->getName().c_str()
        );

        m_running = this->_runFramework();

        return m_running;
    }
    //////////////////////////////////////////////////////////////////////////
    void FrameworkBase::stopFramework()
    {
        MENGINE_ASSERTION_FATAL( m_initialize == true, "framework '%s' not initialized"
            , this->getName().c_str()
        );

        MENGINE_ASSERTION_FATAL( m_running == true, "framework '%s' not running"
            , this->getName().c_str()
        );

        this->_stopFramework();

        m_running = false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FrameworkBase::_initializeFramework()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FrameworkBase::_finalizeFramework()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool FrameworkBase::_runFramework()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FrameworkBase::_stopFramework()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}