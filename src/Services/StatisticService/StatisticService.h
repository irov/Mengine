#pragma once

#include "Interface/StatisticServiceInterface.h"
#include "Interface/StatisticInterface.h"
#include "Interface/ThreadSharedMutexInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class StatisticService
        : public ServiceBase<StatisticServiceInterface>
    {
        DECLARE_FACTORABLE( StatisticService );

    public:
        StatisticService();
        ~StatisticService() override;

    public:
        const ServiceRequiredList & requiredServices() const override;
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void setStatisticEnabled( StatisticId _id, bool _enable ) override;
        bool isStatisticEnabled( StatisticId _id ) const override;

    public:
        void addStatisticInteger( StatisticId _id, int64_t _value ) override;
        void delStatisticInteger( StatisticId _id, int64_t _value ) override;
        int64_t getStatisticInteger( StatisticId _id ) const override;
        void resetStatisticInteger( StatisticId _id ) override;

    public:
        void addStatisticDouble( StatisticId _id, double _value ) override;
        double getStatisticDouble( StatisticId _id ) const override;
        void resetStatisticDouble( StatisticId _id ) override;

    public:
        void setStatisticConstString( StatisticId _id, const ConstString & _value ) override;
        const ConstString & getStatisticConstString( StatisticId _id ) const override;
        void resetStatisticConstString( StatisticId _id ) override;

    protected:
        ThreadSharedMutexInterfacePtr m_mutex;

        bool m_statisticEnabled[MENGINE_STATISTIC_MAX_COUNT] = {true};
        int64_t m_statisticIntegers[MENGINE_STATISTIC_MAX_COUNT] = {MENGINE_INT64_C( 0 )};
        double m_statisticDoubles[MENGINE_STATISTIC_MAX_COUNT] = {0.0};
        ConstString m_statisticConstStrings[MENGINE_STATISTIC_MAX_COUNT];
    };
}
