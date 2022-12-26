#include "AnalyticsEventConstParameterDouble.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventConstParameterDouble::AnalyticsEventConstParameterDouble()
        : m_value( 0.0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventConstParameterDouble::~AnalyticsEventConstParameterDouble()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEventConstParameterDouble::setValue( double _value )
    {
        m_value = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    EAnalyticsEventParameterType AnalyticsEventConstParameterDouble::getType() const
    {
        return EAEPT_DOUBLE;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterInterfacePtr AnalyticsEventConstParameterDouble::resolveParameter() const
    {
        return AnalyticsEventParameterInterfacePtr::from( this );
    }
    //////////////////////////////////////////////////////////////////////////
    double AnalyticsEventConstParameterDouble::resolveValue() const
    {
        return m_value;
    }
    //////////////////////////////////////////////////////////////////////////
}