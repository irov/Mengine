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
        AnalyticsEventInterfacePtr makeAnalyticsEvent( EAnalyticsEventType _type, const ConstString & _name ) override;

        AnalyticsEventParameterBooleanInterfacePtr makeAnalyticsEventConstParameterBoolean( bool _value ) override;
        AnalyticsEventParameterConstStringInterfacePtr makeAnalyticsEventConstParameterConstString( const ConstString & _value ) override;
        AnalyticsEventParameterStringInterfacePtr makeAnalyticsEventConstParameterString( const String & _value ) override;
        AnalyticsEventParameterIntegerInterfacePtr makeAnalyticsEventConstParameterInteger( int64_t _value ) override;
        AnalyticsEventParameterDoubleInterfacePtr makeAnalyticsEventConstParameterDouble( double _value ) override;

        AnalyticsEventParameterBooleanInterfacePtr makeAnalyticsEventGetterParameterBoolean( const LambdaAnalyticsParameterGetterBoolean & _lambda ) override;
        AnalyticsEventParameterConstStringInterfacePtr makeAnalyticsEventGetterParameterConstString( const LambdaAnalyticsParameterGetterConstString & _lambda ) override;
        AnalyticsEventParameterStringInterfacePtr makeAnalyticsEventGetterParameterString( const LambdaAnalyticsParameterGetterString & _lambda ) override;
        AnalyticsEventParameterIntegerInterfacePtr makeAnalyticsEventGetterParameterInteger( const LambdaAnalyticsParameterGetterInteger & _lambda ) override;
        AnalyticsEventParameterDoubleInterfacePtr makeAnalyticsEventGetterParameterDouble( const LambdaAnalyticsParameterGetterDouble & _lambda ) override;

    public:
        AnalyticsContextInterfacePtr makeAnalyticsContext() override;

    protected:
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
