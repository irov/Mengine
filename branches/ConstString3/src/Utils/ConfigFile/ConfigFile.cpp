#	include "ConfigFile.h"

#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ConfigFile::ConfigFile()
		: m_ini(false, true, true)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigFile::load( FileInputStreamInterface* _file )
	{
		if( _file == 0 )
		{
			return false;
		}

		m_ini.Reset();

		size_t size = _file->size();

		char * buff = new char[size + 1];
		buff[size] = 0;

		_file->read( buff, size );

		SI_Error err = m_ini.LoadData( buff, size );

		delete [] buff;

		if( err < 0 )
		{
			return false;
		}
				
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigFile::save( FileOutputStreamInterface* _file )
	{
		if( _file == 0 )
		{
			return false;
		}

		String buff;
		m_ini.Save( buff );

		_file->write( buff.c_str(), buff.size() );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigFile::getSetting( const WString& _section, const WString& _key, WString & _value ) const
	{
		const WChar * inivalue = m_ini.GetValue( _section.c_str(), _key.c_str() );

		if( inivalue == NULL )
		{
			return false;
		}

		_value.assign( inivalue );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigFile::getSettings( const WString& _section, const WString& _key, TVectorWString & _values ) const
	{
		CSimpleIniCaseW::TNamesDepend values;
		if( m_ini.GetAllValues( _section.c_str(), _key.c_str(), values ) == false )
		{
			return false;
		}

		for( CSimpleIniCaseW::TNamesDepend::const_iterator 
			it = values.begin(),
			it_end = values.end();
		it != it_end;
		++it )
		{
			_values.push_back( it->pItem );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigFile::getSettingUInt( const WString& _section, const WString& _key, size_t & _value ) const
	{
		WString setting;
		if( this->getSetting( _section, _key, setting ) == false )
		{
			return false;
		}

        if( Utils::wstringToUnsigned( setting, _value ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigFile::setSetting( const WString& _section, const WString& _key, const WString & _value )
	{
		SI_Error err = m_ini.SetValue( _section.c_str(), _key.c_str(), _value.c_str() );

        if( err != 0 )
        {
            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigFile::getAllSettings( const WString& _section, TMapWString & _values )
	{
		CSimpleIniCaseW::TNamesDepend values;
		if( m_ini.GetAllKeys( _section.c_str(), values ) == false )
		{
			return false;
		}

		for( CSimpleIniCaseW::TNamesDepend::const_iterator 
			it = values.begin(),
			it_end = values.end();
		it != it_end;
		++it )
		{
			WString val;
			this->getSetting( _section, it->pItem, val );
			
			_values.insert(  std::make_pair( it->pItem, val ) );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
