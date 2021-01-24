#pragma once

#include "ResourceConvertServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Resource.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceConvertService
        : public ServiceBase<ResourceConvertServiceInterface>
    {
    public:
        ResourceConvertService();
        ~ResourceConvertService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        void convertResource_( Resource * _resource );
    };
}