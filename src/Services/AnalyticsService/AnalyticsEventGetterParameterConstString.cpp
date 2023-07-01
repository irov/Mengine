#include "AnalyticsEventGetterParameterConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventGetterParameterConstString::AnalyticsEventGetterParameterConstString()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventGetterParameterConstString::~AnalyticsEventGetterParameterConstString()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEventGetterParameterConstString::setAnalyticsFactory( const AnalyticsFactoryInterfacePtr & _analyticsFactory )
    {
        m_analyticsFactory = _analyticsFactory;
    }
    //////////////////////////////////////////////////////////////////////////
    const AnalyticsFactoryInterfacePtr & AnalyticsEventGetterParameterConstString::getAnalyticsFactory() const
    {
        return m_analyticsFactory;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEventGetterParameterConstString::setGetter( const LambdaAnalyticsParameterGetterConstString & _lambda )
    {
        m_lambda = _lambda;
    }
    //////////////////////////////////////////////////////////////////////////
    EAnalyticsEventParameterType AnalyticsEventGetterParameterConstString::getType() const
    {
        return EAEPT_CONSTSTRING;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterInterfacePtr AnalyticsEventGetterParameterConstString::resolveParameter( const DocumentPtr & _doc ) const
    {
        MENGINE_UNUSED( _doc );

        return AnalyticsEventParameterInterfacePtr::from( this );
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & AnalyticsEventGetterParameterConstString::resolveValue() const
    {
        m_cache = m_lambda();

        return m_cache;
    }
    //////////////////////////////////////////////////////////////////////////
}