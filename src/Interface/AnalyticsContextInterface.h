#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/AnalyticsEventParameterInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/String.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class AnalyticsContextInterface> AnalyticsContextInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool()> LambdaAnalyticsParameterGetterBoolean;    
    typedef Lambda<int64_t()> LambdaAnalyticsParameterGetterInteger;
    typedef Lambda<double()> LambdaAnalyticsParameterGetterDouble;
    typedef Lambda<void( String * const )> LambdaAnalyticsParameterGetterString;
    typedef Lambda<ConstString()> LambdaAnalyticsParameterGetterConstString;
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsContextInterface
        : public ServantInterface
    {
    public:
        virtual void addParameter( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter ) = 0;
        
    public:
        virtual size_t getCountParameters() const = 0;

    public:
        typedef Lambda<void( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )> LambdaEventParameter;
        virtual void foreachParameters( const LambdaEventParameter & _lambda ) const = 0;

    public:
        virtual void addParameterBoolean( const ConstString & _name, bool _value, const DocumentInterfacePtr & _doc ) = 0;
        virtual void addParameterInteger( const ConstString & _name, int64_t _value, const DocumentInterfacePtr & _doc ) = 0;
        virtual void addParameterDouble( const ConstString & _name, double _value, const DocumentInterfacePtr & _doc ) = 0;
        virtual void addParameterString( const ConstString & _name, const String & _value, const DocumentInterfacePtr & _doc ) = 0;
        virtual void addParameterConstString( const ConstString & _name, const ConstString & _value, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual void addParameterGetterBoolean( const ConstString & _name, const LambdaAnalyticsParameterGetterBoolean & _lambda, const DocumentInterfacePtr & _doc ) = 0;
        virtual void addParameterGetterInteger( const ConstString & _name, const LambdaAnalyticsParameterGetterInteger & _lambda, const DocumentInterfacePtr & _doc ) = 0;
        virtual void addParameterGetterDouble( const ConstString & _name, const LambdaAnalyticsParameterGetterDouble & _lambda, const DocumentInterfacePtr & _doc ) = 0;
        virtual void addParameterGetterString( const ConstString & _name, const LambdaAnalyticsParameterGetterString & _lambda, const DocumentInterfacePtr & _doc ) = 0;
        virtual void addParameterGetterConstString( const ConstString & _name, const LambdaAnalyticsParameterGetterConstString & _lambda, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual AnalyticsContextInterfacePtr resolveContext( const DocumentInterfacePtr & _doc ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsContextInterface> AnalyticsContextInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
