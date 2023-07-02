#include "AnalyticsEventConstParameterConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventConstParameterConstString::AnalyticsEventConstParameterConstString()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventConstParameterConstString::~AnalyticsEventConstParameterConstString()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEventConstParameterConstString::setValue( const ConstString & _value )
    {
        m_value = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    EAnalyticsEventParameterType AnalyticsEventConstParameterConstString::getType() const
    {
        return EAEPT_CONSTSTRING;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterInterfacePtr AnalyticsEventConstParameterConstString::resolveParameter( const DocumentInterfacePtr & _doc ) const
    {
        MENGINE_UNUSED( _doc );

        return AnalyticsEventParameterInterfacePtr::from( this );
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & AnalyticsEventConstParameterConstString::resolveValue() const
    {
        return m_value;
    }
    //////////////////////////////////////////////////////////////////////////
}