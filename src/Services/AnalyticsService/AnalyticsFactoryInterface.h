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
        virtual AnalyticsEventBuilderInterfacePtr makeEventBuilder( const DocumentInterfacePtr & _doc ) = 0;
        virtual AnalyticsEventInterfacePtr makeEvent( const ConstString & _name, const DocumentInterfacePtr & _doc ) = 0;

        virtual AnalyticsEventParameterBooleanInterfacePtr makeEventConstParameterBoolean( bool _value, const DocumentInterfacePtr & _doc ) = 0;
        virtual AnalyticsEventParameterConstStringInterfacePtr makeEventConstParameterConstString( const ConstString & _value, const DocumentInterfacePtr & _doc ) = 0;
        virtual AnalyticsEventParameterStringInterfacePtr makeEventConstParameterString( const String & _value, const DocumentInterfacePtr & _doc ) = 0;
        virtual AnalyticsEventParameterIntegerInterfacePtr makeEventConstParameterInteger( int64_t _value, const DocumentInterfacePtr & _doc ) = 0;
        virtual AnalyticsEventParameterDoubleInterfacePtr makeEventConstParameterDouble( double _value, const DocumentInterfacePtr & _doc ) = 0;

        virtual AnalyticsEventParameterBooleanInterfacePtr makeEventGetterParameterBoolean( const LambdaAnalyticsParameterGetterBoolean & _lambda, const DocumentInterfacePtr & _doc ) = 0;
        virtual AnalyticsEventParameterConstStringInterfacePtr makeEventGetterParameterConstString( const LambdaAnalyticsParameterGetterConstString & _lambda, const DocumentInterfacePtr & _doc ) = 0;
        virtual AnalyticsEventParameterStringInterfacePtr makeEventGetterParameterString( const LambdaAnalyticsParameterGetterString & _lambda, const DocumentInterfacePtr & _doc ) = 0;
        virtual AnalyticsEventParameterIntegerInterfacePtr makeEventGetterParameterInteger( const LambdaAnalyticsParameterGetterInteger & _lambda, const DocumentInterfacePtr & _doc ) = 0;
        virtual AnalyticsEventParameterDoubleInterfacePtr makeEventGetterParameterDouble( const LambdaAnalyticsParameterGetterDouble & _lambda, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual AnalyticsContextInterfacePtr makeContext( const DocumentInterfacePtr & _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsFactoryInterface> AnalyticsFactoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
