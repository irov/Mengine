#   pragma once

#   include "Interface/ServiceInterface.h"

#   include "Core/ConstString.h"
#   include "Core/FilePath.h"

#   include "Factory/Factory.h"

namespace Menge
{
    struct ResourceDesc
    {
        ConstString pakName;
        ConstString pakType;

        String pakPath;

        FilePath path;
        bool script;
    };

    class ResourceManagerListener
    {
    public:
        virtual void onResourceLoaded( const ConstString& _name ) = 0;
        virtual void onResourceUnLoaded( const ConstString& _name ) = 0;
    };

    class ResourceReference;
    class ResourceVisitor;

    class ResourceServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("ResourceService")

    public:
        virtual void registerResourceFactory( const ConstString & _type, Factory * _factory ) = 0;

    public:
        virtual bool loadResource( const ResourceDesc & _desc ) = 0;

    public:
        virtual ResourceReference * createResource( const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type ) = 0;

        template<class T>
        T * createResourceT( const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type )
        {
            return static_cast<T*>(this->createResource(_category, _group, _name, _type));
        }


    public:
        virtual ResourceReference * getResource( const ConstString& _name ) const = 0;

        template<class T>
        T * getResourceT( const ConstString& _name ) const
        {
            ResourceReference * resource = this->getResource( _name );

            if( resource == NULL )
            {
                return NULL;
            }

            T * t = dynamic_cast<T*>(resource);

            return t;
        }

        virtual ResourceReference * getResourceReference( const ConstString& _name ) const = 0;

        virtual bool hasResource( const ConstString& _name ) const = 0;
        virtual bool validResource( const ConstString& _name ) const = 0;

        virtual bool validResourceType( const ConstString& _name, const ConstString& _type ) const = 0;

        virtual void visitResources( const ConstString & _category, const ConstString & _groupName, ResourceVisitor * _visitor ) const = 0;

    public:
        virtual bool directResourceCompile( const ConstString& _name ) = 0;
        virtual void directResourceRelease( const ConstString& _name ) = 0;

    public:
        virtual void dumpResources( const String & _tag ) = 0;
    };

#   define RESOURCE_SERVICE( serviceProvider )\
    (Menge::getService<Menge::ResourceServiceInterface>(serviceProvider))
}