#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/Visitor.h"

#include "Config/Lambda.h"
#include "Config/String.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/Pointer.h"

#include "Kernel/Factory.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Resource> ResourcePtr;
    //////////////////////////////////////////////////////////////////////////
    typedef PointerT<ResourcePtr> PointerResourceReference;
    //////////////////////////////////////////////////////////////////////////
    class ResourceServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ResourceService" )

    public:
        virtual bool loadResources( const ConstString & _locale, const FileGroupInterfacePtr & _pakName, const FilePath & _path, bool _ignored ) = 0;
        virtual bool unloadResources( const ConstString & _locale, const FileGroupInterfacePtr & _pakName, const FilePath & _path ) = 0;

    public:
        virtual PointerResourceReference generateResource( const ConstString& _type ) const = 0;

    public:
        virtual PointerResourceReference createResource( const ConstString & _locale, const FileGroupInterfacePtr& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type ) = 0;

    public:
        virtual bool removeResource( const ResourcePtr & _resource ) = 0;

    public:
        virtual const ResourcePtr & getResource( const ConstString& _name ) const = 0;

        virtual const ResourcePtr & getResourceReference( const ConstString& _name ) const = 0;

        virtual bool hasResource( const ConstString& _name, ResourcePtr * _resource ) const = 0;

        template<class T>
        bool hasResourceT( const ConstString& _name, T * _resource ) const
        {
            ResourcePtr resource;
            if( this->hasResource( _name, &resource ) == false )
            {
                return false;
            }

#ifndef NDEBUG
            if( resource == nullptr )
            {
                return false;
            }

            if( stdex::intrusive_dynamic_cast<T>(resource) == nullptr )
            {
                throw;
            }
#endif

            *_resource = stdex::intrusive_static_cast<T>(resource);

            return true;
        }

        virtual bool hasResourceWithType( const ConstString& _name, const ConstString& _type ) const = 0;

    public:
        typedef Lambda<void( const ResourcePtr & )> LambdaResource;
        virtual void foreachResources( const LambdaResource & _lambda ) const = 0;

    public:
        virtual void visitResources( const VisitorPtr & _visitor ) const = 0;
        virtual void visitGroupResources( const FileGroupInterfacePtr & _category, const ConstString & _group, const VisitorPtr & _visitor ) const = 0;
    };

#   define RESOURCE_SERVICE()\
    ((Mengine::ResourceServiceInterface *)SERVICE_GET(Mengine::ResourceServiceInterface))
}
