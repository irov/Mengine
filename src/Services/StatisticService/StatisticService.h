#pragma once

#include "Interface/StatisticServiceInterface.h"
#include "Interface/StatisticInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class StatisticService
        : public ServiceBase<StatisticServiceInterface>
    {
    public:
        StatisticService();
        ~StatisticService() override;

    public:
        const ServiceRequiredList & requiredServices() const override;
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void addStatisticInteger( uint32_t _id, int64_t _value ) override;
        void delStatisticInteger( uint32_t _id, int64_t _value ) override;
        int64_t getStatisticInteger( uint32_t _id ) const override;
        void resetStatisticInteger( uint32_t _id ) override;

    public:
        void addStatisticDouble( uint32_t _id, double _value ) override;
        double getStatisticDouble( uint32_t _id ) const override;
        void resetStatisticDouble( uint32_t _id ) override;

    public:
        void setStatisticConstString( uint32_t _id, const ConstString & _value ) override;
        const ConstString & getStatisticConstString( uint32_t _id ) const override;
        void resetStatisticConstString( uint32_t _id ) override;

    protected:
        ThreadMutexInterfacePtr m_mutex;

        int64_t m_statisticIntegers[MENGINE_STATISTIC_MAX_COUNT] = {0LL};
        double m_statisticDoubles[MENGINE_STATISTIC_MAX_COUNT] = {0.0};
        ConstString m_statisticConstStrings[MENGINE_STATISTIC_MAX_COUNT];
    };
}
