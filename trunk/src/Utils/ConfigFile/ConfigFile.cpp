#	include "ConfigFile.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ConfigFile::ConfigFile( ServiceProviderInterface * _serviceProvider )
        : m_serviceProvider(_serviceProvider)
		, m_ini(true, true, true)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigFile::load( const InputStreamInterfacePtr & _file )
	{
		if( _file == nullptr )
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
    namespace
    {
        class StringOutputWriter
            : public TIniParser::OutputWriter
        {
        public:
            StringOutputWriter( String & _str )
                : m_str(_str)
            {
            }

        public:
            void Write( const char * _buff ) override
            {
                m_str.append( _buff );
            }

        public:
            String & m_str;
        };
    }
	//////////////////////////////////////////////////////////////////////////
	bool ConfigFile::save( const OutputStreamInterfacePtr & _file )
	{
		if( _file == nullptr )
		{
			return false;
		}

		String buff;
        StringOutputWriter sow(buff);
		SI_Error ini_error = m_ini.Save( sow );

        if( ini_error != SI_OK )
        {
            return false;
        }

		_file->write( buff.c_str(), buff.size() );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigFile::getSetting( const char * _section, const ConstString & _key, WString & _value ) const
	{
		const char * inivalue = m_ini.GetValue( _section, _key.c_str() );

		if( inivalue == nullptr )
		{
			return false;
		}

        if( Helper::utf8ToUnicodeSize( m_serviceProvider, inivalue, (size_t)-1, _value ) == false )
        {
            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigFile::getSettings( const char * _section, const ConstString & _key, TVectorWString & _values ) const
	{
		TIniParser::TNamesDepend values;
		if( m_ini.GetAllValues( _section, _key.c_str(), values ) == false )
		{
			return false;
		}

		for( TIniParser::TNamesDepend::const_iterator 
			it = values.begin(),
			it_end = values.end();
		it != it_end;
		++it )
		{
            const char * inivalue = it->pItem;

            WString value;
            if( Helper::utf8ToUnicodeSize( m_serviceProvider, inivalue, (size_t)-1, value ) == false )
            {
                return false;
            }

			_values.push_back( value );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigFile::getSettingUInt( const char * _section, const ConstString & _key, size_t & _value ) const
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
}	// namespace Menge
