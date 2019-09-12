#include "IniUtil.h"

#include "Config/Stringstream.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Interface/SecureServiceInterface.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/Ravingcode.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"

#include <stdio.h>
#include <string.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

namespace Mengine
{
    namespace IniUtil
    {
        //////////////////////////////////////////////////////////////////////////
        IniStore::IniStore()
        {
            tinyini_initialize( &ini );
        }
        //////////////////////////////////////////////////////////////////////////
        IniStore::~IniStore()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool loadIni( IniStore & _ini, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
        {
            InputStreamInterfacePtr stream = FILE_SERVICE()
                ->openInputFile( _fileGroup, _filePath, false, MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( stream, false, "open ini file '%s:%s'"
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
            );

            _ini.path = _filePath;

            return loadIni( _ini, stream );
        }
        //////////////////////////////////////////////////////////////////////////
        bool loadIni( IniStore & _ini, const InputStreamInterfacePtr & _stream )
        {
            size_t size = _stream->size();

            MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
                ->createMemoryBuffer( MENGINE_DOCUMENT_FUNCTION );

            Char * memory_buffer = memory->newBuffer( size + 1, MENGINE_DOCUMENT_FUNCTION );

            _stream->read( memory_buffer, size );
            memory_buffer[size] = '\0';

            if( memory_buffer[0] == 'R' && memory_buffer[1] == 'G' && memory_buffer[2] == 'C' && memory_buffer[3] == 'D' )
            {
                memory_buffer += 4;

                HashType sequreHash = SECURE_SERVICE()
                    ->getSequreHash();

                uint32_t parrot = (uint32_t)sequreHash;

                Helper::ravingcode( parrot, memory_buffer, size - 4, memory_buffer );
            }

            if( tinyini_load( &_ini.ini, memory_buffer ) == TINYINI_RESULT_FAILURE )
            {
                LOGGER_ERROR( "ini invalid load '%s'"
                    , tinyini_get_error_message( &_ini.ini )
                );

                return false;
            }

            _ini.memory = memory;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool hasIniSection( const IniStore & _ini, const Char * _section )
        {
            if( tinyini_has_section( &_ini.ini, _section ) == TINYINI_RESULT_FAILURE )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool hasIniValue( const IniStore & _ini, const Char * _section, const Char * _key )
        {
            if( tinyini_has_property( &_ini.ini, _section, _key ) == TINYINI_RESULT_FAILURE )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, const Char ** _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            *_value = ini_value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, ConstString & _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            _value = Helper::stringizeString( ini_value );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, FilePath & _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

#ifdef MENGINE_DEBUG
            if( strchr( ini_value, '\\' ) != nullptr )
            {
                LOGGER_ERROR( "get ini '%s' filepath section '%s' key '%s' has invalid slash"
                    , _ini.path.c_str()
                    , _section
                    , _key
                );

                MENGINE_THROW_EXCEPTION( "get ini '%s' filepath section '%s' key '%s' has invalid slash"
                    , _ini.path.c_str()
                    , _section
                    , _key
                );

                return false;
            }
#endif

            const ConstString & cs_value = Helper::stringizeString( ini_value );

            _value = FilePath( cs_value );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, Tags & _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            Stringstream ss( ini_value );

            String single_string;
            while( ss >> single_string )
            {
                const Char * str_single_string = single_string.c_str();

                ConstString cstr_value = Helper::stringizeString( str_single_string );

                _value.addTag( cstr_value );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, Resolution & _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            uint32_t width;
            uint32_t height;
            if( sscanf( ini_value, "%u %u", &width, &height ) != 2 )
            {
                LOGGER_ERROR( "section '%s' key '%s' value '%s' invalid parse resolution"
                    , _section
                    , _key
                    , ini_value
                );

                return false;
            }

            _value.setWidth( width );
            _value.setHeight( height );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, Color & _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            float r;
            float g;
            float b;
            float a;
            if( sscanf( ini_value, "%f %f %f %f", &r, &g, &b, &a ) != 4 )
            {
                LOGGER_ERROR( "section '%s' key '%s' value '%s' invalid parse ColorValue"
                    , _section
                    , _key
                    , ini_value
                );

                return false;
            }

            float coef = 1.f / 255.f;

            r *= coef;
            g *= coef;
            b *= coef;
            a *= coef;

            _value.setRGBA( r, g, b, a );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, bool & _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            uint32_t tmp_value;

            if( sscanf( ini_value, "%" PRIu32, &tmp_value ) != 1 )
            {
                return false;
            }

            _value = tmp_value != 0;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, int8_t & _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            int32_t tmp_value;
            if( sscanf( ini_value, "%" PRId32, &tmp_value ) != 1 )
            {
                return false;
            }

            _value = (int8_t)tmp_value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, uint8_t & _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            uint32_t tmp_value;
            if( sscanf( ini_value, "%" PRIu32, &tmp_value ) != 1 )
            {
                return false;
            }

            _value = (uint8_t)tmp_value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, int32_t & _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            int32_t tmp_value;
            if( sscanf( ini_value, "%" PRId32, &tmp_value ) != 1 )
            {
                return false;
            }

            _value = tmp_value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, uint32_t & _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            uint32_t tmp_value;
            if( sscanf( ini_value, "%" PRIu32, &tmp_value ) != 1 )
            {
                return false;
            }

            _value = tmp_value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, uint64_t & _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            uint64_t tmp_value;
            if( sscanf( ini_value, "%" PRIu64, &tmp_value ) != 1 )
            {
                return false;
            }

            _value = tmp_value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, float & _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            float tmp_value;
            if( sscanf( ini_value, "%f", &tmp_value ) != 1 )
            {
                return false;
            }

            _value = tmp_value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, double & _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            double tmp_value;
            if( sscanf( ini_value, "%lf", &tmp_value ) != 1 )
            {
                return false;
            }

            _value = tmp_value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValues( const IniStore & _ini, const Char * _section, const Char * _key, VectorString & _values )
        {
            uint32_t count = tinyini_count_property_values( &_ini.ini, _section, _key );

            for( uint32_t index = 0; index != count; ++index )
            {
                const Char * value = tinyini_get_property_values( &_ini.ini, _section, _key, index );

                _values.emplace_back( String( value ) );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValues( const IniStore & _ini, const Char * _section, const Char * _key, VectorConstString & _values )
        {
            uint32_t count = tinyini_count_property_values( &_ini.ini, _section, _key );

            for( uint32_t index = 0; index != count; ++index )
            {
                const Char * value = tinyini_get_property_values( &_ini.ini, _section, _key, index );

                ConstString cs = Helper::stringizeString( value );

                _values.emplace_back( cs );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValues( const IniStore & _ini, const Char * _section, const Char * _key, VectorFilePath & _values )
        {
            uint32_t count = tinyini_count_property_values( &_ini.ini, _section, _key );

            for( uint32_t index = 0; index != count; ++index )
            {
                const Char * value = tinyini_get_property_values( &_ini.ini, _section, _key, index );

                FilePath cs = Helper::stringizeFilePath( value );

                _values.emplace_back( cs );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValues( const IniStore & _ini, const Char * _section, const Char * _key, VectorAspectRatioViewports & _values )
        {
            uint32_t count = tinyini_count_property_values( &_ini.ini, _section, _key );

            for( uint32_t index = 0; index != count; ++index )
            {
                const Char * ini_value = tinyini_get_property_values( &_ini.ini, _section, _key, index );

                AspectRatioViewport arv;

                if( sscanf( ini_value, "%f %f %f %f %f %f"
                    , &arv.width
                    , &arv.height
                    , &arv.viewport.begin.x
                    , &arv.viewport.begin.y
                    , &arv.viewport.end.x
                    , &arv.viewport.end.y
                ) != 6 )
                {
                    LOGGER_ERROR( "section '%s' key '%s' value '%s' invalid parse aspect ration"
                        , _section
                        , _key
                        , ini_value
                    );

                    return false;
                }

                _values.emplace_back( arv );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniAllSettings( const IniStore & _ini, const Char * _section, MapParams & _values )
        {
            uint32_t count = tinyini_count_properties( &_ini.ini, _section );

            for( uint32_t index = 0; index != count; ++index )
            {
                const Char * key;
                const Char * value;
                if( tinyini_get_properties( &_ini.ini, _section, index, &key, &value ) == TINYINI_RESULT_FAILURE )
                {
                    return false;
                }

                ConstString c_key = Helper::stringizeString( key );

                _values.emplace( c_key, value );
            }

            return true;
        }
    }
}