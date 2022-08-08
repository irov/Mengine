#pragma once

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Document> DocumentPtr;
    //////////////////////////////////////////////////////////////////////////
    class Documentable
        : public Mixin
    {
    public:
        Documentable();
        ~Documentable() override;

#ifdef MENGINE_DOCUMENT_ENABLE
    public:
        void setDocument( const DocumentPtr & _document );
        const DocumentPtr & getDocument() const;

    protected:
        DocumentPtr m_document;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Documentable> DocumentablePtr;
    //////////////////////////////////////////////////////////////////////////
}