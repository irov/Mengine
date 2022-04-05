#include "DevToDebugWidget.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidget::DevToDebugWidget()
        : m_hide( false )
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
        m_hide = _hide;
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
}