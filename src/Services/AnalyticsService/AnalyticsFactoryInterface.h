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
        virtual AnalyticsEventBuilderInterfacePtr makeEventBuilder( const DocumentPtr & _doc ) = 0;
        virtual AnalyticsEventInterfacePtr makeEvent( EAnalyticsEventType _type, const ConstString & _name, const DocumentPtr & _doc ) = 0;

        virtual AnalyticsEventParameterBooleanInterfacePtr makeEventConstParameterBoolean( bool _value, const DocumentPtr & _doc ) = 0;
        virtual AnalyticsEventParameterConstStringInterfacePtr makeEventConstParameterConstString( const ConstString & _value, const DocumentPtr & _doc ) = 0;
        virtual AnalyticsEventParameterStringInterfacePtr makeEventConstParameterString( const String & _value, const DocumentPtr & _doc ) = 0;
        virtual AnalyticsEventParameterIntegerInterfacePtr makeEventConstParameterInteger( int64_t _value, const DocumentPtr & _doc ) = 0;
        virtual AnalyticsEventParameterDoubleInterfacePtr makeEventConstParameterDouble( double _value, const DocumentPtr & _doc ) = 0;

        virtual AnalyticsEventParameterBooleanInterfacePtr makeEventGetterParameterBoolean( const LambdaAnalyticsParameterGetterBoolean & _lambda, const DocumentPtr & _doc ) = 0;
        virtual AnalyticsEventParameterConstStringInterfacePtr makeEventGetterParameterConstString( const LambdaAnalyticsParameterGetterConstString & _lambda, const DocumentPtr & _doc ) = 0;
        virtual AnalyticsEventParameterStringInterfacePtr makeEventGetterParameterString( const LambdaAnalyticsParameterGetterString & _lambda, const DocumentPtr & _doc ) = 0;
        virtual AnalyticsEventParameterIntegerInterfacePtr makeEventGetterParameterInteger( const LambdaAnalyticsParameterGetterInteger & _lambda, const DocumentPtr & _doc ) = 0;
        virtual AnalyticsEventParameterDoubleInterfacePtr makeEventGetterParameterDouble( const LambdaAnalyticsParameterGetterDouble & _lambda, const DocumentPtr & _doc ) = 0;

    public:
        virtual AnalyticsContextInterfacePtr makeContext( const DocumentPtr & _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsFactoryInterface> AnalyticsFactoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
