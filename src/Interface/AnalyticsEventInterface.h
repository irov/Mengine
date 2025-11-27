#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/AnalyticsContextInterface.h"

#include "Kernel/ConstString.h"

#include "Config/Timestamp.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EAnalyticsEventCategory
    {
        AEEC_SYSTEM = 0,
        AEEC_CUSTOM = 1
    };
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventInterface
        : public ServantInterface
    {
    public:
        virtual const ConstString & getName() const = 0;

    public:
        virtual void setCategory( EAnalyticsEventCategory _category ) = 0;
        virtual EAnalyticsEventCategory getCategory() const = 0;

    public:
        virtual void setTimestamp( Timestamp _time ) = 0;
        virtual Timestamp getTimestamp() const = 0;

    public:
        virtual void setContext( const AnalyticsContextInterfacePtr & _context ) = 0;
        virtual const AnalyticsContextInterfacePtr & getContext() const = 0;

    public:
        virtual void setLocalContext( const AnalyticsContextInterfacePtr & _context ) = 0;
        virtual const AnalyticsContextInterfacePtr & getLocalContext() const = 0;

    public:
        virtual void setGlobalContext( const AnalyticsContextInterfacePtr & _globalContext ) = 0;
        virtual const AnalyticsContextInterfacePtr & getGlobalContext() const = 0;

    public:
        virtual size_t getCountParameters() const = 0;

        typedef Lambda<void( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )> LambdaEventParameter;
        virtual void foreachParameters( const LambdaEventParameter & _lambda ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventInterface> AnalyticsEventInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}