#pragma once

#include "Config/Config.h"

//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DOCUMENT_ENABLE)
#   include "Kernel/Documentable.h"
#   include "Kernel/Identity.h"

#   include "Config/DynamicCast.h"
#   include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        void getDocumentableMessage( const Documentable * _documentable, const Identity * _identity, Char * const _message, size_t _capacity );
        //////////////////////////////////////////////////////////////////////////
        template<class D, class I>
        void getDocumentableMessage( const D * _documentable, const I * _identity, Char * const _message, size_t _capacity )
        {
            Detail::getDocumentableMessage( Helper::dynamicCast<const Mengine::Documentable *>( _documentable ), Helper::dynamicCast<const Mengine::Identity *>( _identity ), _message, _capacity );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class D, class I>
        const Char * getDocumentableThreadLocalMessage( const D * _documentable, const I * _identity, const Char * _message )
        {
            static MENGINE_THREAD_LOCAL Char message[4096 + 1] = {'\0'};
            Detail::getDocumentableMessage( _documentable, _identity, message, 4096 );
            StdString::strcat_safe( message, " [", 4096 );
            StdString::strcat_safe( message, _message, 4096 );
            StdString::strcat_safe( message, "]", 4096 );
            return message;

        }
        //////////////////////////////////////////////////////////////////////////
    }
}
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_DOCUMENTABLE_STR(Ptr, Message) Mengine::Detail::getDocumentableThreadLocalMessage(Ptr, Ptr, Message)
//////////////////////////////////////////////////////////////////////////
#else
#   define MENGINE_DOCUMENTABLE_STR(Ptr, Message) (Message)
#endif
//////////////////////////////////////////////////////////////////////////