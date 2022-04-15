#include "DevToDebugWidget.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidget::DevToDebugWidget()
        : m_hide( false )
        , m_invalidate( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidget::~DevToDebugWidget()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidget::setId( const ConstString & _id )
    {
        m_id = _id;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & DevToDebugWidget::getId() const
    {
        return m_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidget::setHide( bool _hide )
    {
        if( m_hide == _hide )
        {
            return;
        }

        m_hide = _hide;

        this->invalidate();
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugWidget::getHide() const
    {
        return m_hide;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidget::fillJson( jpp::object & _jwidget )
    {
        _jwidget.set( "id", m_id );
        _jwidget.set( "hide", m_hide );

        this->_fillTypeJson( _jwidget );

        jpp::object jdata = jpp::make_object();

        this->_fillDataJson( jdata );

        _jwidget.set( "data", jdata );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidget::invalidate()
    {
        m_invalidate = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugWidget::isInvalidate() const
    {
        if( m_invalidate == true )
        {
            m_invalidate = false;

            return true;
        }

        if( this->_checkInvalidate() == true )
        {
            m_invalidate = false;

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}