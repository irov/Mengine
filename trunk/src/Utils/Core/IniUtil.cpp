#   include "Utils/Core/IniUtil.h"

#   include "Interface/UnicodeInterface.h"
#   include "Interface/StringizeInterface.h"

#   include "Logger/Logger.h"

#   include <stdio.h>
#   include <string.h>

namespace Menge
{
    namespace IniUtil
    {
		//////////////////////////////////////////////////////////////////////////
		bool loadIni( IniStore & _ini, const InputStreamInterfacePtr & _input, ServiceProviderInterface * _serviceProvider )
		{
			size_t size = _input->size();

			if( size >= MENGINE_INI_BUFFER_SIZE )
			{
				LOGGER_ERROR(_serviceProvider)("IniUtil::loadIni ini size %d max %d"
					, size
					, MENGINE_INI_BUFFER_SIZE
					);

				return false;
			}

			_input->read( _ini.buff, size );
			_ini.buff[size] = '\0';

			if( _ini.load( _ini.buff ) == false )
			{
				LOGGER_ERROR(_serviceProvider)("IniUtil::loadIni ini invalid load"
					, size
					, MENGINE_INI_BUFFER_SIZE
					);

				return false;
			}

			return true;
		}
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, WString & _value, ServiceProviderInterface * _serviceProvider )
        {
            const Char * ini_value = _ini.getSettingValue( _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            if( Helper::utf8ToUnicodeSize( _serviceProvider, ini_value, UNICODE_UNSIZE, _value ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, String & _value, ServiceProviderInterface * _serviceProvider )
        {
            (void)_serviceProvider;

            const Char * ini_value = _ini.getSettingValue( _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            _value.assign( ini_value );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, FilePath & _value, ServiceProviderInterface * _serviceProvider )
        {
            const Char * ini_value = _ini.getSettingValue( _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            size_t len = strlen(ini_value);

            if( STRINGIZE_SERVICE(_serviceProvider)
                ->stringize( ini_value, len, false, _value ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, Resolution & _value, ServiceProviderInterface * _serviceProvider )
        {
            const Char * ini_value = _ini.getSettingValue( _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            int width;
            int height;
            if( sscanf( ini_value, "%d %d", &width, &height ) != 2 )
            {
                LOGGER_ERROR(_serviceProvider)("getIniValue section %s key %s value %s invalid parse resolution"
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
		bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, ColourValue & _value, ServiceProviderInterface * _serviceProvider )
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
				LOGGER_ERROR(_serviceProvider)("getIniValue section %s key %s value %s invalid parse ColourValue"
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

			_value.setARGB( a, r, g, b );

			return true;
		}
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, bool & _value, ServiceProviderInterface * _serviceProvider )
        {
            (void)_serviceProvider;

            const Char * ini_value = _ini.getSettingValue( _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            int tmp_value;

            if( sscanf( ini_value, "%d", &tmp_value ) != 1 )
            {
                return false;
            }

            _value = tmp_value != 0;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, size_t & _value, ServiceProviderInterface * _serviceProvider )
        {
            (void)_serviceProvider;

            const Char * ini_value = _ini.getSettingValue( _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            size_t tmp_value;
            if( sscanf( ini_value, "%u", &tmp_value ) != 1 )
            {
                return false;
            }

            _value = tmp_value;

            return true;
        }
		//////////////////////////////////////////////////////////////////////////
		bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, float & _value, ServiceProviderInterface * _serviceProvider )
		{
			(void)_serviceProvider;

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
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, TVectorString & _values, ServiceProviderInterface * _serviceProvider )
        {
            (void)_serviceProvider;

            size_t count = _ini.countSettingValues( _section, _key );

            for( size_t index = 0; index != count; ++index )
            {
                const char * value = _ini.getSettingValues( _section, _key, index );

                _values.push_back( value );
            }

            return true;
        }
		//////////////////////////////////////////////////////////////////////////
		bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, TVectorConstString & _values, ServiceProviderInterface * _serviceProvider )
		{
			(void)_serviceProvider;

			size_t count = _ini.countSettingValues( _section, _key );

			for( size_t index = 0; index != count; ++index )
			{
				const char * value = _ini.getSettingValues( _section, _key, index );

				ConstString cs = Helper::stringizeString( _serviceProvider, value );

				_values.push_back( cs );
			}

			return true;
		}
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, TVectorWString & _values, ServiceProviderInterface * _serviceProvider )
        {
            size_t count = _ini.countSettingValues( _section, _key );

            for( size_t index = 0; index != count; ++index )
            {
                const char * value = _ini.getSettingValues( _section, _key, index );

                WString w_value;
                Helper::utf8ToUnicodeSize( _serviceProvider, value, UNICODE_UNSIZE, w_value );

                _values.push_back( w_value );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, TVectorAspectRatioViewports & _values, ServiceProviderInterface * _serviceProvider )
        {
            size_t count = _ini.countSettingValues( _section, _key );

            for( size_t index = 0; index != count; ++index )
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
                    LOGGER_ERROR(_serviceProvider)("getIniValue section %s key %s value %s invalid parse aspect ration"
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
        bool getIniAllSettings( const IniStore & _ini, const Char * _section, TMapParams & _values, ServiceProviderInterface * _serviceProvider )
        {
            size_t count = _ini.countSettings( _section );

            for( size_t index = 0; index != count; ++index )
            {
                const char * key;
                const char * value;
                _ini.getSettings( _section, index, &key, &value );


                size_t len = strlen(key);

                ConstString c_key;
                if( STRINGIZE_SERVICE(_serviceProvider)
                    ->stringize( key, len, false, c_key ) == false )
                {
                    return false;
                }

                WString w_value;
                if( Helper::utf8ToUnicodeSize( _serviceProvider, value, UNICODE_UNSIZE, w_value ) == false )
                {
                    return false;
                }

                _values.insert( std::make_pair( c_key, w_value ) );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeIniSection( ServiceProviderInterface * _serviceProvider, const OutputStreamInterfacePtr & _file, const char * _section, size_t _sectionSize )
        {        
            (void)_serviceProvider;

            _file->write( _section, _sectionSize );
            _file->write( "\n", sizeof("\n") - 1 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeIniSetting( ServiceProviderInterface * _serviceProvider, const OutputStreamInterfacePtr & _file, const char * _key, const WString & _value )
        {
            size_t len = strlen(_key);
            _file->write( _key, len );
            _file->write( " = ", sizeof(" = ") - 1 );

            size_t utf8_size = 0;

            char utf8_value[512];
            if( UNICODE_SERVICE(_serviceProvider)
                ->unicodeToUtf8( _value.c_str(), _value.size(), utf8_value, 512, &utf8_size ) == false )
            {
                return false;
            }

            _file->write( utf8_value, utf8_size );
            _file->write( "\n", sizeof("\n") - 1 );

            return true;
        }
    }
}