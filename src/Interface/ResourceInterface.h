#   pragma once

#   include "Interface/ServiceInterface.h"

#   include "Config/String.h"

#   include "Core/ConstString.h"
#   include "Core/FilePath.h"

#   include "Factory/Factory.h"

namespace Menge
{
    class ResourceReference;
    class ResourceVisitor;

    class ResourceServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("ResourceService")

	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

    public:
        virtual bool loadResource( const ConstString & _pakName, const FilePath & _path ) = 0;

	public:
		virtual ResourceReference * generateResource( const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type ) = 0;

	public:
		template<class T>
		T generateResourceT( const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type )
		{
			ResourceReference * resource = this->generateResource( _category, _group, _name, _type );

#   ifdef _DEBUG
			if( dynamic_cast<T>(resource) == nullptr )
			{
				return nullptr;
			}
#   endif

			T t = static_cast<T>(resource);

			return t;
		}

	public:
        virtual ResourceReference * createResource( const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type ) = 0;

	public:
        template<class T>
        T createResourceT( const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type )
        {
			ResourceReference * resource = this->createResource(_category, _group, _name, _type);

#   ifdef _DEBUG
			if( dynamic_cast<T>(resource) == nullptr )
			{
				return nullptr;
			}
#   endif

			T t = static_cast<T>(resource);

			return t;
        }
		
    public:
        virtual ResourceReference * getResource( const ConstString& _name ) const = 0;

        template<class T>
        T getResourceT( const ConstString & _name ) const
        {
            ResourceReference * resource = this->getResource( _name );

#   ifdef _DEBUG
            if( dynamic_cast<T>(resource) == nullptr )
            {
                return nullptr;
            }
#   endif

            T t = static_cast<T>(resource);

            return t;
        }

        virtual ResourceReference * getResourceReference( const ConstString& _name ) const = 0;

        template<class T>
        T getResourceReferenceT( const ConstString & _name ) const
        {
            ResourceReference * resource = this->getResourceReference( _name );

#   ifdef _DEBUG
            if( dynamic_cast<T>(resource) == nullptr )
            {
                return nullptr;
            }
#   endif

            T t = static_cast<T>(resource);

            return t;
        }

        virtual bool hasResource( const ConstString& _name, ResourceReference ** _resource ) const = 0;
		
		template<class T>
		bool hasResourceT( const ConstString& _name, T * _resource ) const
		{
			ResourceReference * resource;			
			if( this->hasResource( _name, &resource ) == false )
			{
				return false;
			}
			
#   ifdef _DEBUG
			if( dynamic_cast<T>(resource) == nullptr )
			{
				return false;
			}
#   endif

			*_resource = static_cast<T>(resource);

			return true;
		}

        virtual bool validResource( const ConstString& _name ) const = 0;

        virtual bool validResourceType( const ConstString& _name, const ConstString& _type ) const = 0;

        virtual void visitResources( ResourceVisitor * _visitor ) const = 0;
		virtual void visitGroupResources( const ConstString & _category, const ConstString & _group, ResourceVisitor * _visitor ) const = 0;

		virtual size_t getGroupResourcesMemoryUse( const ConstString & _category, const ConstString & _group ) const = 0;

    public:
        virtual bool validationResources() const = 0; 

    public:
        virtual void dumpResources( const String & _tag ) = 0;
    };

#   define RESOURCE_SERVICE( serviceProvider )\
    ((Menge::ResourceServiceInterface *)SERVICE_GET(serviceProvider, Menge::ResourceServiceInterface))
}