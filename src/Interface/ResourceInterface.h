#pragma once

#include "Interface/ServiceInterface.h"

#include "Core/Visitor.h"

#include "Config/String.h"

#include "Core/ConstString.h"
#include "Core/FilePath.h"

#include "Factory/Factory.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<class ResourceReference> ResourceReferencePtr;
	//////////////////////////////////////////////////////////////////////////
    class ResourceServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("ResourceService")

    public:
		virtual bool loadResources( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path, bool _ignored ) = 0;
		virtual bool unloadResources( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path ) = 0;

	public:
		virtual ResourceReferencePtr generateResource( const ConstString& _type ) const = 0;

	public:
		template<class T>
		T generateResourceT( const ConstString& _type ) const
		{
			ResourceReferencePtr resource = this->generateResource( _type );

#   ifdef _DEBUG
			if( resource == nullptr )
			{
				return nullptr;
			}

			if( stdex::intrusive_dynamic_cast<T>(resource) == nullptr )
			{
				throw;
			}
#   endif

			T t = stdex::intrusive_static_cast<T>(resource);

			return t;
		}

	public:
		virtual ResourceReferencePtr createResource( const ConstString & _locale, const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type ) = 0;

	public:
        template<class T>
		T createResourceT( const ConstString & _locale, const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type )
        {
			ResourceReferencePtr resource = this->createResource( _locale, _category, _group, _name, _type );

#   ifdef _DEBUG
			if( resource == nullptr )
			{
				return nullptr;
			}
			
			if( stdex::intrusive_dynamic_cast<T>(resource) == nullptr )
			{
				throw;
			}
#   endif

			T t = stdex::intrusive_static_cast<T>(resource);

			return t;
        }

	public:
		virtual bool removeResource( const ResourceReferencePtr & _resource ) = 0;
		
    public:
		virtual ResourceReferencePtr getResource( const ConstString& _name ) const = 0;

        template<class T>
        T getResourceT( const ConstString & _name ) const
        {
			ResourceReferencePtr resource = this->getResource( _name );

#   ifdef _DEBUG
			if( resource == nullptr )
			{
				return nullptr;
			}
			
			if( stdex::intrusive_dynamic_cast<T>(resource) == nullptr )
            {
				throw;
            }
#   endif

			T t = stdex::intrusive_static_cast<T>(resource);

            return t;
        }

		virtual ResourceReferencePtr getResourceReference( const ConstString& _name ) const = 0;

        template<class T>
        T getResourceReferenceT( const ConstString & _name ) const
        {
			ResourceReferencePtr resource = this->getResourceReference( _name );

#   ifdef _DEBUG
			if( resource == nullptr )
			{
				return nullptr;
			}

			if( stdex::intrusive_dynamic_cast<T>(resource) == nullptr )
			{
				throw;
			}
#   endif

			T t = stdex::intrusive_static_cast<T>(resource);

            return t;
        }

		virtual bool hasResource( const ConstString& _name, ResourceReferencePtr * _resource ) const = 0;
		
		template<class T>
		bool hasResourceT( const ConstString& _name, T * _resource ) const
		{
			ResourceReferencePtr resource;
			if( this->hasResource( _name, &resource ) == false )
			{
				return false;
			}
			
#   ifdef _DEBUG
			if( resource == nullptr )
			{
				return nullptr;
			}

			if( stdex::intrusive_dynamic_cast<T>(resource) == nullptr )
			{
				throw;
			}
#   endif

			*_resource = stdex::intrusive_static_cast<T>(resource);

			return true;
		}

        virtual bool validResource( const ConstString& _name ) const = 0;

        virtual bool validResourceType( const ConstString& _name, const ConstString& _type ) const = 0;

        virtual void visitResources( Visitor * _visitor ) const = 0;
		virtual void visitGroupResources( const ConstString & _category, const ConstString & _group, Visitor * _visitor ) const = 0;

    public:
		virtual bool validateResources( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path ) const = 0;

    public:
        virtual void dumpResources( const String & _tag ) = 0;
    };

#   define RESOURCE_SERVICE()\
    ((Mengine::ResourceServiceInterface *)SERVICE_GET(Mengine::ResourceServiceInterface))
}
