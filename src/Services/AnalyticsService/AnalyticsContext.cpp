#include "AnalyticsContext.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AnalyticsContext::AnalyticsContext()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsContext::~AnalyticsContext()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsContext::setAnalyticsFactory( const AnalyticsFactoryInterfacePtr & _analyticsFactory )
    {
        m_analyticsFactory = _analyticsFactory;
    }
    //////////////////////////////////////////////////////////////////////////
    const AnalyticsFactoryInterfacePtr & AnalyticsContext::getAnalyticsFactory() const
    {
        return m_analyticsFactory;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsContext::addParameter( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )
    {
        AnalyticsEventParameterDesc desc;
        desc.name = _name;
        desc.parameter = _parameter;

        m_parameters.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AnalyticsContext::getCountParameters() const
    {
        VectorAnalyticsEventParameter::size_type parameters_count = m_parameters.size();

       return (uint32_t)parameters_count;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsContext::foreachParameters( const LambdaForeachParameters & _lambda ) const
    {
        for( const AnalyticsEventParameterDesc & desc : m_parameters )
        {
            const ConstString & name = desc.name;
            const AnalyticsEventParameterInterfacePtr & parameter = desc.parameter;

            _lambda( name, parameter );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsContext::addParameterBoolean( const ConstString & _name, bool _value, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventParameterInterfacePtr parameter = m_analyticsFactory->makeEventConstParameterBoolean( _value, _doc );

        this->addParameter( _name, parameter );
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsContext::addParameterString( const ConstString & _name, const String & _value, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventParameterInterfacePtr parameter = m_analyticsFactory->makeEventConstParameterString( _value, _doc );

        this->addParameter( _name, parameter );
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsContext::addParameterConstString( const ConstString & _name, const ConstString & _value, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventParameterInterfacePtr parameter = m_analyticsFactory->makeEventConstParameterConstString( _value, _doc );

        this->addParameter( _name, parameter );
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsContext::addParameterInteger( const ConstString & _name, int64_t _value, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventParameterInterfacePtr parameter = m_analyticsFactory->makeEventConstParameterInteger( _value, _doc );

        this->addParameter( _name, parameter );
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsContext::addParameterDouble( const ConstString & _name, double _value, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventParameterInterfacePtr parameter = m_analyticsFactory->makeEventConstParameterDouble( _value, _doc );

        this->addParameter( _name, parameter );
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsContext::addParameterGetterBoolean( const ConstString & _name, const LambdaAnalyticsParameterGetterBoolean & _lambda, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventParameterInterfacePtr parameter = m_analyticsFactory->makeEventGetterParameterBoolean( _lambda, _doc );
        
        this->addParameter( _name, parameter );
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsContext::addParameterGetterString( const ConstString & _name, const LambdaAnalyticsParameterGetterString & _lambda, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventParameterInterfacePtr parameter = m_analyticsFactory->makeEventGetterParameterString( _lambda, _doc );

        this->addParameter( _name, parameter );
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsContext::addParameterGetterConstString( const ConstString & _name, const LambdaAnalyticsParameterGetterConstString & _lambda, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventParameterInterfacePtr parameter = m_analyticsFactory->makeEventGetterParameterConstString( _lambda, _doc );

        this->addParameter( _name, parameter );
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsContext::addParameterGetterInteger( const ConstString & _name, const LambdaAnalyticsParameterGetterInteger & _lambda, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventParameterInterfacePtr parameter = m_analyticsFactory->makeEventGetterParameterInteger( _lambda, _doc );

        this->addParameter( _name, parameter );
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsContext::addParameterGetterDouble( const ConstString & _name, const LambdaAnalyticsParameterGetterDouble & _lambda, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventParameterInterfacePtr parameter = m_analyticsFactory->makeEventGetterParameterDouble( _lambda, _doc );

        this->addParameter( _name, parameter );
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsContextInterfacePtr AnalyticsContext::resolveContext( const DocumentInterfacePtr & _doc ) const
    {
        AnalyticsContextInterfacePtr context = m_analyticsFactory->makeContext( _doc );

        for( const AnalyticsEventParameterDesc & desc : m_parameters )
        {
            const ConstString & name = desc.name;
            const AnalyticsEventParameterInterfacePtr & parameter = desc.parameter;

            AnalyticsEventParameterInterfacePtr resolve_parameter = parameter->resolveParameter( _doc );

            context->addParameter( name, resolve_parameter );
        }

        return context;
    }
    //////////////////////////////////////////////////////////////////////////
}