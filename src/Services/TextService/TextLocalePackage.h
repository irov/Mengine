#pragma once

#include "Interface/MemoryInterface.h"
#include "Interface/ContentInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/Factorable.h"

#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TextLocalePackage
        : public Factorable
    {
        DECLARE_FACTORABLE( TextLocalePackage );

    public:
        TextLocalePackage();
        ~TextLocalePackage() override;

    public:
        bool initialize( const ConstString & _locale, const ContentInterfacePtr & _content );
        void finalize();

    public:
        const ContentInterfacePtr & getContent() const;

    public:
        MemoryInterfacePtr getXmlBuffer() const;

    protected:
        ConstString m_locale;

        ContentInterfacePtr m_content;

        MemoryBufferInterfacePtr m_memory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TextLocalePackage> TextLocalePackagePtr;
    //////////////////////////////////////////////////////////////////////////
}
