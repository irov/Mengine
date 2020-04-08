#include "Kernel/Documentable.h"
#include "Kernel/Document.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Documentable::Documentable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Documentable::~Documentable()
    {
    }
#ifdef MENGINE_DEBUG
    //////////////////////////////////////////////////////////////////////////
    void Documentable::setDocument( const DocumentPtr & _document )
    {
        m_document = _document;
    }
    //////////////////////////////////////////////////////////////////////////
    const DocumentPtr & Documentable::getDocument() const
    {
        return m_document;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
}
