#pragma once

#include "Interface/ResourceBankInterface.h"

#include "Kernel/Hashtable.h"

namespace Mengine
{
    class ResourceBank
        : public ResourceBankInterface
    {
    public:
        ResourceBank();
        ~ResourceBank() override;

    public:
        bool initialize( const ThreadMutexInterfacePtr & _mutex, uint32_t _reserved ) override;
        void finalize() override;

    public:
        ResourcePointer createResource( const ConstString & _locale, const ConstString & _groupName, const ConstString & _name, const ConstString & _type, bool _keep, Resource ** _override, const DocumentPtr & _doc ) override;

    public:
        void removeResource( const ResourcePtr & _resource ) override;

    protected:
        void destroyResource( Resource * _resource ) override;

    public:
        const ResourcePtr & getResource( const ConstString & _name ) const override;
        const ResourcePtr & getResourceReference( const ConstString & _name ) const override;
        const ConstString & getResourceType( const ConstString & _name ) const override;

        bool hasResource( const ConstString & _name, ResourcePtr * _resource ) const override;

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

        bool hasResourceWithType( const ConstString & _name, const ConstString & _type, ResourcePtr * _resource ) const override;

    protected:        
        void foreachResources( const LambdaResourceView & _lambda ) const override;

    protected:
        ThreadMutexInterfacePtr m_mutex;
        
        typedef Hashtable<ConstString, ResourcePtrView> HashtableResources;
        HashtableResources m_resources;

        friend class Resource;
        friend class ResourceService;

    protected:
        const HashtableResources & getResources() const;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceBank, ResourceBankInterface> ResourceBankPtr;
    //////////////////////////////////////////////////////////////////////////
}