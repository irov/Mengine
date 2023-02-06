#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_STATISTIC
#   ifdef MENGINE_DEBUG
#       define MENGINE_STATISTIC 1
#   else
#       define MENGINE_STATISTIC 0
#   endif
#endif
//////////////////////////////////////////////////////////////////////////
#if MENGINE_STATISTIC == 1
#   define MENGINE_STATISTIC_ENABLE
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    class StatisticServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "StatisticService" )

    public:
        virtual void addStatisticInteger( const ConstString & _name, int64_t _value ) = 0;
        virtual bool hasStatisticInteger( const ConstString & _name ) const = 0;
        virtual int64_t getStatisticInteger( const ConstString & _name ) const = 0;
        virtual void resetStatisticInteger( const ConstString & _name ) = 0;

    public:
        virtual void addStatisticDouble( const ConstString & _name, double _value ) = 0;
        virtual bool hasStatisticDouble( const ConstString & _name ) const = 0;
        virtual double getStatisticDouble( const ConstString & _name ) const = 0;
        virtual void resetStatisticDouble( const ConstString & _name ) = 0;

    public:
        virtual void setStatisticConstString( const ConstString & _name, const ConstString & _value ) = 0;
        virtual bool hasStatisticConstString( const ConstString & _name ) const = 0;
        virtual const ConstString & getStatisticConstString( const ConstString & _name ) const = 0;
        virtual void resetStatisticConstString( const ConstString & _name ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define STATISTIC_SERVICE()\
    ((Mengine::StatisticServiceInterface*)SERVICE_GET(Mengine::StatisticServiceInterface))
//////////////////////////////////////////////////////////////////////////
