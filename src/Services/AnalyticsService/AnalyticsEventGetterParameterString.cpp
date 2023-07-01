#include "AnalyticsEventGetterParameterString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventGetterParameterString::AnalyticsEventGetterParameterString()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventGetterParameterString::~AnalyticsEventGetterParameterString()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEventGetterParameterString::setAnalyticsFactory( const AnalyticsFactoryInterfacePtr & _analyticsFactory )
    {
        m_analyticsFactory = _analyticsFactory;
    }
    //////////////////////////////////////////////////////////////////////////
    const AnalyticsFactoryInterfacePtr & AnalyticsEventGetterParameterString::getAnalyticsFactory() const
    {
        return m_analyticsFactory;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEventGetterParameterString::setGetter( const LambdaAnalyticsParameterGetterString & _lambda )
    {
        m_lambda = _lambda;
    }
    //////////////////////////////////////////////////////////////////////////
    EAnalyticsEventParameterType AnalyticsEventGetterParameterString::getType() const
    {
        return EAEPT_STRING;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterInterfacePtr AnalyticsEventGetterParameterString::resolveParameter( const DocumentPtr & _doc ) const
    {
        MENGINE_UNUSED( _doc );

        return AnalyticsEventParameterInterfacePtr::from( this );
    }
    //////////////////////////////////////////////////////////////////////////
    const String & AnalyticsEventGetterParameterString::resolveValue() const
    {
        m_lambda( &m_cache );

        return m_cache;
    }
    //////////////////////////////////////////////////////////////////////////
}