#pragma once

#include "Kernel/Documentable.h"
#include "Kernel/Identity.h"

//////////////////////////////////////////////////////////////////////////
#if MENGINE_DOCUMENT_ENABLE
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        void getDocumentableMessage( const Documentable * _documentable, const Identity * _identity, Char * const _message, size_t _capacity );
    }
}
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_DOCUMENTABLE_STR(Ptr, Empty) [](auto _ptr){ static MENGINE_THREAD_LOCAL Char message[4096] = {'\0'}; Detail::getDocumentableMessage(dynamic_cast<const Mengine::Documentable*>(_ptr), dynamic_cast<const Mengine::Identity*>(_ptr), message, 4096); return message;}(Ptr)
//////////////////////////////////////////////////////////////////////////
#else
#   define MENGINE_DOCUMENTABLE_STR(Ptr, Empty) (Empty)
#endif
//////////////////////////////////////////////////////////////////////////