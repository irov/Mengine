#pragma once

#include "Kernel/Documentable.h"
#include "Kernel/Identity.h"

//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DOCUMENT_ENABLE)
#   include "Config/DynamicCast.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        void getDocumentableMessage( const Documentable * _documentable, const Identity * _identity, Char * const _message, size_t _capacity );

        template<class D, class I>
        void getDocumentableMessage( const D * _documentable, const I * _identity, Char * const _message, size_t _capacity )
        {
            Detail::getDocumentableMessage( Helper::dynamicCast<const Mengine::Documentable *>( _documentable ), Helper::dynamicCast<const Mengine::Identity *>( _identity ), _message, _capacity );
        }

        template<class D, class I>
        const Char * getDocumentableThreadLocalMessage( const D * _documentable, const I * _identity )
        {
            static MENGINE_THREAD_LOCAL Char message[4096] = {'\0'};
            Detail::getDocumentableMessage( _documentable, _identity, message, 4096 );
            return message;
        
        }
    }
}
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_DOCUMENTABLE_STR(Ptr, Empty) Mengine::Detail::getDocumentableThreadLocalMessage(Ptr, Ptr)
//////////////////////////////////////////////////////////////////////////
#else
#   define MENGINE_DOCUMENTABLE_STR(Ptr, Empty) (Empty)
#endif
//////////////////////////////////////////////////////////////////////////