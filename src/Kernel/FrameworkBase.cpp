#include "FrameworkBase.h"

#include "Kernel/Assertion.h"

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

        MENGINE_ASSERTION_FATAL( m_initialize == false );
        MENGINE_ASSERTION_FATAL( m_running == false );

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

        MENGINE_ASSERTION_FATAL( m_initialize == true );
        MENGINE_ASSERTION_FATAL( m_running == false );

        this->_finalizeFramework();
    }
    //////////////////////////////////////////////////////////////////////////
    bool FrameworkBase::runFramework()
    {
        MENGINE_ASSERTION_FATAL( m_initialize == true );
        MENGINE_ASSERTION_FATAL( m_running == false );

        m_running = this->_runFramework();

        return m_running;
    }
    //////////////////////////////////////////////////////////////////////////
    void FrameworkBase::stopFramework()
    {
        MENGINE_ASSERTION_FATAL( m_initialize == true );
        MENGINE_ASSERTION_FATAL( m_running == true );

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
}