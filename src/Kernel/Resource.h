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
        void setLocale( const ConstString & _locale );
        MENGINE_INLINE const ConstString & getLocale() const;

        void setFileGroup( const FileGroupInterfacePtr & _fileGroup );
        MENGINE_INLINE const FileGroupInterfacePtr & getFileGroup() const;

        void setGroupName( const ConstString & _groupName );
        MENGINE_INLINE const ConstString & getGroupName() const;

    public:
        void setTags( const Tags & _tags );
        const Tags & getTags() const;

    public:
        bool initialize();

    protected:
        virtual bool _initialize();

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
        uint32_t m_compileReferenceCount;
        uint32_t m_prefetchReferenceCount;

        ConstString m_locale;
        FileGroupInterfacePtr m_fileGroup;
        ConstString m_groupName;
        Tags m_tags;

        bool m_cache;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Resource> ResourcePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const ConstString & Resource::getLocale() const
    {
        return m_locale;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const FileGroupInterfacePtr & Resource::getFileGroup() const
    {
        return m_fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const ConstString & Resource::getGroupName() const
    {
        return m_groupName;
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
