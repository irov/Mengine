#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/AnalyticsContextInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/String.h"

#include "Config/Timestamp.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventBuilderInterface
        : public ServantInterface
    {
    public:
        virtual void setLocalContext( const AnalyticsContextInterfacePtr & _localContext ) = 0;
        virtual const AnalyticsContextInterfacePtr & getLocalContext() const = 0;

    public:
        virtual AnalyticsEventBuilderInterface * addParameterBoolean( const ConstString & _name, bool _value ) = 0;
        virtual AnalyticsEventBuilderInterface * addParameterInteger( const ConstString & _name, int64_t _value ) = 0;
        virtual AnalyticsEventBuilderInterface * addParameterDouble( const ConstString & _name, double _value ) = 0;
        virtual AnalyticsEventBuilderInterface * addParameterString( const ConstString & _name, const String & _value ) = 0;
        virtual AnalyticsEventBuilderInterface * addParameterConstString( const ConstString & _name, const ConstString & _value ) = 0;

    public:
        virtual Timestamp log() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventBuilderInterface> AnalyticsEventBuilderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}