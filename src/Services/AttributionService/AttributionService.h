#pragma once

#include "Interface/AttributionServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/Map.h"
#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AttributionService
        : public ServiceBase<AttributionServiceInterface>
    {
        DECLARE_FACTORABLE( AttributionService );

    public:
        AttributionService();
        ~AttributionService() override;

    public:
        bool registerAttribution( const ConstString & _name, EAttributionType _type ) override;

    public:
        bool setAttribution( const ConstString & _name, const ParamVariant & _value ) override;
        ParamVariant getAttribution( const ConstString & _name ) const override;

    public:
        void clearAttributions() override;

    public:
        void addProvider( const AttributionProviderInterfacePtr & _provider ) override;
        void removeProvider( const AttributionProviderInterfacePtr & _provider ) override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        //////////////////////////////////////////////////////////////////////////
        struct AttributionDesc
        {
            ParamVariant value;
            EAttributionType type;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef Map<ConstString, AttributionDesc> MapAttributions;
        typedef Vector<AttributionProviderInterfacePtr> VectorProviders;
        //////////////////////////////////////////////////////////////////////////
    protected:
        void notifyProviders_( const ConstString & _name, EAttributionType _type, const ParamVariant & _value );

    protected:
        MapAttributions m_values;
        VectorProviders m_providers;
        ThreadMutexInterfacePtr m_mutex;
    };
    //////////////////////////////////////////////////////////////////////////
}
