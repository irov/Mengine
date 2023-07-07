#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/AnalyticsContextInterface.h"

#include "Kernel/ConstString.h"

#include "Config/Timestamp.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EAnalyticsEventType
    {
        EAET_CUSTOM = 0,
        EAET_EARN_VIRTUAL_CURRENCY = 1,
        EAET_SPEND_VIRTUAL_CURRENCY = 2,
        EAET_UNLOCK_ACHIEVEMENT = 3,
        EAET_LEVEL_UP = 4,
        EAET_LEVEL_START = 5,
        EAET_LEVEL_END = 6,
        EAET_SELECT_ITEM = 7,
        EAET_TUTORIAL_BEGIN = 8,
        EAET_TUTORIAL_COMPLETE = 9,
        EAET_SCREEN_VIEW = 10
    };
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventInterface
        : public ServantInterface
    {
    public:
        virtual EAnalyticsEventType getType() const = 0;
        virtual const ConstString & getName() const = 0;

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
        virtual uint32_t getCountParameters() const = 0;

        typedef Lambda<void( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )> LambdaEventParameter;
        virtual void foreachParameters( const LambdaEventParameter & _lambda ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventInterface> AnalyticsEventInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}