#pragma once

#include "Interface/DocumentInterface.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Documentable
        : public Mixin
    {
    public:
        Documentable();
        ~Documentable() override;

#if defined(MENGINE_DOCUMENT_ENABLE)
    public:
        void setDocument( const DocumentInterfacePtr & _document );
        const DocumentInterfacePtr & getDocument() const;

    protected:
        DocumentInterfacePtr m_document;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Documentable> DocumentablePtr;
    //////////////////////////////////////////////////////////////////////////
}