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

namespace Mengine
{
    class ResourceServiceInterface;

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
        void setResourceService( ResourceServiceInterface * _service );
        MENGINE_INLINE ResourceServiceInterface * getResourceService() const;

    public:
        void setLocale( const ConstString & _locale );
        MENGINE_INLINE const ConstString & getLocale() const;

        void setGroupName( const ConstString & _groupName );
        MENGINE_INLINE const ConstString & getGroupName() const;

        void setGroupCache( bool _groupCache );
        MENGINE_INLINE bool isGroupCache() const;

        void setGlobal( bool _global );
        MENGINE_INLINE bool isGlobal() const;

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
        ResourceServiceInterface * m_service;

        uint32_t m_compileReferenceCount;
        uint32_t m_prefetchReferenceCount;

        ConstString m_locale;
        ConstString m_groupName;
        Tags m_tags;

        bool m_cache;
        bool m_groupCache;
        bool m_global;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Resource> ResourcePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE ResourceServiceInterface * Resource::getResourceService() const
    {
        return m_service;
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
    MENGINE_INLINE bool Resource::isGlobal() const
    {
        return m_global;
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
    template<class T>
    using IntrusiveResourcePtr = IntrusivePtr<T, Resource>;
    //////////////////////////////////////////////////////////////////////////
}
