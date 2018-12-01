#pragma once

#include "ResourceValidateServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceValidateService
        : public ServiceBase<ResourceValidateServiceInterface>
        , public Observable
    {
    public:
        ResourceValidateService();
        ~ResourceValidateService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool validResource( const ResourcePtr & _resource ) const override;

    protected:
        bool visitableResource_( const ResourcePtr & _resource ) const;
        void visitableResources_() const;
    };
}