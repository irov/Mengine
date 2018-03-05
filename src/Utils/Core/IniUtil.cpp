#   include "Utils/Core/IniUtil.h"

#   include "Interface/UnicodeInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/StringizeInterface.h"

#   include "Logger/Logger.h"

#	include <sstream>

#   include <stdio.h>
#   include <string.h>

namespace Menge
{
    namespace IniUtil
    {
		//////////////////////////////////////////////////////////////////////////
		bool loadIni( IniStore & _ini, const ConstString & _category, const FilePath & _path )
		{
			InputStreamInterfacePtr stream = FILE_SERVICE()
				->openInputFile( _category, _path, false );

			if( stream == nullptr )
			{
                LOGGER_ERROR( "loadIni invalid open ini file '%s:%s'"
					, _category.c_str()
					, _path.c_str()
					);

				return false;
			}

			_ini.category = _category;
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
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, WString & _value )
        {
            const Char * ini_value = _ini.getSettingValue( _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            if( Helper::utf8ToUnicodeSize( ini_value, UNICODE_UNSIZE, _value ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, String & _value )
        {
            const Char * ini_value = _ini.getSettingValue( _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            _value.assign( ini_value );

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

#	ifdef _DEBUG
			if( strstr( ini_value, "\\" ) != nullptr )
			{
				LOGGER_ERROR( "get ini '%s:%s' filepath section '%s' key '%s' has invalid slash"
					, _ini.category.c_str()
					, _ini.path.c_str()
					, _section
					, _key
					);

				MENGINE_THROW_EXCEPTION( "get ini '%s:%s' filepath section '%s' key '%s' has invalid slash"
					, _ini.category.c_str()
					, _ini.path.c_str()
					, _section
					, _key
				);

				return false;
			}
#	endif

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
			
			std::stringstream ss( ini_value );

			std::string single_string;
			while( ss >> single_string )
			{
				const char * str_single_string = single_string.c_str();

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
		bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, ColourValue & _value )
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
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, TVectorString & _values )
        {
            uint32_t count = _ini.countSettingValues( _section, _key );

            for( uint32_t index = 0; index != count; ++index )
            {
                const char * value = _ini.getSettingValues( _section, _key, index );

                _values.push_back( String(value) );
            }

            return true;
        }
		//////////////////////////////////////////////////////////////////////////
		bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, TVectorConstString & _values )
		{
			uint32_t count = _ini.countSettingValues( _section, _key );

			for( uint32_t index = 0; index != count; ++index )
			{
				const char * value = _ini.getSettingValues( _section, _key, index );

				ConstString cs = Helper::stringizeString( value );

				_values.push_back( cs );
			}

			return true;
		}
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, TVectorWString & _values )
        {
            uint32_t count = _ini.countSettingValues( _section, _key );

            for( uint32_t index = 0; index != count; ++index )
            {
                const char * value = _ini.getSettingValues( _section, _key, index );

                WString w_value;
                Helper::utf8ToUnicodeSize( value, UNICODE_UNSIZE, w_value );

                _values.push_back( w_value );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, TVectorAspectRatioViewports & _values )
        {
            uint32_t count = _ini.countSettingValues( _section, _key );

            for( uint32_t index = 0; index != count; ++index )
            {
                const char * ini_value = _ini.getSettingValues( _section, _key, index );

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

                _values.push_back( arv );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniAllSettings( const IniStore & _ini, const Char * _section, TMapWParams & _values )
        {
            uint32_t count = _ini.countSettings( _section );

            for( uint32_t index = 0; index != count; ++index )
            {
                const char * key;
                const char * value;
                _ini.getSettings( _section, index, &key, &value );

                ConstString c_key = Helper::stringizeString( key );
                
                WString w_value;
                if( Helper::utf8ToUnicodeSize( value, UNICODE_UNSIZE, w_value ) == false )
                {
                    return false;
                }

                _values.insert( std::make_pair( c_key, w_value ) );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeIniSection( const OutputStreamInterfacePtr & _file, const char * _section, uint32_t _sectionSize )
        {
            _file->write( _section, _sectionSize );
            _file->write( "\n", sizeof("\n") - 1 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeIniSetting( const OutputStreamInterfacePtr & _file, const char * _key, const ConstString & _value )
        {
            size_t len = strlen( _key );
            _file->write( _key, len );
            _file->write( " = ", sizeof( " = " ) - 1 );

            _file->write( _value.c_str(), _value.size() );
            _file->write( "\n", sizeof( "\n" ) - 1 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeIniSetting( const OutputStreamInterfacePtr & _file, const char * _key, const WString & _value )
        {
            size_t len = strlen(_key);
            _file->write( _key, len );
            _file->write( " = ", sizeof(" = ") - 1 );

            size_t utf8_size = 0;

            Char utf8_value[512];
            if( UNICODE_SYSTEM()
                ->unicodeToUtf8( _value.c_str(), _value.size(), utf8_value, 512, &utf8_size ) == false )
            {
                return false;
            }

            utf8_value[utf8_size] = '\0';

            _file->write( utf8_value, utf8_size );
            _file->write( "\n", sizeof("\n") - 1 );

            return true;
        }
    }
}