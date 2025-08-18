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

            StdString::strcat_safe( _message, message, _capacity );

            Char delim[] = " ==> ";

            StdString::strcat_safe( _message, delim, _capacity );

            size_t capacity = Detail::getParentDocumentableMessage( parent, _message, _capacity );

            return capacity;
        }
        //////////////////////////////////////////////////////////////////////////
        void getDocumentableMessage( const Documentable * _documentable, const Identity * _identity, Char * const _message, size_t _capacity )
        {
            StdString::strcpy_safe( _message, "", _capacity );

            const DocumentInterfacePtr & document = _documentable->getDocument();

            if( document == nullptr )
            {
                StdString::strcat_safe( _message, "[None Document]", _capacity );

                return;
            }

            size_t capacity = Detail::getParentDocumentableMessage( document, _message, _capacity );

            const Char * message = document->getMessage();

            StdString::strcat_safe( _message, message, capacity );

            if( _identity != nullptr )
            {
                const ConstString & name = _identity->getName();

                Char suffix[] = " Name: ";

                StdString::strcat_safe( _message, suffix, _capacity );
                StdString::strcat_safe( _message, name.c_str(), _capacity );
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
//////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////
