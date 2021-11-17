#include "JSONConfig.h"

#include "Interface/MemoryServiceInterface.h"
#include "Interface/SecureServiceInterface.h"

#include "JSONCast.h"

#include "Kernel/Exception.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/Logger.h"
#include "Kernel/Ravingcode.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ArrayString.h"
#include "Kernel/FileStreamHelper.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////        
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static bool getJSONValue( const JSONStorageInterfacePtr & _storage, const Char * _section, const Char * _key, T * const _value )
        {
            const jpp::object & j_root = _storage->getJSON();

            jpp::object j_section;
            if( j_root.exist( _section, &j_section ) == false )
            {
                return false;
            }

            jpp::object j_value;
            if( j_section.exist( _key, &j_value ) == false )
            {
                return false;
            }

            *_value = j_value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static void getJSONValues( const JSONStorageInterfacePtr & _storage, const Char * _section, const Char * _key, T * const _values )
        {
            typedef typename T::value_type value_type;

            const jpp::object & j_root = _storage->getJSON();

            jpp::object j_section;
            if( j_root.exist( _section, &j_section ) == false )
            {
                return;
            }

            jpp::object j_value;
            if( j_section.exist( _key, &j_value ) == false )
            {
                return;
            }

            for( const jpp::object & j_element : jpp::array( j_value ) )
            {
                value_type element = j_element;

                _values->emplace_back( element );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static bool hasJSONValueT2( const JSONStorageInterfacePtr & _storage, const Char * _prefix, const Tags & _platform, const Char * _section, const Char * _key, T * const _value )
        {
            ArrayString<128> platform_section;
            platform_section.append( _prefix );
            platform_section.append( _section );

            const VectorConstString & tags = _platform.getValues();

            for( const ConstString & tag : tags )
            {
                platform_section.append( '-' );
                platform_section.append( tag );

                if( Detail::getJSONValue( _storage, platform_section.c_str(), _key, _value ) == true )
                {
                    return true;
                }
            }

            ArrayString<128> section;
            section.append( _prefix );
            section.append( _section );

            if( Detail::getJSONValue( _storage, section.c_str(), _key, _value ) == true )
            {
                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static bool hasJSONValueT( const VectorJSONStorages & _storages, const Tags & _platform, const Char * _section, const Char * _key, const T & _default, T * const _value )
        {
            for( const JSONStorageInterfacePtr & storage : _storages )
            {
#ifdef MENGINE_BUILD_PUBLISH
                if( Detail::hasJSONValueT2( storage, "Publish-", _platform, _section, _key, _value ) == true )
                {
                    return true;
                }
#endif

                if( Detail::hasJSONValueT2( storage, MENGINE_MASTER_VALUE( "Alpha-", "Develop-" ), _platform, _section, _key, _value ) == true )
                {
                    return true;
                }

                if( Detail::hasJSONValueT2( storage, "", _platform, _section, _key, _value ) == true )
                {
                    return true;
                }
            }

            *_value = _default;

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static void calcJSONValuesT2( const JSONStorageInterfacePtr & _storage, const Char * _prefix, const Tags & _platform, const Char * _section, const Char * _key, T * const _value )
        {
            ArrayString<128> platform_section;
            platform_section.append( _prefix );
            platform_section.append( _section );

            const VectorConstString & tags = _platform.getValues();

            for( const ConstString & tag : tags )
            {
                platform_section.append( '-' );
                platform_section.append( tag );

                Detail::getJSONValues( _storage, platform_section.c_str(), _key, _value );
            }

            ArrayString<128> section;
            section.append( _prefix );
            section.append( _section );

            Detail::getJSONValues( _storage, section.c_str(), _key, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static void calcJSONValuesT( const VectorJSONStorages & _storages, const Tags & _platform, const Char * _section, const Char * _key, T * const _value )
        {
            for( const JSONStorageInterfacePtr & storage : _storages )
            {
#ifdef MENGINE_BUILD_PUBLISH
                Detail::calcJSONValuesT2( storage, "Publish-", _platform, _section, _key, _value );
#endif

                Detail::calcJSONValuesT2( storage, MENGINE_MASTER_VALUE( "Alpha-", "Develop-" ), _platform, _section, _key, _value );

                Detail::calcJSONValuesT2( storage, "", _platform, _section, _key, _value );
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    JSONConfig::JSONConfig()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONConfig::~JSONConfig()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONConfig::setPlatformTags( const Tags & _platformTags )
    {
        m_platformTags = _platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    const Tags & JSONConfig::getPlatformTags() const
    {
        return m_platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::load( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "invalid stream config" );

        size_t size = _stream->size();

        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryBuffer( _doc );

        Char * memory_buffer = memory->newBuffer( size + 1 );

        _stream->read( memory_buffer, size );
        memory_buffer[size] = '\0';

        if( memory_buffer[0] == 'R' && memory_buffer[1] == 'G' && memory_buffer[2] == 'C' && memory_buffer[3] == 'D' )
        {
            memory_buffer += 4;

            uint64_t secureHash = SECURE_SERVICE()
                ->getSecureHash();

            Helper::ravingcode( secureHash, memory_buffer, size - 4, memory_buffer );
        }

        JSONStorageInterfacePtr storage = JSON_SERVICE()
            ->loadJSONStreamFromMemory( memory, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( storage );

        m_storages.emplace_back( storage );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONConfig::unload()
    {
        m_storages.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::existValue( const Char * _section, const Char * _key ) const
    {
        for( const JSONStorageInterfacePtr & storage : m_storages )
        {
            const jpp::object & j_storage = storage->getJSON();

            ArrayString<128> platform_section;
            platform_section.append( _section );

            const VectorConstString & tags = m_platformTags.getValues();

            for( const ConstString & tag : tags )
            {
                platform_section.append( '-' );
                platform_section.append( tag );

                jpp::object j_tag_section;
                if( j_storage.exist( platform_section.c_str(), &j_tag_section ) == false )
                {
                    continue;
                }

                if( j_tag_section.exist( _key, nullptr ) == false )
                {
                    continue;
                }

                return true;
            }

            jpp::object j_section;
            if( j_storage.exist( _section, &j_section ) == false )
            {
                continue;
            }

            if( j_section.exist( _key, nullptr ) == false )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, bool _default, bool * const _value ) const
    {
        return Detail::hasJSONValueT( m_storages, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, int8_t _default, int8_t * const  _value ) const
    {
        return Detail::hasJSONValueT( m_storages, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, uint8_t _default, uint8_t * const _value ) const
    {
        return Detail::hasJSONValueT( m_storages, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, int32_t _default, int32_t * const _value ) const
    {
        return Detail::hasJSONValueT( m_storages, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, uint32_t _default, uint32_t * const _value ) const
    {
        return Detail::hasJSONValueT( m_storages, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, int64_t _default, int64_t * const _value ) const
    {
        return Detail::hasJSONValueT( m_storages, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, uint64_t _default, uint64_t * const _value ) const
    {
        return Detail::hasJSONValueT( m_storages, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, float _default, float * const _value ) const
    {
        return Detail::hasJSONValueT( m_storages, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, double _default, double * const _value ) const
    {
        return Detail::hasJSONValueT( m_storages, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, const Char * _default, const Char ** const _value ) const
    {
        return Detail::hasJSONValueT( m_storages, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, const ConstString & _default, ConstString * const _value ) const
    {
        return Detail::hasJSONValueT( m_storages, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, const FilePath & _default, FilePath * const _value ) const
    {
        return Detail::hasJSONValueT( m_storages, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, const Tags & _default, Tags * const _value ) const
    {
        return Detail::hasJSONValueT( m_storages, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, const Resolution & _default, Resolution * const _value ) const
    {
        return Detail::hasJSONValueT( m_storages, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, const Color & _default, Color * const _value ) const
    {
        return Detail::hasJSONValueT( m_storages, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONConfig::getValues( const Char * _section, const Char * _key, VectorAspectRatioViewports * const _values ) const
    {
        Detail::calcJSONValuesT( m_storages, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONConfig::getValues( const Char * _section, const Char * _key, VectorFilePath * const _values ) const
    {
        Detail::calcJSONValuesT( m_storages, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONConfig::getValues( const Char * _section, const Char * _key, VectorConstString * const _values ) const
    {
        Detail::calcJSONValuesT( m_storages, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONConfig::getValues( const Char * _section, const Char * _key, VectorString * const _values ) const
    {
        Detail::calcJSONValuesT( m_storages, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasSection( const Char * _section ) const
    {
        for( const JSONStorageInterfacePtr & storage : m_storages )
        {
            const jpp::object & j_storage = storage->getJSON();

            ArrayString<128> platform_section;
            platform_section.append( _section );

            const VectorConstString & tags = m_platformTags.getValues();

            for( const ConstString & tag : tags )
            {
                platform_section.append( '-' );
                platform_section.append( tag );

                if( j_storage.exist( platform_section.c_str(), nullptr ) == false )
                {
                    continue;
                }

                return true;
            }

            if( j_storage.exist( _section, nullptr ) == false )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}
