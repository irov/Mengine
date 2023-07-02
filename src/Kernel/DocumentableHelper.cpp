#include "DocumentableHelper.h"

#include "Interface/DocumentInterface.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DOCUMENT_ENABLE)
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        static size_t getParentDocumentableMessage( const DocumentInterfacePtr & _document, Char * const _message, size_t _capacity )
        {
            const DocumentInterfacePtr & parent = _document->getParent();

            if( parent == nullptr )
            {
                return _capacity;
            }

            const Char * message = parent->getMessage();

            MENGINE_STRNCAT( _message, message, _capacity );

            size_t size = parent->getMessageSize();

            Char delim[] = " ==> ";

            MENGINE_STRNCAT( _message, delim, _capacity - size );

            size_t size_delim = MENGINE_STRLEN( delim );

            size_t capacity = Detail::getParentDocumentableMessage( parent, _message, _capacity - size - size_delim );

            return capacity;
        }
        //////////////////////////////////////////////////////////////////////////
        void getDocumentableMessage( const Documentable * _documentable, const Identity * _identity, Char * const _message, size_t _capacity )
        {
            MENGINE_STRNCPY( _message, "", _capacity );

            const DocumentInterfacePtr & document = _documentable->getDocument();

            if( document == nullptr )
            {
                MENGINE_STRNCAT( _message, "[None Document]", _capacity );

                return;
            }

            size_t capacity = Detail::getParentDocumentableMessage( document, _message, _capacity );

            const Char * message = document->getMessage();

            MENGINE_STRNCAT( _message, message, capacity );

            if( _identity != nullptr )
            {
                size_t size = document->getMessageSize();

                const ConstString & name = _identity->getName();

                Char suffix[] = " Name: ";

                size_t size_suffix = MENGINE_STRLEN( suffix );

                MENGINE_STRNCAT( _message, suffix, _capacity - size );
                MENGINE_STRNCAT( _message, name.c_str(), _capacity - size - size_suffix );
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
//////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////
