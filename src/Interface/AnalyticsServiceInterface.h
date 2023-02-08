#pragma once

#include "Interface/Interface.h"
#include "Interface/ServantInterface.h"
#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/String.h"

#include "Config/Time.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EAnalyticsEventParameterType
    {
        EAEPT_BOOLEAN,
        EAEPT_INTEGER,
        EAEPT_DOUBLE,
        EAEPT_CONSTSTRING,
        EAEPT_STRING
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
        virtual AnalyticsEventParameterInterfacePtr resolveParameter() const = 0;
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
    typedef IntrusivePtr<class AnalyticsContextInterface> AnalyticsContextInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool()> LambdaAnalyticsParameterGetterBoolean;
    typedef Lambda<ConstString()> LambdaAnalyticsParameterGetterConstString;
    typedef Lambda<void( String * const )> LambdaAnalyticsParameterGetterString;
    typedef Lambda<int64_t()> LambdaAnalyticsParameterGetterInteger;
    typedef Lambda<double()> LambdaAnalyticsParameterGetterDouble;
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsContextInterface
        : public ServantInterface
    {
    public:
        virtual void addParameter( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter ) = 0;
        virtual const AnalyticsEventParameterInterfacePtr & getParameter( const ConstString & _name ) const = 0;

    public:
        virtual void setParameterBoolean( const ConstString & _name, bool _value ) = 0;
        virtual void setParameterString( const ConstString & _name, const String & _value ) = 0;
        virtual void setParameterInteger( const ConstString & _name, int64_t _value ) = 0;
        virtual void setParameterDouble( const ConstString & _name, double _value ) = 0;

    public:
        virtual void setParameterGetterBoolean( const ConstString & _name, const LambdaAnalyticsParameterGetterBoolean & _lambda ) = 0;
        virtual void setParameterGetterString( const ConstString & _name, const LambdaAnalyticsParameterGetterString & _lambda ) = 0;
        virtual void setParameterGetterInteger( const ConstString & _name, const LambdaAnalyticsParameterGetterInteger & _lambda ) = 0;
        virtual void setParameterGetterDouble( const ConstString & _name, const LambdaAnalyticsParameterGetterDouble & _lambda ) = 0;

    public:
        virtual AnalyticsContextInterfacePtr resolveContext() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsContextInterface> AnalyticsContextInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    enum EAnalyticsEventType
    {
        EAET_CUSTOM = 0,
        EAET_EARN_VIRTUAL_CURRENCY = 1,
        EAET_SPEND_VIRTUAL_CURRENCY = 2,
        EAET_UNLOCK_ACHIEVEMENT = 3,
    };
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventInterface
        : public ServantInterface
    {
    public:
        virtual EAnalyticsEventType getType() const = 0;
        virtual const ConstString & getName() const = 0;

    public:
        virtual void setContext( const AnalyticsContextInterfacePtr & _context ) = 0;
        virtual const AnalyticsContextInterfacePtr & getContext() const = 0;

    public:
        virtual void setTimestamp( TimeMilliseconds _timestamp ) = 0;
        virtual TimeMilliseconds getTimestamp() const = 0;

    public:
        virtual void addParameter( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter ) = 0;

        virtual uint32_t getCountParameters() const = 0;

        typedef Lambda<void( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )> LambdaEventParameter;
        virtual void foreachParameters( const LambdaEventParameter & _lambda ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventInterface> AnalyticsEventInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventProviderInterface
        : public Interface
    {
    public:
        virtual void onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventProviderInterface> AnalyticsEventProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventBuilderInterface
        : public ServantInterface
    {
    public:
        virtual AnalyticsEventBuilderInterface * addParameterBoolean( const ConstString & _name, bool _value ) = 0;
        virtual AnalyticsEventBuilderInterface * addParameterConstString( const ConstString & _name, const ConstString & _value ) = 0;
        virtual AnalyticsEventBuilderInterface * addParameterString( const ConstString & _name, const String & _value ) = 0;
        virtual AnalyticsEventBuilderInterface * addParameterInteger( const ConstString & __namekey, int64_t _value ) = 0;
        virtual AnalyticsEventBuilderInterface * addParameterDouble( const ConstString & _name, double _value ) = 0;

    public:
        virtual void log() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventBuilderInterface> AnalyticsEventBuilderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AnalyticsService" )

    public:
        virtual void addEventProvider( const AnalyticsEventProviderInterfacePtr & _provider ) = 0;
        virtual void removeEventProvider( const AnalyticsEventProviderInterfacePtr & _provider ) = 0;

    public:
        virtual void logEvent( const AnalyticsEventInterfacePtr & _event ) = 0;

    public:
        virtual void logEarnVirtualCurrency( const ConstString & _currencyName, double _value ) = 0;
        virtual void logSpendVirtualCurrency( const ConstString & _itemName, const ConstString & _currencyName, double _value ) = 0;
        virtual void logUnlockAchievement( const ConstString & _achievementId ) = 0;

    public:
        virtual AnalyticsEventBuilderInterfacePtr buildEvent( const ConstString & _eventName ) = 0;
    
    public:
        virtual const AnalyticsContextInterfacePtr & getGlobalContext() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ANALYTICS_SERVICE()\
    ((Mengine::AnalyticsServiceInterface *)SERVICE_GET(Mengine::AnalyticsServiceInterface))
//////////////////////////////////////////////////////////////////////////