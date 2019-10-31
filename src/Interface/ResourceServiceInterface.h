#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ResourceBankInterface.h"

#include "Kernel/Visitor.h"

#include "Config/Lambda.h"

#include "Kernel/ConstString.h"
#include "Kernel/Tags.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    class ResourceServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ResourceService" )

    public:
        virtual ResourceBankInterfacePtr createResourceBank( uint32_t _reserved, const Char * _doc ) = 0;


    public:
        virtual ResourcePointer createResource( const ConstString & _locale, const ConstString & _groupName, const ConstString & _name, const ConstString & _type, bool _groupCache, bool _keep, const Char * _doc ) = 0;

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

        virtual bool hasResourceWithType( const ConstString & _name, const ConstString & _type, ResourcePtr * _resource ) const = 0;

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

