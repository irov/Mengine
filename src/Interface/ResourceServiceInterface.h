#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/Visitor.h"

#include "Config/Lambda.h"

#include "Kernel/ConstString.h"
#include "Kernel/Tags.h"
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
        virtual bool loadResources( const ConstString & _locale, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const Tags & _tags, bool _ignored ) = 0;
        virtual bool unloadResources( const ConstString & _locale, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) = 0;

    public:
        virtual PointerResourceReference generateResource( const ConstString & _type, const Char * _doc ) const = 0;

    public:
        virtual PointerResourceReference createResource( const ConstString & _locale, const ConstString & _groupName, const ConstString & _name, const ConstString & _type, const Char * _doc ) = 0;

    public:
        virtual bool removeResource( const ResourcePtr & _resource ) = 0;

    public:
        virtual const ResourcePtr & getResource( const ConstString & _name ) const = 0;

        virtual const ResourcePtr & getResourceReference( const ConstString & _name ) const = 0;

        virtual bool hasResource( const ConstString & _name, ResourcePtr * _resource ) const = 0;

        template<class T>
        bool hasResourceT( const ConstString & _name, T * _resource ) const
        {
            ResourcePtr resource;
            if( this->hasResource( _name, &resource ) == false )
            {
                return false;
            }

#ifdef MENGINE_DEBUG
            if( resource == nullptr )
            {
                return false;
            }

            if( stdex::intrusive_dynamic_cast<T>(resource) == nullptr )
            {
                throw;
            }
#endif

            * _resource = stdex::intrusive_static_cast<T>(resource);

            return true;
        }

        virtual bool hasResourceWithType( const ConstString & _name, const ConstString & _type ) const = 0;

    public:
        typedef Lambda<void( const ResourcePtr & )> LambdaResource;
        virtual void foreachResources( const LambdaResource & _lambda ) const = 0;
        virtual void foreachGroupResources( const ConstString & _groupName, const LambdaResource & _lambda ) const = 0;
        virtual void foreachTagsResources( const Tags & _tags, const LambdaResource & _lambda ) const = 0;

    public:
        virtual void visitResources( const VisitorPtr & _visitor ) const = 0;
        virtual void visitGroupResources( const ConstString & _groupName, const VisitorPtr & _visitor ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define RESOURCE_SERVICE()\
    ((Mengine::ResourceServiceInterface *)SERVICE_GET(Mengine::ResourceServiceInterface))
//////////////////////////////////////////////////////////////////////////

