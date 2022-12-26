#include "AnalyticsEventGetterParameterInteger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventGetterParameterInteger::AnalyticsEventGetterParameterInteger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventGetterParameterInteger::~AnalyticsEventGetterParameterInteger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEventGetterParameterInteger::setAnalyticsFactory( const AnalyticsFactoryInterfacePtr & _analyticsFactory )
    {
        m_analyticsFactory = _analyticsFactory;
    }
    //////////////////////////////////////////////////////////////////////////
    const AnalyticsFactoryInterfacePtr & AnalyticsEventGetterParameterInteger::getAnalyticsFactory() const
    {
        return m_analyticsFactory;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEventGetterParameterInteger::setGetter( const LambdaAnalyticsParameterGetterInteger & _lambda )
    {
        m_lambda = _lambda;
    }
    //////////////////////////////////////////////////////////////////////////
    EAnalyticsEventParameterType AnalyticsEventGetterParameterInteger::getType() const
    {
        return EAEPT_INTEGER;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterInterfacePtr AnalyticsEventGetterParameterInteger::resolveParameter() const
    {
        return AnalyticsEventParameterInterfacePtr::from( this );
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t AnalyticsEventGetterParameterInteger::resolveValue() const
    {
        int64_t value = m_lambda();

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
}