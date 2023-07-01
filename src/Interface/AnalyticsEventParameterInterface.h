#pragma once

#include "Interface/ServantInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EAnalyticsEventParameterType
    {
        EAEPT_BOOLEAN,
        EAEPT_INTEGER,
        EAEPT_DOUBLE,
        EAEPT_STRING,
        EAEPT_CONSTSTRING
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class AnalyticsEventParameterInterface> AnalyticsEventParameterInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventParameterInterface
        : public ServantInterface
    {
    public:
        virtual EAnalyticsEventParameterType getType() const = 0;

    public:
        virtual AnalyticsEventParameterInterfacePtr resolveParameter( const DocumentPtr & _doc ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventParameterInterface> AnalyticsEventParameterInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventParameterBooleanInterface
        : public AnalyticsEventParameterInterface
    {
    public:
        virtual bool resolveValue() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventParameterBooleanInterface, AnalyticsEventParameterInterface> AnalyticsEventParameterBooleanInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventParameterConstStringInterface
        : public AnalyticsEventParameterInterface
    {
    public:
        virtual const ConstString & resolveValue() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventParameterConstStringInterface, AnalyticsEventParameterInterface> AnalyticsEventParameterConstStringInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventParameterStringInterface
        : public AnalyticsEventParameterInterface
    {
    public:
        virtual const String & resolveValue() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventParameterStringInterface, AnalyticsEventParameterInterface> AnalyticsEventParameterStringInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventParameterIntegerInterface
        : public AnalyticsEventParameterInterface
    {
    public:
        virtual int64_t resolveValue() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventParameterIntegerInterface, AnalyticsEventParameterInterface> AnalyticsEventParameterIntegerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventParameterDoubleInterface
        : public AnalyticsEventParameterInterface
    {
    public:
        virtual double resolveValue() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventParameterDoubleInterface, AnalyticsEventParameterInterface> AnalyticsEventParameterDoubleInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}