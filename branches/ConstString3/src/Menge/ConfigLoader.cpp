#	include "ConfigLoader.h"

#	include "FileEngine.h"
#	include "LogEngine.h"

#	include "Interface/UnicodeInterface.h"
#	include "Interface/StringizeInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ConfigLoader::ConfigLoader( ServiceProviderInterface * _serviceProvider )
	{
		m_unicodeService =
			_serviceProvider->getServiceT<UnicodeServiceInterface>("UnicodeService");

		m_stringizeService =
			_serviceProvider->getServiceT<StringizeServiceInterface>("StringizeService");
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigLoader::loadFile( const ConstString& _fileSystemName, const WString& _filename )
	{
		if( FileEngine::get()
			->existFile( _fileSystemName, _filename ) == false )
		{
			MENGE_LOG_ERROR("ConfigLoader::loadFile: not exist ini file %s:%S"
				, _fileSystemName.c_str()
				, _filename.c_str()
				);

			return false;
		}

		FileInputStreamInterface * file = FileEngine::get()
			->openInputFile( _fileSystemName, _filename );

		if( file == NULL )
		{
			MENGE_LOG_ERROR("ConfigLoader::loadFile: not found ini file %s:%S"
				, _fileSystemName.c_str()
				, _filename.c_str()
				);

			return false;
		}

		if( m_config.load( file ) == false )
		{
			return false;
		}
		
		file->close();

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

		bool u_locale_default_setting_successful;
		String locale_default = m_unicodeService->unicodeToUtf8( wstring_value, u_locale_default_setting_successful );
				
		_value = m_stringizeService->stringize( locale_default );

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

		int tmp_value;
		if( swscanf( wvalue.c_str(), L"%d", &tmp_value ) != 1 )
		{
			return false;
		}

		_value = tmp_value;

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
		if( swscanf( wvalue.c_str(), L"%d;%d", &width, &height ) != 2 )
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
		if( swscanf( wvalue.c_str(), L"%d;%d;%d;%d", &left, &top, &right, &bottom ) != 4 )
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
}