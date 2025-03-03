#include "SecureService.h"

#include "SecureUnsignedValue.h"
#include "SecureStringValue.h"

#include "Kernel/Logger.h"
#include "Kernel/SHA1.h"
#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/Ravingcode.h"
#include "Kernel/PrototypeHelper.h"

#include "Config/StdString.h"

#include "Configuration/SecureValue.h"

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

        size_t secure_len = StdString::strlen( secure );

        Helper::makeSHA1( secure, secure_len, reinterpret_cast<uint8_t *>(m_secureHash), sizeof( m_secureHash ) );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( ConstString::none(), SecureUnsignedValue::getFactorableType(), Helper::makeDefaultPrototypeGenerator<SecureUnsignedValue, 64>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( ConstString::none(), SecureStringValue::getFactorableType(), Helper::makeDefaultPrototypeGenerator<SecureStringValue, 64>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SecureService::_finalizeService()
    {
        Helper::removePrototype<SecureUnsignedValue>( ConstString::none() );
        Helper::removePrototype<SecureStringValue>( ConstString::none() );
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
