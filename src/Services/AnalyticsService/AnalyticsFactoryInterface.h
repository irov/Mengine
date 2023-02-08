#pragma once

#include "Interface/ServantInterface.h"

#include "Interface/AnalyticsServiceInterface.h"

namespace Mengine
{
    class AnalyticsFactoryInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual AnalyticsEventInterfacePtr makeAnalyticsEvent( EAnalyticsEventType _type, const ConstString & _name ) = 0;

        virtual AnalyticsEventParameterBooleanInterfacePtr makeAnalyticsEventConstParameterBoolean( bool _value ) = 0;
        virtual AnalyticsEventParameterConstStringInterfacePtr makeAnalyticsEventConstParameterConstString( const ConstString & _value ) = 0;
        virtual AnalyticsEventParameterStringInterfacePtr makeAnalyticsEventConstParameterString( const String & _value ) = 0;
        virtual AnalyticsEventParameterIntegerInterfacePtr makeAnalyticsEventConstParameterInteger( int64_t _value ) = 0;
        virtual AnalyticsEventParameterDoubleInterfacePtr makeAnalyticsEventConstParameterDouble( double _value ) = 0;

        virtual AnalyticsEventParameterBooleanInterfacePtr makeAnalyticsEventGetterParameterBoolean( const LambdaAnalyticsParameterGetterBoolean & _lambda ) = 0;
        virtual AnalyticsEventParameterConstStringInterfacePtr makeAnalyticsEventGetterParameterConstString( const LambdaAnalyticsParameterGetterConstString & _lambda ) = 0;
        virtual AnalyticsEventParameterStringInterfacePtr makeAnalyticsEventGetterParameterString( const LambdaAnalyticsParameterGetterString & _lambda ) = 0;
        virtual AnalyticsEventParameterIntegerInterfacePtr makeAnalyticsEventGetterParameterInteger( const LambdaAnalyticsParameterGetterInteger & _lambda ) = 0;
        virtual AnalyticsEventParameterDoubleInterfacePtr makeAnalyticsEventGetterParameterDouble( const LambdaAnalyticsParameterGetterDouble & _lambda ) = 0;

    public:
        virtual AnalyticsContextInterfacePtr makeAnalyticsContext() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsFactoryInterface> AnalyticsFactoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
