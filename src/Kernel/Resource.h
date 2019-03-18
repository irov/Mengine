#pragma once

#include "Interface/LoadableInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/Compilable.h"
#include "Kernel/Identity.h"
#include "Kernel/Reference.h"
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
        , public Reference
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
        inline const ConstString & getLocale() const;

        void setFileGroup( const FileGroupInterfacePtr & _fileGroup );
        inline const FileGroupInterfacePtr & getFileGroup() const;

        void setGroupName( const ConstString & _groupName );
        inline const ConstString & getGroupName() const;

    public:
        void setTags( const Tags & _tags );
        const Tags & getTags() const;

    public:
        bool initialize();

    protected:
        virtual bool _initialize();

    public:
        bool cache();
        void uncache();

    public:
        inline bool isCache() const;

    protected:
        virtual void _cache();
        virtual void _uncache();

    protected:
        bool _incrementZero() override;
        void _decrementZero() override;

    protected:
        ConstString m_locale;
        FileGroupInterfacePtr m_fileGroup;
        ConstString m_groupName;
        Tags m_tags;

        bool m_cache;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Resource> ResourcePtr;
    //////////////////////////////////////////////////////////////////////////
    inline const ConstString & Resource::getLocale() const
    {
        return m_locale;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const FileGroupInterfacePtr & Resource::getFileGroup() const
    {
        return m_fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const ConstString & Resource::getGroupName() const
    {
        return m_groupName;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool Resource::isCache() const
    {
        return m_cache;
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    using IntrusiveResourcePtr = IntrusivePtr<T, Resource>;
	//////////////////////////////////////////////////////////////////////////
}
