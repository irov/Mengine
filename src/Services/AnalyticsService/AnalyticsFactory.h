#pragma once

#include "AnalyticsFactoryInterface.h"

#include "Interface/FactoryInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class AnalyticsFactory
        : public AnalyticsFactoryInterface
    {
    public:
        AnalyticsFactory();
        ~AnalyticsFactory() override;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        AnalyticsEventBuilderInterfacePtr makeEventBuilder( const DocumentPtr & _doc ) override;
        AnalyticsEventInterfacePtr makeEvent( EAnalyticsEventType _type, const ConstString & _name, const DocumentPtr & _doc ) override;

        AnalyticsEventParameterBooleanInterfacePtr makeEventConstParameterBoolean( bool _value, const DocumentPtr & _doc ) override;
        AnalyticsEventParameterConstStringInterfacePtr makeEventConstParameterConstString( const ConstString & _value, const DocumentPtr & _doc ) override;
        AnalyticsEventParameterStringInterfacePtr makeEventConstParameterString( const String & _value, const DocumentPtr & _doc ) override;
        AnalyticsEventParameterIntegerInterfacePtr makeEventConstParameterInteger( int64_t _value, const DocumentPtr & _doc ) override;
        AnalyticsEventParameterDoubleInterfacePtr makeEventConstParameterDouble( double _value, const DocumentPtr & _doc ) override;

        AnalyticsEventParameterBooleanInterfacePtr makeEventGetterParameterBoolean( const LambdaAnalyticsParameterGetterBoolean & _lambda, const DocumentPtr & _doc ) override;
        AnalyticsEventParameterConstStringInterfacePtr makeEventGetterParameterConstString( const LambdaAnalyticsParameterGetterConstString & _lambda, const DocumentPtr & _doc ) override;
        AnalyticsEventParameterStringInterfacePtr makeEventGetterParameterString( const LambdaAnalyticsParameterGetterString & _lambda, const DocumentPtr & _doc ) override;
        AnalyticsEventParameterIntegerInterfacePtr makeEventGetterParameterInteger( const LambdaAnalyticsParameterGetterInteger & _lambda, const DocumentPtr & _doc ) override;
        AnalyticsEventParameterDoubleInterfacePtr makeEventGetterParameterDouble( const LambdaAnalyticsParameterGetterDouble & _lambda, const DocumentPtr & _doc ) override;

    public:
        AnalyticsContextInterfacePtr makeContext( const DocumentPtr & _doc ) override;

    protected:
        FactoryInterfacePtr m_factoryAnalyticsEventBuilder;
        FactoryInterfacePtr m_factoryAnalyticsEvent;

        FactoryInterfacePtr m_factoryAnalyticsEventConstParameterBoolean;
        FactoryInterfacePtr m_factoryAnalyticsEventConstParameterConstString;
        FactoryInterfacePtr m_factoryAnalyticsEventConstParameterString;
        FactoryInterfacePtr m_factoryAnalyticsEventConstParameterInteger;
        FactoryInterfacePtr m_factoryAnalyticsEventConstParameterDouble;

        FactoryInterfacePtr m_factoryAnalyticsEventGetterParameterBoolean;
        FactoryInterfacePtr m_factoryAnalyticsEventGetterParameterConstString;
        FactoryInterfacePtr m_factoryAnalyticsEventGetterParameterString;
        FactoryInterfacePtr m_factoryAnalyticsEventGetterParameterInteger;
        FactoryInterfacePtr m_factoryAnalyticsEventGetterParameterDouble;

        FactoryInterfacePtr m_factoryAnalyticsContext;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsFactory, AnalyticsFactoryInterface> AnalyticsFactoryPtr;
    //////////////////////////////////////////////////////////////////////////
}
