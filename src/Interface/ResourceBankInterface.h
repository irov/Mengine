#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/IntrusivePtrView.h"
#include "Kernel/ConstString.h"
#include "Kernel/Document.h"
#include "Kernel/Tags.h"
#include "Kernel/FilePath.h"
#include "Kernel/Pointer.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Resource> ResourcePtr;
    typedef IntrusivePtrView<class Resource> ResourcePtrView;
    //////////////////////////////////////////////////////////////////////////
    typedef PointerT<ResourcePtr> ResourcePointer;
    //////////////////////////////////////////////////////////////////////////
    class ResourceBankInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize( const ThreadMutexInterfacePtr & _mutex, uint32_t _reserved ) = 0;
        virtual void finalize() = 0;

    public:
        virtual ResourcePointer createResource( const ConstString & _locale
            , const ConstString & _groupName
            , const ConstString & _name
            , const ConstString & _type
            , bool _keep
            , Resource ** const _prev
            , const DocumentPtr & _doc ) = 0;

    public:
        virtual void removeResource( const ResourcePtr & _resource ) = 0;

    public:
        virtual void destroyResource( Resource * _resource ) = 0;

    public:
        virtual const ResourcePtr & getResource( const ConstString & _name ) const = 0;
        virtual const ResourcePtr & getResourceReference( const ConstString & _name ) const = 0;

    public:
        virtual bool hasResource( const ConstString & _name, ResourcePtr * const _resource ) const = 0;

        template<class T>
        bool hasResourceT( const ConstString & _name, T * const _resource ) const
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

        virtual bool hasResourceWithType( const ConstString & _name, const ConstString & _type, ResourcePtr * const _resource ) const = 0;

    public:
        typedef Lambda<void( const ResourcePtrView & )> LambdaResourceView;
        virtual void foreachResources( const LambdaResourceView & _lambda ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceBankInterface> ResourceBankInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}