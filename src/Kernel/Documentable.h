#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/Document.h"

namespace Mengine
{
    class Documentable
        : public Mixin
    {
    public:
        Documentable();
        ~Documentable();

    public:
        void setDocument( const Char * _doc );
        const Char * getDocument() const;

    protected:
        Document m_document;
    };
}