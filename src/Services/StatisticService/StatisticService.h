#pragma once

#include "Interface/StatisticServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Map.h"

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
        bool _availableService() const override;
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void addStatisticInteger( const ConstString & _name, int64_t _value ) override;
        bool hasStatisticInteger( const ConstString & _name ) const override;
        int64_t getStatisticInteger( const ConstString & _name ) const override;
        void resetStatisticInteger( const ConstString & _name ) override;

    public:
        void addStatisticDouble( const ConstString & _name, double _value ) override;
        bool hasStatisticDouble( const ConstString & _name ) const override;
        double getStatisticDouble( const ConstString & _name ) const override;
        void resetStatisticDouble( const ConstString & _name ) override;

    public:
        void setStatisticConstString( const ConstString & _name, const ConstString & _value ) override;
        bool hasStatisticConstString( const ConstString & _name ) const override;
        const ConstString & getStatisticConstString( const ConstString & _name ) const override;
        void resetStatisticConstString( const ConstString & _name ) override;

    protected:
        ThreadMutexInterfacePtr m_mutex;

        typedef Map<ConstString, int64_t> MapStatisticIntegers;
        MapStatisticIntegers m_statisticIntegers;

        typedef Map<ConstString, double> MapStatisticDoubles;
        MapStatisticDoubles m_statisticDoubles;

        typedef Map<ConstString, ConstString> MapStatisticConstStrings;
        MapStatisticConstStrings m_statisticConstStrings;
    };
}
