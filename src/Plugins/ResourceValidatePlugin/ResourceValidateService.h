#pragma once

#include "ResourceValidateInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Resource.h"
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
        void addResourceValidateVisitor( const VisitorPtr & _visitor ) override;
        void removeResourceValidateVisitor( const VisitorPtr & _visitor ) override;
        bool validResource( const ResourcePtr & _resource ) const override;
        
    protected:
        bool visitableResource_( const ResourcePtr & _resource ) const;
        void visitableResources_() const;

    protected:
        typedef Vector<VisitorPtr> VectorResourceValidateVisitor;
        VectorResourceValidateVisitor m_visitors;
    };
}