#pragma once

#include "Interface/LoadableInterface.h"
#include "Interface/ContentInterface.h"

#include "Kernel/Compilable.h"
#include "Kernel/Identity.h"
#include "Kernel/Scriptable.h"
#include "Kernel/Visitable.h"
#include "Kernel/Unknowable.h"
#include "Kernel/Magicable.h"
#include "Kernel/Factorable.h"
#include "Kernel/Factory.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/Tags.h"
#include "Kernel/ReferenceCounter.h"

#ifdef MENGINE_DEBUG
#   include <type_traits>
#   include <stdexcept>
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceBankInterface;
    //////////////////////////////////////////////////////////////////////////
    class Resource
        : public Factorable
        , public Compilable
        , public Magicable
        , public Identity
        , public Scriptable
        , public Visitable
        , public Unknowable
        , public LoadableInterface
    {
        DECLARE_VISITABLE_BASE();

    public:
        Resource();
        ~Resource() override;

    public:
        void setResourceBank( ResourceBankInterface * _bank );
        MENGINE_INLINE ResourceBankInterface * getResourceBank() const;

    public:
        void setContent( const ContentInterfacePtr & _content );
        MENGINE_INLINE const ContentInterfacePtr & getContent() const;

    public:
        void setLocale( const ConstString & _locale );
        MENGINE_INLINE const ConstString & getLocale() const;

        void setGroupName( const ConstString & _groupName );
        MENGINE_INLINE const ConstString & getGroupName() const;

        void setGroupCache( bool _groupCache );
        MENGINE_INLINE bool isGroupCache() const;

        void setKeep( bool _keep );
        MENGINE_INLINE bool isKeep() const;

        void setMapping( bool _mapping );
        MENGINE_INLINE bool isMapping() const;

        void setPrecompile( bool _precompile );
        MENGINE_INLINE bool isPrecompile() const;

        void setIgnored( bool _ignored );
        MENGINE_INLINE bool isIgnored() const;

    public:
        void setTags( const Tags & _tags );
        const Tags & getTags() const;

    public:
        bool initialize();
        void finalize();

    protected:
        virtual bool _initialize();
        virtual void _finalize();

    public:
        MENGINE_INLINE bool isInitialize() const;

    public:
        bool compile() override;
        void release() override;

    public:
        MENGINE_INLINE uint32_t getCompileReferenceCount() const;

    public:
        typedef Lambda<bool()> LambdaPrefetch;
        bool prefetch( const LambdaPrefetch & _lambda );

        typedef Lambda<bool()> LambdaUnfetch;
        bool unfetch( const LambdaUnfetch & _lambda );

    public:
        bool cache();
        void uncache();

    public:
        MENGINE_INLINE bool isCache() const;

    protected:
        virtual void _cache();
        virtual void _uncache();

    protected:
        ResourceBankInterface * m_resourceBank;

        ContentInterfacePtr m_content;

        ReferenceCounter m_compileReferenceCount;
        ReferenceCounter m_prefetchReferenceCount;
        ReferenceCounter m_cacheReferenceCount;

        ConstString m_locale;
        ConstString m_groupName;
        Tags m_tags;

        bool m_initialize;
        bool m_groupCache;
        bool m_keep;
        bool m_mapping;
        bool m_precompile;
        bool m_ignored;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Resource> ResourcePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE ResourceBankInterface * Resource::getResourceBank() const
    {
        return m_resourceBank;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const ContentInterfacePtr & Resource::getContent() const
    {
        return m_content;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const ConstString & Resource::getLocale() const
    {
        return m_locale;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const ConstString & Resource::getGroupName() const
    {
        return m_groupName;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Resource::isGroupCache() const
    {
        return m_groupCache;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Resource::isKeep() const
    {
        return m_keep;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Resource::isMapping() const
    {
        return m_mapping;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Resource::isPrecompile() const
    {
        return m_precompile;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Resource::isInitialize() const
    {
        return m_initialize;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE uint32_t Resource::getCompileReferenceCount() const
    {
        uint32_t count = m_compileReferenceCount.getReferenceCount();

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Resource::isCache() const
    {
        bool referencing = m_cacheReferenceCount.isReferencing();

        return referencing;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Resource::isIgnored() const
    {
        return m_ignored;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        namespace Detail
        {
            template<class T>
            struct reinterpret_resource_cast_void_t
            {
                typedef void * type;
            };
            //////////////////////////////////////////////////////////////////////////
            template<class U>
            struct reinterpret_resource_cast_void_t<const U *>
            {
                typedef const void * type;
            };
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T reinterpretResourceCast( void * _resource )
        {
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Resource, std::remove_pointer_t<T>>, "reinterpret resource cast use on non 'Resourcable' type");

            if( _resource == nullptr )
            {
                return nullptr;
            }

            if( dynamic_cast<typename Detail::reinterpret_resource_cast_void_t<T>::type>(static_cast<T>(_resource)) == nullptr )
            {
                throw std::runtime_error( "reinterpret resource cast" );
            }
#endif

            return reinterpret_cast<T>(_resource);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T staticResourceCast( Resource * _resource )
        {
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Resource, std::remove_pointer_t<T>>, "static resource cast use on non 'Resourcable' type");

            if( _resource == nullptr )
            {
                return nullptr;
            }

            if( dynamic_cast<T>(_resource) == nullptr )
            {
                throw std::runtime_error( "static resource cast" );
            }
#endif

            return static_cast<T>(_resource);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T staticResourceCast( const Resource * _resource )
        {
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Resource, std::remove_pointer_t<T>>, "static resource cast use on non 'Resourcable' type");

            if( _resource == nullptr )
            {
                return nullptr;
            }

            if( dynamic_cast<T>(_resource) == nullptr )
            {
                throw std::runtime_error( "static resource cast" );
            }
#endif

            return static_cast<T>(_resource);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T staticResourceCast( const ResourcePtr & _resource )
        {
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Resource, std::remove_pointer_t<typename T::value_type>>, "static resource cast use on non 'Resourcable' type");

            if( _resource == nullptr )
            {
                return nullptr;
            }

            if( stdex::intrusive_dynamic_cast<T>(_resource) == nullptr )
            {
                throw std::runtime_error( "static resource cast" );
            }
#endif

            T t = stdex::intrusive_static_cast<T>(_resource);

            return t;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T dynamicResourceCast( Resource * _resource )
        {
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Resource, std::remove_pointer_t<T>>, "dynamic resource cast use on non 'Resourcable' type");
#endif

            T t = dynamic_cast<T>(_resource);

            return t;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T dynamicResourceCast( const Resource * _resource )
        {
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Resource, std::remove_pointer_t<T>>, "dynamic resource cast use on non 'Resourcable' type");
#endif

            T t = dynamic_cast<T>(_resource);

            return t;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T dynamicResourceCast( const ResourcePtr & _resource )
        {
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Resource, std::remove_pointer_t<typename T::value_type>>, "dynamic resource cast use on non 'Resourcable' type");
#endif

            T t = stdex::intrusive_dynamic_cast<T>(_resource);

            return t;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    using IntrusiveResourcePtr = IntrusivePtr<T, Resource>;
    //////////////////////////////////////////////////////////////////////////
}