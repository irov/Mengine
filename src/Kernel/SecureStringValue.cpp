#include "SecureStringValue.h"

#include "Interface/SecureServiceInterface.h"

#include "Kernel/CRC32.h"
#include "Kernel/Blobject.h"
#include "Kernel/ContainerReader.h"
#include "Kernel/ContainerWriter.h"
#include "Kernel/Ravingcode.h"
#include "Kernel/Base64.h"

#include "Config/StdIO.h"
#include "Config/StdLib.h"
#include "Config/StdString.h"
#include "Config/Abs.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SecureStringValue::SecureStringValue()
        : m_hash( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SecureStringValue::~SecureStringValue()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SecureStringValue::setUnprotectedValue( const String & _value )
    {
        const Char * value_str = _value.c_str();
        String::size_type value_size = _value.size();

        union
        {
            uint32_t hash32;
            uint8_t hash8[4];
        };

        hash32 = Helper::makeCRC32( value_str, value_size );

        m_value.resize( value_size );

        for( String::size_type index = 0; index != value_size; ++index )
        {
            m_value[index] = _value[index] ^ hash8[index % 4];
        }

        m_hash = hash32;

        m_buffer.resize( value_size );
        m_buffer.assign( m_value.rbegin(), m_value.rend() );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SecureStringValue::getUnprotectedValue( String * const _value ) const
    {
        String::size_type value_size = m_value.size();

        for( String::size_type index = 0; index != value_size; ++index )
        {
            if( m_buffer[index] == m_value[value_size - index - 1] )
            {
                continue;
            }

            return false;
        }

        union
        {
            uint32_t hash32;
            uint8_t hash8[4];
        };

        hash32 = m_hash;

        _value->resize( value_size );

        Char * value_data = _value->data();

        for( String::size_type index = 0; index != value_size; ++index )
        {
            value_data[index] = m_value[index] ^ hash8[index % 4];
        }

        value_data[value_size] = '\0';

        const Char * value_str = _value->c_str();

        uint32_t hash = Helper::makeCRC32( value_str, value_size );

        if( m_hash != hash )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SecureStringValue::cmpSecureValue( const SecureStringValuePtr & _value, int32_t * const _result ) const
    {
        String base_unprotected_value;
        if( this->getUnprotectedValue( &base_unprotected_value ) == false )
        {
            return false;
        }

        String test_unprotected_value;
        if( _value->getUnprotectedValue( &test_unprotected_value ) == false )
        {
            return false;
        }

        *_result = MENGINE_STRCMP( test_unprotected_value.c_str(), base_unprotected_value.c_str() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SecureStringValue::saveBase64( String * const _base64 ) const
    {
        uint32_t value_size = (uint32_t)m_value.size();

        Blobject blob;
        ContainerWriter<Blobject> writer( blob );

        writer.writePOD( m_hash );
        writer.writePOD( value_size );
        writer.writeBuffer( m_value.c_str(), m_value.size() );
        writer.writeBuffer( m_buffer.c_str(), m_buffer.size() );

        HashType secureHash = SECURE_SERVICE()
            ->getSecureHash();

        uint32_t parrot = (uint32_t)secureHash;
        
        Blobject blob_raving;
        blob_raving.resize( blob.size() );

        Helper::ravingcode( parrot, blob.data(), blob.size(), blob_raving.data() );

        _base64->resize( blob.size() * 2 );

        Helper::encodeBase64( blob_raving.data(), blob_raving.size(), true, _base64->data(), _base64->size(), nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void SecureStringValue::loadBase64( const String & _base64 )
    {
        Blobject blob_raving;
        blob_raving.resize( _base64.size() );

        Helper::decodeBase64( _base64.c_str(), _base64.size(), blob_raving.data(), blob_raving.size(), nullptr );

        Blobject blob;
        blob.resize( _base64.size() );

        HashType secureHash = SECURE_SERVICE()
            ->getSecureHash();

        uint32_t parrot = (uint32_t)secureHash;

        Helper::ravingcode( parrot, blob_raving.data(), blob_raving.size(), blob.data() );

        ContainerReader<Blobject> reader( blob );

        reader.readPOD( m_hash );

        uint32_t value_size;
        reader.readPOD( value_size );

        m_value.resize( value_size );
        reader.readBuffer( m_value.data(), m_value.size() );

        m_buffer.resize( value_size );
        reader.readBuffer( m_buffer.data(), m_buffer.size() );        
    }
    //////////////////////////////////////////////////////////////////////////
}