#pragma once

#include "Interface/ServiceInterface.h"

#include "Core/Visitor.h"

#include "Config/String.h"

#include "Core/ConstString.h"
#include "Core/FilePath.h"
#include "Core/Pointer.h"

#include "Factory/Factory.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class ResourceReference> ResourceReferencePtr;
	//////////////////////////////////////////////////////////////////////////
    typedef PointerT<ResourceReferencePtr> PointerResourceReference;
	//////////////////////////////////////////////////////////////////////////
    class ResourceServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("ResourceService")

    public:
		virtual bool loadResources( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path, bool _ignored ) = 0;
		virtual bool unloadResources( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path ) = 0;

	public:
		virtual PointerResourceReference generateResource( const ConstString& _type ) const = 0;

	public:
		virtual PointerResourceReference createResource( const ConstString & _locale, const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type ) = 0;

	public:
		virtual bool removeResource( const ResourceReferencePtr & _resource ) = 0;
		
    public:
		virtual PointerResourceReference getResource( const ConstString& _name ) const = 0;

		virtual ResourceReferencePtr getResourceReference( const ConstString& _name ) const = 0;

		virtual bool hasResource( const ConstString& _name, ResourceReferencePtr * _resource ) const = 0;
		
		template<class T>
		bool hasResourceT( const ConstString& _name, T * _resource ) const
		{
			ResourceReferencePtr resource;
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
