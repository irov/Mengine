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
        ~Documentable();

#ifdef MENGINE_DEBUG
    public:
        void setDocument( const DocumentPtr & _document );
        const DocumentPtr & getDocument() const;

    protected:
        DocumentPtr m_document;
#endif
    };
}