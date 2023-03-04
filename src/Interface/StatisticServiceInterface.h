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
        virtual void addStatisticInteger( uint32_t _id, int64_t _value ) = 0;
        virtual void delStatisticInteger( uint32_t _id, int64_t _value ) = 0;
        virtual int64_t getStatisticInteger( uint32_t _id ) const = 0;
        virtual void resetStatisticInteger( uint32_t _id ) = 0;

    public:
        virtual void addStatisticDouble( uint32_t _id, double _value ) = 0;
        virtual double getStatisticDouble( uint32_t _id ) const = 0;
        virtual void resetStatisticDouble( uint32_t _id ) = 0;

    public:
        virtual void setStatisticConstString( uint32_t _id, const ConstString & _value ) = 0;
        virtual const ConstString & getStatisticConstString( uint32_t _id ) const = 0;
        virtual void resetStatisticConstString( uint32_t _id ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define STATISTIC_SERVICE()\
    ((Mengine::StatisticServiceInterface*)SERVICE_GET(Mengine::StatisticServiceInterface))
//////////////////////////////////////////////////////////////////////////
