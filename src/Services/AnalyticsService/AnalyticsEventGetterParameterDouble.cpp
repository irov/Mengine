#include "AnalyticsEventGetterParameterDouble.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventGetterParameterDouble::AnalyticsEventGetterParameterDouble()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventGetterParameterDouble::~AnalyticsEventGetterParameterDouble()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEventGetterParameterDouble::setAnalyticsFactory( const AnalyticsFactoryInterfacePtr & _analyticsFactory )
    {
        m_analyticsFactory = _analyticsFactory;
    }
    //////////////////////////////////////////////////////////////////////////
    const AnalyticsFactoryInterfacePtr & AnalyticsEventGetterParameterDouble::getAnalyticsFactory() const
    {
        return m_analyticsFactory;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEventGetterParameterDouble::setGetter( const LambdaAnalyticsParameterGetterDouble & _lambda )
    {
        m_lambda = _lambda;
    }
    //////////////////////////////////////////////////////////////////////////
    EAnalyticsEventParameterType AnalyticsEventGetterParameterDouble::getType() const
    {
        return EAEPT_DOUBLE;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterInterfacePtr AnalyticsEventGetterParameterDouble::resolveParameter( const DocumentPtr & _doc ) const
    {
        MENGINE_UNUSED( _doc );

        return AnalyticsEventParameterInterfacePtr::from( this );
    }
    //////////////////////////////////////////////////////////////////////////
    double AnalyticsEventGetterParameterDouble::resolveValue() const
    {
        double value = m_lambda();

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
}