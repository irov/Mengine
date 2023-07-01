#include "AnalyticsEventConstParameterString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventConstParameterString::AnalyticsEventConstParameterString()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventConstParameterString::~AnalyticsEventConstParameterString()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEventConstParameterString::setValue( const String & _value )
    {
        m_value = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    EAnalyticsEventParameterType AnalyticsEventConstParameterString::getType() const
    {
        return EAEPT_CONSTSTRING;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterInterfacePtr AnalyticsEventConstParameterString::resolveParameter( const DocumentPtr & _doc ) const
    {
        MENGINE_UNUSED( _doc );

        return AnalyticsEventParameterInterfacePtr::from( this );
    }
    //////////////////////////////////////////////////////////////////////////
    const String & AnalyticsEventConstParameterString::resolveValue() const
    {
        return m_value;
    }
    //////////////////////////////////////////////////////////////////////////
}