#pragma once

#include "ResourceValidateServiceInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{    
    class ResourceValidateService
        : public ServiceBase<ResourceValidateServiceInterface>
    {
        DECLARE_FACTORABLE( ResourceValidateService );

    public:
        ResourceValidateService();
        ~ResourceValidateService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool validateResource( const ResourcePtr & _resource ) const override;

    protected:
        bool visitableResource_( const ResourcePtr & _resource ) const;
        void notifyEnablePackages_( const ConstString & _locale, const Tags & _platformTags ) const;
    };
}