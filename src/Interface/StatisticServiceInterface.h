#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"

#include "Config/StatisticId.h"

//////////////////////////////////////////////////////////////////////////
#if !defined(MENGINE_STATISTIC)
#   if defined(MENGINE_DEBUG)
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
        virtual void addStatisticInteger( StatisticId _id, int64_t _value ) = 0;
        virtual void delStatisticInteger( StatisticId _id, int64_t _value ) = 0;
        virtual int64_t getStatisticInteger( StatisticId _id ) const = 0;
        virtual void resetStatisticInteger( StatisticId _id ) = 0;

    public:
        virtual void addStatisticDouble( StatisticId _id, double _value ) = 0;
        virtual double getStatisticDouble( StatisticId _id ) const = 0;
        virtual void resetStatisticDouble( StatisticId _id ) = 0;

    public:
        virtual void setStatisticConstString( StatisticId _id, const ConstString & _value ) = 0;
        virtual const ConstString & getStatisticConstString( StatisticId _id ) const = 0;
        virtual void resetStatisticConstString( StatisticId _id ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define STATISTIC_SERVICE()\
    ((Mengine::StatisticServiceInterface*)SERVICE_GET(Mengine::StatisticServiceInterface))
//////////////////////////////////////////////////////////////////////////