#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/RenderInterface.h"

#include "Kernel/Node.h"
#include "Kernel/RenderVisitor.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceValidateServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ResourceValidateService" );

    public:
        virtual void addResourceValidateVisitor( const VisitorPtr & _visitor ) = 0;
        virtual void removeResourceValidateVisitor( const VisitorPtr & _visitor ) = 0;

    public:
        virtual bool validResource( const ConstString& _name ) const = 0;
        //virtual bool validateResources( const ConstString & _locale, const FileGroupInterfacePtr & _pakName, const FilePath & _path ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#   define RESOURCEVALIDATE_SERVICE()\
    ((Mengine::ResourceValidateServiceInterface *)SERVICE_GET(Mengine::ResourceValidateServiceInterface))
}