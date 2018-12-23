#include "IniUtil.h"

#include "Config/Stringstream.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "Kernel/Logger.h"

#include <stdio.h>
#include <string.h>

namespace Mengine
{
    namespace IniUtil
    {
        //////////////////////////////////////////////////////////////////////////
        bool loadIni( IniStore & _ini, const FileGroupInterfacePtr & _category, const FilePath & _path )
        {
            InputStreamInterfacePtr stream = FILE_SERVICE()
                ->openInputFile( _category, _path, false );

            if( stream == nullptr )
            {
                LOGGER_ERROR( "loadIni invalid open ini file '%s:%s'"
                    , _category->getName().c_str()
                    , _path.c_str()
                );

                return false;
            }

            _ini.path = _path;

            return loadIni( _ini, stream );
        }
        //////////////////////////////////////////////////////////////////////////
        bool loadIni( IniStore & _ini, const InputStreamInterfacePtr & _input )
        {
            size_t size = _input->size();

            if( size >= MENGINE_INI_BUFFER_SIZE )
            {
                LOGGER_ERROR( "IniUtil::loadIni ini size %u max %u"
                    , size
                    , MENGINE_INI_BUFFER_SIZE
                );

                return false;
            }

            _input->read( _ini.buff, size );
            _ini.buff[size] = '\0';

            if( _ini.load( _ini.buff ) == false )
            {
                LOGGER_ERROR( "IniUtil::loadIni ini invalid load '%s'"
                    , _ini.getError()
                );

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool hasIniValue( const IniStore & _ini, const Char * _section, const Char * _key )
        {
            bool successful = _ini.hasSettingValue( _section, _key );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, const Char ** _value )
        {
            const Char * ini_value = _ini.getSettingValue( _section, _key );

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
            const Char * ini_value = _ini.getSettingValue( _section, _key );

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
            const Char * ini_value = _ini.getSettingValue( _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

#ifndef NDEBUG
            if( strstr( ini_value, "\\" ) != nullptr )
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
            const Char * ini_value = _ini.getSettingValue( _section, _key );

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
            const Char * ini_value = _ini.getSettingValue( _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            uint32_t width;
            uint32_t height;
            if( sscanf( ini_value, "%u %u", &width, &height ) != 2 )
            {
                LOGGER_ERROR( "getIniValue section %s key %s value %s invalid parse resolution"
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
            const Char * ini_value = _ini.getSettingValue( _section, _key );

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
                LOGGER_ERROR( "getIniValue section %s key %s value %s invalid parse ColourValue"
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
            const Char * ini_value = _ini.getSettingValue( _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            uint32_t tmp_value;

            if( sscanf( ini_value, "%u", &tmp_value ) != 1 )
            {
                return false;
            }

            _value = tmp_value != 0;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, int32_t & _value )
        {
            const Char * ini_value = _ini.getSettingValue( _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            int32_t tmp_value;
            if( sscanf( ini_value, "%d", &tmp_value ) != 1 )
            {
                return false;
            }

            _value = tmp_value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, uint32_t & _value )
        {
            const Char * ini_value = _ini.getSettingValue( _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            uint32_t tmp_value;
            if( sscanf( ini_value, "%u", &tmp_value ) != 1 )
            {
                return false;
            }

            _value = tmp_value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, uint64_t & _value )
        {
            const Char * ini_value = _ini.getSettingValue( _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            uint64_t tmp_value;
            if( sscanf( ini_value, "%llu", &tmp_value ) != 1 )
            {
                return false;
            }

            _value = tmp_value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, float & _value )
        {
            const Char * ini_value = _ini.getSettingValue( _section, _key );

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
            const Char * ini_value = _ini.getSettingValue( _section, _key );

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
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, VectorString & _values )
        {
            uint32_t count = _ini.countSettingValues( _section, _key );

            for( uint32_t index = 0; index != count; ++index )
            {
                const Char * value = _ini.getSettingValues( _section, _key, index );

                _values.emplace_back( String( value ) );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, VectorConstString & _values )
        {
            uint32_t count = _ini.countSettingValues( _section, _key );

            for( uint32_t index = 0; index != count; ++index )
            {
                const Char * value = _ini.getSettingValues( _section, _key, index );

                ConstString cs = Helper::stringizeString( value );

                _values.emplace_back( cs );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, VectorAspectRatioViewports & _values )
        {
            uint32_t count = _ini.countSettingValues( _section, _key );

            for( uint32_t index = 0; index != count; ++index )
            {
                const Char * ini_value = _ini.getSettingValues( _section, _key, index );

                AspectRatioViewports arv;

                if( sscanf( ini_value, "%f %f %f %f %f %f"
                    , &arv.width
                    , &arv.height
                    , &arv.viewport.begin.x
                    , &arv.viewport.begin.y
                    , &arv.viewport.end.x
                    , &arv.viewport.end.y
                ) != 6 )
                {
                    LOGGER_ERROR( "getIniValue section %s key %s value %s invalid parse aspect ration"
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
            uint32_t count = _ini.countSettings( _section );

            for( uint32_t index = 0; index != count; ++index )
            {
                const Char * key;
                const Char * value;
                _ini.getSettings( _section, index, &key, &value );

                ConstString c_key = Helper::stringizeString( key );

                _values.emplace( c_key, value );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeIniSection( const OutputStreamInterfacePtr & _file, const Char * _section, uint32_t _sectionSize )
        {
            _file->write( _section, _sectionSize );
            _file->write( "\n", sizeof( "\n" ) - 1 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeIniSetting( const OutputStreamInterfacePtr & _file, const Char * _key, const ConstString & _value )
        {
            size_t len = strlen( _key );
            _file->write( _key, len );
            _file->write( " = ", sizeof( " = " ) - 1 );

            _file->write( _value.c_str(), _value.size() );
            _file->write( "\n", sizeof( "\n" ) - 1 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeIniSetting( const OutputStreamInterfacePtr & _file, const Char * _key, const String & _value )
        {
            size_t len = strlen( _key );
            _file->write( _key, len );
            _file->write( " = ", sizeof( " = " ) - 1 );
            _file->write( _value.c_str(), _value.size() );
            _file->write( "\n", sizeof( "\n" ) - 1 );

            return true;
        }
    }
}