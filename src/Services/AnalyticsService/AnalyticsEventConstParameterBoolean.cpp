#include "AnalyticsEventConstParameterBoolean.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventConstParameterBoolean::AnalyticsEventConstParameterBoolean()
        : m_value( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventConstParameterBoolean::~AnalyticsEventConstParameterBoolean()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEventConstParameterBoolean::setValue( bool _value )
    {
        m_value = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    EAnalyticsEventParameterType AnalyticsEventConstParameterBoolean::getType() const
    {
        return EAEPT_BOOLEAN;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterInterfacePtr AnalyticsEventConstParameterBoolean::resolveParameter( const DocumentInterfacePtr & _doc ) const
    {
        MENGINE_UNUSED( _doc );

        return AnalyticsEventParameterInterfacePtr::from( this );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AnalyticsEventConstParameterBoolean::resolveValue() const
    {
        return m_value;
    }
    //////////////////////////////////////////////////////////////////////////
}