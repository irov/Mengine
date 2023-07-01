#include "AnalyticsEventGetterParameterBoolean.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventGetterParameterBoolean::AnalyticsEventGetterParameterBoolean()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventGetterParameterBoolean::~AnalyticsEventGetterParameterBoolean()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEventGetterParameterBoolean::setAnalyticsFactory( const AnalyticsFactoryInterfacePtr & _analyticsFactory )
    {
        m_analyticsFactory = _analyticsFactory;
    }
    //////////////////////////////////////////////////////////////////////////
    const AnalyticsFactoryInterfacePtr & AnalyticsEventGetterParameterBoolean::getAnalyticsFactory() const
    {
        return m_analyticsFactory;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEventGetterParameterBoolean::setGetter( const LambdaAnalyticsParameterGetterBoolean & _lambda )
    {
        m_lambda = _lambda;
    }
    //////////////////////////////////////////////////////////////////////////
    EAnalyticsEventParameterType AnalyticsEventGetterParameterBoolean::getType() const
    {
        return EAEPT_BOOLEAN;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventParameterInterfacePtr AnalyticsEventGetterParameterBoolean::resolveParameter( const DocumentPtr & _doc ) const
    {
        MENGINE_UNUSED( _doc );

        return AnalyticsEventParameterInterfacePtr::from( this );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AnalyticsEventGetterParameterBoolean::resolveValue() const
    {
        bool value = m_lambda();

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
}