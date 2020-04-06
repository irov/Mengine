#pragma once

#include "Interface/LoadableInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/Compilable.h"
#include "Kernel/Identity.h"
#include "Kernel/Scriptable.h"
#include "Kernel/Visitable.h"
#include "Kernel/Unknowable.h"
#include "Kernel/Contentable.h"
#include "Kernel/Magicable.h"
#include "Kernel/Factorable.h"
#include "Kernel/Factory.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/Tags.h"

#ifdef MENGINE_DEBUG
#include <type_traits>
#endif

namespace Mengine
{
    class ResourceBankInterface;

    class Resource
        : public Factorable
        , public Compilable
        , public Contentable
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
        void _destroy() override;

    protected:
        ResourceBankInterface * m_resourceBank;

        uint32_t m_compileReferenceCount;
        uint32_t m_prefetchReferenceCount;

        ConstString m_locale;
        ConstString m_groupName;
        Tags m_tags;

        bool m_cache;
        bool m_groupCache;
        bool m_keep;
        bool m_mapping;
        bool m_precompile;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Resource> ResourcePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE ResourceBankInterface * Resource::getResourceBank() const
    {
        return m_resourceBank;
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
    MENGINE_INLINE uint32_t Resource::getCompileReferenceCount() const
    {
        return m_compileReferenceCount;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Resource::isCache() const
    {
        return m_cache;
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

            try
            {
                if( dynamic_cast<typename Detail::reinterpret_resource_cast_void_t<T>::type>(static_cast<T>(_resource)) == nullptr )
                {
                    throw;
                }
            }
            catch( const std::exception & )
            {
                throw;
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
                throw;
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
                throw;
            }
#endif

            return static_cast<T>(_resource);
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
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    using IntrusiveResourcePtr = IntrusivePtr<T, Resource>;
    //////////////////////////////////////////////////////////////////////////
}