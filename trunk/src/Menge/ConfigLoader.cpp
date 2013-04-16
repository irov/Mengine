#	include "ConfigLoader.h"

#	include "Logger/Logger.h"

#   include "Utils/Core/String.h"

#	include "Interface/UnicodeInterface.h"
#	include "Interface/StringizeInterface.h"
#   include "Interface/FileSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ConfigLoader::ConfigLoader( ServiceProviderInterface * _serviceProvider )
        : m_serviceProvider(_serviceProvider)
	{
        
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigLoader::loadFile( const ConstString& _fileSystemName, const FilePath& _filename )
	{
		if( FILE_SERVICE(m_serviceProvider)->existFile( _fileSystemName, _filename, NULL ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ConfigLoader::loadFile: not exist ini file %s:%s"
				, _fileSystemName.c_str()
				, _filename.c_str()
				);

			return false;
		}

		InputStreamInterface * file = FILE_SERVICE(m_serviceProvider)
            ->openInputFile( _fileSystemName, _filename );

		if( file == NULL )
		{
			LOGGER_ERROR(m_serviceProvider)("ConfigLoader::loadFile: not found ini file %s:%s"
				, _fileSystemName.c_str()
				, _filename.c_str()
				);

			return false;
		}

		if( m_config.load( file ) == false )
		{
			return false;
		}
		
		file->destroy();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigLoader::getSetting( const WString & _section, const WString & _key, WString & _value ) const
	{
		bool result = m_config.getSetting( _section, _key, _value );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigLoader::getSetting( const WString & _section, const WString & _key, ConstString & _value ) const
	{
		WString wstring_value;
		if( this->getSetting( _section, _key, wstring_value ) == false )
		{
			return false;
		}

		String locale_default;
        if( Helper::unicodeToUtf8( m_serviceProvider, wstring_value, locale_default ) == false )
        {
            return false;
        }
				
		_value = Helper::stringizeString(m_serviceProvider, locale_default);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigLoader::getSetting( const WString & _section, const WString & _key, WString & _value, const WString & _default ) const
	{
		bool result = this->getSetting( _section, _key, _value );

		if( result == false )
		{
			_value = _default;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigLoader::getSetting( const WString & _section, const WString & _key, ConstString & _value, const ConstString & _default ) const
	{
		bool result = this->getSetting( _section, _key, _value );

		if( result == false )
		{
			_value = _default;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigLoader::getSetting( const WString & _section, const WString & _key, bool & _value ) const
	{
		int tmp_value;
		if( this->getSetting( _section, _key, tmp_value ) == false )
		{
			return false;
		}

		_value = tmp_value != 0;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigLoader::getSetting( const WString & _section, const WString & _key, int & _value ) const
	{
		WString wvalue;
		bool result = this->getSetting( _section, _key, wvalue );

		if( result == false )
		{
			return false;
		}
        
        if( Utils::wstringToInt( wvalue, _value ) == false )
        {
            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigLoader::getSetting( const WString & _section, const WString & _key, Resolution & _value ) const
	{
		WString wvalue;
		bool result = this->getSetting( _section, _key, wvalue );

		if( result == false )
		{
			return false;
		}

		int width;
		int height;

        if( Utils::wstringToInt2( wvalue, width, height, true ) == false )
        {
            return false;
        }

		_value.setWidth( width );
		_value.setHeight( height );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigLoader::getSetting( const WString & _section, const WString & _key, Viewport & _value ) const
	{
		WString wvalue;
		bool result = this->getSetting( _section, _key, wvalue );

		if( result == false )
		{
			return false;
		}

		int left;
		int top;
		int right;
		int bottom;

        if( Utils::wstringToInt4( wvalue, left, top, right, bottom, true ) == false )
        {
            return false;
        }

		_value.begin.x = float(left);
		_value.begin.y = float(top);
		_value.end.x = float(right);
		_value.end.y = float(bottom);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigLoader::getSettings( const WString & _section, const WString & _key, TVectorWString & _values ) const
	{
		bool result = m_config.getSettings( _section, _key, _values );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigLoader::getAllSettings( const WString& _section, TMapWString & _values )
	{
		bool result = m_config.getAllSettings( _section, _values );

		return result;
	}

}