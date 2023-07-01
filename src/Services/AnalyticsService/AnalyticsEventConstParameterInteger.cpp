#include "AnalyticsEventConstParameterInteger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventConstParameterInteger::AnalyticsEventConstParameterInteger()
        :m_value( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventConstParameterInteger::~AnalyticsEventConstParameterInteger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEventConstParameterInteger::setValue( int64_t _value )
    {
        m_value = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    EAnalyticsEventParameterType AnalyticsEventConstParameterInteger::getType() const
    {
        return EAEPT_INTEGER;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterInterfacePtr AnalyticsEventConstParameterInteger::resolveParameter( const DocumentPtr & _doc ) const
    {
        MENGINE_UNUSED( _doc );

        return AnalyticsEventParameterInterfacePtr::from( this );
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t AnalyticsEventConstParameterInteger::resolveValue() const
    {
        return m_value;
    }
    //////////////////////////////////////////////////////////////////////////
}