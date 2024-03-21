#include "SecureService.h"

#include "Interface/PrototypeServiceInterface.h"

#include "SecureUnsignedValue.h"
#include "SecureStringValue.h"

#include "Kernel/Logger.h"
#include "Kernel/SHA1.h"
#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/Ravingcode.h"

#include "Config/StdString.h"

#ifndef MENGINE_SECURE_VALUE
#define MENGINE_SECURE_VALUE "0123456789A"
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SecureService, Mengine::SecureService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SecureService::SecureService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SecureService::~SecureService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SecureService::_initializeService()
    {
        const Char * secure = GET_OPTION_VALUE( "secure", MENGINE_SECURE_VALUE );

        size_t secure_len = MENGINE_STRLEN( secure );

        Helper::makeSHA1( secure, secure_len, reinterpret_cast<uint8_t *>(m_secureHash), sizeof( m_secureHash ) );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "SecureUnsignedValue" ), ConstString::none(), Helper::makeDefaultPrototypeGenerator<SecureUnsignedValue, 64>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "SecureStringValue" ), ConstString::none(), Helper::makeDefaultPrototypeGenerator<SecureStringValue, 64>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SecureService::_finalizeService()
    {
        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "SecureUnsignedValue" ), ConstString::none(), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "SecureStringValue" ), ConstString::none(), nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void SecureService::protectData( uint32_t _complexity, const void * _in, size_t _size, void * const _out ) const
    {
        MENGINE_ASSERTION_FATAL( _complexity <= 1, "complexity %u"
            , _complexity
        );

        Helper::ravingcode( m_secureHash, _complexity, _in, _size, _out );
    }
    //////////////////////////////////////////////////////////////////////////
    void SecureService::unprotectData( uint32_t _complexity, const void * _in, size_t _size, void * const _out ) const
    {
        MENGINE_ASSERTION_FATAL( _complexity <= 1, "complexity %u"
            , _complexity
        );

        Helper::ravingcode( m_secureHash, _complexity, _in, _size, _out );
    }
    //////////////////////////////////////////////////////////////////////////
}
